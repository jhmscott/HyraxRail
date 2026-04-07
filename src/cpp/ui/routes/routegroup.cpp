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
#include <ui/common/pointedwidget.hpp>
#include <ui/routes/routegroup.hpp>

namespace ui::routes
{

class RouteButton : public QWidget
    {
public:
    RouteButton (const QString& name, QWidget* parent) :
        QWidget (parent)
        {
        QVBoxLayout* layout = new QVBoxLayout{ this };
        QPushButton* button = new common::PointedButton{ QIcon{ ":/icons/misc/path.svg" }, "", this };

        button->setFixedSize (50, 50);
        button->setIconSize (QSize{ 30, 30 });
        button->setStyleSheet ("QPushButton:pressed { background-color: royalblue; }");

        layout->addWidget (button);
        layout->addWidget (new QLabel{ name, this }, 0, Qt::AlignHCenter);

        setContentsMargins (0, 20, 0, 0);

        setLayout (layout);
        }
    };

RouteGroup::RouteGroup (control::ControllerBase& controller, QWidget* parent) :
    QGroupBox (controller.getFriendlyName ().c_str (), parent),
    m_controller (&controller)
    {
    common::AutoGridLayout* layout =
        new common::AutoGridLayout{ common::AutoGridLayout::expand::ROW_FIRST, 5, this };

    auto routes = m_controller->getRoutes ();

    for (const layout::Route& route : routes)
        {
        layout->addWidget (new RouteButton{ route.getName ().c_str (), this});
        }

    layout->setAlignment (Qt::AlignTop | Qt::AlignLeft);

    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Maximum);

    setLayout (layout);
    }


}
