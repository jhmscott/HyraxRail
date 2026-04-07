/**
 * @file        config/controllergroup.cpp
 * @brief       Group box for the controller settings
 * @author      Justin Scott
 * @date        2026-03-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/hovericon.hpp>
#include <ui/common/pointedwidget.hpp>
#include <ui/common/seperator.hpp>

#include <ui/config/controllergroup.hpp>
#include <ui/controllerinfo.hpp>
#include <ui/config/dialog.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QLabel>

namespace ui::config
{

ControllerGroup::ControllerGroup (const std::vector<control::ControllerBase*>&  controllers,
                                  QWidget*                                      parent) :
    QGroupBox ("Controllers", parent),
    m_controllers (controllers)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };

    setLayout (layout);

    for (control::ControllerBase* controller : controllers)
        {
        addControllerInfo (*controller);
        }

    QHBoxLayout* addController  = new QHBoxLayout{ this };
    QPushButton* plusIcon       = new common::PointedButton{ QIcon{ ":/icons/misc/plus.svg" }, "", this};
    QLabel*      plusLabel      = new QLabel{ "New Controller", this };
    QFont        font           = QApplication::font ();

    font.setPixelSize (16);

    plusIcon->setIconSize ((QSize{ 20, 20 }));
    plusIcon->setSizePolicy (QSizePolicy::Maximum,
                             QSizePolicy::Maximum);

    plusLabel->setFont (font);

    common::makeFrameless (*plusIcon);

    addController->addWidget (plusIcon);
    addController->addWidget (plusLabel);
    addController->setAlignment (Qt::AlignLeft);

    layout->addLayout (addController);
    m_hasAddController = true;

    connect (plusIcon,
            &QPushButton::released,
             this,
            &ControllerGroup::addController);

    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Maximum);
    }

void ControllerGroup::addControllerInfo (control::ControllerBase& controller)
    {
    QVBoxLayout* myLayout = static_cast<QVBoxLayout*> (layout ());

    ui::ControllerInfo* info = new ui::ControllerInfo{ &controller, this, true };

    info->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    if (m_hasAddController)
        {
        myLayout->insertWidget (myLayout->count () - 1, info);
        myLayout->insertWidget (myLayout->count () - 1, new common::Separator{ this });
        }
    else
        {
        myLayout->addWidget (info);
        myLayout->addWidget (new common::Separator{ this });
        }

    connect (info,
            &ui::ControllerInfo::deleted,
             this,
            &ControllerGroup::onControllerDeleted);
    }


void ControllerGroup::addController ()
    {
    Dialog dlg{ this };

    if (QDialog::Accepted == dlg.exec ())
        {
        emit controllerAdded (dlg.createController ());
        }
    }


void ControllerGroup::onControllerDeleted (control::ControllerBase& controller)
    {
    ControllerInfo* info    = static_cast<ControllerInfo*> (sender ());
    int             idx     = layout ()->indexOf (info);

    // Remove both the controller info and the spacer
    common::removeWidgetFromLayout (*layout (), idx + 1);
    common::removeWidgetFromLayout (*layout (), idx);

    emit controllerDeleted (controller);
    }
}
