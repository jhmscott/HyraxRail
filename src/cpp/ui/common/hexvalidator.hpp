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

//////////////////////////////////////////////////////////////////////////////
/// Integer validation for hex formatted strings
///
//////////////////////////////////////////////////////////////////////////////
class HexValidator : public QValidator
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  min     Minimum acceptable value
    /// @param[in]  max     Maximum acceptable value
    /// @param[in]  parent  Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    HexValidator (uint min, uint max, QObject* parent);

    //////////////////////////////////////////////////////////////////////////////
    /// Validate the input
    ///
    /// @param[in]  input   Input to validate
    /// @param[out] pos     Position of first character with input
    ///
    /// @return     Validation state
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual QValidator::State validate (QString& input, int& pos) const override;

private:
    uint m_min; ///< Minimum acceptable value
    uint m_max; ///< Maximum acceptable value
    };

} // namespace ui::common