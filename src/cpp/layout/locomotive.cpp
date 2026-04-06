/**
 * @file        layout/locomotive.cpp
 * @brief       Provides an interface for controlling a locomotive
 * @author      Justin Scott
 * @date        2026-03-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <layout/locomotive.hpp>

namespace layout
{

Locomotive::Locomotive (LocomotiveController* controller, const std::string& name, size_t id) :
    ComponentDerived<LocomotiveController> (controller, id),
    m_name (name)
    {}

void Locomotive::setSpeed (int8_t speed) { if (NULL != m_controller) { m_controller->setSpeed (m_id, speed); } }

void Locomotive::requestControl () {  if (NULL != m_controller) { m_controller->requestControl (m_id); } }

void Locomotive::releaseControl () {  if (NULL != m_controller) { m_controller->releaseControl (m_id); } }

void Locomotive::setFunc (uint8_t func, bool enable) {  if (NULL != m_controller) { m_controller->setFunc (m_id, func, enable); } }

std::vector<funcInfo> Locomotive::getFunctions () const { return NULL != m_controller ? m_controller->getFunctions (m_id) : std::vector<funcInfo>(); }
}
