/**
 * @file        common/tiereddropdown.hpp
 * @brief       ComboBox with two tiers of items, parent and child
 * @author      Justin Scott
 * @date        2026-06-28
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/common/schemeicon.hpp>

namespace ui::common
{

///////////////////////////////////////////////////////////////////////////////
/// ComboBox with two tiers of items, parent and child
///
///////////////////////////////////////////////////////////////////////////////
class TieredDropdown : public SchemeComboBox
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit TieredDropdown (QWidget* parent = NULL);

    ///////////////////////////////////////////////////////////////////////////////
    /// Add a parent item
    ///
    /// @param[in]  text        Item text
    /// @param[in]  icon        (optional) Item icon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void addParentItem (const QString& text, const utils::resources::Icon& icon = {});

    ///////////////////////////////////////////////////////////////////////////////
    /// Add a child icon
    ///
    /// @param[in]  text        Item text
    /// @param[in]  data        (optional) Item uyser data (user role)
    /// @param[in]  icon        (optional) Item icon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void addChildItem (const QString&                   text,
                       const QVariant&                  data = {},
                       const utils::resources::Icon&    icon = {});

    };

} // namespace ui::common
