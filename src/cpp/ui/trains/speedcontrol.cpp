/**
 * @file        trains/speedcontrol.cpp
 * @brief       Slider bar to control the speed of a locomotive
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <QApplication>
#include <QBoxLayout>
#include <QFontDatabase>
#include <QPainter>
#include <QPushButton>

#include <ui/common/centeredslider.hpp>
#include <ui/common/pointedwidget.hpp>
#include <ui/common/utils.hpp>
#include <ui/trains/speedcontrol.hpp>

namespace ui::trains
{


SpeedControlWidget::SpeedControlWidget (QWidget* parent) :
    QWidget (parent)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };

    m_stop = new common::PointedIconButton
                    {
                    "misc/stop",
                    this
                    };

    m_stop->setIconSize (QSize{ 50, 50 });
    common::makeFrameless (*m_stop);

    m_slider    = new common::CenteredSlider{ Qt::Vertical, this };
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
            &SpeedControlWidget::onSliderChange,
             Qt::QueuedConnection);

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

} // namespace ui::trains
