/**
 * @file        common/optionaldropdowntest.hpp
 * @brief       Test suite for the optional dropdown widget
 * @author      Justin Scott
 * @date        2026-09-27
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <common.hpp>
#include <names.hpp>

#include <ui/common/optionaldropdown.hpp>

#include <QtTest>

using namespace ui::common;


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the optional dropdown widget
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class OptionalDropdownTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test an optional dropdown with a single item
    ///
    /// @see    ui::common::OptionalDropdown
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void singleItemTest ()
        {
        OptionalDropdown dropdown{ NULL };

        dropdown.addItem ("Single Item");

        dropdown.show ();

        auto* single    = dropdown.findChild<QLabel*> (OBJNAME_OPT_DROPDOWN_SINGLE_ITEM);
        auto* combobox  = dropdown.findChild<QComboBox*> (OBJNAME_OPT_DROPDOWN_DROPDOWN);

        QCOMPARE_NE (single,    NULL);
        QCOMPARE_NE (combobox,  NULL);

        QVERIFY (single->isVisible ());
        QVERIFY (combobox->isHidden ());

        QCOMPARE (single->text (), "Single Item");
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test an optional dropdown with multiple items
    ///
    /// @see    ui::common::OptionalDropdown
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void multiItemTest ()
        {
        OptionalDropdown dropdown{ NULL };

        dropdown.addItem ("Multi Item 1");
        dropdown.addItem ("Multi Item 2");

        dropdown.show ();

        auto* single = dropdown.findChild<QLabel*> (OBJNAME_OPT_DROPDOWN_SINGLE_ITEM);
        auto* combobox = dropdown.findChild<QComboBox*> (OBJNAME_OPT_DROPDOWN_DROPDOWN);

        QCOMPARE_NE (single, NULL);
        QCOMPARE_NE (combobox, NULL);

        QVERIFY (single->isHidden ());
        QVERIFY (combobox->isVisible ());

        QCOMPARE (combobox->itemText (0), "Multi Item 1");
        QCOMPARE (combobox->itemText (1), "Multi Item 2");
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests an optional dropdown starting with one item, that converts to a true
    /// dropdown when you add more items
    ///
    /// @see    ui::common::OptionalDropdown
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void singleToMultiItemTest ()
        {
        OptionalDropdown dropdown{ NULL };

        dropdown.addItem ("Single Item");

        dropdown.show ();

        auto* single = dropdown.findChild<QLabel*> (OBJNAME_OPT_DROPDOWN_SINGLE_ITEM);
        auto* combobox = dropdown.findChild<QComboBox*> (OBJNAME_OPT_DROPDOWN_DROPDOWN);

        QCOMPARE_NE (single, NULL);
        QCOMPARE_NE (combobox, NULL);

        QVERIFY (single->isVisible ());
        QVERIFY (combobox->isHidden ());

        QCOMPARE (single->text (), "Single Item");

        dropdown.addItem ("Multi Item");

        QVERIFY (single->isHidden ());
        QVERIFY (combobox->isVisible ());

        QCOMPARE (combobox->itemText (0), "Single Item");
        QCOMPARE (combobox->itemText (1), "Multi Item");
        }
    };

QTEST_MAIN (OptionalDropdownTest)

#include "optionaldropdowntest.moc"
