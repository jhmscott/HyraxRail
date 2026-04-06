/**
 * @file        common/toggleswitch.hpp
 * @brief       Pill style toggle switch
 * @author      Justin Scott
 * @date        2026-04-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QAbstractButton>
#include <QPropertyAnimation>

namespace ui::common
{

class ToggleSwitch : public QAbstractButton
    {
    Q_OBJECT

    Q_PROPERTY (int offset READ offset WRITE setOffset);
public:
    ToggleSwitch (QWidget* parent = nullptr);

    QSize sizeHint () const override;

    int offset() const { return m_x; }

    void setOffset(int o) { m_x = o; update(); }
protected:
    void paintEvent (QPaintEvent*) override;

    void mouseReleaseEvent (QMouseEvent*) override;

    void enterEvent (QEnterEvent* event) override;

private:
    bool    m_switch;
    qreal   m_opacity;
    int     m_x;
    int     m_y;
    int     m_height;
    int     m_margin;
    QBrush  m_thumb;
    QBrush  m_track;
    QBrush  m_brush;
    QPropertyAnimation* m_anim = nullptr;
    };
}