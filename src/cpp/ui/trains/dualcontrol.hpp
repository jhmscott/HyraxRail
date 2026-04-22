/**
 * @file        trains/dualcontrol.hpp
 * @brief       Panel to control up to two locos at once, if enough space
 *              is available. If not, behaves like a single loco controller
 * @author      Justin Scott
 * @date        2026-01-31
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/manager.hpp>

#include <ui/trains/lococontrolpanel.hpp>

#include <QWidget>

namespace ui::trains
{
class DualControlWidget : public QWidget
    {
public:
    DualControlWidget (control::ControllerManager* controllers, QWidget* parent);

private:
    LocoControlPanel* m_left;
    LocoControlPanel* m_right;

    virtual void resizeEvent (QResizeEvent* event) override;
    };
}