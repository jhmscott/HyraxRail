/**
 * @file        common/hexvalidator.hpp
 * @brief       Integer validator that expects the field to be hexadecimal
 *              format instead of decimal
 * @author      Justin Scott
 * @date        2026-02-06
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <Qvalidator>

namespace ui::common
{

class HexValidator : public QValidator
    {
public:
    HexValidator (int min, int max, QObject* parent);

    virtual QValidator::State validate (QString& input, int& pos) const override;

private:
    int m_min;
    int m_max;
    };

}