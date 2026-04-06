/**
 * @file        common/separator.hpp
 * @brief       Simple line to visually seprate sections/widgets
 * @author      Justin Scott
 * @date        2026-03-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QFrame>


namespace ui::common
{

class Separator : public QFrame
    {
public:
    explicit Separator (QWidget* parent) :
        QFrame (parent)
        {
        setFrameShape (QFrame::HLine);
        setFrameShadow (QFrame::Sunken);
        }

private:

    };

}