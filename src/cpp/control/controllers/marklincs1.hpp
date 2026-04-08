/**
 * @file        controller/marklincs1.hpp
 * @brief       Controller implementation for the Märklin
 *              Central Station 1. Model number 60212
 *              This supports the oirginal fimrware,
 *              not the "reloaded" firmware from ESU
 * @author      Justin Scott
 * @date        2026-02-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <control/controllers/base.hpp>
#include <control/protocols/ecos.hpp>

#include <utils/string.hpp>

#include <array>

namespace control
{

class MarklinCS1 : public ControllerBase
    {
    CONTROLLER_DEFINE (MarklinCS1, "Märklin Central Station 1", ECoSProtocol);
public:
    MarklinCS1 (const std::string&                 friendlyName,
                std::unique_ptr<ProtocolBase>&&    proto);

    ~MarklinCS1 ();

    virtual std::vector<layout::Locomotive> getLocomotives () const override;

    virtual std::vector<layout::Actuator> getActuators () const override;

    virtual std::vector<layout::Route> getRoutes () const override;

    virtual void createRoute (const std::string&                name,
                              const std::vector<routeMember>&   actuators) override;

    virtual void eStop (bool stop) override;

    virtual bool isEStopped () override;

private:
    template<class T>
    std::vector<T> getSwitchingItems () const;

    virtual void setSpeed (size_t id, int8_t speed) override;

    virtual void setFunc (size_t id, uint8_t func, bool enable) override;

    virtual void requestControl (size_t id) override;

    virtual void releaseControl (size_t id) override;

    virtual void requestActuatorControl (size_t id) override;

    virtual void releaseActuatorControl (size_t id) override;

    virtual void requestRouteControl (size_t id) override;

    virtual void releaseRouteControl (size_t id) override;

    virtual std::vector<layout::funcInfo> getFunctions (size_t id) const override;

    virtual void setActuator (size_t id, bool val) override;

    virtual void setRoute (size_t id) override;

    };

}