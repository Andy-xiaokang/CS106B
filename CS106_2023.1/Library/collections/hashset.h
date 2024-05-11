/*
 * File: hashset.h
 * ---------------
 * This file exports the <code>HashSet</code> class, which
 * implements an efficient abstraction for storing sets of values.
 */

#ifndef _hashset_h
#define _hashset_h

#include <initializer_list>
#include <iostream>

#include "collections.h"
#include "hashmap.h"

/* Traits type for the HashSet, which wraps an underlying HashMap. */
namespace stanfordcpplib {
    namespace collections {
        template <typename T> struct HashSetTraits {
            using ValueType = T;
            using MapType   = HashMap<T, bool>;
            static std::string name() {
                return "HashSet";
            }
            /* You can default-construct a HashSet. */
            static MapType construct() {
                return {};
            }

            /* However, you can't pass in any other arguments. */
            template <typename... Args>
            static void construct(Args&&...) {
                static_assert(Fail<Args...>::value, "Oops! Seems like you tried to initialize a HashSet incorrectly. Click here for details.");

                /*
                 * Hello student! If you are reading this message, it means that you tried to
                 * initialize a HashSet improperly. For example, you might have tried to
                 * write something like this:
                 *
                 *     HashSet<int> mySet = 137; // Oops!
                 *
                 * Here, for example, you're trying to assign an int to a HashSet<int>.
                 *
                 * or perhaps you had a function like this one:
                 *
                 *     void myFunction(HashSet<int>& mySet);
                 *
                 * and you called it by writing
                 *
                 *     myFunction(someSet + someOtherSet); // Oops!
                 *     myFunction({ });                    // Oops!
                 *
                 * In these cases, you're trying to pass a value into a function that takes
                 * its argument by (non-const) reference. C++ doesn't allow you to do this.
                 *
                 * To see where the actual error comes from, look in the list of error messages
                 * in Qt Creator. You should see a line that says "required from here" that
                 * points somewhere in your code. That's the actual line you wrote that caused
                 * the problem, so double-click on that error message and see where it takes
                 * you. Now you know where to look!
                 *
                 * Hope this helps!
                 */
                error("static_assert succeeded?");
            }
        };
    }
}

/*
 * A set of elements stored in no particular order. Elements can only be stored here
 * if they support a function
 *
 *     int hashCode(ValueType);
 *
 * that returns a nonnegative integer, along with equality comparison using ==.
 */
template <typename ValueType>
    using HashSet = stanfordcpplib::collections::GenericSet<stanfordcpplib::collections::HashSetTraits<ValueType>>;

#endif // _hashset_h
