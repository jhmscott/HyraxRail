/**
 * @file        testutils_test/mousespytest.cpp
 * @brief       Test suite for UI test utilities
 * @author      Justin Scott
 * @date        2026-10-31
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <testutils/mousespy.hpp>

#include <QtTest>

using namespace testutils;


///////////////////////////////////////////////////////////////////////////////
/// Test data generate for the three standard mouse buttons
///
///////////////////////////////////////////////////////////////////////////////
static void generateMouseButtons ()
    {
    QTest::addColumn<Qt::MouseButton> ("button");

    QTest::newRow ("left")  << Qt::LeftButton;
    QTest::newRow ("middle")<< Qt::MiddleButton;
    QTest::newRow ("right") << Qt::RightButton;
    }

///////////////////////////////////////////////////////////////////////////////
/// Generates a set of points to test with
///
///////////////////////////////////////////////////////////////////////////////
static void generatePoints ()
    {
    QTest::addColumn<QPoint> ("point");

    QTest::newRow ("Q1") << QPoint{ 5,  5 };
    QTest::newRow ("Q2") << QPoint{ 5, -5 };
    QTest::newRow ("Q3") << QPoint{-5, -5 };
    QTest::newRow ("Q4") << QPoint{ 5, -5 };
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for UI test utilities
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class MouseSpyTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for clickTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clickTest_data () { generateMouseButtons (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests clicking on a mouse spy
    ///
    /// @see    testutils::MouseSpy::mousePressEvent()
    /// @see    testutils::MouseSpy::mouseReleaseEvent()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clickTest ()
        {
        QFETCH (Qt::MouseButton, button);

        MouseSpy spy;

        spy.setFixedSize (50, 50);

        QTest::mouseClick (&spy, button);

        QCOMPARE (spy.count (), 2);

        auto first = spy.begin ();
        auto second= std::next (first);

        QCOMPARE (first->button (),  button);
        QCOMPARE (first->type (),    QEvent::MouseButtonPress);

        QCOMPARE (second->button (), button);
        QCOMPARE (second->type (),   QEvent::MouseButtonRelease);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for clickPointTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clickPointTest_data () { generatePoints (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests clicking a specific point on the mouse spy
    ///
    /// @see    testutils::MouseSpy::mousePressEvent()
    /// @see    testutils::MouseSpy::mouseReleaseEvent()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clickPointTest ()
        {
        QFETCH (QPoint, point);

        MouseSpy spy;

        spy.setFixedSize (50, 50);

        QTest::mouseClick (&spy, Qt::LeftButton, {}, point);

        QCOMPARE (spy.count (), 2);

        auto first = spy.begin ();
        auto second= std::next (first);

        QCOMPARE (first->position (), point);
        QCOMPARE (first->type (), QEvent::MouseButtonPress);

        QCOMPARE (second->position (), point);
        QCOMPARE (second->type (), QEvent::MouseButtonRelease);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for doubleClickTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void doubleClickTest_data () { generateMouseButtons (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests double clicking on a mouse spy
    ///
    /// @see    testutils::MouseSpy::mouseDoubleClickEvent()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void doubleClickTest ()
        {
        QFETCH (Qt::MouseButton, button);

        MouseSpy spy;

        spy.setFixedSize (50, 50);

        QTest::mouseDClick (&spy, button);

        QCOMPARE (spy.count (), 1);

        auto first = spy.begin ();

        QCOMPARE (first->button (),  button);
        QCOMPARE (first->type (),    QEvent::MouseButtonDblClick);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for moveTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void moveTest_data () { generatePoints ();}

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests moving a mouse on a mouse spy
    ///
    /// @see    testutils::MouseSpy::mouseMoveEvent()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void moveTest ()
        {
        QFETCH (QPoint, point);

        MouseSpy spy;

        spy.setFixedSize (50, 50);
        spy.show ();

        QTest::mousePress (&spy, Qt::LeftButton);
        QTest::mouseMove (&spy, point);
        QTest::mouseRelease (&spy, Qt::LeftButton);

        QCOMPARE (spy.count (), 3);

        auto first  = spy.begin ();
        auto second = std::next (first);
        auto third  = std::next (second);


        QCOMPARE (first->type (),       QEvent::MouseButtonPress);

        QCOMPARE (second->type (),      QEvent::MouseMove);
        QCOMPARE (second->position (),  point);


        QCOMPARE (third->type (),       QEvent::MouseButtonRelease);
        }
    };

QTEST_MAIN (MouseSpyTest)

#include "mousespytest.moc"
