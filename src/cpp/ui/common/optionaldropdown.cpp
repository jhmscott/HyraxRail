/**
 * @file        common/optionaldropdown.cpp
 * @brief       DropDown/ComboBox widget. When only one entry exists in the dropdown,
 *              it will be replaced with a static text field
 * @author      Justin Scott
 * @date        2026-03-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/optionaldropdown.hpp>

namespace ui::common
{
OptionalDropdown::OptionalDropdown (QWidget* parent) :
    QWidget (parent)
    {
    QHBoxLayout* layout = new QHBoxLayout{ this };

    m_singleItem = new QLabel{ this };
    m_dropdown = new QComboBox{ this };

    m_dropdown->setVisible (false);

    layout->addWidget (m_singleItem);
    layout->addWidget (m_dropdown);

    connect (m_dropdown,
             &QComboBox::currentIndexChanged,
             this,
             &OptionalDropdown::currentIndexChanged);

    setLayout (layout);
    }


void OptionalDropdown::insertItem (int idx, const QString& text, const QVariant& userData)
    {
    m_dropdown->insertItem (idx, text, userData);
    refreshActiveWidget ();
    }


void OptionalDropdown::clear ()
    {
    m_dropdown->clear ();
    refreshActiveWidget ();
    }

void OptionalDropdown::refreshActiveWidget ()
    {
    if (count () >= 0)
        {
        if (0 == count ())
            {
            m_singleItem->setText ("");
            }
        else
            {
            m_singleItem->setText (m_dropdown->itemText (0));
            }

        m_dropdown->setVisible (false);
        m_singleItem->setVisible (true);
        }
    else
        {
        m_dropdown->setVisible (true);
        m_singleItem->setVisible (false);
        }
    }


}
