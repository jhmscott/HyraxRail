/**
 * @file        trains/functionpanel.hpp
 * @brief       Panel containing a grid of function buttons for
 *              a given locomotive
 * @author      Justin Scott
 * @date        2026-01-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <layout/locomotive.hpp>

#include <QBoxLayout>
#include <QPushButton>
#include <QWidget>

#include <common.hpp>

namespace ui::trains
{

class FunctionButton;

class FunctionPanel : public QWidget
    {
public:
    FunctionPanel (vAlignment align, QWidget* parent);

    void setLocomotive (const layout::Locomotive& loco);

    void clear ();
private:
    layout::Locomotive              m_loco;
    std::array<FunctionButton*, 32> m_btns;

    void buttonPressed (uint8_t id, bool enabled)
        { m_loco.setFunc (id, enabled); }
    };

}