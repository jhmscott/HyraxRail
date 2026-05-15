/**
 * @file        common/hostentry.cpp
 * @brief       Entry field for network host name
 * @author      Justin Scott
 * @date        2026-05-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/common/hostentry.hpp>

#include <QHostInfo>
#include <QTimer>
#include <QValidator>

using namespace std::chrono_literals;

namespace ui::common
{
// Regex to validate a hostname for RFC 1123 compliance
// This check is done before attempting a hostname lookup
//
static const QRegularExpression RFC_1123_FQDN_REGEX{
    R"(^(?![0-9]+$)(?!-)[a-zA-Z0-9-]{1,63}(?<!-)(\.(?!-)[a-zA-Z0-9-]{1,63}(?<!-))*$)" };

HostEntryField::HostEntryField (QWidget* parent) :
    QLineEdit (parent)
    {
    m_timer = new QTimer{ this };

    setValidator (new QRegularExpressionValidator{ RFC_1123_FQDN_REGEX, this });

    m_timer->callOnTimeout (this, &HostEntryField::inputStable);

    connect (this,
            &QLineEdit::textChanged,
             this,
            &HostEntryField::inputChanged);
    }

bool HostEntryField::hasAcceptableInput () const
    {
    return QLineEdit::hasAcceptableInput () && m_validHostName;
    }

HostEntryField::validity HostEntryField::getValidityState () const
    {
    validity valid;

    if (not QLineEdit::hasAcceptableInput ())
        {
        valid = validity::NON_RFC_1123_COMPLIANT;
        }
    else if (not m_validHostName)
        {
        valid = validity::NON_EXISTENT_HOST;
        }
    else // (QLineEdit::hasAcceptableInput () && m_validHostName)
        {
        valid = validity::VALID;
        }

    return valid;
    }

void HostEntryField::inputStable ()
    {
    m_timer->stop ();

    // We've had 1 second without entry, lets check if what's entered is valid
    QHostInfo::lookupHost (text (), this, &HostEntryField::hostLookedUp);
    }

void HostEntryField::hostLookedUp (const QHostInfo& host)
    {
    if (QHostInfo::NoError == host.error ())
        {
        // Lookup succeeded!
        // Now let's just make sure that this hasn't changed
        if (host.hostName () == text ())
            {
            m_validHostName = true;
            setToolTip ("IP: " + host.addresses ()[0].toString ());
            }
        else
            {
            setToolTip ("");
            }

        emit validityChanged ();
        }
    }

void HostEntryField::inputChanged ()
    {
    if (QLineEdit::hasAcceptableInput ())
        {
        // Reset the timer every time we have input
        // We want to wait for 1 second with no user input
        //
        m_timer->start (1000ms);
        }
    else if (m_timer->isActive ())
        {
        // The timer was running, so we had valid input
        // But now we don't so stop it until we have valid input again
        //
        m_timer->stop ();
        }
    bool wasValid = m_validHostName;

    // Always reset this, we don't know if it's valid until the host name
    // lookup is done
    //
    m_validHostName = false;
    setToolTip ("");

    // Signal new, unvalidated entry has occurred
    if (wasValid)
        {
        emit validityChanged ();
        }
    }

} // namespace ui::common
