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
AutoGridLayout::AutoGridLayout (expand exp, int numRowsOrColumns, QWidget* parent) :
    QLayout (parent),
    m_expand (exp),
    m_numRowsOrColumns (numRowsOrColumns)
    {
    if (EXPAND == m_numColumns && expand::ROW_FIRST != exp)
        {
        throw std::domain_error ("EXPAND only supported with ROW_FIRST");
        }
    }

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

    int     nn          = 0;
    int     top         = rect.topLeft ().y ();
    int     left        = rect.topLeft ().x ();
    int     width       = rect.width ();
    int     height      = rect.height ();
    int     numShown    = 0;

    if (EXPAND == m_numRowsOrColumns)
        {
        int     availableSpace;
        size_t  start       = 0;
        int     usedSpace   = 0;
        int     maxCellSize = 0;
        int     x           = left;
        int     y           = top;
        int     margin;

        int     marginLeft;
        int     marginRight;
        int     marginTop;
        int     marginBot;

        getContentsMargins (&marginLeft,
                            &marginTop,
                            &marginRight,
                            &marginBot);


        if (expand::ROW_FIRST == m_expand)
            {
            availableSpace  = width;
            margin          = marginLeft + marginRight;
            }
        else
            {
            availableSpace  = height;
            margin          = marginTop + marginBot;
            }

        auto fillRowOrColumn = [&] (size_t start, size_t end)
            {
            int cellSpacing = availableSpace / numShown;

            for (size_t jj = start; jj < end; ++jj)
                {
                if (isItemShown (jj))
                    {
                    int cellWidth;
                    int cellHeight;

                    if (expand::ROW_FIRST == m_expand)
                        {
                        cellWidth = cellSpacing;
                        cellHeight = maxCellSize;
                        }
                    else
                        {
                        cellWidth = maxCellSize;
                        cellHeight = cellSpacing;
                        }

                    m_items[jj]->setGeometry (QRect{ x, y, cellWidth, cellHeight });

                    if (expand::ROW_FIRST == m_expand)
                        {
                        x += cellWidth;
                        }
                    else
                        {
                        y += cellHeight;
                        }
                    }
                }

            };

        for (size_t ii = 0; ii < m_items.size (); ++ii)
            {
            if (isItemShown (ii))
                {
                int itemSpace;

                if (expand::ROW_FIRST == m_expand)
                    {
                    itemSpace = m_items[ii]->minimumSize ().width ();
                    }
                else
                    {
                    itemSpace = m_items[ii]->minimumSize ().height ();
                    }

                itemSpace += margin;

                if (availableSpace > (usedSpace + itemSpace))
                    {
                    usedSpace += itemSpace;
                    ++numShown;
                    }
                else
                    {
                    fillRowOrColumn (start, ii);

                    if (expand::ROW_FIRST == m_expand)
                        {
                        x  = left;
                        y += maxCellSize + marginTop + marginBot;
                        }
                    else
                        {
                        x += maxCellSize + marginLeft + marginRight;
                        y  = top;
                        }

                    start       = ii;
                    maxCellSize = 0;
                    numShown    = 1;
                    usedSpace   = 0;
                    }


                if (expand::ROW_FIRST == m_expand)
                    {
                    maxCellSize = std::max (maxCellSize, m_items[ii]->minimumSize ().height ());
                    }
                else
                    {
                    maxCellSize = std::max (maxCellSize, m_items[ii]->minimumSize ().width ());
                    }
                }
            }

        if (numShown > 0)
            {
            fillRowOrColumn (start, m_items.size ());
            }
        }
    else
        {
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

                    cellWidth   = (width / m_numColumns);
                    cellHeight  = (height / numRows);

                    x = (nn % m_numColumns) * cellWidth;
                    y = (nn / m_numColumns) * cellHeight;
                    }
                else // (expand::COLUMN_FIRST == m_expand)
                    {
                    int numColumns = utils::math::divCeil (numShown, m_numRows);

                    cellWidth   = (width / numColumns);
                    cellHeight  = (height / m_numRows);

                    x = (nn / m_numRows) * cellWidth;
                    y = (nn % m_numRows) * cellHeight;
                    }

                m_items[ii]->setGeometry (QRect{ x, y, cellWidth, cellHeight });

                ++nn;
                }
            }
        }
    }

int AutoGridLayout::minimumHeightForWidth (int width) const
    {
    int     availableSpace;
    int     usedSpace = 0;
    int     maxCellSize = 0;
    int     margin;
    int     numShown = 0;
    int     x = 0;
    int     y = 0;

    int     marginLeft;
    int     marginRight;
    int     marginTop;
    int     marginBot;

    getContentsMargins (&marginLeft,
                        &marginTop,
                        &marginRight,
                        &marginBot);

    availableSpace  = width;
    margin          = marginLeft + marginRight;

    for (size_t ii = 0; ii < m_items.size (); ++ii)
        {
        if (isItemShown (ii))
            {
            int itemSpace;

            itemSpace = m_items[ii]->minimumSize ().width ();

            itemSpace += margin;

            if (availableSpace > (usedSpace + itemSpace))
                {
                usedSpace += itemSpace;
                ++numShown;
                }
            else
                {
                x  = 0;
                y += maxCellSize + marginTop + marginBot;

                maxCellSize = 0;
                numShown    = 1;
                usedSpace   = 0;
                }


            maxCellSize = std::max (maxCellSize, m_items[ii]->minimumSize ().height ());
            }
        }

    if (numShown > 0)
        {
        y += maxCellSize;
        }

    return y;
    }

QSize AutoGridLayout::getSizeInternal (QSize (QLayoutItem::* getSize)() const) const
    {
    QSize size{ 0, 0 };

    if (EXPAND == m_numRowsOrColumns)
        {
        for (size_t ii = 0; ii < m_items.size (); ++ii)
            {
            if (isItemShown (ii))
                {
                QSize itemSize = (m_items[ii]->*getSize) ();
                if (expand::ROW_FIRST == m_expand)
                    {
                    size.rwidth ()   = std::max (size.width (),  itemSize.width ());
                    size.rheight ()  = std::max (size.height (), itemSize.height ());
                    }
                else
                    {
                    size.rwidth ()  = std::max (size.width (), itemSize.width ());
                    size.rheight () = std::max (size.height (), itemSize.height ());
                    }
                }
            }
        }
    else
        {
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