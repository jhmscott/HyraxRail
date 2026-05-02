/**
 * @file        common/optionaldropdown.hpp
 * @brief       DropDown/ComboBox widget. When only one entry exists in the dropdown,
 *              it will be replaced with a static text field
 * @author      Justin Scott
 * @date        2026-02-06
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <ui/common/utils.hpp>

#include <QboxLayout>
#include <QComboBox>
#include <QLabel>

namespace ui::common
{


//////////////////////////////////////////////////////////////////////////////
/// Dropdown/combobox widget which reverts to a static text label if only one
/// element is added
///
//////////////////////////////////////////////////////////////////////////////
class OptionalDropdown : public QWidget
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    explicit OptionalDropdown (QWidget* parent);

    //////////////////////////////////////////////////////////////////////////////
    /// Add an item to the dropdown at an index
    ///
    /// @param[in]  idx         Index to add element at
    /// @param[in]  text        Text to add to dropdown
    /// @param[in]  userData    (optional) User data for added item
    ///
    //////////////////////////////////////////////////////////////////////////////
    void insertItem (int idx, const QString& text, const QVariant& userData = QVariant ());

    //////////////////////////////////////////////////////////////////////////////
    /// Add an item at the end of the dropdown
    ///
    /// @param[in]  text        Text to add to dropdown
    /// @param[in]  userData    (optional) User data for added item
    ///
    //////////////////////////////////////////////////////////////////////////////
    void addItem (const QString& text, const QVariant& userData = QVariant ())
        { insertItem (count (), text, userData); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the index of the currently select item
    ///
    /// @return     Selected index
    ///
    //////////////////////////////////////////////////////////////////////////////
    int currentIndex () const { return m_dropdown->currentIndex (); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the text for the currently selected item
    ///
    /// @return     Selected item text
    ///
    //////////////////////////////////////////////////////////////////////////////
    QString currentText () const { return m_dropdown->currentText (); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the data for the currently selected item
    ///
    /// @param[in]  role        (optional) Data role. Defaults to user data
    ///
    /// @return     Selected item data
    ///
    //////////////////////////////////////////////////////////////////////////////
    QVariant currentData (int role = Qt::UserRole) const
        { return m_dropdown->currentData (role); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the text of a given item
    ///
    /// @param[in]  index       Item indes
    ///
    /// @return     Text of item at index
    ///
    //////////////////////////////////////////////////////////////////////////////
    QString itemText (int index) const { return m_dropdown->itemText (index); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the data for a given item
    ///
    /// @param[in]  index       Item index
    /// @param[in]  role        (optional) Data role. Defalts to user data
    ///
    /// @return     Data of item at index
    ///
    //////////////////////////////////////////////////////////////////////////////
    QVariant itemData (int index, int role = Qt::UserRole) const
        { return m_dropdown->itemData (index, role); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the number of items in the dropdown
    ///
    /// @return     Number of items in the dropdown
    ///
    //////////////////////////////////////////////////////////////////////////////
    int count () const { return m_dropdown->count (); }

    //////////////////////////////////////////////////////////////////////////////
    /// Remove all items from the dropdown
    ///
    //////////////////////////////////////////////////////////////////////////////
    void clear ();

    //////////////////////////////////////////////////////////////////////////////
    /// Set currently selected item to the first element with a given user data
    /// value
    ///
    /// @tparam     T       User data type
    ///
    /// @param[in]  ud      User data value
    ///
    //////////////////////////////////////////////////////////////////////////////
    template<class T>
    void setIndexByUserData (const T& ud)
        { setComboBoxIndexByUserData (*m_dropdown, ud); }

signals:
    //////////////////////////////////////////////////////////////////////////////
    /// Signals the selected item as changed
    ///
    /// @param[in]  index       New index
    ///
    //////////////////////////////////////////////////////////////////////////////
    void currentIndexChanged (int index);

private:
    QLabel*     m_singleItem;   ///< Static label to use when only one item is available
    QComboBox*  m_dropdown;     ///< Drop down to use when multiple items are added

    //////////////////////////////////////////////////////////////////////////////
    /// Update which of the two widgets is displayed
    ///
    //////////////////////////////////////////////////////////////////////////////
    void refreshActiveWidget ();
    };

} // namespace ui::common
