/**
 * @file        common/ipfield.cpp
 * @brief       IP address entry widget, supporting both IPv4 and IPv6
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/ipfield.hpp>
#include <ui/common/shortcutlineedit.hpp>

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QValidator>



#ifdef Q_OS_WIN
#include <winsock2.h>
#endif // Q_OS_WIN

namespace ui::common
{

IpField::IpField (QChar delimeter, size_t numOctets, bool hex, QWidget* parent) :
    QWidget (parent),
    m_hex (hex),
    m_delimiter (delimeter)
    {
    static constexpr int padding = 5;

    QHBoxLayout* layout = new QHBoxLayout{ this };

    m_fields.resize (numOctets);

    for (size_t ii = 0; ii < numOctets; ++ii)
        {
        layout->addWidget (m_fields[ii] = new ShortcutLineEdit{ delimeter, this });

        QFontMetrics    fm{ m_fields[ii]->font () };
        int             width;

        if (hex)
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

        if (ii != numOctets - 1)
            {
            layout->addWidget (new QLabel{ delimeter, this });

            connect (m_fields[ii],
                     &ShortcutLineEdit::next,
                     this,
                     std::bind (&IpField::next, this, ii));
            }

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
        }

    setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    setLayout (layout);
    }

QHostAddress IpField::getValue () const
    {
    QString addrString;

    for (size_t ii = 0; ii < m_fields.size () - 1; ++ii)
        {
        addrString += m_fields[ii]->text ();
        addrString += m_delimiter;
        }

    addrString += m_fields.back ()->text ();

    return QHostAddress{ addrString };
    }


void IpField::setValue (const QHostAddress& addr)
    {
    QString addrString = addr.toString ();

    QList<QStringView> addrPart;

    addrPart = addrString.tokenize (m_delimiter).toContainer ();

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

IpV6Field::IpV6Field (QWidget* parent) :
    IpField (':', 16, true, parent)
    {
    layout ()->setSpacing (0);
    setContentsMargins (0, 0, 0, 0);
    }

}