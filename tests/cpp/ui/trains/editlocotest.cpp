/**
 * @file        common/editlocotest.hpp
 * @brief       Test suite for the Locomotive edit dialog
 * @author      Justin Scott
 * @date        2026-08-23
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <layout/locomotive.hpp>

#include <testutils/mockcontroller.hpp>

#include <ui/trains/editloco.hpp>

#include <QLabel>
#include <QtTest>

Q_DECLARE_METATYPE (layout::protocolMask);
Q_DECLARE_METATYPE (layout::trackProtocol);

using namespace ui::trains;

///////////////////////////////////////////////////////////////////////////////
/// Get the protocol dropdown
///
/// @param[in]  dlg     Edit dialog
///
/// @return     Protocol dropdown, or NULL if not found
///
///////////////////////////////////////////////////////////////////////////////
static ui::common::SchemeComboBox* getProtocolDropdown (const EditLocoDialog& dlg)
    {
    auto combobox = dlg.findChild<QComboBox*> ("EditLocoProtocol", Qt::FindDirectChildrenOnly);

    return static_cast<ui::common::SchemeComboBox*> (combobox);
    }

///////////////////////////////////////////////////////////////////////////////
/// Assert that the protocol dropdown contains the correct protocols
///
/// @param[in]  dlg         Edit dialog
/// @param[in]  protocols   Expected protocol
///
///////////////////////////////////////////////////////////////////////////////
static void assertProtocols (const EditLocoDialog& dlg, layout::protocolMask protocols)
    {
    auto* protoCb = getProtocolDropdown (dlg);

    QCOMPARE_NE (protoCb, NULL);

    int ii = 0;

    for (int jj = 0; jj < layout::TRACK_PROTO_UNKNOWN; ++jj)
        {
        if (protocols[jj])
            {
            if (ii >= protoCb->count ())
                {
                QFAIL ("More enabled protocols than in combobox");
                }

            layout::protocolInfo proto = layout::PROTOCOLS[jj];

            QCOMPARE (protoCb->itemText (ii), proto.description);
            QCOMPARE (protoCb->itemSchemeIcon (ii), proto.icon);

            ++ii;
            }
        }

    QCOMPARE (ii, protoCb->count ());
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the Locomotive edit dialog
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class EditLocoTest : public QObject
    {
    Q_OBJECT
public:
    EditLocoTest () :
        controllers (NULL)
        {}

private:
    control::ControllerManager controllers;  ///< List of controllers for testing

private slots:

    ///////////////////////////////////////////////////////////////////////////////
    /// Test case init; run before each test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void init ()
        {
        controllers.append (
            testutils::mockedControllerInfo ("Edit Loco Controller"));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test case cleanup; run after each test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void cleanup ()
        {
        controllers.clear ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for trackProtocolMetaTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void trackProtocolMetaTest_data ()
        {
        QTest::addColumn<layout::protocolMask> ("protocols");

        for (uint64_t ii = 1; ii < (1 << layout::TRACK_PROTO_UNKNOWN); ++ii)
            {
            QTest::addRow ("Protocol Mask %llu", ii) << layout::protocolMask{ ii };
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests the track protocol dropdown matches the controller's meta class
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void trackProtocolMetaTest ()
        {
        QFETCH (layout::protocolMask, protocols);

        layout::LocomotiveControllerMetaClass metaClass{ protocols };

        auto& controller = static_cast<testutils::MockController&> (controllers[0]);

        fakeit::When (Method (*controller.locomotiveController,
                              getLocoMetaClass)).AlwaysReturn (metaClass);

        EditLocoDialog dlg{ controllers, NULL, &controller };


        QCOMPARE (dlg.windowTitle (), "Add Locomotive");

        assertProtocols (dlg, protocols);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for changeControllerTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void changeControllerTest_data ()
        {
        QTest::addColumn<layout::protocolMask> ("protocols");

        for (uint64_t ii = 1; ii < (1 << layout::TRACK_PROTO_UNKNOWN) - 1; ++ii)
            {
            QTest::addRow ("Protocol Mask %llu", ii) << layout::protocolMask{ ii };
            }
        }


    ///////////////////////////////////////////////////////////////////////////////
    /// Tests changing the controller combobox, and asserting protocols are updated
    /// correctly
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void changeControllerTest ()
        {
        QFETCH (layout::protocolMask, protocols);

        layout::protocolMask iprotocols = ~protocols;

        layout::LocomotiveControllerMetaClass metaClass1{ protocols };
        layout::LocomotiveControllerMetaClass metaClass2{ iprotocols };

        controllers.append (
            testutils::mockedControllerInfo ("Edit Loco Controller 2"));

        auto& controller  = static_cast<testutils::MockController&> (controllers[0]);
        auto& controller2 = static_cast<testutils::MockController&> (controllers[1]);

        fakeit::When (Method (*controller.locomotiveController,
                              getLocoMetaClass)).AlwaysReturn (metaClass1);

        fakeit::When (Method (*controller2.locomotiveController,
                              getLocoMetaClass)).AlwaysReturn (metaClass2);

        EditLocoDialog dlg{ controllers, NULL, &controller };

        auto* controllerWdgt = dlg.findChild<QComboBox*> ("EditLocoController",
                                                          Qt::FindDirectChildrenOnly);

        QCOMPARE (controllerWdgt->currentText (), controller.getFriendlyName ().c_str ());
        assertProtocols (dlg, protocols);

        controllerWdgt->setCurrentIndex (1);

        QCOMPARE (controllerWdgt->currentText (), controller2.getFriendlyName ().c_str ());
        assertProtocols (dlg, iprotocols);

        controllerWdgt->setCurrentIndex (0);

        QCOMPARE (controllerWdgt->currentText (), controller.getFriendlyName ().c_str ());
        assertProtocols (dlg, protocols);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test selecting protocols and confirm the address field is correctly limited
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void selectProtocolAddressTest ()
        {
        layout::LocomotiveControllerMetaClass metaClass{ layout::ALL_TRACK_PROTOCOLS };

        auto& controller = static_cast<testutils::MockController&> (controllers[0]);

        fakeit::When (Method (*controller.locomotiveController,
                              getLocoMetaClass)).AlwaysReturn (metaClass);

        EditLocoDialog dlg{ controllers, NULL, &controller };

        auto* address = dlg.findChild<QSpinBox*> (Qt::FindDirectChildrenOnly);
        auto* protoCb = getProtocolDropdown (dlg);

        QCOMPARE_NE (address, NULL);
        QCOMPARE_NE (protoCb, NULL);

        QCOMPARE (address->minimum (), 1);
        QCOMPARE (address->maximum (), layout::PROTOCOLS[0].numAddresses);

        for (int ii = 1; ii < layout::TRACK_PROTO_UNKNOWN; ++ii)
            {
            protoCb->setCurrentIndex (ii);

            QCOMPARE (address->minimum (), 1);
            QCOMPARE (address->maximum (), layout::PROTOCOLS[ii].numAddresses);
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for entryTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void entryTest_data ()
        {
        static const std::string NAMES[] = { "Name 1", "Name 2" };

        QTest::addColumn<layout::trackProtocol> ("protocol");
        QTest::addColumn<std::string> ("name");
        QTest::addColumn<uint> ("address");
        QTest::addColumn<int> ("controllerNum");

        for (uint ii = 0; ii < layout::TRACK_PROTO_UNKNOWN; ++ii)
            {
            for (const std::string& name : NAMES)
                {
                for (int controllerNum = 0; controllerNum < 2; ++controllerNum)
                    {
                    layout::trackProtocol proto = static_cast<layout::trackProtocol> (ii);
                    QTest::addRow ("Protocol = %u, Name = %s, min address",
                                   ii,
                                   name.c_str ())
                        << proto
                        << name
                        << 1u
                        << controllerNum;

                    QTest::addRow ("Protocol = %u, Name = %s, max address",
                                   ii,
                                   name.c_str ())
                        << proto
                        << name
                        << layout::PROTOCOLS[ii].numAddresses
                        << controllerNum;


                    QTest::addRow ("Protocol = %u, Name = %s, mid address",
                                   ii,
                                   name.c_str ())
                        << proto
                        << name
                        << layout::PROTOCOLS[ii].numAddresses / 2
                        << controllerNum;
                    }
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test entering data in the dialog
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void entryTest ()
        {
        QFETCH (layout::trackProtocol,  protocol);
        QFETCH (std::string,            name);
        QFETCH (uint,                   address);
        QFETCH (int,                    controllerNum);

        layout::LocomotiveControllerMetaClass metaClass{ layout::ALL_TRACK_PROTOCOLS };

        controllers.append (
            testutils::mockedControllerInfo ("Edit Loco Controller 2"));

        auto& controller = static_cast<testutils::MockController&> (controllers[0]);
        auto& controller2 = static_cast<testutils::MockController&> (controllers[1]);

        fakeit::When (Method (*controller.locomotiveController,
                              getLocoMetaClass)).AlwaysReturn (metaClass);

        fakeit::When (Method (*controller2.locomotiveController,
                              getLocoMetaClass)).AlwaysReturn (metaClass);

        EditLocoDialog dlg{ controllers, NULL, &controller };

        auto* controllerWdgt    = dlg.findChild<QComboBox*> ("EditLocoController",
                                                             Qt::FindDirectChildrenOnly);
        auto* nameWdgt          = dlg.findChild<QLineEdit*> ("EditLocoName",
                                                             Qt::FindDirectChildrenOnly);
        auto* protocolWdgt      = getProtocolDropdown (dlg);
        auto* addressWdgt       = dlg.findChild<QSpinBox*> ("EditLocoAddress",
                                                            Qt::FindDirectChildrenOnly);

        QCOMPARE_NE (controllerWdgt,    NULL);
        QCOMPARE_NE (nameWdgt,          NULL);
        QCOMPARE_NE (protocolWdgt,      NULL);
        QCOMPARE_NE (addressWdgt,       NULL);

        controllerWdgt->setCurrentIndex (controllerNum);
        protocolWdgt->setCurrentIndex (protocol);

        QTest::keyClicks (nameWdgt,     name.c_str ());
        QTest::keyClick  (addressWdgt,  Qt::Key_Delete);
        QTest::keyClicks (addressWdgt,  QString::number (address));

        QCOMPARE (dlg.getController (), &controllers[controllerNum]);
        QCOMPARE (dlg.getName (),       name);
        QCOMPARE (dlg.getProtocol(),    protocol);
        QCOMPARE (dlg.getAddress (),    address);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Pass a locomotive to edit, and confirm the fields are populated from the
    /// locomotive's data
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void editLocomotiveTest ()
        {
        static const std::string            NAME    = "Test Loco";
        static const layout::trackProtocol  PROTO   = layout::TRACK_PROTO_MM14;
        static const uint                   ADDR    = 79;
        static const size_t                 ID      = 56768;

        layout::LocomotiveControllerMetaClass metaClass{ layout::ALL_TRACK_PROTOCOLS };

        auto& controller = static_cast<testutils::MockController&> (controllers[0]);

        fakeit::When (Method (*controller.locomotiveController,
                              getLocoMetaClass)).AlwaysReturn (metaClass);

        layout::Locomotive loco{ &controller,
                                 NAME,
                                 PROTO,
                                 ADDR,
                                 ID };

        EditLocoDialog dlg{ controllers, NULL, loco };

        QCOMPARE (dlg.windowTitle (), "Edit Locomotive");

        auto* label = dlg.findChild<QLabel*> ("EditLocoControllerLabel");

        QCOMPARE_NE (label, NULL);
        QCOMPARE (label->text (), controller.getFriendlyName ().c_str ());

        QCOMPARE (dlg.getName (),       NAME);
        QCOMPARE (dlg.getProtocol(),    PROTO);
        QCOMPARE (dlg.getAddress (),    ADDR);
        }
    };

QTEST_MAIN (EditLocoTest)

#include "editlocotest.moc"
