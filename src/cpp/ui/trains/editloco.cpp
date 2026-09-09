/**
 * @file        tains/editloco.cpp
 * @brief       Dialog box to create or edit a locomotive
 * @author      Justin Scott
 * @date        2026-08-22
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/pointedwidget.hpp>
#include <ui/common/seperator.hpp>
#include <ui/common/utils.hpp>

#include <ui/trains/editloco.hpp>
#include <ui/trains/resources.hpp>

#include <utils/string.hpp>

#include <QBoxLayout>
#include <QGroupBox>
#include <QMessageBox>

namespace ui::trains
{

///////////////////////////////////////////////////////////////////////////////
/// Insert a function into the combobox
///
/// @param[in,out]  functions   Combo box to add to
/// @param[in]      idx         index to add at
/// @param[in]      func        Function info to add at idx
///
///////////////////////////////////////////////////////////////////////////////
static void insertFunctionToCombobox (common::SchemeComboBox&   functions,
                                      int                       idx,
                                      const layout::funcInfo&   func)
    {
    functions.insertItem (idx,
                          resources::getFunctionInfo (func.icon).icon,
                          func.uiName (),
                          QVariant::fromValue (func));

    }

EditLocoDialog::EditLocoDialog (const control::ControllerManager&   manager,
                                QWidget*                            parent,
                                const control::ControllerBase*      controller,
                                const layout::Locomotive*           loco) :
    common::FormDialog (parent),
    m_manager (manager),
    m_edit (NULL != loco)
    {
    QVBoxLayout*    layout         = new QVBoxLayout{ this };
    QHBoxLayout*    funcBarLayout  = new QHBoxLayout;

    // Build top level components
    m_form          = new QFormLayout;
    m_funcForm      = new QFormLayout;

    m_functions     = new common::SchemeComboBox{ this };
    m_funcBar       = new QWidget;

    m_functionGroup = new QGroupBox{ this };
    m_functionGroup->setLayout (m_funcForm);

    // Create the function bar
    // [ Drop down ] [ Delete] [ Add ]

    funcBarLayout->addWidget (m_functions);
    funcBarLayout->addWidget (m_trash = new common::PointedIconButton{ "misc/trash", this });
    funcBarLayout->addWidget (m_plus  = new common::PointedIconButton{ "misc/plus", this });
    funcBarLayout->setContentsMargins (0, 0, 0, 0);

    m_functions->setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_trash->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);
    m_plus->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    common::makeFrameless (*m_trash);
    common::makeFrameless (*m_plus);

#ifdef Q_OS_ANDROID
    m_trash->setIconSize (QSize{ 20, 20 });
    m_plus->setIconSize  (QSize{ 20, 20 });
#endif // Q_OS_ANDROID

    m_funcBar->setLayout (funcBarLayout);
    m_funcBar->setContentsMargins (0, 0, 0, 0);

    // Setup the controller field
    // Dependent on add/edit

    if (NULL == loco) // Add mode
        {
        m_form->addRow (new QLabel{ this },
                        m_controllerWdgt = m_controllerCb = new QComboBox{ this });

        for (const control::ControllerBase& controllerToAdd : m_manager)
            {
            auto ud = QVariant::fromValue<const control::ControllerBase*> (&controllerToAdd);
            m_controllerCb->addItem (controllerToAdd.getFriendlyName ().c_str (), ud);
            }

        if (NULL != controller)
            {
            common::setComboBoxIndexByUserData (*m_controllerCb, controller);
            }
        else
            {
            controller = &m_manager[0];
            }

        m_controllerCb->setObjectName ("EditLocoController");

        connect (m_controllerCb,
                &QComboBox::currentIndexChanged,
                 this,
                &EditLocoDialog::onControllerChange);
        }
    else // edit mode
        {
        m_controllerCb = NULL;
        m_form->addRow (new QLabel{ this },
                        m_controllerWdgt = new QLabel{ controller->getFriendlyName ().c_str (), this });

        m_controllerWdgt->setObjectName ("EditLocoControllerLabel");

        for (const layout::funcInfo func : loco->getFunctions ())
            {
            insertFunctionToCombobox (*m_functions, m_functions->count (), func);
            }
        }

    // Populate form fields

    m_form->addRow (new QLabel{ this }, m_name      = new QLineEdit{ this });
    m_form->addRow (new QLabel{ this }, m_protocol  = new common::SchemeComboBox{ this });
    m_form->addRow (new QLabel{ this }, m_address   = new QSpinBox{ this });
    m_form->addRow (new QLabel{ this }, m_funcBar);

    m_funcForm->addRow (new QLabel{ this }, m_funcIcons     = new common::TieredDropdown{ this });
    m_funcForm->addRow (new QLabel{ this }, m_funcNumbers   = new QComboBox{ this });

    // Set object names

    m_name->setObjectName     ("EditLocoName");
    m_protocol->setObjectName ("EditLocoProtocol");
    m_address->setObjectName  ("EditLocoAddress");

    // Fill dropdowns

    populateProtocols (*controller);
    populateFunctionIcons ();
    populateFunctionNumbers ();

    m_funcIcons->setCurrentIndex (1);

    // Setup top level layout

    layout->addLayout (m_form);
    layout->addWidget (m_functionGroup);
    layout->addWidget (m_buttons, 0, Qt::AlignHCenter | Qt::AlignBottom);

    // Populate fields from loco (edit mode only)

    if (NULL != loco)
        {
        m_name->setText     (loco->getName ().c_str ());
        m_address->setValue (loco->getAddress ());

        common::setComboBoxIndexByUserData (*m_protocol, loco->getProtocol ());
        }

    // Restrict fields

    limitAddress ();
    updateButtonStates ();

    m_name->setValidator (new QRegularExpressionValidator{
                                    QRegularExpression{ utils::str::NON_EMPTY_REGEX } });

    // Signals -> slots

    connect (m_name,
            &QLineEdit::textChanged,
             this,
            &common::FormDialog::inputChanged);

    connect (m_address,
            &QSpinBox::valueChanged,
             this,
            &common::FormDialog::inputChanged);

    connect (m_protocol,
            &QComboBox::currentIndexChanged,
             this,
            &EditLocoDialog::onProtocolChange);

    connect (m_functions,
            &QComboBox::currentIndexChanged,
             this,
            &EditLocoDialog::onFunctionChange);

    connect (m_funcIcons,
            &QComboBox::currentIndexChanged,
             this,
            &EditLocoDialog::readFunctionFields);

    connect (m_funcNumbers,
            &QComboBox::currentIndexChanged,
             this,
            &EditLocoDialog::readFunctionFields);

    connect (m_trash,
            &common::PointedButton::released,
             this,
            &EditLocoDialog::deleteFunction);

    connect (m_plus,
            &common::PointedButton::released,
             this,
            &EditLocoDialog::addFunction);

    // Set text fields and window icon
    setLabels ();
    setWindowIcon ("misc/train");

    // Assign layout
    setLayout (layout);

    // Initial input changed signal, so the validity of the form is evaluated
    emit inputChanged ();
    }

std::vector<layout::funcInfo> EditLocoDialog::getFunctions () const
    {
    std::vector<layout::funcInfo>   functions;
    const int                       numFuncs = m_functions->count ();

    functions.reserve (numFuncs);

    for (int ii = 0; ii < numFuncs; ++ii)
        {
        functions.push_back (m_functions->itemData (ii).value<layout::funcInfo> ());
        }

    return functions;
    }

bool EditLocoDialog::hasAcceptableInput () const
    {
    return m_name->hasAcceptableInput () &&
           m_address->hasAcceptableInput ();
    }

void EditLocoDialog::setLabels ()
    {
    setWindowTitle (m_edit ? tr ("Edit Locomotive") : tr ("Add Locomotive"));

    m_functionGroup->setTitle (tr ("Function"));

    common::setFormRowText (*m_form, *m_controllerWdgt, tr ("Controller"));
    common::setFormRowText (*m_form, *m_name,           tr ("Name"));
    common::setFormRowText (*m_form, *m_protocol,       tr ("Protocol"));
    common::setFormRowText (*m_form, *m_address,        tr ("Address"));
    common::setFormRowText (*m_form, *m_funcBar,        tr ("Functions"));

    common::setFormRowText (*m_funcForm, *m_funcIcons,  tr ("Icon"));
    common::setFormRowText (*m_funcForm, *m_funcNumbers,tr ("Number"));

    common::refreshComboboxItems (*m_funcIcons,
                                  std::bind (&EditLocoDialog::populateFunctionIcons,
                                             this));

    for (int ii = 0; ii < m_functions->count (); ++ii)
        {
        auto info = m_functions->itemData (ii).value<layout::funcInfo> ();

        m_functions->setItemText (ii, info.uiName ());
        }
    }

void EditLocoDialog::populateProtocols (const control::ControllerBase& controller)
    {
    const auto& metaClass = controller.getLocoMetaClass ();

    m_controller = &controller;

    for (int ii = 0; ii < layout::TRACK_PROTO_UNKNOWN; ++ii)
        {
        if (metaClass.protocols[ii])
            {
            m_protocol->addItem (layout::PROTOCOLS[ii].icon,
                                 layout::PROTOCOLS[ii].description,
                                 QVariant::fromValue (
                                     static_cast<layout::trackProtocol> (ii)));
            }
        }
    }

void EditLocoDialog::limitAddress ()
    {
    const layout::trackProtocol protocol =
        m_protocol->currentData ().value<layout::trackProtocol> ();

    m_address->setMinimum (1);
    m_address->setMaximum (layout::PROTOCOLS[protocol].numAddresses);
    }

void EditLocoDialog::populateFunctionIcons ()
    {
    auto addItemRange = [&] (layout::funcInfo::icon_t start,
                             layout::funcInfo::icon_t end)
        {
        auto addItem = [&] (layout::funcInfo::icon_t icon) -> void
            {
            auto func = resources::getFunctionInfo (icon);

            m_funcIcons->addChildItem (func.name,
                                       QVariant::fromValue (icon),
                                       func.icon);
            };

        for (layout::funcInfo::icon_t icon : utils::algorithm::EnumRange{ start, end })
            {
            addItem (icon);
            }
        };

    m_funcIcons->addParentItem (tr ("Lights"), "functions/lightbulb");

    addItemRange (layout::funcInfo::ICON_FUNC_LIGHT_START,
                  layout::funcInfo::ICON_FUNC_LIGHT_END);

    m_funcIcons->addParentItem (tr ("Sounds"), "functions/speaker-high");

    addItemRange (layout::funcInfo::ICON_FUNC_SOUND_START,
                  layout::funcInfo::ICON_FUNC_SOUND_END);

    m_funcIcons->addParentItem (tr ("Miscellaneous"), "misc/square-function");

    addItemRange (layout::funcInfo::ICON_FUNC_MISC_START,
                  layout::funcInfo::ICON_FUNC_MISC_END);
    }

void EditLocoDialog::populateFunctionNumbers ()
    {
    std::vector<layout::funcInfo>   functions;
    const layout::funcInfo          info    = m_functions->currentData ().value<layout::funcInfo> ();

    auto funcAvailable = [&functions] (int func)
        {
        return functions.end () ==
            std::find_if (functions.begin (),
                          functions.end (),
                          [func] (const layout::funcInfo& info) -> bool
                          { return func == info.id; });
        };

    int numUsedFunc     = m_functions->count ();
    int numTotalFunc    = getMaxFunctions ();

    functions.reserve (numUsedFunc);

    for (int ii = 0; ii < numUsedFunc; ++ii)
        {
        functions.push_back (m_functions->itemData (ii).value<layout::funcInfo> ());
        }

    for (int ii = 0; ii < numTotalFunc; ++ii)
        {
        if (funcAvailable (ii) || info.id == ii)
            {
            m_funcNumbers->addItem (QString::number (ii),
                                    QVariant::fromValue (ii));
            }
        }
    }

void EditLocoDialog::refreshFunctionNumbers ()
    {
    common::refreshComboboxItems (*m_funcNumbers,
                                  std::bind (&EditLocoDialog::populateFunctionNumbers,
                                             this));
    }

void EditLocoDialog::setFunctionFields (int idx)
    {
    const layout::funcInfo info = m_functions->itemData (idx).value<layout::funcInfo> ();

    // block, even tho this changes the number
    utils::qobj::SignalGuard grd{ *m_funcNumbers, true };

    refreshFunctionNumbers ();

    common::setComboBoxIndexByUserData (*m_funcIcons,   info.icon,  false);
    common::setComboBoxIndexByUserData (*m_funcNumbers, info.id,    false);
    }

void EditLocoDialog::readFunctionFields ()
    {
    const int        idx  = m_functions->currentIndex ();
    layout::funcInfo info = m_functions->currentData ().value<layout::funcInfo> ();

    info.icon   = m_funcIcons->currentData ().value<layout::funcInfo::icon_t> ();
    info.id     = m_funcNumbers->currentData ().toInt ();

    m_functions->setItemText        (idx, info.uiName ());
    m_functions->setItemData        (idx, QVariant::fromValue (info));
    m_functions->setItemSchemeIcon  (idx, resources::getFunctionInfo (info.icon).icon);

    emit inputChanged ();
    }

void EditLocoDialog::updateButtonStates ()
    {
    const int maxFunctions = getMaxFunctions ();
    const int numFunctions = m_functions->count ();

    // Can't delete if there are none to delete
    m_trash->setDisabled (0 == numFunctions);
    // Can't add if list is full
    m_plus->setDisabled (maxFunctions == numFunctions);
    }

uint EditLocoDialog::getMaxFunctions () const
    {
    return m_controller->getNumberOfFunctions (
             m_protocol->currentData ().value<layout::trackProtocol> ());
    }


void EditLocoDialog::onControllerChange (int idx)
    {
    const control::ControllerBase*  controller  = m_controllerCb->itemData (idx).
                                                    value<const control::ControllerBase*> ();

    common::refreshComboboxItems (*m_protocol,
                                  [&] () -> void
                                  { populateProtocols (*controller); });
    limitAddress ();

    emit inputChanged ();
    }

void EditLocoDialog::onProtocolChange (int idx)
    {
    limitAddress ();
    refreshFunctionNumbers ();

    emit inputChanged ();
    }

void EditLocoDialog::onFunctionChange (int idx)
    {
    setFunctionFields (idx);

    emit inputChanged ();
    }

void EditLocoDialog::deleteFunction ()
    {
    const layout::funcInfo info = m_functions->currentData ().value<layout::funcInfo> ();

    if (QMessageBox::Yes == QMessageBox::question (this,
                                                   tr ("Delete Function"),
                                                   tr ("Would you like to delete %1?").
                                                   arg (info.uiName ())))
        {
        m_functions->removeItem (m_functions->currentIndex ());
        updateButtonStates ();
        }
    }

void EditLocoDialog::addFunction ()
    {
    layout::funcInfo    info;
    auto                functions = getFunctions ();
    size_t              idx;

    // Find the first available function number

    if (0 == functions.size ())
        {
        idx = info.id = 0;
        }
    else
        {
        info.id = functions.back ().id + 1;
        idx     = functions.size ();

        for (size_t ii = 1; ii < functions.size (); ++ii)
            {
            if (functions[ii - 1].id + 1 != functions[ii].id)
                {
                info.id = functions[ii - 1].id + 1;
                idx     = ii;
                break;
                }
            }
        }

    info.icon = layout::funcInfo::ICON_FUNC_LIGHT_HEADLIGHT;

    insertFunctionToCombobox (*m_functions,
                              static_cast<int> (idx),
                              info);
    m_functions->setCurrentIndex (static_cast<int> (idx));

    updateButtonStates ();
    }

} // namespace ui::trains