/**
 * @file        common/utilstest.hpp
 * @brief       Test suite for the UI common utilities
 * @author      Justin Scott
 * @date        2026-08-14
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <common.hpp>

#include <ui/common/utils.hpp>

#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QtTest>

using namespace ui::common;

/// Custom user data type for combobox tests
struct customData
    {
    int myInt;

    bool operator== (const customData& other) const { return myInt == other.myInt; }
    };

///////////////////////////////////////////////////////////////////////////////
/// Template function to test setFormRowText() with an arbitrary field type
///
/// @tparam T   Field type
///
/// @see    ui::common::setFormRowText()
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
static void utilsTestForm ()
    {
    static constexpr int NUM_ROWS       = 20;
    static const QString NEW_TEXT_FMT   = "New Text %1";

    QFormLayout form;
    T fields[NUM_ROWS];


    for (int ii = 0; ii < NUM_ROWS; ++ii)
        {
        form.addRow (QString{ "Initial Text %1" }.arg (ii),
                     &fields[ii]);
        }

    for (int ii = 0; ii < NUM_ROWS; ++ii)
        {
        setFormRowText (form,
                        fields[ii],
                        NEW_TEXT_FMT.arg (ii));
        }

    for (int ii = 0; ii < NUM_ROWS; ++ii)
        {
        QCOMPARE (static_cast<QLabel*> (
                        form.itemAt (ii,
                                     QFormLayout::LabelRole)->widget ())->text (),
                  NEW_TEXT_FMT.arg (ii));
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the UI common utilities
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class UtilsTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test the setFormRowText() function with widget fields
    ///
    /// @see    ui::common::setFormRowText()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setFormRowTextWidgetTest ()
        {
        // Test a couple common widgets
        utilsTestForm<QLabel> ();
        utilsTestForm<QPushButton> ();
        utilsTestForm<QComboBox> ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the setFormRowText() function with layout fields
    ///
    /// @see    ui::common::setFormRowText()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setFormRowTextLayoutTest ()
        {
        // Test a couple common layouts
        utilsTestForm<QVBoxLayout> ();
        utilsTestForm<QHBoxLayout> ();
        utilsTestForm<QFormLayout> ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test removing actions from a widget with removeAllActions()
    ///
    /// @see    ui::common::removeAllActions()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void removeActionTest ()
        {
        static constexpr int NUM_ACTIONS = 16;

        QWidget widget;

        for (int ii = 0; ii < NUM_ACTIONS; ++ii)
            {
            widget.addAction (new QAction{ &widget });
            }

        QCOMPARE (widget.actions ().count (), NUM_ACTIONS);

        removeAllActions (widget);

        QCOMPARE (widget.actions ().count (), 0);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test removing all items from a layout using clearLayout()
    ///
    /// @see    ui::common::clearLayout()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clearLayoutTest ()
        {
        QHBoxLayout     parent;
        QVBoxLayout*    child;
        QWidget*        widget;

        parent.addWidget (new QComboBox);
        parent.addWidget (new QPushButton);
        parent.addLayout (child = new QVBoxLayout);
        parent.addWidget (widget = new QLabel);

        child->addWidget (new QComboBox);
        child->addWidget (new QPushButton);
        child->addWidget (new QLabel);

        QSignalSpy spy1{ child,   &QObject::destroyed };
        QSignalSpy spy2{ widget,  &QObject::destroyed };

        clearLayout (parent);

        // Confirm the widget and layout were destroyed
        QCOMPARE (spy1.count (), 1);
        QCOMPARE (spy2.count (), 1);

        QCOMPARE (parent.count (), 0);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test removing a widget from a layout with removeWidgetFromLayout()
    ///
    /// @see    ui::common::removeWidgetFromLayout()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void removeWidgetFromLayoutTest ()
        {
        QVBoxLayout layout;
        QPushButton* middle;

        layout.addWidget (new QComboBox);
        layout.addWidget (middle = new QPushButton);
        layout.addWidget (new QLabel);

        QSignalSpy spy{ middle,  &QObject::destroyed };

        removeWidgetFromLayout (layout, 1);

        // Confirm the widget was destroyed
        QCOMPARE (spy.count (), 1);

        // Confirm a widget has been removed
        QCOMPARE (layout.count (), 2);

        // Confirm the remaining widgets are correct
        QCOMPARE (layout.itemAt (0)->widget ()->metaObject ()->className (),
                  "QComboBox");
        QCOMPARE (layout.itemAt (1)->widget ()->metaObject ()->className (),
                  "QLabel");
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the getTopLevelWidget function
    ///
    /// @see    ui::common::getTopLevelWidget()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void getTopLevelWidgetTest ()
        {
        QPushButton btn;
        QLabel      label;
        QComboBox   cb;

        QCOMPARE (getTopLevelWidget<QPushButton> (),    &btn);
        QCOMPARE (getTopLevelWidget<QLabel> (),         &label);
        QCOMPARE (getTopLevelWidget<QComboBox> (),      &cb);

        // No top level menu should be open
        QCOMPARE (getTopLevelWidget<QMenu> (),          NULL);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test refreshing the combobox contents, with one item with the same user data
    ///
    /// @see    ui::common::refreshComboboxItems()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void refreshComboboxTest ()
        {
        QComboBox   cb;

        cb.addItem ("Test 1", 1);
        cb.addItem ("Test 2", 2);
        cb.addItem ("Test 3", 3);
        cb.addItem ("Test 4", 4);
        cb.addItem ("Test 5", 5);

        cb.setCurrentIndex (2); // Should set this to test 3

        QSignalSpy  spy{ &cb, &QComboBox::currentIndexChanged };

        // Re-fill the combobox
        refreshComboboxItems (cb,
            [&] () -> void
            {
            cb.addItem ("Test 6", 6);
            cb.addItem ("Test 7", 7);
            cb.addItem ("Test 8", 8);
            cb.addItem ("Test 9", 9);
            cb.addItem ("Test 10", 3); // New "test 3"
            cb.addItem ("Test 11", 11);
            });

        // Since Test 10 is the new Test 3 (based on user data)
        // We shouldn't see an index changed signal
        //
        QCOMPARE (spy.count (), 0);
        QCOMPARE (cb.currentIndex (), 4);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test refreshing the combobox contents, with the currently selected item not
    /// in the new set of items
    ///
    /// @see    ui::common::refreshComboboxItems()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void refreshComboboxCurrentSelectionRemovedTest ()
        {
        QComboBox   cb;

        cb.addItem ("Test 1", 1);
        cb.addItem ("Test 2", 2);
        cb.addItem ("Test 3", 3);
        cb.addItem ("Test 4", 4);
        cb.addItem ("Test 5", 5);

        cb.setCurrentIndex (2); // Should set this to test 3

        QSignalSpy  spy{ &cb, &QComboBox::currentIndexChanged };

        // Re-fill the combobox
        refreshComboboxItems (cb,
            [&] () -> void
            {
            cb.addItem ("Test 6", 6);
            cb.addItem ("Test 7", 7);
            cb.addItem ("Test 8", 8);
            cb.addItem ("Test 9", 9);
            cb.addItem ("Test 10", 10);
            cb.addItem ("Test 11", 11);
            });

        // Theres is no item with data 3, so we should return to item 1,
        // and signal that it has changed
        //
        QCOMPARE (spy.count (), 1);
        QCOMPARE (cb.currentIndex (), 0);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for setComboBoxIndexByUserDataTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setComboBoxIndexByUserDataTest_data ()
        {
        QTest::addColumn<bool> ("notify");

        QTest::newRow ("Notify") << true;
        QTest::newRow ("Block")  << false;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests setting the combobox index to a given user data value
    ///
    /// @see    ui::common::setComboBoxIndexByUserData()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setComboBoxIndexByUserDataTest ()
        {
        QFETCH (bool, notify);

        QComboBox   cb;

        cb.addItem ("Test 1", QVariant::fromValue (customData{ 1 }));
        cb.addItem ("Test 2", QVariant::fromValue (customData{ 2 }));
        cb.addItem ("Test 3", QVariant::fromValue (customData{ 3 }));
        cb.addItem ("Test 4", QVariant::fromValue (customData{ 4 }));
        cb.addItem ("Test 5", QVariant::fromValue (customData{ 5 }));

        QSignalSpy  spy{ &cb, &QComboBox::currentIndexChanged };

        // Test user data we have an item for
        setComboBoxIndexByUserData (cb, customData{ 4 }, notify);

        QCOMPARE (cb.currentIndex (), 3);

        if (notify)
            {
            QCOMPARE (spy.count (), 1);
            }
        else
            {
            QCOMPARE (spy.count (), 0);
            }

        spy.clear ();

        // Test user data we have don't have an item for
        setComboBoxIndexByUserData (cb, customData{ 6 }, true);

        QCOMPARE (cb.currentIndex (), 3);
        QCOMPARE (spy.count (), 0);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests settting the combobox index by user data, when the items have
    /// inconsistent and sometimes non-existent user data
    ///
    /// @see    ui::common::setComboBoxIndexByUserData()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setComboBoxIndexByUserDataInvalidUserDataTest ()
        {
        QComboBox   cb;

        cb.addItem ("Test 1");                                          // no user data
        cb.addItem ("Test 2", "Test 2 data");                           // different data type
        cb.addItem ("Test 3", QVariant::fromValue (customData{ 1 }));   // Same data type, different value
        cb.addItem ("Test 4", QVariant::fromValue (customData{ 0 }));   // The one we're looking for!
        cb.addItem ("Test 5", QVariant::fromValue (customData{ 2 }));   // Another dud

        QSignalSpy  spy{ &cb, &QComboBox::currentIndexChanged };

        // Testing with 0, to make sure that empty variants aren't confused with 0
        setComboBoxIndexByUserData (cb, customData{ 0 });

        QCOMPARE (cb.currentIndex (), 3);
        QCOMPARE (spy.count (), 1);
        }
    };

QTEST_MAIN (UtilsTest)

#include "utilstest.moc"
