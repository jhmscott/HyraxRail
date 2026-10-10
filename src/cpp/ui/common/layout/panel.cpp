/**
 * @file        layout/panel.cpp
 * @brief       Panel class for layout components
 * @author      Justin Scott
 * @date        2026-10-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/algorithm.hpp>

#include <ui/common/utils.hpp>

#include <ui/common/layout/group.hpp>
#include <ui/common/layout/panel.hpp>


namespace ui::common
{

LayoutPanel::LayoutPanel (control::ControllerManager&   controllers,
                          LayoutDelegate&               delegate,
                          control::controllerCapability type,
                          QWidget*                      parent) :
    QWidget (parent),
    m_controllers (controllers),
    m_delegate (delegate),
    m_type (type)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };

    for (control::ControllerBase& controller : controllers)
        {
        if (controller.getMetaClass ().hasCapability (type))
            {
            layout->addWidget (new LayoutGroup{ controller, delegate, type, this });
            }
        }

    layout->setAlignment (Qt::AlignTop);

    connect (&m_controllers,
             &control::ControllerManager::controllerAdded,
              this,
             &LayoutPanel::addController);

    connect (&m_controllers,
             &control::ControllerManager::controllerDeleted,
              this,
             &LayoutPanel::remove);

    connect (&m_controllers,
             &control::ControllerManager::disconnected,
              this,
             &LayoutPanel::onDisconnect);

    connect (&m_controllers,
             &control::ControllerManager::cacheRebuilt,
              this,
             &LayoutPanel::onRefresh);

    setLayout (layout);
    }

void LayoutPanel::remove (const control::ControllerBase& controller)
    {
    ptrdiff_t idx = m_controllers.indexOf (controller);

    if (idx >= 0)
        {
        common::removeWidgetFromLayout (*layout (), idx);
        }
    }

void LayoutPanel::addController (control::ControllerBase& controller)
    {
    QVBoxLayout* myLayout = static_cast<QVBoxLayout*> (layout ());

    if (controller.getMetaClass ().hasCapability (m_type))
        {
        myLayout->addWidget (new LayoutGroup{ controller, m_delegate, m_type, this });
        }
    }

LayoutGroup* LayoutPanel::getGroupForController (const control::ControllerBase& controller)
    {
    auto groups = findChildren<LayoutGroup*> ();

    auto it = std::find_if (groups.begin (),
                            groups.end (),
                            [&] (LayoutGroup* group) -> bool
                            { return &controller == &group->getController (); });

                            
    if (groups.end () == it )
        {
        return NULL;
        }
    else    
        {
        return *it;
        }
    }

void LayoutPanel::onDisconnect (const control::ControllerBase& controller)
    {
    LayoutGroup* group = getGroupForController (controller);

    if (NULL != group)
        {
        group->setDisabled (true);
        }
    }

    
void LayoutPanel::onRefresh (const control::ControllerBase& controller)
    {
    LayoutGroup* group = getGroupForController (controller);

    if (NULL != group)
        {
        group->refresh ();
        group->setDisabled (false);
        }
    }
}
