/**
 * @file        common/utils.hpp
 * @brief       Misc. UI helper functions
 * @author      Justin Scott
 * @date        2026-03-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <QApplication>
#include <QComboBox>
#include <QLayout>
#include <QLayoutItem>
#include <QPushButton>
#include <QStyleHints>

namespace ui::common
{
///////////////////////////////////////////////////////////////////////////////
/// Set the current item in a combobox to the first item with a given user data
///
/// @tparam         T       Type of user data
///
/// @param[in,out]  cb      Combobox to set current item for
/// @param[in]      ud      User data
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
void setComboBoxIndexByUserData (QComboBox& cb, const T& ud)
    {
    for (int ii = 0; ii < cb.count (); ++ii)
        {
        if (cb.itemData (ii).value<T> () == ud)
            {
            cb.setCurrentIndex (ii);
            break;
            }
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Remove the first item in a combobox with a given user data
///
/// @tparam         T       Type of user data
///
/// @param[in,out]  cb      Combobox to remove item from
/// @param[in]      ud      User data
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
void removeComboBoxItemByUserData (QComboBox& cb, const T& ud)
    {
    for (int ii = 0; ii < cb.count (); ++ii)
        {
        bool check;
        if constexpr (std::is_pointer_v<T>)
            {
            check = *cb.itemData (ii).value<T> () == *ud;
            }
        else
            {
            check = cb.itemData (ii).value<T> () == ud;
            }

        if (check)
            {
            cb.removeItem (ii);

            if constexpr (std::is_pointer_v<T>)
                {
                delete cb.itemData (ii).value<T> ();
                }

            break;
            }
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Set the current item in a combobox to the first item with a given text
///
/// @param[in,out]  cb      Combobox to set current item for
/// @param[in]      text    Text of item to find and set
///
///////////////////////////////////////////////////////////////////////////////
inline void setComboBoxIndexByText (QComboBox& cb, const QString& text)
    {
    for (int ii = 0; ii < cb.count (); ++ii)
        {
        if (cb.itemText (ii) == text)
            {
            cb.setCurrentIndex (ii);
            break;
            }
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Setting a stylesheet results in the widget not automatically updating its
/// colors when the color scheme changes. Calling this function fixes this
/// by refreshing the style sheet when the color scheme changes. Call this
/// if you set the stylesheet for a widget
///
/// @param[in,out]  wdgt        Widget stylesheet was added to
///
///////////////////////////////////////////////////////////////////////////////
inline void refreshStyleSheetOnColorSchemeChange (QWidget& wdgt)
    {
    wdgt.connect (qApp->styleHints (),
                 &QStyleHints::colorSchemeChanged,
                 &wdgt,
                  [&wdgt] (Qt::ColorScheme scheme) -> void
                  { wdgt.setStyleSheet (wdgt.styleSheet ()); },
                  // This needs to execute AFTER all the other color scheme
                  // handlers, and queued connection ensures that
                  Qt::QueuedConnection);
    }

///////////////////////////////////////////////////////////////////////////////
/// Remove the frame from a widget
///
/// @param[in,out]  wdgt    Widget to remove frame from
///
///////////////////////////////////////////////////////////////////////////////
inline void makeFrameless (QWidget& wdgt)
    {
    const QMetaObject*  metaobj     = wdgt.metaObject ();
    QString             classname;

    // Follow up the object inheritance tree to the first standard QT widget
    do
        {
        classname   = metaobj->className ();
        metaobj     = metaobj->superClass ();
        } while ('Q' != classname[0] && NULL != metaobj);

    wdgt.setStyleSheet (wdgt.styleSheet ()                  +
                        " "                                 +
                        classname                           +
                        " { border: none; background: transparent; }");

    // We are adding a stylesheet, so we need to call this to not
    // break automatic colour scheme handling
    refreshStyleSheetOnColorSchemeChange (wdgt);
    }

///////////////////////////////////////////////////////////////////////////////
/// Remove a widget from a layout
///
/// @param[in,out]  layout  Layout to remove widget
/// @param[in]      nn      Index of widget to remove
///
///////////////////////////////////////////////////////////////////////////////
inline void removeWidgetFromLayout (QLayout& layout, uint nn)
    {
    QLayoutItem* item = layout.takeAt (nn);

    if (NULL != item)
        {
        QWidget* widget = item->widget ();

        delete widget;
        delete item;
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Remove all the items from a layout
///
/// @param[in,out]  layout      Layout to clear
///
///////////////////////////////////////////////////////////////////////////////
inline void clearLayout (QLayout& layout)
    {
    QLayoutItem*    item;
    QLayout*        childLayout;
    QWidget*        widget;

    while (NULL != (item = layout.takeAt (0)))
        {
        if (NULL != (widget = item->widget ()))
            {
            delete widget;
            }
        else if (NULL != (childLayout = item->layout ()))
            {
            clearLayout (*childLayout);
            }

        delete item;
        }
    }

}