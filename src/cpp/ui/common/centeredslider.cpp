/**
 * @file        common/centeredslider.cpp
 * @brief       Slider bar widget, with positive/negative values with the 0 position at the
 *              center, and the progress drawn from this origin point
 * @author      Justin Scott
 * @date        2026-08-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/centeredslider.hpp>

#include <QProxyStyle>
#include <QStyle>
#include <QStyleHints>
#include <QStyleOption>

namespace ui::common
{

static constexpr int MAX_SLIDER_GROOVE_WIDTH = 20;
static constexpr int HANDLE_RADIUS =
#ifdef Q_OS_ANDROID
        9;
#else
        3;
#endif

namespace // anonymous
{

///////////////////////////////////////////////////////////////////////////////
/// Proxy style for the slider bar to increase the rect for the handle and
/// groove
///
///////////////////////////////////////////////////////////////////////////////
class SliderStyle : public QProxyStyle
    {
public:
    using QProxyStyle::QProxyStyle;


    ///////////////////////////////////////////////////////////////////////////////
    /// Get the rect for a sub control
    ///
    /// @param[in]  cc      Complex control
    /// @param[in]  opt     Style options
    /// @param[in]  sc      Sub control
    /// @param[in]  widget  Widget this complex control belongs to
    ///
    /// @return     Rect for the sub control
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QRect subControlRect (ComplexControl                cc,
                          const QStyleOptionComplex*    opt,
                          SubControl                    sc,
                          const QWidget*                widget) const override
        {
        QRect widgetRect    = widget->rect ();
        QRect rect          = QProxyStyle::subControlRect (cc, opt, sc, widget);

        switch (sc)
            {
            case SubControl::SC_SliderHandle:
                {
#ifdef Q_OS_ANDROID
                // Already centered on android
                // Make it larger to be easier to use on touch screens
                rect.adjust (-20, -5, 20, 5);
#else
                QRect   grooveRect = subControlRect (cc,
                                                     opt,
                                                     QStyle::SC_SliderGroove,
                                                     widget);

                rect.moveCenter (QPoint{ grooveRect.center ().x (), rect.center ().y () });
                rect.adjust (-5, 0, 5, 0);
#endif // Q_OS_ANDROID

                break;
                }
            case QStyle::SC_SliderGroove:
                {
                // Already centered on android
#ifdef Q_OS_ANDROID
                rect.adjust (10, 5, -10, -5);
#else
                int centerNudge = widgetRect.center ().x () - rect.center ().x ();

                rect.moveLeft (centerNudge);
                rect.adjust (0, 5, 0, -5);
#endif
                break;
                }
            default:
                {
                break;
                }
            }

        return rect;
        }

    };

} // namespace anonymous

CenteredSlider::CenteredSlider (Qt::Orientation ornt, QWidget* parent) :
    common::TouchSlider (ornt, parent)
    {
    setStyle (new SliderStyle);
    }

void CenteredSlider::paintEvent (QPaintEvent* event)
    {
    QPainter painter (this);
    painter.setRenderHint (QPainter::Antialiasing, true);

    QStyleOptionSlider option;

#ifdef Q_OS_WIN
    option.initFrom (this);
#else
    initStyleOption (&option);
#endif // Q_OS_WIN


    QRect   grooveRect;
    QRect   handleRect;
    QRect   progressRect;

    QPoint  handlePoint;
    int     handlePos   = 0;
    QRect   widgetRect  = rect ();
    bool    darkMode    = Qt::ColorScheme::Dark == qApp->styleHints ()->colorScheme ();

    if (Qt::Horizontal == orientation ())
        {
        QRect grooveRect = style ()->subControlRect (QStyle::CC_Slider,
                                                    &option,
                                                     QStyle::SC_SliderGroove,
                                                     this);
        QRect handleRect = style ()->subControlRect (QStyle::CC_Slider,
                                                    &option,
                                                     QStyle::SC_SliderHandle,
                                                     this);

        QPoint grooveCenter = grooveRect.center ();
        QRect progressRect = grooveRect;

        handlePos = grooveRect.left () +
                    ((grooveRect.width () - handleRect.width ()) *
                        (-value () - minimum ()) /
                        (maximum () - minimum ()));


        handlePoint = QPoint (handlePos, grooveCenter.y ());
        handleRect.moveLeft (handlePoint.x ());

        //from the center
        progressRect.setLeft (grooveCenter.x ());
        //to the handle's position
        progressRect.setRight (handlePoint.x ());
        }
    else // (Qt::Vertical == orientation ())
        {
        option.orientation = Qt::Vertical;
        grooveRect = style ()->subControlRect (QStyle::CC_Slider,
                                              &option,
                                               QStyle::SC_SliderGroove,
                                               this);
        handleRect = style ()->subControlRect (QStyle::CC_Slider,
                                              &option,
                                               QStyle::SC_SliderHandle,
                                               this);



        // On Mac the groove expands to fill the space
        // This makes it look more reasonable
#ifdef Q_OS_MACOS
        if (grooveRect.width() > MAX_SLIDER_GROOVE_WIDTH)
            {
            int delta = grooveRect.width() - MAX_SLIDER_GROOVE_WIDTH;

            grooveRect.adjust (delta / 2, 0, -delta / 2, 0);
            handleRect.adjust (delta / 2, 0, -delta / 2, 0);
            }
#endif // Q_OS_MACOS

        QPoint grooveCenter = grooveRect.center ();
        progressRect = grooveRect;

        handlePos = grooveRect.top () +
                    ((grooveRect.height () - handleRect.height ()) *
                        (-value () - minimum ()) / (maximum () - minimum ()));

        handlePoint = QPoint (grooveCenter.x (), handlePos);
        handleRect.moveTop (handlePoint.y ());

        //from the center
        progressRect.setTop (grooveCenter.y ());
        //to the handle's position
        progressRect.setBottom (handlePoint.y ());
        }

    //draw groove
    painter.setBrush (Qt::lightGray);
    painter.setPen (Qt::darkGray);
    painter.drawRoundedRect (grooveRect, 5, 5);

    //draw progress
    painter.setBrush (Qt::cyan);
    painter.setPen (Qt::darkGray);
    painter.drawRect (progressRect);

    //draw handle
    painter.setBrush (Qt::gray);
    painter.setPen (Qt::darkGray);
    painter.drawRoundedRect (handleRect,
                             HANDLE_RADIUS,
                             HANDLE_RADIUS);

    if (0 != tickInterval ())
        {
        // Draw ticks
        const double    stepSpacing = grooveRect.height () /
                                        static_cast<double> (maximum () - minimum ());
        const int       minTick     = minimum () + ((tickInterval () -
                                        (minimum () % tickInterval ())) % tickInterval ());
        const int       maxTick     = maximum () - (maximum () % tickInterval ());

        painter.setPen (darkMode ? Qt::white : Qt::black);

        for (int ii = minimum (); ii < maximum (); ++ii)
            {
            const int posAbsolute   = static_cast<int> (round ((ii - minimum ()) * stepSpacing));
            const int width         = 0 == ii || minTick == ii || maxTick == ii ? 10 : 4;

            if (0 == (ii % tickInterval ()))
                {
                if (tickPosition () & TicksLeft)
                    {
                    painter.drawLine (grooveRect.center ().x () - 5 - handleRect.width () / 2,
                                      grooveRect.top () + posAbsolute,
                                      grooveRect.center ().x () - 5 - width - handleRect.width () / 2,
                                      grooveRect.top () + posAbsolute);
                    }

                if (tickPosition () & TicksRight)
                    {
                    painter.drawLine (grooveRect.center ().x () + 5 + handleRect.width () / 2,
                                      grooveRect.top () + posAbsolute,
                                      grooveRect.center ().x () + 5 + width + handleRect.width () / 2,
                                      grooveRect.top () + posAbsolute);
                    }
                }
            }
        }
    }

} // namespace ui::common
