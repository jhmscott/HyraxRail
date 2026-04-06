/**
 * @file        common/utils.hpp
 * @brief       Misc. UI helper functions
 * @author      Justin Scott
 * @date        2026-03-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <QComboBox>
#include <QLayout>
#include <QLayoutItem>
#include <QPushButton>

namespace ui::common
{

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

inline void makeFrameless (QWidget& wdgt)
    {
    wdgt.setStyleSheet (wdgt.styleSheet ()                  +
                        " "                                 +
                        wdgt.metaObject ()->className ()    +
                        " { border: none; background: transparent; }");
    }

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
}