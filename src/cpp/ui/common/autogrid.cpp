/**
 * @file        common/autogrid.cpp
 * @brief       Grid layout that automatically expands or contracts as
 *              elements are add/removed
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/autogrid.hpp>

namespace ui::common
{
QLayoutItem* AutoGridLayout::itemAt (int idx) const
    {
    QLayoutItem* item = NULL;

    if (idx >= 0 && idx < count ())
        {
        item = m_items.at (idx);
        }

    return item;
    }

QLayoutItem* AutoGridLayout::takeAt (int idx)
    {
    QLayoutItem* item = itemAt (idx);

    if (idx >= 0 && idx < count ())
        {
        m_items.erase (m_items.begin () + idx);
        }

    return item;
    }

void AutoGridLayout::setGeometry (const QRect& rect)
    {
    QLayout::setGeometry (rect);

    int     nn = 0;
    int     top = rect.topLeft ().y ();
    int     left = rect.topLeft ().x ();
    int     width = rect.width ();
    int     height = rect.height ();
    int     numShown = 0;

    for (size_t ii = 0; ii < m_items.size (); ++ii)
        {
        if (isItemShown (ii))
            {
            ++numShown;
            }
        }

    for (size_t ii = 0; ii < m_items.size (); ++ii)
        {
        if (isItemShown (ii))
            {
            int x;
            int y;
            int cellWidth;
            int cellHeight;

            if (expand::ROW_FIRST == m_expand)
                {
                int numRows = utils::math::divCeil (numShown, m_numColumns);

                cellWidth = (width / m_numColumns);
                cellHeight = (height / numRows);

                x = (nn % m_numColumns) * cellWidth;
                y = (nn / m_numColumns) * cellHeight;
                }
            else // (expand::COLUMN_FIRST == m_expand)
                {
                int numColumns = utils::math::divCeil (numShown, m_numRows);

                cellWidth = (width / numColumns);
                cellHeight = (height / m_numRows);

                x = (nn / m_numRows) * cellWidth;
                y = (nn % m_numRows) * cellHeight;
                }

            m_items[ii]->setGeometry (QRect{ x, y, cellWidth, cellHeight });

            ++nn;
            }
        }
    }
QSize AutoGridLayout::getSizeInternal (QSize (QLayoutItem::* getSize)() const) const
    {
    QSize size{ 0, 0 };
    size_t nn = 0;

    for (size_t ii = 0; ii < m_items.size (); ++ii)
        {
        if (isItemShown (ii))
            {
            if (expand::ROW_FIRST == m_expand)
                {
                if (0 == nn / m_numColumns)
                    {
                    size.rwidth () += (m_items[ii]->*getSize) ().width () + spacing ();
                    }

                if (0 == nn % m_numColumns)
                    {
                    size.rheight () += (m_items[ii]->*getSize) ().height () + spacing ();
                    }
                }
            else // (expand::COLUMN_FIRST == m_expand)
                {
                if (0 == nn / m_numRows)
                    {
                    size.rheight () += (m_items[ii]->*getSize) ().height () + spacing ();
                    }

                if (0 == nn % m_numRows)
                    {
                    size.rwidth () += (m_items[ii]->*getSize) ().width () + spacing ();
                    }
                }

            ++nn;
            }
        }

    return size;
    }

bool AutoGridLayout::isItemShown (size_t idx) const
    {
    bool        shown = false;
    QWidget* widget;

    if (NULL != (widget = m_items[idx]->widget ()))
        {
        shown = widget->isVisible ();
        }

    return shown;
    }

}