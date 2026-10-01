/**
 * @file        names.hpp
 * @brief       Common header for QObject names,
                so they can easily be identified for uniqueness
 * @author      Justin Scott
 * @date        2026-08-27
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QString>

// Edit loco dialog widget names

inline const QString OBJNAME_EDITLOCO_NAME              = "EditLocoDialog_Name";
inline const QString OBJNAME_EDITLOCO_PROTO             = "EditLocoDialog_Protocol";
inline const QString OBJNAME_EDITLOCO_ADDRESS           = "EditLocoDialog_Address";
inline const QString OBJNAME_EDITLOCO_CONTROLLER        = "EditLocoDialog_Controller";
inline const QString OBJNAME_EDITLOCO_CONTROLLER_LABEL  = "EditLocoDialog_ControllerLabel";
inline const QString OBJNAME_EDITLOCO_FUNC_DROPDOWN     = "EditLocoDialog_FuncDropdown";
inline const QString OBJNAME_EDITLOCO_ADD_FUNC          = "EditLocoDialog_AddFuncButton";
inline const QString OBJNAME_EDITLOCO_DELETE_FUNC       = "EditLocoDialog_DeleteFuncButton";
inline const QString OBJNAME_EDITLOCO_FUNC_ICON         = "EditLocoDialog_FuncIconDropdown";
inline const QString OBJNAME_EDITLOCO_FUNC_NUM          = "EditLocoDialog_FuncNumDropdown";


// Edit auto dialog widget names

inline const QString OBJNAME_EDITAUTO_NAME              = "EditAutoDialog_NameEdit";
inline const QString OBJNAME_EDITAUTO_ITEMS             = "EditAutoDialog_ItemsDropdown";
inline const QString OBJNAME_EDITAUTO_ACTIONS           = "EditAutoDialog_ActionsDropdown";
inline const QString OBJNAME_EDITAUTO_CONDITIONS        = "EditAutoDialog_ConditionDropdown";
inline const QString OBJNAME_EDITAUTO_DO_ONCE           = "EditAutoDialog_DoOnceCheck";
inline const QString OBJNAME_EDITAUTO_ENABLED           = "EditAutoDialog_EnabledCheck";


// Optional dropdown widget names

inline const QString OBJNAME_OPT_DROPDOWN_SINGLE_ITEM   = "OptionalDropdown_SingleItem";
inline const QString OBJNAME_OPT_DROPDOWN_DROPDOWN      = "OptionalDropdown_Dropdown";


// Controller settings dialog

inline const QString OBJNAME_CONFIG_DIALOG_NAME         = "ConfigDialog_ControllerName";
inline const QString OBJNAME_CONFIG_DIALOG_CONTROLLER   = "ConfigDialog_ControllerDropdown";
inline const QString OBJNAME_CONFIG_DIALOG_PROTOCOL     = "ConfigDialog_ControllerProtocol";
inline const QString OBJNAME_CONFIG_DIALOG_TRANSPORT    = "ConfigDialog_TransportProtocol";
