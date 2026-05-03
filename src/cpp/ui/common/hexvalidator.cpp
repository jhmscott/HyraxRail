/**
 * @file        common/hexvalidator.cpp
 * @brief       Integer validator that expects the field to be hexadecimal
 *              format instead of decimal
 * @author      Justin Scott
 * @date        2026-03-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/common/autogrid.hpp>
#include "hexvalidator.hpp"

namespace ui::common
{
HexValidator::HexValidator (uint min, uint max, QObject* parent) :
    QValidator (parent),
    m_min (min),
    m_max (max)
    {}


QValidator::State ui::common::HexValidator::validate (QString & input, int& pos) const
    {
    QValidator::State   state;
    bool                ok = false;

    uint val = input.toInt (&ok, 16);

    if (not ok)
        {
        if (input.isEmpty ())
            {
            state = QValidator::Acceptable;
            }
        else
            {
            state = QValidator::Invalid;
            }
        }
    else if (val < m_min || val > m_max)
        {
        state = QValidator::Invalid;
        }
    else
        {
        state = QValidator::Acceptable;
        }

    return state;
    }
}