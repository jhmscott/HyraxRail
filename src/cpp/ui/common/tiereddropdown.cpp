/**
 * @file        common/tiereddropdown.cpp
 * @brief       ComboBox with two tiers of items, parent and child
 * @author      Justin Scott
 * @date        2026-06-28
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/tiereddropdown.hpp>

#include <QApplication>
#include <QItemDelegate>
#include <QStandardItemModel>
#include <QStyleHints>

/// Tier or level of a dropdown
enum class tier
    {
    PARENT,     ///< Parent item
    CHILD,      ///< Child item
    ICON_CHILD  ///< Child of a parent with an icon
    };

static const QString INDENT{ 4, QChar {' '} };  ///< Four space ident

namespace // anonymous
{

///////////////////////////////////////////////////////////////////////////////
/// Style delegate for a tiered dropdown
///
///////////////////////////////////////////////////////////////////////////////
class TieredDropdownDelegate : public QItemDelegate
    {
public:
    using QItemDelegate::QItemDelegate;

    ///////////////////////////////////////////////////////////////////////////////
    /// Paint a combobox item
    ///
    /// @param[in,out]  painter     Painter instance
    /// @param[in]      option      Default style options
    /// @param[in]      index       Index of the item
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void paint (QPainter*                   painter,
                        const QStyleOptionViewItem& option,
                        const QModelIndex&          index) const override
        {
        tier                    type        = index.data (dropDownTier).value<tier> ();
        QStyleOptionViewItem    myOption    = option;

        switch (type)
            {
            case tier::PARENT:
                {
                myOption.state |= QStyle::State_Enabled;
                break;
                }
            case tier::CHILD:
            case tier::ICON_CHILD:
                {
                int indent = option.fontMetrics.horizontalAdvance (INDENT);

                if (tier::ICON_CHILD == type)
                    {
                    indent *= 2;
                    }

                myOption.rect.adjust (indent, 0, 0, 0);
                myOption.textElideMode = Qt::ElideNone;
                break;
                }
            }

        QItemDelegate::paint (painter, myOption, index);
        }
    };

} // namespace anonymous


///////////////////////////////////////////////////////////////////////////////
/// Set the scheme icon for a combobox icon
///
/// @param[in,out]  item        Item to set icon for
/// @param[in]      icon        Icon to set
///
///////////////////////////////////////////////////////////////////////////////
static void setSchemeIcon (QStandardItem& item, const utils::resources::Icon& icon)
    {
    if (icon)
        {
        item.setData (QVariant::fromValue (icon), schemeIcon);
        item.setIcon (icon.toIcon (qApp->
                                    styleHints ()->
                                        colorScheme ()));
        }
    }

namespace ui::common
{
TieredDropdown::TieredDropdown (QWidget* parent) :
    SchemeComboBox (parent)
    { setItemDelegate (new TieredDropdownDelegate{}); }


void TieredDropdown::addParentItem (const QString&                  text,
                                    const utils::resources::Icon&   icon)
    {
    QStandardItem* item = new QStandardItem{ text };

    item->setFlags (item->flags () & ~(Qt::ItemIsEnabled | Qt::ItemIsSelectable));
    item->setData (QVariant::fromValue (tier::PARENT), dropDownTier);
    setSchemeIcon (*item, icon);

    QFont font = item->font ();

    font.setBold (true);
    item->setFont (font);

    QStandardItemModel* itemModel = static_cast<QStandardItemModel*> (model ());
    itemModel->appendRow (item);
    }

void TieredDropdown::addChildItem (const QString&                   text,
                                   const QVariant&                  data,
                                   const utils::resources::Icon&    icon)
    {
    QStandardItem*  item = new QStandardItem{ text + INDENT };

    item->setData (data, Qt::UserRole);
    item->setData (QVariant::fromValue (tier::CHILD), dropDownTier);
    setSchemeIcon (*item, icon);

    // If there's no icon, but the parent does, we need to add an extra indent
    if (not icon)
        {
        for (int ii = count () - 1; ii >= 0; --ii)
            {
            if (tier::PARENT == itemData (ii, dropDownTier).value<tier> ())
                {
                if (itemData (ii, schemeIcon).value<utils::resources::Icon> ())
                    {
                    item->setData (QVariant::fromValue (tier::ICON_CHILD), dropDownTier);
                    }
                break;
                }
            }
        }

    QStandardItemModel* itemModel = static_cast<QStandardItemModel*> (model ());
    itemModel->appendRow (item);
    }

} // namespace ui::common