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
/// @remarks    Three types of index are used:<BR>
/// <ul>
///     <li>Absolute index : The index of within the combo box</li>
///     <li>Parent index : The index within the list of parent items</li>
///     <li>Child index : The index of a child amongst its siblings.
///         Index of parent is needed to get absolute index</li>
/// </ul>
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
    /// @param[in]  data        (optional) Item user data (user role)
    /// @param[in]  icon        (optional) Item icon
    /// @param[in]  parent      (optional) Index of parent to add this item
    ///                         Defaults to the last parent in the list
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void addChildItem (const QString&                   text,
                       const QVariant&                  data    = {},
                       const utils::resources::Icon&    icon    = {},
                       const int                        parent  = -1);

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the text for a parent item
    ///
    /// @param[in]  idx     Parent item idx
    /// @param[in]  text    New item text
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setParentItemText (int idx, const QString& text);

    ///////////////////////////////////////////////////////////////////////////////
    /// Is a given item a parent item?
    ///
    /// @param[in]  idx     Absolute index
    ///
    /// @return     true for parent item
    ///             false for child item
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool isParentItem (int idx) const;
    };

} // namespace ui::common
