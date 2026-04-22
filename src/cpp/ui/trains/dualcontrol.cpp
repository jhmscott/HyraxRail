/**
 * @file        trains/dualcontrol.cpp
 * @brief       Panel to control up to two locos at once, if enough space
 *              is available. If not, behaves like a single loco controller
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/trains/dualcontrol.hpp>

#include <QBoxLayout>
#include <qevent.h>

namespace ui::trains
{
DualControlWidget::DualControlWidget (control::ControllerManager* controllers, QWidget* parent) :
    QWidget (parent)
    {
    QHBoxLayout* layout = new QHBoxLayout{ this };

    m_left  = new LocoControlPanel{ controllers, vAlignment::LEFT, this };
    m_right = new LocoControlPanel{ controllers, vAlignment::RIGHT, this };

    m_right->setVisible (false);

    layout->addWidget (m_left);
    layout->addWidget (m_right);

    setLayout (layout);
    }


void DualControlWidget::resizeEvent (QResizeEvent* event)
    {
    int spacing = layout ()->spacing ();
    int minWidth = m_left->sizeHint ().width () +
        m_right->sizeHint ().width () +
        spacing * 4;
    int availableWidth = event->size ().width ();

    if (availableWidth > minWidth)
        {
        m_right->setVisible (true);
        }
    else
        {
        m_right->setVisible (false);
        }

    QWidget::resizeEvent (event);
    }


}