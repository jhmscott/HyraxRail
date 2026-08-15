/**
 * @file        common/utilstest.hpp
 * @brief       Test suite for the UI common utilities
 * @author      Justin Scott
 * @date        2026-08-14
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/utils.hpp>

#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QtTest>

using namespace ui::common;

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
    };

QTEST_MAIN (UtilsTest)

#include "utilstest.moc"
