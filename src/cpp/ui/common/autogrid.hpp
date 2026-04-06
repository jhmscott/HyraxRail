/**
 * @file        common/autogrid.hpp
 * @brief       Grid layout that automatically expands or contracts as
 *              elements are add/removed
 * @author      Justin Scott
 * @date        2026-02-01
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QLayout>
#include <QWidget>

#include <utils/math.hpp>

namespace ui::common
{

class AutoGridLayout : public QLayout
    {
public:
    enum class expand
        {
        ROW_FIRST,
        COLUMN_FIRST
        };

    static constexpr int EXPAND = -1;

    AutoGridLayout (expand exp, int numRowsOrColumns, QWidget* parent) :
        QLayout (parent),
        m_expand (exp),
        m_numRowsOrColumns (numRowsOrColumns)
        {}

    void addItem (QLayoutItem* item) override { m_items.push_back (item); }

    QSize sizeHint () const override { return getSizeInternal (&QLayoutItem::sizeHint); }

    QSize minimumSize () const override { return getSizeInternal (&QLayoutItem::minimumSize); }

    int count () const override { return static_cast<int> (m_items.size ()); }

    QLayoutItem* itemAt (int idx) const override;

    QLayoutItem* takeAt (int idx) override;

    void setGeometry (const QRect& rect) override;
private:
    std::vector<QLayoutItem*>   m_items;
    expand                      m_expand;

    // These are all the same value, making it a union makes
    // the code a bit more readable
    //
    union
        {
        int                     m_numRowsOrColumns;
        int                     m_numRows;
        int                     m_numColumns;
        };

    QSize getSizeInternal (QSize (QLayoutItem::* getSize) () const) const;

    bool isItemShown (size_t idx) const;
    };

} // namespace ui::common
