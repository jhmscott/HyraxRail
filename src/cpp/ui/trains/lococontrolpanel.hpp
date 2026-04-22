/**
 * @file        trains/lococontrolpanel.hpp
 * @brief       Control panel for a single locomotive
 * @author      Justin Scott
 * @date        2026-01-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/manager.hpp>

#include <ui/trains/speedcontrol.hpp>
#include <ui/trains/functionpanel.hpp>
#include <ui/controllerinfo.hpp>

#include <QComboBox>

namespace ui::trains
{

class LocoControlPanel : public QWidget
    {
    Q_OBJECT
public:
    explicit LocoControlPanel (control::ControllerManager* controllers, vAlignment align, QWidget* parent);

private:
    layout::Locomotive      m_currentLoco;
    SpeedControlWidget*     m_speed;
    FunctionPanel*          m_functions;
    QComboBox*              m_locos;
    ControllerInfo*         m_controllerInfo;

private slots:
    void add (control::ControllerBase& controller);

    void onLocoChange (int idx);

    void locoDeleted ();

    };

}