/**
 * @file        common/schemeicon.cpp
 * @brief       Common widget types that automatically update
 *              their icons whenever the color scheme changes
 * @author      Justin Scott
 * @date        2026-05-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/schemeicon.hpp>

#include <QApplication>
#include <QStyleHints>


namespace ui::common
{

SchemeIconButton::SchemeIconButton (const utils::resources::Icon& icon, QWidget* parent) :
    QPushButton (icon.toIcon (qApp->styleHints ()->colorScheme ()), "", parent),
    m_icon (icon)
    {
    connect (qApp->styleHints (),
            &QStyleHints::colorSchemeChanged,
             this,
            &SchemeIconButton::colorSchemeChanged);
    }

void SchemeIconButton::setIcon (const utils::resources::Icon& icon)
    {
    m_icon = icon;
    QPushButton::setIcon (m_icon.toIcon (qApp->styleHints ()->colorScheme ()));
    }

void SchemeIconButton::colorSchemeChanged (Qt::ColorScheme scheme)
    {
    QPushButton::setIcon (m_icon.toIcon (scheme));
    }


SchemeTabWidget::SchemeTabWidget (QWidget* parent) :
    QTabWidget (parent)
    {
    connect (qApp->styleHints (),
            &QStyleHints::colorSchemeChanged,
             this,
            &SchemeTabWidget::colorSchemeChanged);
    }

int SchemeTabWidget::addTab (QWidget* widget, const utils::resources::Icon& icon)
    {
    int tabIdx = QTabWidget::addTab (widget,
                                     icon.toIcon (qApp->
                                                    styleHints ()->
                                                        colorScheme ()),
                                     "");

    m_icons.resize (tabIdx + 1);

    m_icons[tabIdx] = icon;

    return tabIdx;
    }

void SchemeTabWidget::tabRemoved (int index)
    {
    m_icons.erase (m_icons.begin () + index);
    }


void SchemeTabWidget::colorSchemeChanged (Qt::ColorScheme scheme)
    {
    for (int ii = 0; ii < m_icons.size (); ++ii)
        {
        setTabIcon (ii, m_icons[ii].toIcon (scheme));
        }
    }

SchemeDialog::SchemeDialog (QWidget* parent) :
    QDialog (parent)
    {
    connect (qApp->styleHints (),
            &QStyleHints::colorSchemeChanged,
             this,
            &SchemeDialog::colorSchemeChanged);
    }

void SchemeDialog::setWindowIcon (const utils::resources::Icon& icon)
    {
    QDialog::setWindowIcon (icon.toIcon (qApp->
                                            styleHints ()->
                                                colorScheme ()));
    m_icon = icon;
    }



void SchemeDialog::colorSchemeChanged (Qt::ColorScheme scheme)
    {
    if (m_icon)
        {
        QDialog::setWindowIcon (m_icon.toIcon (scheme));
        }
    }

SchemeComboBox::SchemeComboBox (QWidget* parent) :
    QComboBox (parent)
    {
    connect (qApp->styleHints (),
            &QStyleHints::colorSchemeChanged,
             this,
            &SchemeComboBox::colorSchemeChanged);

    }

void SchemeComboBox::addItem (const utils::resources::Icon& icon,
                              const QString&                text,
                              const QVariant&               userData)
    {
    int idx = count ();

    QComboBox::addItem (icon.toIcon (qApp->
                                        styleHints ()->
                                            colorScheme ()),
                        text,
                        userData);

    // Store the source icon in user data, so we can update the scheme on the fly
    setItemData (idx, QVariant::fromValue (icon), schemeIcon);
    }

void SchemeComboBox::colorSchemeChanged (Qt::ColorScheme scheme)
    {
    for (int ii = 0; ii < count (); ++ii)
        {
        utils::resources::Icon icon =
            itemData (ii, schemeIcon).value<utils::resources::Icon> ();

        setItemIcon (ii, icon.toIcon (scheme));
        }
    }

} // namespace ui::common
