/**
 * @file        clock/config.cpp
 * @brief       Configuration form for the fast clock
 * @author      Justin Scott
 * @date        2026-05-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <control/automation/clock.hpp>

#include <ui/clock/config.hpp>
#include <ui/common/utils.hpp>

#include <QBoxLayout>

namespace ui::clock
{

ConfigForm::ConfigForm (QWidget* parent) :
    QWidget (parent)
    {
    auto&       clock       = control::FastClock::instance ();
    QHBoxLayout*ratioLayout = new QHBoxLayout;
    QDateTime   now         = clock.qDateTime ();

    m_style     = new common::TieredDropdown{ this };
    m_layout    = new QFormLayout{ this };

    m_style->addParentItem (tr ("Analog"),  "misc/clock");
    m_style->addChildItem  (tr ("Basic"),   CLOCK_TYPE_ANALOG_BASIC);
    m_style->addChildItem  (tr ("Sub"),     CLOCK_TYPE_ANALOG_SUB);

    m_style->addParentItem (tr ("Digital"), "misc/binary");
    m_style->addChildItem  (tr ("Basic"),   CLOCK_TYPE_DIGITAL_BASIC);

    m_style->setCurrentIndex (1);

    ratioLayout->addWidget (new QLabel{ "1 :", this });
    ratioLayout->addWidget (m_ratio = new QSpinBox{ this });
    ratioLayout->setContentsMargins (0, 0, 0, 0);

    m_ratio->setMinimum (1);
    m_ratio->setMaximum (60);
    m_ratio->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    m_layout->addRow (new QLabel{ this }, m_running     = new common::ToggleSwitch{ this });
    m_layout->addRow (new QLabel{ this }, m_ratioLayout = ratioLayout);
    m_layout->addRow (new QLabel{ this }, m_date        = new QDateEdit{ this });
    m_layout->addRow (new QLabel{ this }, m_time        = new QTimeEdit{ this });
    m_layout->addRow (new QLabel{ this }, m_style);


    m_running->setFixedWidth (40);

    m_running->setSwitch(clock.isRunning ());
    m_ratio->setValue   (clock.getRatio  ());
    m_date->setDisabled (clock.isRunning ());
    m_time->setDisabled (clock.isRunning ());

    m_date->setDate (now.date ());
    m_time->setTime (now.time ());

    connect (m_running,
            &common::ToggleSwitch::switched,
             this,
            &ConfigForm::runningToggled);

    connect (m_ratio,
            &QSpinBox::valueChanged,
            &clock,
            &control::FastClock::setRatio);

    connect (&clock,
             &control::FastClock::tick,
              this,
             &ConfigForm::updateTime);

    connect (m_date,
            &QDateEdit::dateChanged,
             this,
            &ConfigForm::dateTimeChanged);

    connect (m_time,
            &QTimeEdit::timeChanged,
             this,
            &ConfigForm::dateTimeChanged);

    connect (m_style,
            &QComboBox::currentIndexChanged,
             this,
            &ConfigForm::styleIndexChanged);

    setLabels ();
    }


void ConfigForm::setLabels ()
    {
    common::setFormRowText (*m_layout, *m_running,      tr ("Running"));
    common::setFormRowText (*m_layout, *m_ratioLayout,  tr ("Ratio"));
    common::setFormRowText (*m_layout, *m_date,         tr ("Date"));
    common::setFormRowText (*m_layout, *m_time,         tr ("Time"));
    common::setFormRowText (*m_layout, *m_style,        tr ("Style"));

    m_style->setItemText (0, tr ("Analog"));
    m_style->setItemText (1, tr ("Basic"));
    m_style->setItemText (2, tr ("Sub"));
    m_style->setItemText (3, tr ("Digital"));
    m_style->setItemText (4, tr ("Basic"));
    }


void ConfigForm::runningToggled (bool running)
    {
    auto& clock = control::FastClock::instance ();

    m_date->setDisabled (running);
    m_time->setDisabled (running);

    clock.setRunning (running);
    }


void ConfigForm::updateTime (const control::FastClock::time_point& time)
    {
    auto&       clock       = control::FastClock::instance ();
    QDateTime   dateTime    = utils::time::fromTP (time);

    m_date->setDate (dateTime.date ());
    m_time->setTime (dateTime.time ());
    }


void ConfigForm::dateTimeChanged ()
    {
    auto& clock = control::FastClock::instance ();

    // Prevents a feedback loop
    if (not clock.isRunning ())
        {
        clock.setTime (utils::time::toTP (m_date->date (),
                                          m_time->time ()));
        }
    }

void ConfigForm::styleIndexChanged (int idx)
    {
    emit styleChanged (m_style->itemData (idx).value<clockStyle> ());
    }

} // namespace ui::clock