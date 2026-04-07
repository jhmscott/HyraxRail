/**
 * @file        trains/speedcontrol.cpp
 * @brief       Slider bar to control the speed of a locomotive
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <QApplication>
#include <QPainter>
#include <QFontDatabase>
#include <QStyleOption>
#include <QBoxLayout>
#include <QPushButton>
#include <QStyle>

#include <ui/common/pointedwidget.hpp>
#include <ui/common/utils.hpp>
#include <ui/trains/speedcontrol.hpp>

namespace ui::trains
{

class CenteredSlider : public QSlider
    {
public:
    CenteredSlider (Qt::Orientation ornt, QWidget* parent) :
        QSlider (ornt, parent)
        {}
protected:
    virtual void paintEvent (QPaintEvent* event) override
        {
        QPainter painter (this);
        painter.setRenderHint (QPainter::Antialiasing, true);

        QStyleOptionSlider option;
        option.initFrom (this);

        QRect   grooveRect;
        QRect   handleRect;
        QRect   progressRect;

        QPoint  handlePoint;
        int     handlePos   = 0;
        QRect   widgetRect  = rect ();

        if (orientation () == Qt::Horizontal)
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
        else
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

            int centerNudge = widgetRect.center ().x () - grooveRect.center ().x ();

            grooveRect.moveLeft (centerNudge);
            handleRect.moveLeft (centerNudge);
            grooveRect.adjust (0, 5, 0, -5);

            QPoint grooveCenter = grooveRect.center ();
            progressRect = grooveRect;

            handlePos = grooveRect.top () +
                        ((grooveRect.height () - handleRect.height ()) *
                         (-value () - minimum ()) / (maximum () - minimum ()));

            handlePoint = QPoint (grooveCenter.x (), handlePos);
            handleRect.moveTop (handlePoint.y ());
            handleRect.adjust (-5, 0, 5, 0);

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
        painter.drawRoundedRect (handleRect, 3, 3);

        if (0 != tickInterval ())
            {
            // Draw ticks
            const double    stepSpacing = grooveRect.height () /
                                          static_cast<double> (maximum () - minimum ());
            const int       minTick     = minimum () + ((tickInterval () - (minimum () % tickInterval ())) % tickInterval ());
            const int       maxTick     = maximum () - (maximum () % tickInterval ());

            painter.setPen (Qt::black);

            for (int ii = minimum (); ii < maximum (); ++ii)
                {
                const int posAbsolute   = static_cast<int> (round ((ii - minimum ()) * stepSpacing));
                const int width         = 0 == ii || minTick == ii || maxTick == ii ? 10 : 4;

                if (0 == (ii % tickInterval ()))
                    {
                    // if (0 == (tickPosition () & TicksLeft))
                        {
                        painter.drawLine (grooveRect.left () - 10,
                                          grooveRect.top () + posAbsolute,
                                          grooveRect.left () - 10 - width,
                                          grooveRect.top () + posAbsolute);
                        }


                    // if (0 == (tickPosition () & TicksRight))
                        {

                        painter.drawLine (grooveRect.right () + 10,
                                          grooveRect.top () + posAbsolute,
                                          grooveRect.right () + 10 + width,
                                          grooveRect.top () + posAbsolute);
                        }
                    }
                }
            }
        }
private:

    };

SpeedControlWidget::SpeedControlWidget (QWidget* parent) :
    QWidget (parent)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };

    m_stop = new common::PointedButton
                    {
                    QIcon{ ":/icons/misc/stop.svg" },
                    "",
                    this
                    };

    m_stop->setIconSize (QSize{ 50, 50 });
    common::makeFrameless (*m_stop);

    m_slider    = new CenteredSlider{ Qt::Vertical, this };
    m_label     = new QLabel{ "00", this};

    m_slider->setMinimumSize (100, 200);

    m_slider->setMinimum (-127);
    m_slider->setMaximum (127);
    m_slider->setValue (0);

    m_slider->setTickPosition (QSlider::TicksBothSides);
    m_slider->setTickInterval (20);
    m_slider->setPageStep (10);

    m_stop->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    int     id      = QFontDatabase::addApplicationFont (":/fonts/DSEG7-Classic/DSEG7Classic-Regular.ttf");
    QString family  = QFontDatabase::applicationFontFamilies (id).at (0);
    QFont   font{ family };

    font.setPointSize (30);

    m_label->setFont (font);

    layout->addWidget (m_slider,    0, Qt::AlignHCenter);
    layout->addWidget (m_stop,      0, Qt::AlignHCenter);
    layout->addWidget (m_label,     0, Qt::AlignHCenter);

    connect (m_slider,
            &QSlider::valueChanged,
             this,
            &SpeedControlWidget::onSliderChange);

    connect (m_stop,
            &QPushButton::released,
             this,
            &SpeedControlWidget::onStopButton);

    setLayout (layout);
    }

void SpeedControlWidget::setLocomotive (const layout::Locomotive& loco)
    {
    m_loco = loco;

    m_slider->setDisabled (false);
    m_stop  ->setDisabled (false);
    m_label ->setDisabled (false);
    }

void SpeedControlWidget::clear ()
    {
    m_loco = {};

    m_slider->setValue (0);

    m_slider->setDisabled (true);
    m_stop  ->setDisabled (true);
    m_label ->setDisabled (true);
    }
}
