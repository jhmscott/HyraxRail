#include <ui/routes/editroute.hpp>
#include <ui/routes/routebutton.hpp>
#include <ui/routes/routedelegate.hpp>

namespace ui::routes
{
common::LayoutButton* RouteDelegate::openCreate (control::ControllerBase&  controller,
                                                 QWidget*                  parent)
    {
    common::LayoutButton*   btn = NULL;
    layout::Route           route;

    EditRouteDialog dlg{ controller, parent };

    if (QDialog::Accepted == dlg.exec ())
        {
        route = controller.createRoute (dlg.getName (),
                                        dlg.getActuators ());
        btn = makeButton (controller, route);
        }

    return btn;
    }

void RouteDelegate::createButtons (control::ControllerBase&                    controller,
                                   std::function<void (common::LayoutButton*)> addToLayout)
    {
    for (const layout::Route& route : controller.getRoutes ())
        {
        addToLayout (makeButton (controller, route));
        }
    }

common::LayoutButton* RouteDelegate::makeButton (control::ControllerBase&   controller,
                                                 const layout::Route&       route)
    {
    return new RouteButton{ route, controller, m_automations, NULL };
    }

} // namespace ui::routes
