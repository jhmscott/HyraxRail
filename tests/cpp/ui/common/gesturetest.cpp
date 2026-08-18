/**
 * @file        common/gesturetest.hpp
 * @brief       Test suite for the UI gesture utilities
 * @author      Justin Scott
 * @date        2026-08-14
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/gesture.hpp>

#include <QtTest>

using namespace std::chrono_literals;


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the UI gesture utilities
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class GestureTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test that long pressing a HoldToOpenMenu widget opens the context menu
    ///
    /// @see    ui::common::HoldToOpenMenu
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void holdToOpenMenuTest ()
        {
        using GestureMenuTest = ui::common::HoldToOpenMenu<QWidget>;

        static const QPoint TOUCH_POINT{ 25, 50 };

        QPointingDevice*    dev = QTest::createTouchDevice ();
        GestureMenuTest     widget;

        widget.setAttribute         (Qt::WA_AcceptTouchEvents, true);
        widget.setFixedSize         (100, 100);
        widget.setSizePolicy        (QSizePolicy::Fixed, QSizePolicy::Fixed);
        widget.setContextMenuPolicy (Qt::CustomContextMenu);

        widget.show ();

        QSignalSpy spy{ &widget, &QWidget::customContextMenuRequested };

        auto touchSequence = QTest::touchEvent (&widget, dev, false);

        touchSequence.press (0, TOUCH_POINT, &widget);
        touchSequence.commit ();

        QTest::qWait (1s);

        touchSequence.release (0, TOUCH_POINT, &widget);
        touchSequence.commit ();


        QCOMPARE_GE (spy.count (), 1);
        QCOMPARE (spy.at (0).at (0).value<QPoint> (),
                  widget.mapToGlobal (TOUCH_POINT));
        }
    };

QTEST_MAIN (GestureTest)

#include "gesturetest.moc"
