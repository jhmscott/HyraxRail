/**
 * @file        layout/group.cpp
 * @brief       Group box triggering components. One per controller
 * @author      Justin Scott
 * @date        2026-10-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <QBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTimer>

#include <ui/common/autogrid.hpp>
#include <ui/common/seperator.hpp>
#include <ui/common/pointedwidget.hpp>
#include <ui/common/utils.hpp>

#include <ui/routes/routebutton.hpp>
#include <ui/common/layout/group.hpp>


namespace ui::common
{


LayoutGroup::LayoutGroup (control::ControllerBase&      controller,
                          LayoutDelegate&               delegate,
                          control::controllerCapability type,
                          QWidget*                      parent) :
    QGroupBox (controller.getFriendlyName ().c_str (), parent),
    m_controller (controller),
    m_delegate (delegate),
    m_type (type)
    {
    QVBoxLayout*    layout      = new QVBoxLayout{};

    m_gridLayout = new common::AutoGridLayout
        {
        common::AutoGridLayout::expand::ROW_FIRST,
        common::AutoGridLayout::EXPAND,
        NULL
        };

    setEnabled (controller.getConnectionHealth ().isConnected ());
    refresh ();

    m_gridLayout->setAlignment (Qt::AlignTop | Qt::AlignLeft);

    layout->addItem (m_gridLayout);
    layout->addWidget (new common::Separator{ this });
    layout->addWidget (m_addLabel = new common::AddButton{ this });

    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Maximum);

    connect (m_addLabel,
            &common::AddButton::addPressed,
             this,
            &LayoutGroup::add);

    setLabels ();
    setLayout (layout);
    }

void LayoutGroup::setLabels ()
    {
    switch (m_type)
        {
        case control::CAPABILITY_ROUTE:
            {
            m_addLabel->setLabelText (tr ("Add Route"));
            break;
            }
        case control::CAPABILITY_ACTUATOR:
            { 
            m_addLabel->setLabelText (tr ("Add Switching Item"));
            break;
            }
        default:
            {
            break;
            }
        }
    }

void LayoutGroup::remove ()
    {
    LayoutButton*   btn         = static_cast<LayoutButton*> (QObject::sender ());
    QRect           geometry    = m_gridLayout->geometry ();

    m_gridLayout->removeWidget (btn);

    btn->deleteLater ();

    // Force a refresh
    m_gridLayout->setGeometry (geometry);
    }

void LayoutGroup::add ()
    {
    QWidget* btn = m_delegate.openCreate (m_controller, this);

    if (NULL != btn)
        {
        btn->setParent (this);

        m_gridLayout->addWidget (btn);
        m_gridLayout->itemAt (m_gridLayout->count () - 1)->widget ()->show ();

        // Force a refresh
        m_gridLayout->setGeometry (m_gridLayout->geometry ());
        }
    }

void LayoutGroup::refresh ()
    {
    common::clearLayout (*m_gridLayout);

    m_delegate.createButtons (m_controller,
        [&] (LayoutButton* btn) -> void
        {
        connect (btn,
                &LayoutButton::removeRequested,
                 this,
                &LayoutGroup::remove);

        btn->setParent (this);
        m_gridLayout->addWidget (btn);
        m_gridLayout->itemAt (m_gridLayout->count () - 1)->widget ()->show ();
        });

    // Force a refresh
    m_gridLayout->setGeometry (m_gridLayout->geometry ());
    }
} // namespace ui::common