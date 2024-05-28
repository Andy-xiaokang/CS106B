#ifndef TemporaryComponent_Included
#define TemporaryComponent_Included

#include "gwindow.h"
#include "gobjects.h"
#include <string>

/* A type representing a graphics object with a short lifetime. The object
 * is created and installed when the Temporary is created, and it is
 * removed and deallocated when the Temporary is destroyed.
 */
template <typename Component> class Temporary {
public:
    /* Constructor installs the new component. */
    Temporary(Component* component, GWindow& window, const std::string& location);

    /* Default constructor does nothing. */
    Temporary() = default;

    /* Destructor removes it. */
    ~Temporary();

    /* Access underlying component. */
    Component* get() const;
    Component& operator*  () const;
    Component* operator-> () const;

    /* Release underlying component. */
    void release();

    /* Copying is not allowed. */
    Temporary(const Temporary &) = delete;

    /* Moving is allowed. */
    Temporary(Temporary &&);
    Temporary& operator= (Temporary);

private:
    /* Underlying component, window, and location. */
    Component* component   = nullptr;
    GWindow*   window      = nullptr;
    std::string location;
};

/* Utility function to make a new Temporary. */
template <typename Component, typename... Args>
Temporary<Component> make_temporary(GWindow& window, const std::string& region, Args&&... args) {
    return Temporary<Component>(new Component(std::forward<Args>(args)...), window, region);
}

template <typename Component, typename Pointer>
bool operator== (const Temporary<Component>& lhs, const Pointer* rhs) {
    return lhs.get() == rhs;
}

template <typename Component, typename Pointer>
bool operator!= (const Temporary<Component>& lhs, const Pointer* rhs) {
    return !(lhs == rhs);
}

template <typename Component, typename Pointer>
bool operator== (const Pointer* lhs, const Temporary<Component>& rhs) {
    return rhs == lhs;
}

template <typename Component, typename Pointer>
bool operator!= (const Pointer* lhs, const Temporary<Component>& rhs) {
    return rhs != lhs;
}

/* * * * * Implementation Below This Point * * * * */
template <typename Component>
Temporary<Component>::Temporary(Component* component,
                                GWindow& window,
                                const std::string& location)
    : component(component), window(&window), location(location) {
    window.addToRegion(component, location);
}

/* Destructor removes and frees the component, assuming that there is one to begin with. */
template <typename Component>
Temporary<Component>::~Temporary() {
    release();
}

/* Release removes the component from its containing window. */
template <typename Component>
void Temporary<Component>::release() {
    if (component != nullptr) {
        window->removeFromRegion(component, location);
        delete component;

        /* The center region is unusual in that the canvas is supposed to go there.
         * If we removed something out of the center, we need to restore the canvas.
         */
        if (location == "CENTER") {
            window->addToRegion(window->getCanvas(), "CENTER");
        }
    }
}

/* Accessor just hands back the underlying component. */
template <typename Component>
Component* Temporary<Component>::get() const {
    return component;
}

/* Star and arrow operators. */
template <typename Component>
Component& Temporary<Component>::operator *() const {
    return *component;
}
template <typename Component>
Component* Temporary<Component>::operator->() const {
    return &**this;
}

/* Move support. */
template <typename Component>
Temporary<Component>::Temporary(Temporary&& rhs) {
    /* Swap everything over. */
    std::swap(component, rhs.component);
    std::swap(window,    rhs.window);
    std::swap(location,  rhs.location);
}

template <typename Component>
Temporary<Component>& Temporary<Component>::operator= (Temporary rhs) {
    std::swap(component, rhs.component);
    std::swap(window,    rhs.window);
    std::swap(location,  rhs.location);
    return *this;
}

#endif
