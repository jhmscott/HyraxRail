/**
 * @file        mainwidget.cpp
 * @brief       Top level widget
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/mainwidget.hpp>
#include <ui/actuators/actuatorpanel.hpp>
#include <ui/clock/clockpanel.hpp>
#include <ui/config/configpanel.hpp>
#include <ui/sensors/sensorpanel.hpp>
#include <ui/trains/dualcontrol.hpp>
#include <ui/routes/routepanel.hpp>

#include <utils/algorithm.hpp>
#include <utils/os.hpp>

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <QProxyStyle>
#include <QStyleHints>
#include <QStyleOptionTab>

namespace ui
{
namespace // anonymous
{

///////////////////////////////////////////////////////////////////////////////
/// Style class that ensures tab icons are centered and expand to fill the tab.
/// Used on android with the expanding tab style
///
///////////////////////////////////////////////////////////////////////////////
class CenterTabStyle : public QProxyStyle
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Draw a tab bar sub-control
    ///
    /// @param[in]  element     Which control to draw
    /// @param[in]  option      Style options
    /// @param[in]  painter     Qt Painter
    /// @param[in]  widget      Tab bar widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void drawControl (ControlElement        element,
                      const QStyleOption*   option,
                      QPainter*             painter,
                      const QWidget*        widget) const override
        {
        if (CE_TabBarTabLabel == element)
            {
            const QStyleOptionTab* tabOption;

            if (NULL == (tabOption = qstyleoption_cast<const QStyleOptionTab*>(option)))
                {
                qDebug () << "Failed to cast style option to tab";
                }
            else
                {
                QRect   rect    = tabOption->rect;
                QSize   size    = rect.size ();
                int     extent  = std::min (size.width (), size.height ()) - 12;
                QPixmap icon    = tabOption->icon.pixmap (extent);

                rect.setSize (QSize{ extent, extent });
                rect.moveCenter (tabOption->rect.center ());

                painter->drawPixmap (rect, icon);
                }
            }
        else
            {
            // Allow all other UI components to draw normally
            QProxyStyle::drawControl (element, option, painter, widget);
            }
        }
    };
} // namespace anonymous


MainWidget::MainWidget (QWidget* parent) :
    common::SchemeTabWidget (parent),
    m_controllers (new control::ControllerManager{ this }),
    m_automations (new control::AutomationManager{ this })
    {
    QSettings   settings{ QSettings::UserScope };
    int         numControllers = settings.value ("NumControllers", 0).toInt ();

#ifdef Q_OS_ANDROID
    setTabPosition (QTabWidget::South);
#endif // Q_OS_ANDROID

    for (int ii = 0; ii < numControllers; ++ii)
        {
        utils::device::deviceInfo info;

        QString controller      = "Controller" + QString::number (ii);

        std::string type        = settings.value (controller + "/type",     "").toString    ()
                                                                               .toUtf8      ()
                                                                               .toStdString ();
        std::string name        = settings.value (controller + "/name",     "").toString    ()
                                                                               .toUtf8      ()
                                                                               .toStdString ();
        std::string proto       = settings.value (controller + "/proto",    "").toString    ()
                                                                               .toUtf8      ()
                                                                               .toStdString ();
        QString     transport   = settings.value (controller + "/transport","").toString    ();
        QString     host        = settings.value (controller + "/host",     "").toString    ();
        int         port        = settings.value (controller + "/port",      0).toInt       ();
        int         hostType    = settings.value (controller + "/hostType",  0).toInt       ();

        if ("TCP" == transport)
            {
            info.type = utils::device::TYPE_TCP;
            }
        else if ("UDP" == transport)
            {
            info.type = utils::device::TYPE_UDP;
            }
        else if ("SERIAL" == transport)
            {
            info.type = utils::device::TYPE_SERIAL;
            }

        utils::device::socketInfo skt;

        skt.host = utils::device::HostInfo::fromString (host,
                                                        static_cast<utils::device::HostInfo::type> (hostType));
        skt.port = port;

        info.info = skt;

        m_controllers->append ({ type, name, proto, info });
        }

    setUsesScrollButtons (false);

    addTab (new trains::DualControlWidget{ *m_controllers, this },
            "misc/train");
    addTab (new actuators::ActuatorPanel{ *m_controllers, *m_automations, this},
            "misc/split");
    addTab (new sensors::SensorPanel{ this },
            "misc/train-track");
    addTab (new routes::RoutePanel{ *m_controllers, *m_automations, this },
            "misc/path");
    addTab (new clock::ClockPanel{ *m_controllers, *m_automations, this },
            "misc/clock");
    addTab (new config::ConfigPanel{ *m_controllers, this},
            "misc/gear");

    setTooltips ();


#ifdef Q_OS_ANDROID
    setIconSize ({ 36, 36 });

    // Fill available space
    tabBar ()->setExpanding (true);
    // needed for expanding to do anything
    tabBar ()->setDocumentMode (true);
    // Document mode left aligns icons, this ensures they're centered
    tabBar ()->setStyle (new CenterTabStyle);
#else
    setIconSize ({ 24, 24 });
#endif // Q_OS_ANDROID

    // Mac tab button sizing is a bit big for my taste
#ifdef Q_OS_MACOS
    setStyleSheet("QTabBar::tab { width: 28px; height: 28px; }");
#endif // Q_OS_MACOS
    }

MainWidget::~MainWidget ()
    {
    QSettings   settings{ QSettings::UserScope };
    int         numControllers = static_cast<int> (m_controllers->size ());

    settings.setValue ("NumControllers", numControllers);

    for (int ii = 0; ii < numControllers; ++ii)
        {
        utils::device::deviceInfo info  = (*m_controllers)[ii].getDeviceInfo ();
        utils::device::socketInfo skt   = std::get<utils::device::socketInfo> (info.info);

        QString controller      = "Controller" + QString::number (ii);
        QString transport;

        switch (info.type)
            {
            case utils::device::TYPE_SERIAL:
                {
                transport = "SERIAL";
                break;
                }
            case utils::device::TYPE_TCP:
                {
                transport = "TCP";
                break;
                }
            case utils::device::TYPE_UDP:
                {
                transport = "UDP";
                break;
                }
            default:
                {
                qWarning () << "Unexpected device type";
                break;
                }
            }

        settings.setValue (controller + "/type",
                           (*m_controllers)[ii].getMetaClass ().name.c_str ());
        settings.setValue (controller + "/name",
                           (*m_controllers)[ii].getFriendlyName ().c_str ());
        settings.setValue (controller + "/proto",
                           (*m_controllers)[ii].getProtocol ().name.c_str ());
        settings.setValue (controller + "/transport",
                           transport);
        settings.setValue (controller + "/host",
                           skt.host.toString ());
        settings.setValue (controller + "/hostType",
                           static_cast<int> (skt.host.getType ()));
        settings.setValue (controller + "/port",
                           skt.port);
        }
    }

void MainWidget::setTooltips ()
    {
    setTabToolTip (0, tr ("Trains"));
    setTabToolTip (1, tr ("Actuators"));
    setTabToolTip (2, tr ("Sensors"));
    setTabToolTip (3, tr ("Routes"));
    setTabToolTip (4, tr ("Fast Clock"));
    setTabToolTip (5, tr ("Settings"));
    }


}
