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


class OptionalDropdown : public QWidget
    {
    Q_OBJECT
public:
    explicit OptionalDropdown (QWidget* parent);

    void insertItem (int idx, const QString& text, const QVariant& userData = QVariant ());

    void addItem (const QString& text, const QVariant& userData = QVariant ())
        { insertItem (count (), text, userData); }

    int currentIndex () const { return m_dropdown->currentIndex (); }
    QString currentText () const { return m_dropdown->currentText (); }
    QVariant currentData (int role = Qt::UserRole) const
        { return m_dropdown->currentData (role); }

    QString itemText (int index) const { return m_dropdown->itemText (index); }
    QVariant itemData (int index, int role = Qt::UserRole) const
        { return m_dropdown->itemData (index, role); }

    int count () const { return m_dropdown->count (); }

    void clear ();

    template<class T>
    void setIndexByUserData (const T& ud)
        { setComboBoxIndexByUserData (*m_dropdown, ud); }

signals:
    void currentIndexChanged (int index);

private:
    QLabel* m_singleItem;
    QComboBox* m_dropdown;

    void refreshActiveWidget ();
    };
}
