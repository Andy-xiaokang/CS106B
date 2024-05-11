/*
 * File: qgscrollbar.h
 * -------------------
 *
 * @author Marty Stepp
 * @version 2018/09/08
 * - added doc comments for new documentation generation
 * @version 2018/08/23
 * - renamed to gscrollbar.h to replace Java version
 * @version 2018/07/16
 * - initial version
 */


#ifndef _gscrollbar_h
#define _gscrollbar_h

#include <string>
#include <QScrollBar>

#include "ginteractor.h"

class _Internal_QScrollBar;

/**
 * A GScrollBar represents a horizontal or vertical scroll bar that can be
 * dragged by the user.  The bar does not inherently cause any other interactor
 * to scroll itself.  If you want the bar to cause any effect, you must wait
 * for scroll events and respond to them.
 *
 * A given scroll bar has a range of values it can represent, with a min and max,
 * along with a current value.
 * The "extent" of a scrollbar represents the amount of the scrollbar in view.
 */
class GScrollBar : public GInteractor {
public:
    /**
     * The two valid orientations of scrollbars.
     */
    enum Orientation {
        HORIZONTAL = 0,
        VERTICAL   = 1
    };

    /**
     * Creates a new scroll bar with the given orientation and value range.
     * @throw ErrorException if min > max or value is not between min and max
     */
    GScrollBar(Orientation orientation = VERTICAL, int value = 0, int extent = 10, int min = 0, int max = 100, QWidget* parent = nullptr);

    /**
     * Frees memory allocated internally by the scroll bar.
     */
    ~GScrollBar() override;

    /**
     * Returns the scroll bar's extent, meaning the amount of its range that is
     * currently in view.
     */
    virtual int getExtent() const;

    /* @inherit */
    _Internal_QWidget* getInternalWidget() const override;

    /**
     * Returns the maximum allowed value of the scroll bar.
     */
    virtual int getMax() const;

    /**
     * Returns the minimum allowed value of the scroll bar.
     */
    virtual int getMin() const;

    /**
     * Returns the orientation of the scroll bar, either HORIZONTAL or VERTICAL.
     */
    virtual Orientation getOrientation() const;

    /* @inherit */
    std::string getType() const override;

    /**
     * Returns the current value of the scroll bar.
     */
    virtual int getValue() const;

    /* @inherit */
    QWidget* getWidget() const override;

    /**
     * Sets the scroll bar's extent, meaning the amount of its range that is
     * currently in view.
     */
    virtual void setExtent(int extent);

    /**
     * Sets the maximum allowed value of the scroll bar.
     * @throw ErrorException if min > max
     */
    virtual void setMax(int max);

    /**
     * Sets the minimum allowed value of the scroll bar.
     * @throw ErrorException if min > max
     */
    virtual void setMin(int min);

    /**
     * Sets all of the relevant state of the scroll bar.
     * @throw ErrorException if min > max or value is not between min and max
     */
    virtual void setState(int value, int extent, int min, int max);

    /**
     * Sets the current value of the scroll bar.
     * @throw ErrorException if value is not between min and max
     */
    virtual void setValue(int value);

protected:
    /**
     * @private
     */
    std::string getActionEventType() const override;

private:
    Q_DISABLE_COPY(GScrollBar)

    _Internal_QScrollBar* _iqscrollbar;

    friend class _Internal_QScrollBar;

    void updateSize();
};

/**
 * Internal class; not to be used by clients.
 * @private
 */
class _Internal_QScrollBar : public QScrollBar, public _Internal_QWidget {
    Q_OBJECT

public:
    _Internal_QScrollBar(GScrollBar* qgscrollbar, Qt::Orientation orientation, QWidget* parent = nullptr);
    void detach() override;
    QSize sizeHint() const override;

public slots:
    void handleValueChange(int value);

private:
    GScrollBar* _gscrollbar;

    friend class GScrollBar;
};

#endif // _gscrollbar_h
