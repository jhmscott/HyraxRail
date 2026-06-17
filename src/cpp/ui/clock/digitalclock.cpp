/**
 * @file        clock/digitalclock.cpp
 * @brief       Digital clock widget
 * @author      Justin Scott
 * @date        2026-06-14
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/clock/digitalclock.hpp>

#include <utils/string.hpp>
#include <utils/time.hpp>

#include <QBoxLayout>
#include <QFontDatabase>

namespace ui::clock
{
DigitalClock::DigitalClock (QWidget* parent) :
    ClockWidget (parent)
    {
    QHBoxLayout*    timeLayout  = new QHBoxLayout;
    QVBoxLayout*    secAmPm     = new QVBoxLayout;
    QVBoxLayout*    layout      = new QVBoxLayout{ this };
    int             id7         = QFontDatabase::addApplicationFont (
                                        ":/fonts/DSEG7-Classic/DSEG7Classic-Regular.ttf");
    QString         family7     = QFontDatabase::applicationFontFamilies (id7).at (0);
    int             id14        = QFontDatabase::addApplicationFont (
                                        ":/fonts/DSEG14-Classic/DSEG14Classic-Regular.ttf");
    QString         family14    = QFontDatabase::applicationFontFamilies (id14).at (0);

    QFont           font7{ family7 };
    QFont           font14{ family14 };

    m_time      = new QLabel{ this };
    m_seconds   = new QLabel{ this };
    m_date      = new QLabel{ this };
    m_amPm      = new QLabel{ this };

    font7.setPointSize (20);
    m_time->setFont (font7);

    font7.setPointSize (10);
    m_seconds->setFont (font7);

    font14.setPointSize (6);
    m_date->setFont (font14);
    m_amPm->setFont (font14);

    secAmPm->addWidget (m_amPm,    0, Qt::AlignVCenter | Qt::AlignTop);
    secAmPm->addWidget (m_seconds, 0, Qt::AlignVCenter | Qt::AlignBottom);

    timeLayout->addWidget (m_time, 0, Qt::AlignCenter);
    timeLayout->addLayout (secAmPm);
    timeLayout->setAlignment (Qt::AlignCenter);

    layout->addWidget (m_date, 0, Qt::AlignCenter);
    layout->addLayout (timeLayout);

    setTime (control::FastClock::now ());
    setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);
    setLayout (layout);
    }


void DigitalClock::setTime (const control::FastClock::time_point& time)
    {
    QDateTime   dateTime    = utils::time::fromTP (time);
    QDate       date        = dateTime.date ();
    QTime       qTime       = dateTime.time ();
    QLocale     locale;
    bool        is24Hour    = ui::lang::uses24HourFormat (locale);

    if (is24Hour)
        {
        m_time->setText (QString::asprintf ("%02d:%02d",
                                            qTime.hour (),
                                            qTime.minute ()));
        m_amPm->hide ();
        }
    else
        {
        int hour = qTime.hour ();

        m_time->setText (QString::asprintf ("%d:%02d",
                                            hour % 12,
                                            qTime.minute ()));
        m_amPm->setText (hour > 12 ? locale.pmText () : locale.amText ());
        m_amPm->show ();
        }

    m_seconds->setText (QString::asprintf ("%02d", qTime.second ()));

    // This font doesn't render accents so strip them off
    auto month  = utils::str::removeDiactrics (
                    utils::time::monthAbreviation (
                        static_cast<utils::time::month> (date.month () - 1)));
    auto dow    = utils::str::removeDiactrics (
                    utils::time::dayOfWeekAbreviation (
                        static_cast<utils::time::dayOfTheWeek> (date.dayOfWeek () - 1)));

    m_date->setText (QString::asprintf ("%04d-%s-%02d %s",
                                        date.year (),
                                        month.toStdString ().c_str (),
                                        date.day (),
                                        dow.toStdString ().c_str ()));
    }

} // namespace ui::clock