/**
 * @file        config/helpgroup.hpp
 * @brief       Group box for the application help and about pages
 * @author      Justin Scott
 * @date        2026-03-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <QGroupBox>

namespace ui::config
{

class HelpGroup : public QGroupBox
    {
    Q_OBJECT
public:
    explicit HelpGroup (QWidget* parent);
private:

    void about ();

    void help ();

    void license ();

    void credits ();
    };
}