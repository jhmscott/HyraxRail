/**
 * @file        common/touchslidertest.cpp
 * @brief       Test suite for the touch enabled slider widget
 * @author      Justin Scott
 * @date        2026-10-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/touchslider.hpp>

#include <QPointingDevice>
#include <QtTest>

using namespace ui::common;

static int SLIDER_MAX = 200;

///////////////////////////////////////////////////////////////////////////////
/// Make the touch point test data
///
///////////////////////////////////////////////////////////////////////////////
static void makeTouchPoints ()
    {
    QTest::addColumn<int> ("position");

    for (int ii = 0; ii < SLIDER_MAX; ii += 10)
        {
        QTest::addRow ("%d", ii) << ii;
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the touch enabled slider widget
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class TouchSliderTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for verticalSliderTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void verticalSliderTest_data () { makeTouchPoints (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the touch slider in the vertical configuration
    ///
    /// @see    ui::common::TouchSlider
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void verticalSliderTest ()
        {
        QFETCH (int, position);

        QPointingDevice* dev = QTest::createTouchDevice ();

        TouchSlider slider{ Qt::Vertical, NULL };

        slider.setFixedSize (50, SLIDER_MAX);
        slider.setMinimum (0);
        slider.setMaximum (SLIDER_MAX);

        QTest::touchEvent (&slider, dev).press (0, QPoint{ 25, position });

        QCOMPARE (slider.value (), position);
        }


    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for horizontalSliderTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void horizontalSliderTest_data () { makeTouchPoints (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the touch slider in the horizontal configuration
    ///
    /// @see    ui::common::TouchSlider
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void horizontalSliderTest ()
        {
        QFETCH (int, position);

        QPointingDevice* dev = QTest::createTouchDevice ();

        TouchSlider slider{ Qt::Horizontal, NULL };

        slider.setFixedSize (SLIDER_MAX, 50);
        slider.setMinimum (0);
        slider.setMaximum (SLIDER_MAX);

        QTest::touchEvent (&slider, dev).press (0, QPoint{ position, 25 });

        QCOMPARE (slider.value (), position);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test that a disabled slider ignores touch event
    ///
    ///  @see    ui::common::TouchSlider
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void disabledSliderTest ()
        {
        QPointingDevice* dev = QTest::createTouchDevice ();

        TouchSlider slider{ Qt::Vertical, NULL };

        slider.setFixedSize (50, SLIDER_MAX);
        slider.setMinimum (0);
        slider.setMaximum (SLIDER_MAX);
        slider.setValue (SLIDER_MAX / 2);
        slider.setDisabled (true);

        QTest::touchEvent (&slider, dev).press (0, QPoint{ 25, SLIDER_MAX / 4 });

        QCOMPARE (slider.value (), SLIDER_MAX / 2);
        }
    };

QTEST_MAIN (TouchSliderTest)

#include "touchslidertest.moc"
