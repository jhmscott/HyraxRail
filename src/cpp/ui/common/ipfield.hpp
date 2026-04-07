/**
 * @file        common/ipfield.hpp
 * @brief       IP address entry widget, supporting both IPv4 and IPv6
 * @author      Justin Scott
 * @date        2026-02-06
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QHostAddress>
#include <QWidget>

#include <ui/common/hexvalidator.hpp>
#include <ui/common/shortcutlineedit.hpp>



namespace ui::common
{

class IpField : public QWidget
    {
    Q_OBJECT
public:
    IpField (QChar delimeter, size_t numOctets, bool hex, QWidget* parent);

    QHostAddress getValue () const;

    void setValue (const QHostAddress& addr);

    bool hasAcceptableInput () const
        {
        return std::all_of (m_fields.begin (),
                            m_fields.end (),
                            [] (ShortcutLineEdit* edit) -> bool
                            { return edit->hasAcceptableInput (); });
        }

signals:
    void inputChanged ();
protected:
    std::vector<ShortcutLineEdit*>  m_fields;
    bool                            m_hex;
    QChar                           m_delimiter;

    void next (int idx);

    void back (int idx);
    };


class IpV4Field : public IpField
    {
public:
    explicit IpV4Field (QWidget* parent) :
        IpField ('.', 4, false, parent)
        {}
    };

class IpV6Field : public IpField
    {
public:
    explicit IpV6Field (QWidget* parent);
    };
}
