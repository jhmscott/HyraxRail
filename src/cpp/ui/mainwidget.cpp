/**
 * @file        mainwidget.cpp
 * @brief       Top level widget
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/mainwidget.hpp>
#include <ui/actuators/actuatordelegate.hpp>
#include <ui/clock/clockpanel.hpp>
#include <ui/common/layout/panel.hpp>
#include <ui/config/configpanel.hpp>
#include <ui/sensors/sensorpanel.hpp>
#include <ui/trains/dualcontrol.hpp>

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


MainWidget::MainWidget (control::ControllerManager& controllers, QWidget* parent) :
    common::SchemeTabWidget (parent),
    m_controllers (controllers),
    m_automations (new control::AutomationManager{ this }),
    m_routeDel (*m_automations),
    m_actuatorDel (*m_automations)
    {
#ifdef Q_OS_ANDROID
    setTabPosition (QTabWidget::South);
#endif // Q_OS_ANDROID

    setUsesScrollButtons (false);

    addTab (new trains::DualControlWidget{ m_controllers, this },
            "misc/train");
    addTab (new common::LayoutPanel{ m_controllers, m_actuatorDel, control::CAPABILITY_ACTUATOR, this },
            "misc/split");
    addTab (new sensors::SensorPanel{ this },
            "misc/train-track");
    addTab (new common::LayoutPanel{ m_controllers, m_routeDel, control::CAPABILITY_ROUTE, this },
            "misc/path");
    addTab (new clock::ClockPanel{ m_controllers, *m_automations, this },
            "misc/clock");
    addTab (new config::ConfigPanel{ m_controllers, this},
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
    saveControllersToUserSettings (m_controllers);
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
