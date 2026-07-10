/**
 * @file        clock/analogclock.cpp
 * @brief       Analog clock widget, based on code from Qt Widgets Examples
 *              Tutorial : https://doc.qt.io/qt-6/qtwidgets-widgets-analogclock-example.html
 *              Code : https://code.qt.io/cgit/qt/qtbase.git/tree/examples/widgets/widgets/analogclock?h=6.11
 *              Copyright (C) 2016 The Qt Company Ltd.
 *              SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
 * @author      Justin Scott
 * @date        2026-05-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <control/automation/clock.hpp>

#include <ui/clock/analogclock.hpp>

#include <utils/battery.hpp>
#include <utils/draw.hpp>
#include <utils/magnify.hpp>
#include <utils/math.hpp>
#include <utils/poly.hpp>

#include <QPainter>
#include <QPainterStateGuard>
#include <QStyleHints>
#include <QTime>


namespace // anonymous
{

// Type of date window
enum class dateWindowStyle
    {
    NONE,       ///< No date window
    DATE,       ///< Date of month only
    DAY_DATE,   ///< Day of week + date of month
    MONTH_DATE  ///< Month string + date of month
    };

// Dial shape
enum class dialStyle
    {
    ROUND,  ///< Circle
    RECT    ///< Rectangle
    };

// Scheme dependent style
struct styleInfoScheme
    {
    QColor  brush   = Qt::transparent;
    QColor  pen     = Qt::transparent;

    ///////////////////////////////////////////////////////////////////////////////
    /// Apply this style to a painter
    ///
    /// @param[in,out]  painter     Painter to apply style to
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void apply (QPainter& painter)
        {
        painter.setBrush (brush);
        painter.setPen (pen);
        }
    };

// Style info for a item
struct styleInfo
    {
    styleInfoScheme light;
    styleInfoScheme dark;

    ///////////////////////////////////////////////////////////////////////////////
    /// Apply this style to a painter
    ///
    /// @param[in,out]  painter     Painter to apply style to
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void apply (QPainter& painter)
        {
        if (Qt::ColorScheme::Dark == qApp->styleHints ()->colorScheme ())
            {
            dark.apply (painter);
            }
        else
            {
            light.apply (painter);
            }
        }
    };


///////////////////////////////////////////////////////////////////////////////
/// Abstract base class for Clock index
///
///////////////////////////////////////////////////////////////////////////////
class Index
    {
public:
    styleInfo style;

    ///////////////////////////////////////////////////////////////////////////////
    /// Draw this index to a painter
    ///
    /// @param[in,out]  painter     Painter to draw with
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void paint (QPainter& painter)
        {
        style.apply (painter);
        draw (painter);
        }

    virtual ~Index () {}
private:
    ///////////////////////////////////////////////////////////////////////////////
    /// Internal drawing logic. Implement in derived class
    ///
    /// @param[in,out]  painter     Painter to draw with
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void draw (QPainter& painter) = 0;
    };

///////////////////////////////////////////////////////////////////////////////
/// Rectangular index
///
///////////////////////////////////////////////////////////////////////////////
class RectIndex : public Index
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  rect    Index rect
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit RectIndex (const QRect& rect) :
        m_rect (rect)
        {}

private:
    QRect m_rect;   ///< Index rect

    ///////////////////////////////////////////////////////////////////////////////
    /// Draw this index
    ///
    /// @param[in,out]  painter     Painter to draw with
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void draw (QPainter& painter) override
        { painter.drawRect (m_rect); }

    };


///////////////////////////////////////////////////////////////////////////////
/// Circular index
///
///////////////////////////////////////////////////////////////////////////////
class CircleIndex : public Index
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  offset      Radial offset from the center of the clock face
    /// @param[in]  radius      Radius of the index
    ///
    ///////////////////////////////////////////////////////////////////////////////
    CircleIndex (qreal offset, qreal radius) :
        m_offset (offset),
        m_radius (radius)
        {}

private:
    qreal m_offset; ///< Radial offset from the center of the clock face
    qreal m_radius; ///< Radius of the index

    ///////////////////////////////////////////////////////////////////////////////
    /// Draw this index
    ///
    /// @param[in,out]  painter     Painter to draw with
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void draw (QPainter& painter) override
        { painter.drawEllipse ({ m_offset, 0, }, m_radius, m_radius); }

    };

///////////////////////////////////////////////////////////////////////////////
/// Arabic numeral index
///
///////////////////////////////////////////////////////////////////////////////
class NumberIndex : public Index
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  idx     Index number
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit NumberIndex (uint idx) :
        m_idx (QString::number (idx))
        {}

private:
    QString m_idx;  ///< Index number

    ///////////////////////////////////////////////////////////////////////////////
    /// Draw this index
    ///
    /// @param[in,out]  painter     Painter to draw with
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void draw (QPainter& painter) override
        { painter.drawText (QPoint{0, 0}, m_idx); }

    };

///////////////////////////////////////////////////////////////////////////////
/// Polygon index
///
///////////////////////////////////////////////////////////////////////////////
class PolyIndex : public Index
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  pts     List of polygon points
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit PolyIndex (std::initializer_list<QPoint> pts) :
        m_poly (pts)
        {}

private:
    QPolygon m_poly;    ///< Polygon to render

    ///////////////////////////////////////////////////////////////////////////////
    /// Draw this index
    ///
    /// @param[in,out]  painter     Painter to draw with
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void draw (QPainter& painter) override
        { painter.drawPolygon (m_poly); }
    };

// Clock face indices
using indexArray = std::array<std::unique_ptr<Index>, 12>;

// Variable number of indices, for other indicators
using indexVector = std::vector<std::unique_ptr<Index>>;

// Main spring power indicator. Maps to the device battery
struct powerIndicator
    {
    QPoint                  pos;    ///< Center position
    double                  start;  ///< Start angle in degrees
    double                  end;    ///< End angle in degrees
    int                     radius; ///< Radius of the dial
    indexVector             indices;///< Power indicator indices
    utils::MultiPolygonF    hand;   ///< Power indicaror hand
    };

// Represents a set of hands to draw over the dial
struct hands
    {
    utils::MultiPolygonF                hour;           ///< Hour hand
    utils::MultiPolygonF                minute;         ///< Minute hand
    std::optional<utils::MultiPolygonF> second;         ///< Second hand

    std::optional<QPoint>               hourCenter;     ///< Center point for the hour and minute hands,
                                                        ///  Defaults to dial center
    std::optional<QPoint>               secondCenter;   ///< Center point for the second hand,
                                                        ///  Defaults to dial center
    };


// Dial style info
struct dial
    {
    indexArray  indices;                        ///< Major indices (every hour/5 minutes
    bool        subIndices  = true;             ///< True for sub-indices every minute
    dialStyle   style       = dialStyle::ROUND; ///< Dial shape
    double      dialRatio   = 1.0;              ///< Height to width ratio
    };

// Day/date window
struct dateWindow
    {
    dateWindowStyle                         style;  ///< Style of window
    std::optional<utils::Magnifier<true>>   cyclops;///< "Cyclops" magnification lens
    };

// Clock face
struct clockFace
    {
    hands                           hands;          ///< Clock hands
    dial                            dial;           ///< Clock dial
    dateWindow                      window;         ///< Day/date window
    std::optional<powerIndicator>   powerIndicator;
    };


} // anonymous namespace

namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Make an array of the same kind of index
///
/// @tparam     T       Index type
///
/// @param[in]  index   Index to copy for all 12 indices
/// @param[in]  style   Style info for indices
///
/// @return     Array of 12 indices
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
static indexArray makeUniformIndexArray (T                  index,
                                         const styleInfo&   style)
    {
    static_assert (std::is_base_of_v<Index, T>, "Must be index type");

    index.style = style;

    indexArray indices;

    for (size_t ii = 0; ii < std::size (indices); ++ii)
        {
        indices[ii] = std::make_unique<T> (index);
        }

    return indices;
    }

///////////////////////////////////////////////////////////////////////////////
/// Make a vector of uniform indices
///
/// @tparam     T       Index type
///
/// @param[in]  index   Index to copy for all indices
/// @param[in]  style   Style info for indices
/// @param[in]  count   Number of copies to make of index
///
/// @return     Vector of [count] indices
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
static indexVector makeUniformIndexVector (T                index,
                                           const styleInfo& style,
                                           size_t           count)
    {
    static_assert (std::is_base_of_v<Index, T>, "Must be index type");

    index.style = style;

    indexVector indices;

    for (size_t ii = 0; ii < count; ++ii)
        {
        indices.emplace_back (std::make_unique<T> (index));
        }

    return indices;
    }


///////////////////////////////////////////////////////////////////////////////
/// Make an array of uniform indices with special "key" indices at 6, 9 and 12
///
/// @tparam     T       Main index type
/// @tparam     U       6/9 index type
/// @tparam     V       12 index type
///
/// @param[in]  index   Main index
/// @param[in]  key1    6/9 index
/// @param[in]  key2    12 index
/// @param[in]  style   Index style info
///
///////////////////////////////////////////////////////////////////////////////
template<class T, class U, class V>
static indexArray makeKeyIndexArray (const T&           index,
                                     const U&           key1,
                                     const V&           key2,
                                     const styleInfo&   style)
    {
    static_assert (std::is_base_of_v<Index, T>, "Must be index type");

    indexArray indices;

    for (size_t ii = 0; ii < std::size (indices); ++ii)
        {
        if (0 == ii)
            {
            // Skip day index
            continue;
            }
        else if (3 == ii || 6 == ii)
            {
            indices[ii] = std::make_unique<U> (key1);
            }
        else if (9 == ii)
            {
            indices[ii] = std::make_unique<V> (key2);
            }
        else
            {
            indices[ii] = std::make_unique<T> (index);
            }

        indices[ii]->style = style;
        }

    return indices;
    }

// List of clock faces/styles
static const clockFace FACES[] =
    {
    // Basic
        {
        hands
            {
            // .hour =
            utils::ComplexPolygonF
                {
                QPointF{  5,  14 },
                QPointF{ -5,  14 },
                QPointF{ -4, -71 },
                QPointF{  4, -71 }
                },
            // .minutes =
            utils::ComplexPolygonF
                {
                QPointF{  4,  14 },
                QPointF{ -4,  14 },
                QPointF{ -3, -89 },
                QPointF{  3, -89 }
                },
            // .seconds =
            utils::ComplexPolygonF
                {
                QPointF{  1,  14 },
                QPointF{ -1,  14 },
                QPointF{ -1, -89 },
                QPointF{  1, -89 }
                }
            },
        dial
            {
            /* .indices = */ makeUniformIndexArray (RectIndex{ QRect{73, -3, 16, 6} },
                                                    styleInfo
                                                        {
                                                            { Qt::black, Qt::black },
                                                            { Qt::white, Qt::white },
                                                        })
            },
        dateWindow { /* .style = */ dateWindowStyle::DAY_DATE}
        },
    // Sub style
        {
        hands
            {
            // .hour =
         (((utils::ComplexPolygonF
                {
                QPointF{ -6,  0 },

                QPointF{ -6, -45 },
                QPointF{  0, -60 },
                QPointF{  6, -45 },


                QPointF{  6,  0 }
                } -
            utils::ComplexPolygonF
                {
                QPointF{ -3,  -8 },
                QPointF{ -3,  -45 },
                QPointF{  0,  -51 },
                QPointF{  3,  -45 },
                QPointF{  3,  -8 },
                }) |
            utils::ComplexPolygonF{ utils::poly::circle (QPointF{ 0.0, 0.0 }, 8) } |
            utils::ComplexPolygonF{ utils::poly::circle (QPointF{ 0.0, -33 }, 12) }) -
            utils::ComplexPolygonF{ utils::poly::circle (QPointF{ 0.0, -33 }, 9) }) |
            utils::ComplexPolygonF
                {
                QPointF{  2.0, -23.0 },
                QPointF{  2.0, -31.0 },
                QPointF{  9.0, -38.0 },
                QPointF{  7.0, -40.0 },
                QPointF{  0.0, -34.0 },
                QPointF{ -7.0, -40.0 },
                QPointF{ -9.0, -38.0 },
                QPointF{ -2.0, -31.0 },
                QPointF{ -2.0, -23.0 }
                },

            // .minutes =
           (utils::ComplexPolygonF
                {
                QPointF{ -2,   0 },
                QPointF{ -2,  -4 },
                QPointF{ -4,  -6 },

                QPointF{ -4, -71 },
                QPointF{  0, -89 },
                QPointF{  4, -71 },


                QPointF{  4, -6 },
                QPointF{  2, -4 },
                QPointF{  2,  0 },
                } -
            utils::ComplexPolygonF
                {
                QPointF{ -2,  -8 },
                QPointF{ -2,  -69 },
                QPointF{  2,  -69 },
                QPointF{  2,  -8 },
                }) |
            utils::ComplexPolygonF{ utils::poly::circle (QPointF{ 0.0, 0.0 }, 4) },
            // .seconds =
            utils::ComplexPolygonF
                {
                QPointF{  1,  30 },
                QPointF{ -1,  30 },
                QPointF{ -1, -89 },
                QPointF{  1, -89 }
                } |
            utils::ComplexPolygonF{ utils::poly::circle ({ 0,  30 }, 4) } |
            utils::ComplexPolygonF{ utils::poly::circle ({ 0,   0 }, 3) } |
            utils::ComplexPolygonF{ utils::poly::circle ({ 0, -55 }, 5) }
            },
        dial
            {
            /* .indices = */ makeKeyIndexArray (CircleIndex{ 78, 7 },
                                                RectIndex{ QRect{ 62, -5, 28, 10 } },
                                                PolyIndex
                                                    {
                                                    QPoint{ 60,   0 },
                                                    QPoint{ 88,  10 },
                                                    QPoint{ 88, -10 },
                                                    },
                                                styleInfo
                                                    {
                                                        { Qt::transparent, Qt::black },
                                                        { Qt::transparent, Qt::white }
                                                    })
            },
        dateWindow
            {
            /* .style   = */ dateWindowStyle::DATE,
            /* .cylcops = */ utils::Magnifier<true>{
                                utils::ComplexPolygonF{
                                    utils::poly::roundedRect (
                                        QRectF{ 40, -18, 50, 40 }, 12, 16) }, 2.5 }
            }
        },
    // Snowbank
        {
        hands
            {
            // .hour =
            utils::ComplexPolygonF
                {
                QPointF{  0,  15 },
                QPointF{ -8,  0 },
                QPointF{  0, -70 },
                QPointF{  8,  0 }
                },
            // .minutes =
            utils::ComplexPolygonF
                {
                QPointF{  0, 15 },
                QPointF{ -5,  0 },
                QPointF{ -0, -90 },
                QPointF{  5,  0 }
                },
            // .seconds =
            utils::ComplexPolygonF
                {
                QPointF{  2,  20 },
                QPointF{  0, -90 },
                QPointF{ -2,  20 }
                } |
            utils::ComplexPolygonF{ utils::poly::circle ({ 0, 0 }, 3) }
            },
        dial
            {
            /* .indices = */ makeKeyIndexArray (RectIndex{ QRect{ 62, -2, 28, 4 } },
                                                PolyIndex
                                                    {
                                                    QPoint{ 60,   3 },
                                                    QPoint{ 60,  -3 },
                                                    QPoint{ 88,  -5 },
                                                    QPoint{ 88,   5 },
                                                    },
                                                PolyIndex
                                                    {
                                                    QPoint{ 60,   4 },
                                                    QPoint{ 60,  -4 },
                                                    QPoint{ 88,  -8 },
                                                    QPoint{ 88,   8 },
                                                    },
                                                styleInfo
                                                    {
                                                        { Qt::transparent, Qt::black },
                                                        { Qt::transparent, Qt::white }
                                                    })
            },
        dateWindow
            {
            /* .style   = */ dateWindowStyle::DATE
            },
        powerIndicator
            {
            QPoint{ -15, 40 },
            270.0,
            360.0,
            35,
            makeUniformIndexVector (RectIndex{ QRect{ 25, -1, 8, 2 } },
                                    styleInfo
                                        {
                                            { Qt::black, Qt::black },
                                            { Qt::white, Qt::white }
                                        },
                                    2),
            utils::ComplexPolygonF
                {
                QPointF{  0, 2 },
                QPointF{ 30, 0 },
                QPointF{  0,-2 },
                } |
            utils::ComplexPolygonF{ utils::poly::circle ({ 0, 0 }, 3) }
            }
        }
    };

ASSERT_ARRAY_LENGTH (FACES, AnalogClock::NUM_STYLES);


///////////////////////////////////////////////////////////////////////////////
/// Draw a clock hand
///
/// @tparam     N       Number of hand points
///
/// @Param[in,out]  painter     Painter instance
/// @param[in]      rotation    Hand rotation in degrees [0,360]
/// @param[in]      offfset     Offset to apply to hand
/// @param[in]      hand        Clock hand vector symbol
/// @param[in]      lolipop     Draw a "lolipop" style hand, with a large
///                             circle on the end
///
///////////////////////////////////////////////////////////////////////////////
static void drawHand (QPainter&     painter,
                      const QPoint& offset,
                      const utils::PolygonViewF& hand)
    {
    painter.translate (offset);

    utils::draw::polygon (painter, hand);
    }


///////////////////////////////////////////////////////////////////////////////
/// Draw a clock hand with a "shadow" to provide contrast with hands below it
///
/// @Param[in,out]  painter     Painter instance
/// @param[in]      rotation    Hand rotation in degrees [0,360]
/// @param[in]      handColor   Colour to draw hand in
/// @param[in]      shadowColor Colour of hand shadow
/// @param[in]      hand        Clock hand vector symbol
/// @param[in]      lolipop     Draw a "lolipop" style hand, with a large
///                             circle on the end
///
///////////////////////////////////////////////////////////////////////////////
static void drawHandWithShadow (QPainter&                                   painter,
                                double                                      rotation,
                                const QColor&                               handColor,
                                const QColor&                               shadowColor,
                                const utils::MultiPolygonF&                 hand,
                                const std::optional<utils::Magnifier<true>>&lens = std::nullopt)
    {
    QPainterStateGuard guard{ &painter };

    utils::MultiPolygonF handRotated = hand;

    handRotated.rotate (rotation);

    if (lens.has_value ())
        {
        handRotated = lens->magnify (handRotated);
        }

    painter.setPen (QPen{ shadowColor, 0.5 });
    painter.setBrush (handColor);
    drawHand (painter, { 0, 0 }, handRotated);
    }

AnalogClock::AnalogClock (QWidget* parent) :
    ClockWidget (parent)
    {
    m_font.setFamily ("Cascadia Mono");
    m_font.setPixelSize (14);

    connect (&utils::Battery::instance (),
             &utils::Battery::batteryPercentageChange,
              this,
              QOverload<>::of
                (&AnalogClock::update));
    }

void AnalogClock::setStyle (clockStyle newStyle)
    {
    const clockFace& face = FACES[newStyle];

    m_style = static_cast<style> (newStyle);

    if (face.window.cyclops.has_value ())
        {
        m_font.setPixelSize (
            face.window.cyclops->getPoly ().boundingRect ().height () - 20);
        }
    else
        {
        m_font.setPixelSize (14);
        }

    update ();
    }

void AnalogClock::paintEvent (QPaintEvent* event)
    {
    const QColor hourColor      = palette ().color (QPalette::Text);
    const QColor minuteColor    = palette ().color (QPalette::Text);
    const QColor secondsColor   = palette ().color (QPalette::Accent);
    const QColor shadowColor    = Qt::gray;

    int side = std::min (width(), height());

    QPainter painter{ this };
    // utils::draw::ScopedDebugDraw debugger{ *this };

    const clockFace& face = FACES[m_style];

    // Setup painting parameters
    painter.setRenderHint (QPainter::Antialiasing);
    painter.translate (width () / 2, height () / 2);
    painter.scale (side / 200.0, side / 200.0);

    QDateTime   dateTime    = control::FastClock::qDateTime ();
    QTime       time        = dateTime.time ();
    QDate       date        = dateTime.date ();
    int         battery     = utils::Battery::instance ().getBatteryPercent ().value_or (100);

    // Draw the day/date window
    painter.setFont (m_font);

    switch (face.window.style)
        {
        case dateWindowStyle::MONTH_DATE:
        case dateWindowStyle::DAY_DATE:
            {
            QString text;

            if (dateWindowStyle::MONTH_DATE == face.window.style)
                {
                text = utils::time::monthAbreviation (
                            static_cast<utils::time::month> (date.month () - 1));
                }
            else // (dateWindowStyle::DAY_DATE == face.window.style)
                {
                text = utils::time::dayOfWeekAbreviation (
                            static_cast<utils::time::dayOfTheWeek> (date.dayOfWeek () - 1));
                }

            m_font.setPixelSize (14);

            painter.drawText (18,
                              5,
                              QString{ "%1|%2 "}.arg (
                                  text, QString::number (date.day ())));

            painter.drawRoundedRect (QRect{ 15, -9, 55, 18 }, 3, 3);
            break;
            }
        case dateWindowStyle::DATE:
            {
            QRectF windowRect{ 49, -9, 30, 20 };


            if (face.window.cyclops.has_value ())
                {
                windowRect = face.window.cyclops->getPoly ().boundingRect ();

                windowRect.adjust (10, 8, -10, -8);
                }

            painter.setFont (m_font);
            painter.drawText (windowRect, Qt::AlignCenter, QString::number (date.day ()));
            painter.drawRect (windowRect);
            break;
            }
        case dateWindowStyle::NONE:
            {
            // no-op
            break;
            }
        }

    // Draw power indicator

    if (face.powerIndicator.has_value ())
        {
        const auto&         indicator = *face.powerIndicator;
        QPainterStateGuard  grd{ &painter };

        // drawPie() quirks:
        //  - 0 degrees is 3 O'clock
        //  - negative spanAngle is clockwise
        double              start   = indicator.start - 90;
        double              end     = indicator.end - 90;
        double              span    = start - end;

        painter.translate (indicator.pos);

        painter.drawPie (QRect
                            {
                            QPoint{ -indicator.radius,  indicator.radius },
                            QPoint{  indicator.radius, -indicator.radius}
                            },
                         utils::math::roundToInt (start * 16),
                         utils::math::roundToInt (span  * 16));

        double spacing  = (end - start) / (indicator.indices.size () - 1);
        double ang      = start;

        for (size_t ii = 0; ii < indicator.indices.size (); ang += spacing, ++ii)
            {
            QPainterStateGuard grd{ &painter };

            painter.rotate (ang);
            indicator.indices[ii]->paint (painter);
            }

        drawHandWithShadow (painter,
                            indicator.end - 90.0 -
                            (indicator.end - indicator.start) * battery / 100.0,
                            hourColor,
                            shadowColor,
                            indicator.hand);
        }


    // Draw hour hand

    drawHandWithShadow (painter,
                        30.0 * ((time.hour () % 12 +
                                 time.minute () / 60.0 +
                                 time.second () / 3600.0)),
                        hourColor,
                        shadowColor,
                        face.hands.hour,
                        face.window.cyclops);

    // Draw hour indices
    {
    QPainterStateGuard grd{ &painter };

    for (int i = 0; i < 12; ++i)
        {
        if (NULL != face.dial.indices[i])
            {
            face.dial.indices[i]->paint (painter);
            }

        painter.rotate (30.0);
        }

    }

    // Draw minute hand

    drawHandWithShadow (painter,
                        6.0 * (time.minute () + time.second () / 60.0),
                        minuteColor,
                        shadowColor,
                        face.hands.minute,
                        face.window.cyclops);

    // Draw seconds hands

    if (face.hands.second)
        {
        drawHandWithShadow (painter,
                            6.0 * time.second (),
                            secondsColor,
                            shadowColor,
                            face.hands.second.value (),
                            face.window.cyclops);
        }

    if (face.window.cyclops.has_value ())
        {
        painter.setPen (Qt::black);
        painter.setBrush (Qt::transparent);
        painter.drawPolygon (face.window.cyclops->getPoly ()[0].exteriorRing);
        }

    // Draw minute/second indices

    if (face.dial.subIndices)
        {
        painter.setPen (minuteColor);

        for (int j = 0; j < 60; ++j)
            {
            painter.drawLine (92, 0, 96, 0);
            painter.rotate (6.0);
            }
        }
    }

} // namespace ui::clock
