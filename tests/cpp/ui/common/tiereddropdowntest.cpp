/**
 * @file        common/tiereddropdowntest.hpp
 * @brief       Test suite for the tiered dropdown widget
 * @author      Justin Scott
 * @date        2026-09-26
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <common.hpp>

#include <ui/common/tiereddropdown.hpp>

#include <QtTest>

using namespace ui::common;


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the tiered dropdown widget
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class TieredDropdownTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test adding parent items to a TieredDropdown
    ///
    /// @see    ui::common::TieredDropdown::addParentItem()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void parentItemTest ()
        {
        TieredDropdown dropdown;

        dropdown.addParentItem ("Item 1");
        dropdown.addParentItem ("Item 2");
        dropdown.addParentItem ("Item 3");

        QCOMPARE (dropdown.count (), 3);

        QCOMPARE (dropdown.itemText (0), "Item 1");
        QCOMPARE (dropdown.itemText (1), "Item 2");
        QCOMPARE (dropdown.itemText (2), "Item 3");
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the isParentItem() member function
    ///
    /// @see    ui::common::TieredDropdown::isParentItem()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void isParentItemTest ()
        {
        TieredDropdown dropdown;

        dropdown.addParentItem ("Parent 1");
        dropdown.addChildItem ("Child 1");
        dropdown.addChildItem ("Child 2");

        dropdown.addParentItem ("Parent 2");
        dropdown.addChildItem ("Child 1");
        dropdown.addChildItem ("Child 2");

        QVERIFY (dropdown.isParentItem (0));
        QVERIFY (not dropdown.isParentItem (1));
        QVERIFY (not dropdown.isParentItem (2));

        QVERIFY (dropdown.isParentItem (3));
        QVERIFY (not dropdown.isParentItem (4));
        QVERIFY (not dropdown.isParentItem (5));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests setting a parent item by it's parent index
    ///
    /// @see    ui::common::TieredDropdown::setParentItemText()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setParentItemText ()
        {
        TieredDropdown dropdown;

        dropdown.addParentItem ("Parent 1");
        dropdown.addChildItem ("Child 1");
        dropdown.addChildItem ("Child 2");

        dropdown.addParentItem ("Parent 2");
        dropdown.addChildItem ("Child 1");
        dropdown.addChildItem ("Child 2");

        dropdown.setParentItemText (0, "New Parent 1");
        dropdown.setParentItemText (1, "New Parent 2");

        QCOMPARE (dropdown.itemText (0), "New Parent 1");

        // Qt is adding trailing spaces for some reason
        QCOMPARE (dropdown.itemText (1).trimmed (), "Child 1");
        QCOMPARE (dropdown.itemText (2).trimmed (), "Child 2");

        QCOMPARE (dropdown.itemText (3), "New Parent 2");

        QCOMPARE (dropdown.itemText (4).trimmed (), "Child 1");
        QCOMPARE (dropdown.itemText (5).trimmed (), "Child 2");
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test adding children to a gievn parent
    ///
    /// @see    ui::common::TieredDropdown::addChildItem()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void addChildToParent ()
        {
        TieredDropdown dropdown;

        dropdown.addParentItem ("Parent 1");
        dropdown.addParentItem ("Parent 2");
        dropdown.addParentItem ("Parent 3");

        dropdown.addChildItem ("Child 2", {}, {}, 1);
        dropdown.addChildItem ("Child 1", {}, {}, 0);

        QCOMPARE (dropdown.itemText (1).trimmed (), "Child 1");
        QCOMPARE (dropdown.itemText (3).trimmed (), "Child 2");
        }
    };

QTEST_MAIN (TieredDropdownTest)

#include "tiereddropdowntest.moc"
