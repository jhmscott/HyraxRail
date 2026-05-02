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

//////////////////////////////////////////////////////////////////////////////
/// Grid layout that places widgets automatically, filling in space as widgets
/// are removed
///
//////////////////////////////////////////////////////////////////////////////
class AutoGridLayout : public QLayout
    {
public:
    // Direction to expand in
    enum class expand
        {
        ROW_FIRST,      ///< Add new columns to a row before adding a new row
        COLUMN_FIRST    ///< Add new rows to the column first before adding a new column
        };

    static constexpr int EXPAND = -1;   ///< Expand or shrink to fit geometry
                                        ///  TODO: Not implemented yet

    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  exp                 Expansion direction
    /// @param[in]  numRowsOrColumns    If ROW_FIRST = exp, this is number of columns
    ///                                 to each row
    ///                                 If COLUMN_FIRST = exp, this is number of row
    ///                                 to each column
    /// @param[in]  parent              Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    AutoGridLayout (expand exp, int numRowsOrColumns, QWidget* parent) :
        QLayout (parent),
        m_expand (exp),
        m_numRowsOrColumns (numRowsOrColumns)
        {}

    //////////////////////////////////////////////////////////////////////////////
    /// Add a layout item
    ///
    /// @param[in]  item        Item to add
    ///
    //////////////////////////////////////////////////////////////////////////////
    void addItem (QLayoutItem* item) override { m_items.push_back (item); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the size hint
    ///
    /// @return     Widget size hint
    ///
    //////////////////////////////////////////////////////////////////////////////
    QSize sizeHint () const override { return getSizeInternal (&QLayoutItem::sizeHint); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the minimum size of the widget
    ///
    /// @return     Widget minimum size
    ///
    //////////////////////////////////////////////////////////////////////////////
    QSize minimumSize () const override { return getSizeInternal (&QLayoutItem::minimumSize); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the number of items in the layout
    ///
    /// @return     Number items in the layout
    ///
    //////////////////////////////////////////////////////////////////////////////
    int count () const override { return static_cast<int> (m_items.size ()); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the layout item at an index
    ///
    /// @param[in]  idx     Layout item index
    ///
    /// @return     Layout item at idx
    ///
    //////////////////////////////////////////////////////////////////////////////
    QLayoutItem* itemAt (int idx) const override;

    //////////////////////////////////////////////////////////////////////////////
    /// Remove an item at an index and return it
    ///
    /// @param[in]  idx     Layout item index
    ///
    /// @return     Layout item removed from list at index idx
    ///
    //////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] QLayoutItem* takeAt (int idx) override;

    //////////////////////////////////////////////////////////////////////////////
    /// Update the geometry of the layout
    ///
    /// @param[in]  rect        Rect for the layout to fill
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setGeometry (const QRect& rect) override;
private:
    std::vector<QLayoutItem*>   m_items;            ///< List of the layout items
    expand                      m_expand;           ///< Expansion direction

    // These are all the same value, making it a union makes
    // the code a bit more readable
    //
    union
        {
        int                     m_numRowsOrColumns;
        int                     m_numRows;          ///< Number of rows per column
        int                     m_numColumns;       ///< Number of columns per row
        };

    //////////////////////////////////////////////////////////////////////////////
    /// Internal logic to get one of the size metrics
    ///
    /// @param[in]  getSize     Member function to get the specific size metric
    ///                         you want for each layout item
    ///
    /// @return     Size metric
    ///
    //////////////////////////////////////////////////////////////////////////////
    QSize getSizeInternal (QSize (QLayoutItem::* getSize) () const) const;

    //////////////////////////////////////////////////////////////////////////////
    /// Check if an item is shown or not
    ///
    /// @param[in]  idx         Layout item index
    ///
    /// @return     True if visible
    ///
    //////////////////////////////////////////////////////////////////////////////
    bool isItemShown (size_t idx) const;
    };

} // namespace ui::common
