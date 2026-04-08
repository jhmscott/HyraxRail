/**
 * @file        routes/routegroup.hpp
 * @brief       Group box triggering routes. One per controller
 * @author      Justin Scott
 * @date        2026-03-27
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <ui/common/autogrid.hpp>
#include <ui/common/seperator.hpp>
#include <ui/common/pointedwidget.hpp>
#include <ui/common/utils.hpp>
#include <ui/routes/editroute.hpp>
#include <ui/routes/routegroup.hpp>


namespace ui::routes
{

class RouteButton : public QWidget
    {
public:
    RouteButton (const layout::Route& route, QWidget* parent) :
        QWidget (parent),
        m_route (new layout::Route{ route })
        {
        QVBoxLayout* layout = new QVBoxLayout{ this };
        QPushButton* button = new common::PointedButton{ QIcon{ ":/icons/misc/path.svg" }, "", this };

        m_route->request ();

        button->setFixedSize (50, 50);
        button->setIconSize (QSize{ 30, 30 });
        button->setStyleSheet ("QPushButton:pressed { background-color: royalblue; }");

        layout->addWidget (button);
        layout->addWidget (new QLabel{ m_route->getName ().c_str (), this}, 0, Qt::AlignHCenter);

        setContentsMargins (0, 20, 0, 0);

        connect (button,
                 &QPushButton::released,
                  m_route,
                 &layout::Route::set);

        setLayout (layout);
        }

    ~RouteButton () { delete m_route; }
private:
    layout::Route* m_route;

    };

RouteGroup::RouteGroup (control::ControllerBase& controller, QWidget* parent) :
    QGroupBox (controller.getFriendlyName ().c_str (), parent),
    m_controller (&controller)
    {
    QVBoxLayout*            layout      = new QVBoxLayout{ this };
    QHBoxLayout*            addLayout   = new QHBoxLayout{ this };
    common::AutoGridLayout* gridLayout  =
        new common::AutoGridLayout{ common::AutoGridLayout::expand::ROW_FIRST, 5, this };

    auto routes = m_controller->getRoutes ();

    for (const layout::Route& route : routes)
        {
        gridLayout->addWidget (new RouteButton{ route, this});
        }

    gridLayout->setAlignment (Qt::AlignTop | Qt::AlignLeft);

    common::PointedButton* addBtn = new common::PointedButton{ QIcon{ ":/icons/misc/plus.svg" }, "", this };

    common::makeFrameless (*addBtn);

    addLayout->addWidget (addBtn, 0, Qt::AlignLeft);
    addLayout->addWidget (new QLabel{ "Add Route", this }, 0, Qt::AlignLeft);
    addLayout->setAlignment (Qt::AlignLeft);

    layout->addItem (gridLayout);
    layout->addWidget (new common::Separator{ this });
    layout->addLayout (addLayout);

    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Maximum);

    connect (addBtn,
            &QPushButton::released,
             this,
            &RouteGroup::addRoute);

    setLayout (layout);
    }

void RouteGroup::addRoute ()
    {
    EditRouteDialog dlg{ *m_controller, this };

    if (QDialog::Accepted == dlg.exec ())
        {
        m_controller->createRoute (dlg.getName (), dlg.getActuators ());
        }
    }


}
