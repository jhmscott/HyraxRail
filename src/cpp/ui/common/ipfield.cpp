/**
 * @file        common/ipfield.cpp
 * @brief       IP address entry widget, supporting both IPv4 and IPv6
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/ipfield.hpp>
#include <ui/common/pointedwidget.hpp>
#include <ui/common/shortcutlineedit.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QLabel>
#include <QLineEdit>
#include <QMimeData>
#include <QShortcut>
#include <QValidator>


#ifdef Q_OS_WIN
#include <winsock2.h>
#endif // Q_OS_WIN


namespace ui::common
{

//////////////////////////////////////////////////////////////////////////////
/// Get the delimiter character for a network protocol
///
/// @param[in]  proto   Network protocol version
///
/// @return     Delimiter character
///
//////////////////////////////////////////////////////////////////////////////
static QChar getDelimeter (QAbstractSocket::NetworkLayerProtocol proto)
    {
    return QHostAddress::IPv6Protocol == proto ? ':' : '.';
    }


//////////////////////////////////////////////////////////////////////////////
/// Get the number of octets (1 byte) for a network protocol
///
/// @param[in]  proto   Network protocol version
///
/// @return     Number of octets
///
//////////////////////////////////////////////////////////////////////////////
static size_t numOctets (QAbstractSocket::NetworkLayerProtocol proto)
    {
    return QHostAddress::IPv6Protocol == proto ? 16 : 4;
    }


//////////////////////////////////////////////////////////////////////////////
/// Get whether network protocol should be displayed in hex or decimal
///
/// @param[in]  proto   Network protocol version
///
/// @return     True if it should be displayed in hex.
///             False if it should be displayed in decimal.
///
//////////////////////////////////////////////////////////////////////////////
static bool useHex (QAbstractSocket::NetworkLayerProtocol proto)
    {
    return QHostAddress::IPv6Protocol == proto;
    }



IpField::IpField (QAbstractSocket::NetworkLayerProtocol proto, QWidget* parent) :
    QWidget (parent),
    m_proto (proto)
    {
    static constexpr int padding = 5;

    QHBoxLayout* layout = new QHBoxLayout{ this };

    m_fields.resize (numOctets (proto));

    // Add a field and seperator for each octet

    for (size_t ii = 0; ii < numOctets (proto); ++ii)
        {
        layout->addWidget (m_fields[ii] = new ShortcutLineEdit{ getDelimeter (proto), this});

        QFontMetrics    fm{ m_fields[ii]->font () };
        int             width;

        if (useHex (proto))
            {
            width = fm.horizontalAdvance ("FF");
            m_fields[ii]->setValidator (new common::HexValidator{ 0, 255, this });
            }
        else
            {
            width = fm.horizontalAdvance ("255");
            m_fields[ii]->setValidator (new QIntValidator{ 0, 255, this });
            }

        m_fields[ii]->setFixedWidth (width + 2 * padding);
        m_fields[ii]->setAlignment (Qt::AlignHCenter);

        // Next field shortcut

        if (ii != numOctets (proto) - 1)
            {
            layout->addWidget (new QLabel{ getDelimeter (proto), this });

            connect (m_fields[ii],
                    &ShortcutLineEdit::next,
                     this,
                     std::bind (&IpField::next, this, ii));
            }

        // Previous field shortcut

        if (0 != ii)
            {
            connect (m_fields[ii],
                     &ShortcutLineEdit::back,
                     this,
                     std::bind (&IpField::back, this, ii));
            }

        connect (m_fields[ii],
                &QLineEdit::textChanged,
                 this,
                &IpField::inputChanged);

        // Context menu

        auto pasteHandler = std::bind (&IpField::paste,
                                       this,
                                       m_fields[ii]);

        m_fields[ii]->setContextMenuPolicy (Qt::ActionsContextMenu);

        m_fields[ii]->addAction (tr ("Paste"),
                                 QKeySequence::Paste,
                                 this,
                                 pasteHandler);

        m_fields[ii]->addAction (tr ("Copy"),
                                 QKeySequence::Copy,
                                 m_fields[ii],
                                &QLineEdit::copy);

        m_fields[ii]->addAction (tr ("Cut"),
                                 QKeySequence::Cut,
                                 m_fields[ii],
                                &QLineEdit::cut);

        m_fields[ii]->setPasteHandler (this, pasteHandler);
        }

    // Copy cliboard to keyboard button

    int height  = m_fields[0]->height ();

    m_copy = new PointedIconButton{ "misc/clipboard-copy", this };

    m_copy->setFixedSize (height, height);
    m_copy->setToolTip (tr ("Copy IP Address to Clipboard"));
    m_copy->setEnabled (false);

    layout->addWidget (m_copy);

    // Connections

    connect (m_copy,
            &PointedIconButton::released,
             this,
            &IpField::copyIpToClipboard);

    connect (this,
            &IpField::inputChanged,
             this,
            &IpField::handleInput);

    setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    setLayout (layout);
    }

QHostAddress IpField::getValue () const
    {
    QString addrString;

    for (size_t ii = 0; ii < m_fields.size () - 1; ++ii)
        {
        addrString += m_fields[ii]->text ();
        addrString += getDelimeter (m_proto);
        }

    addrString += m_fields.back ()->text ();

    return QHostAddress{ addrString };
    }


void IpField::setValue (const QHostAddress& addr)
    {
    QString addrString = addr.toString ();

    QList<QStringView> addrPart;

    addrPart = addrString.tokenize (getDelimeter (m_proto)).toContainer ();

    if (addrPart.size () != m_fields.size ())
        {
        // Error
        }
    else
        {
        for (size_t ii = 0; ii < m_fields.size (); ++ii)
            {
            m_fields[ii]->setText (addrPart[ii].toString ());
            }

        m_copy->setEnabled (true);
        }
    }

void IpField::next (int idx)
    {
    m_fields[idx + 1]->setFocus ();
    }

void IpField::back (int idx)
    {
    m_fields[idx - 1]->setFocus ();
    }

void IpField::paste (QLineEdit* field)
    {
    const QMimeData*    mimeData        = qApp->clipboard ()->mimeData ();
    bool                defaultPaste    = true;

    if (NULL == mimeData)
        {

        }
    else if (mimeData->hasText ())
        {
        QString text = mimeData->text ();

        // QHostAddress is too permissive. Do some extra checks
        //
        // There should be one less seperator than octets (e.g. 1.1.1.1)
        //
        if ((numOctets (m_proto) - 1) == text.count (getDelimeter (m_proto)))
            {
            QHostAddress addr{ text };

            if (addr.isNull ())
                {
                // Don't allow the NULL address
                }
            else if (m_proto == addr.protocol ())
                {
                setValue (addr);
                defaultPaste = false;
                }
            }
        }

    if (defaultPaste)
        {
        // Send this to the field
        field->paste ();
        }
    }

void IpField::handleInput ()
    {
    // Only enable this button if the field has a valid IP address
    m_copy->setEnabled (hasAcceptableInput ());
    }

void IpField::copyIpToClipboard ()
    {
    QMimeData* data = new QMimeData;

    data->setText (getValue ().toString ());

    qApp->clipboard ()->setMimeData (data);
    }

IpV6Field::IpV6Field (QWidget* parent) :
    IpField (QHostAddress::IPv6Protocol, parent)
    {
    layout ()->setSpacing (0);
    setContentsMargins (0, 0, 0, 0);
    }

}