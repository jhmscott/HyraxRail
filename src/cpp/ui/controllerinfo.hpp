/**
 * @file        controllerinfo.hpp
 * @brief       Widget to display the state of a controller
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include <control/controllers/base.hpp>

#include <ui/common/icontoggle.hpp>

namespace ui
{

class ControllerInfo : public QWidget
    {
    Q_OBJECT
public:
    ControllerInfo (control::ControllerBase* controller, QWidget* parent, bool includeDelete);

    void clear ();

    void setController (control::ControllerBase& controller);

signals:
    void deleted (control::ControllerBase& controller);
private:
    control::ControllerBase*    m_controller;
    QPushButton*                m_connectionIcon;
    common::IconToggle*         m_stop;
    QPushButton*                m_settings;
    QLabel*                     m_nameLabel;

    void refreshHealthIcon ()
        {
        setHealth (NULL == m_controller ?
                   control::ConnectionWorkerThread::health
                   {
                        control::ConnectionWorkerThread::HEALTH_UNAVAILABLE,
                        std::chrono::milliseconds { 0 }
                    } :
                    m_controller->getConnctionHealth ());
        }

    void eStop () { m_controller->eStop (m_stop->isChecked ()); }

    void setHealth (control::ConnectionWorkerThread::health health);

    void onConfig ();

    void onDelete ();
    };

}