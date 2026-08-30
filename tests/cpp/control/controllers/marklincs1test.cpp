/**
 * @file        controlllers/marklincs1test.hpp
 * @brief       Test suite for the MarklinCS1 Controller
 * @author      Justin Scott
 * @date        2026-08-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <control/controllers/marklincs1.hpp>

#include <testutils/ext/fakeit.hpp>

#include <QtTest>

using namespace control;

using MockProtocol = fakeit::Mock<ECoSProtocol>;

static uint16_t                 ID          = 1234;

static std::string              OLD_NAME    = "Old Loco Name";
static uint                     OLD_ADDR    = 5;
static layout::trackProtocol    OLD_PROTO   = layout::TRACK_PROTO_DCC128;

static std::string              NEW_NAME    = "New Loco Name";
static uint                     NEW_ADDR    = 6;
static layout::trackProtocol    NEW_PROTO   = layout::TRACK_PROTO_MFX;

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the Marklin CS1 controller class
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class MarklinCS1Test : public QObject
    {
    Q_OBJECT
private:
    std::unique_ptr<MarklinCS1>                 m_controller;   ///< Controller under test
    std::unique_ptr<MockProtocol>               m_protocol;     ///< mocked protocol
    std::vector<std::vector<ECoSProtocol::arg>> m_args;         ///< Argument lists from controller requets

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Init the test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void init ()
        {
        utils::device::deviceInfo device{
            utils::device::TYPE_UDP,
            utils::device::socketInfo{
                utils::device::HostInfo::fromString ("127.0.0.1",
                                                     utils::device::HostInfo::type::IP),
                12345 } };

        std::unique_ptr<ECoSProtocol> protocol{ new ECoSProtocol{ device } };

        m_protocol.reset (new MockProtocol{ *protocol });
        m_controller.reset (new MarklinCS1{ "Test Controller", std::move (protocol) });

        // Can't verify inializer lists with fakeit, so capture it for verificiation later
        fakeit::When (Method (*m_protocol, makeRequest)).
            Do ([this] (ECoSProtocol::cmd                           cmd,
                        ECoSProtocol::dynamicId                     id,
                        std::initializer_list<ECoSProtocol::arg>    args) -> ECoSProtocol::reply
            {
            m_args.emplace_back ();
            m_args.back ().reserve (args.size ());

            std::copy (args.begin (),
                       args.end (),
                       std::back_inserter (m_args.back ()));

            return {};
            });
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Cleanup after test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void cleanup ()
        {
        m_protocol.reset ();
        m_controller.reset ();
        m_args.clear ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting the locomotive name
    ///
    /// @see    control::MarklinCS1::setLocomotiveName()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLocoNameTest ()
        {
        layout::Locomotive loco{ m_controller.get (),
                                 OLD_NAME,
                                 OLD_PROTO,
                                 OLD_ADDR,
                                 ID };

        loco.setName (NEW_NAME);

        m_controller->waitForMessages ();

        fakeit::Verify (Method (*m_protocol, makeRequest).
                        Using (ECoSProtocol::CMD_SET, ID, fakeit::_));

        QCOMPARE (m_args.size (), 1);
        QCOMPARE (m_args[0].size (), 1);
        QCOMPARE (m_args[0][0].type,
                  ECoSProtocol::ARG_NAME);
        QCOMPARE (m_args[0][0].val,
                  utils::str::quote (NEW_NAME));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting the locomotive address
    ///
    /// @see    control::MarklinCS1::setLocomotiveAddress()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLocoAddressTest ()
        {
        layout::Locomotive loco{ m_controller.get (),
                                 OLD_NAME,
                                 OLD_PROTO,
                                 OLD_ADDR,
                                 ID };

        loco.setAddress (NEW_ADDR);

        m_controller->waitForMessages ();

        fakeit::Verify (Method (*m_protocol, makeRequest).
                        Using (ECoSProtocol::CMD_SET, ID, fakeit::_));

        QCOMPARE (m_args.size (), 1);
        QCOMPARE (m_args[0].size (), 1);
        QCOMPARE (m_args[0][0].type,
                  ECoSProtocol::ARG_ADDR);
        QCOMPARE (m_args[0][0].val,
                  std::to_string (NEW_ADDR));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting the locomotive track protocol
    ///
    /// @see    control::MarklinCS1::setLocomotiveProtocol()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLocoProtoTest ()
        {
        layout::Locomotive loco{ m_controller.get (),
                                 OLD_NAME,
                                 OLD_PROTO,
                                 OLD_ADDR,
                                 ID };

        loco.setProtocol (NEW_PROTO);

        m_controller->waitForMessages ();

        fakeit::Verify (Method (*m_protocol, makeRequest).
                        Using (ECoSProtocol::CMD_SET, ID, fakeit::_));

        QCOMPARE (m_args.size (), 1);
        QCOMPARE (m_args[0].size (), 1);
        QCOMPARE (m_args[0][0].type,
                  ECoSProtocol::ARG_PROTOCOL);
        QCOMPARE (m_args[0][0].val,
                  "MFX");
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test creation of a locomotive
    ///
    /// @see    control::MarklinCS1::createLocomotive()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void createLocoTest ()
        {
        fakeit::When (Method (*m_protocol, makeRequest)).AlwaysReturn (
            ECoSProtocol::reply{ ECoSProtocol::REPLY_OK,
                               { ECoSProtocol::replyLine{
                                        ECoSProtocol::ID_LOCOMOTIVES,
                                        ECoSProtocol::arg{ ECoSProtocol::ARG_ID,
                                                            std::to_string (ID) } } } });

        auto loco = m_controller->createLocomotive (OLD_NAME, OLD_PROTO, OLD_ADDR);

        QCOMPARE (loco.getId (),        ID);
        QCOMPARE (loco.getName (),      OLD_NAME);
        QCOMPARE (loco.getProtocol (),  OLD_PROTO);
        QCOMPARE (loco.getAddress (),   OLD_ADDR);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test removal of a locomotive
    ///
    /// @see    control::MarklinCS1::removeLocomotive()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void removeLocoTest ()
        {
        layout::Locomotive loco{ m_controller.get (),
                                 OLD_NAME,
                                 OLD_PROTO,
                                 OLD_ADDR,
                                 ID };

        loco.remove ();

        m_controller->waitForMessages ();

        fakeit::Verify (Method (*m_protocol, makeRequest).
                        Using (ECoSProtocol::CMD_DELETE, ID, fakeit::_));
        }
    };

QTEST_GUILESS_MAIN (MarklinCS1Test)

#include "marklincs1test.moc"
