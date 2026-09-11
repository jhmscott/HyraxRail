/**
 * @file        common/schemeicontest.hpp
 * @brief       Test suite for the colour scheme aware icon widgets
 * @author      Justin Scott
 * @date        2026-09-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <common.hpp>

#include <testutils/assert.hpp>

#include <ui/common/schemeicon.hpp>

#include <QStyleHints>
#include <QtTest>



using namespace ui::common;

///////////////////////////////////////////////////////////////////////////////
/// Test data for changing the colour scheme
///
///////////////////////////////////////////////////////////////////////////////
static void colourSchemeChangeData ()
    {
    QTest::addColumn<Qt::ColorScheme> ("starting");
    QTest::addColumn<Qt::ColorScheme> ("changing");

    QTest::addRow ("Light to Dark") << Qt::ColorScheme::Light << Qt::ColorScheme::Dark;
    QTest::addRow ("Dark to Light") << Qt::ColorScheme::Dark  << Qt::ColorScheme::Light;
    }

///////////////////////////////////////////////////////////////////////////////
/// Assert that a combobox has the expected set of icons
///
/// @param[in]  cb          ComboBox to check
/// @param[in]  expected    Expected icons
///
///////////////////////////////////////////////////////////////////////////////
static void assertComboboxIcons (const QComboBox& cb, const std::vector<QIcon>& expected)
    {
    QCOMPARE (cb.count (), expected.size ());

    for (int ii = 0; ii < cb.count (); ++ii)
        {
        COMPARE_ICONS (cb.itemIcon (ii), expected.at (ii));
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Assert that a tab widget has the expected set of icons
///
/// @param[in]  tabs        Tab widget
/// @param[in]  expected    Expected icons
///
///////////////////////////////////////////////////////////////////////////////
static void assertTabWidgetIcons (const QTabWidget& tabs, const std::vector<QIcon>& expected)
    {
    QCOMPARE (tabs.count (), expected.size ());

    for (int ii = 0; ii < tabs.count (); ++ii)
        {
        COMPARE_ICONS (tabs.tabIcon (ii), expected.at (ii));
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the colour scheme aware icon widgets
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class SchemeIconTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for chemeIconButtonTest
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void schemeIconButtonTest_data () { colourSchemeChangeData (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test a colour scheme dependent icon button
    ///
    /// @see    ui::common::SchemeIconButton
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void schemeIconButtonTest ()
        {
        static const std::map<Qt::ColorScheme, QIcon> EXPECTED_ICONS =
            {
                { Qt::ColorScheme::Light, QIcon{ ":/icons/light/misc/binary.svg" } },
                { Qt::ColorScheme::Dark,  QIcon{ ":/icons/dark/misc/binary.svg" } }
            };

        QFETCH (Qt::ColorScheme, starting);
        QFETCH (Qt::ColorScheme, changing);

        qApp->styleHints ()->setColorScheme (starting);

        SchemeIconButton btn{ "misc/binary" };

        COMPARE_ICONS (btn.icon (), EXPECTED_ICONS.at (starting));

        qApp->styleHints ()->setColorScheme (changing);

        COMPARE_ICONS (btn.icon (), EXPECTED_ICONS.at (changing));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for schemeIconDialogTest
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void schemeIconDialogTest_data () { colourSchemeChangeData (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test a colour scheme dependent icon dialog box
    ///
    /// @see    ui::common::SchemeDialog
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void schemeIconDialogTest ()
        {
        static const std::map<Qt::ColorScheme, QIcon> EXPECTED_ICONS =
            {
                { Qt::ColorScheme::Light, QIcon{ ":/icons/light/misc/clock.svg" } },
                { Qt::ColorScheme::Dark,  QIcon{ ":/icons/dark/misc/clock.svg" } }
            };

        QFETCH (Qt::ColorScheme, starting);
        QFETCH (Qt::ColorScheme, changing);

        qApp->styleHints ()->setColorScheme (starting);

        SchemeDialog dlg{ NULL };

        dlg.setWindowIcon ("misc/clock");

        COMPARE_ICONS (dlg.windowIcon (), EXPECTED_ICONS.at (starting));

        qApp->styleHints ()->setColorScheme (changing);

        COMPARE_ICONS (dlg.windowIcon (), EXPECTED_ICONS.at (changing));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for schemeIconComboBoxTest
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void schemeIconComboBoxTest_data () { colourSchemeChangeData (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test a colour scheme dependent icon combobox
    ///
    /// @see    ui::common::SchemeDialog
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void schemeIconComboBoxTest ()
        {
        static const std::map<Qt::ColorScheme, std::vector<QIcon>> EXPECTED_ICONS =
            {
                {
                Qt::ColorScheme::Light,
                    {
                    QIcon{ ":/icons/light/misc/clipboard-copy.svg" },
                    QIcon{ ":/icons/light/misc/pencil.svg" },
                    QIcon{ ":/icons/light/misc/plus.svg" }
                    }
                },
                {
                Qt::ColorScheme::Dark,
                    {
                    QIcon{ ":/icons/dark/misc/clipboard-copy.svg" },
                    QIcon{ ":/icons/dark/misc/pencil.svg" },
                    QIcon{ ":/icons/dark/misc/plus.svg" }
                    }
                }
            };

        QFETCH (Qt::ColorScheme, starting);
        QFETCH (Qt::ColorScheme, changing);

        qApp->styleHints ()->setColorScheme (starting);

        SchemeComboBox cb;

        cb.addItem ("misc/clipboard-copy",  "Copy");
        cb.addItem ("misc/pencil",          "Edit");
        cb.addItem ("misc/plus",            "Add");

        assertComboboxIcons (cb, EXPECTED_ICONS.at (starting));

        qApp->styleHints ()->setColorScheme (changing);

        assertComboboxIcons (cb, EXPECTED_ICONS.at (changing));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests that the contents of items added with addItem() are what we expect,
    /// and that the scheme icon (not QIcon) can be retrieved
    ///
    /// @see        ui::common::SchemeDialog::addItem()
    /// @see        ui::common::SchemeDialog::itemSchemeIcon()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void schemeIconComboBoxItemContentsTest ()
        {
        SchemeComboBox cb;

        cb.addItem ("misc/clipboard-copy",  "Copy");
        cb.addItem ("misc/pencil",          "Edit");
        cb.addItem ("misc/plus",            "Add");

        QCOMPARE (cb.itemText (0), "Copy");
        QCOMPARE (cb.itemText (1), "Edit");
        QCOMPARE (cb.itemText (2), "Add");

        QCOMPARE (cb.itemSchemeIcon (0), "misc/clipboard-copy");
        QCOMPARE (cb.itemSchemeIcon (1), "misc/pencil");
        QCOMPARE (cb.itemSchemeIcon (2), "misc/plus");
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for schemeIconComboBoxSetIconTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void schemeIconComboBoxSetIconTest_data () { colourSchemeChangeData (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests changing the item icon with setItemSchemeIcon()
    ///
    /// @see        ui::common::SchemeDialog::setItemSchemeIcon()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void schemeIconComboBoxSetIconTest ()
        {
        static const std::map<Qt::ColorScheme, QIcon> EXPECTED_ICONS =
            {
                { Qt::ColorScheme::Light, QIcon{ ":/icons/light/misc/key.svg" } },
                { Qt::ColorScheme::Dark,  QIcon{ ":/icons/dark/misc/key.svg" } }
            };

        QFETCH (Qt::ColorScheme, starting);
        QFETCH (Qt::ColorScheme, changing);

        qApp->styleHints ()->setColorScheme (starting);

        SchemeComboBox cb;

        cb.addItem ("misc/clipboard-copy",  "Copy");
        cb.addItem ("misc/pencil",          "Edit");
        cb.addItem ("misc/plus",            "Add");

        cb.setItemSchemeIcon (1, "misc/key");

        QCOMPARE (cb.itemSchemeIcon (1), "misc/key");
        COMPARE_ICONS (cb.itemIcon (1), EXPECTED_ICONS.at (starting));

        qApp->styleHints ()->setColorScheme (changing);

        COMPARE_ICONS (cb.itemIcon (1), EXPECTED_ICONS.at (changing));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for schemeIconTabWidgetTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void schemeIconTabWidgetTest_data () { colourSchemeChangeData (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test a colour scheme dependent tab widget
    ///
    /// @see    ui::common::SchemeTabWidget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void schemeIconTabWidgetTest ()
        {
        static const std::map<Qt::ColorScheme, std::vector<QIcon>> EXPECTED_ICONS =
            {
                {
                Qt::ColorScheme::Light,
                    {
                    QIcon{ ":/icons/light/misc/train.svg" },
                    QIcon{ ":/icons/light/misc/arrow-left.svg" },
                    QIcon{ ":/icons/light/misc/path.svg" }
                    }
                },
                {
                Qt::ColorScheme::Dark,
                    {
                    QIcon{ ":/icons/dark/misc/train.svg" },
                    QIcon{ ":/icons/dark/misc/arrow-left.svg" },
                    QIcon{ ":/icons/dark/misc/path.svg" }
                    }
                }
            };

        QFETCH (Qt::ColorScheme, starting);
        QFETCH (Qt::ColorScheme, changing);

        qApp->styleHints ()->setColorScheme (starting);

        SchemeTabWidget tabs;

        tabs.addTab (new QWidget{ &tabs }, "misc/train");
        tabs.addTab (new QWidget{ &tabs }, "misc/arrow-left");
        tabs.addTab (new QWidget{ &tabs }, "misc/path");

        assertTabWidgetIcons (tabs, EXPECTED_ICONS.at (starting));

        qApp->styleHints ()->setColorScheme (changing);

        assertTabWidgetIcons (tabs, EXPECTED_ICONS.at (changing));
        }
    };

QTEST_MAIN (SchemeIconTest)

#include "schemeicontest.moc"
