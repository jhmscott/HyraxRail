/**
 * @file        clock/addbtn.cpp
 * @brief       Add item button widget
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/addbtn.hpp>
#include <ui/common/pointedwidget.hpp>
#include <ui/common/utils.hpp>

#include <QBoxLayout>

namespace ui::common
{
AddButton::AddButton (QWidget* parent) :
    QWidget (parent)
    {
    QHBoxLayout* layout     = new QHBoxLayout{ this };
    QPushButton* plusIcon   = new common::PointedIconButton{ "misc/plus", this };

    m_plusLabel = new QLabel{ this };


    plusIcon->setIconSize ((QSize{ 20, 20 }));
    plusIcon->setSizePolicy (QSizePolicy::Maximum,
                             QSizePolicy::Maximum);


    common::makeFrameless (*plusIcon);

    layout->addWidget (plusIcon);
    layout->addWidget (m_plusLabel);
    layout->setAlignment (Qt::AlignLeft);


    connect (plusIcon,
            &QPushButton::released,
             this,
            &AddButton::addPressed);
    layout->setContentsMargins (0, 0, 0, 0);
    setContentsMargins (0, 0, 0, 0);
    setLayout (layout);
    }
void AddButton::setFontSize (int size)
    {
    QFont font = QApplication::font ();
    font.setPixelSize (16);
    m_plusLabel->setFont (font);
    }
}