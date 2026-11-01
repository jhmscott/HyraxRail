/**
 * @file        common/swipetab.cpp
 * @brief       Widget that combines a tab bar and swipe view,
                to allow swiping between tabs
 * @author      Justin Scott
 * @date        2026-10-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <names.hpp>

#include <ui/common/swipetab.hpp>
#include <ui/common/utils.hpp>

#include <QBoxLayout>

namespace ui::common
{
SwipeTabWidget::SwipeTabWidget (QWidget* parent) :
    QWidget (parent)
    {
    m_vbox = new QVBoxLayout{ this };
    m_hbox = new QHBoxLayout{ this };

    m_tabs = new SchemeTabBar{ this };
    m_view = new SwipeView{ this };

    m_vbox->setContentsMargins (0, 0, 0, 0);
    m_hbox->setContentsMargins (0, 0, 0, 0);

    m_tabs->setObjectName (OBJNAME_SWIPETAB_TABBAR);
    m_view->setObjectName (OBJNAME_SWIPETAB_SWIPEVIEW);

    // This needs to go both ways

    connect (m_tabs,
            &QTabBar::currentChanged,
             m_view,
            &QStackedWidget::setCurrentIndex);

    connect (m_view,
            &QStackedWidget::currentChanged,
             m_tabs,
            &QTabBar::setCurrentIndex);

    setContentsMargins (0, 0, 0, 0);

    setTabPosition (QTabWidget::North);
    }

int SwipeTabWidget::addTab (QWidget* widget, const utils::resources::Icon& icon)
    {
    m_tabs->addTab (icon);
    m_view->addWidget (widget);

    return count ();
    }

void SwipeTabWidget::setTabPosition (QTabWidget::TabPosition pos)
    {
    clearLayout (*m_vbox, false);
    clearLayout (*m_hbox, false);

    switch (pos)
        {
        case QTabWidget::North:
            {
            m_tabs->setShape (QTabBar::RoundedNorth);

            m_vbox->addWidget (m_tabs);
            m_vbox->addWidget (m_view);
            setLayout (m_vbox);
            break;
            }

        case QTabWidget::South:
            {
            m_tabs->setShape (QTabBar::RoundedSouth);

            m_vbox->addWidget (m_view);
            m_vbox->addWidget (m_tabs);
            setLayout (m_vbox);
            break;
            }

        case QTabWidget::East:
            {
            m_tabs->setShape (QTabBar::RoundedEast);

            m_hbox->addWidget (m_view);
            m_hbox->addWidget (m_tabs);
            setLayout (m_hbox);
            break;
            }

        case QTabWidget::West:
            {
            m_tabs->setShape (QTabBar::RoundedWest);

            m_hbox->addWidget (m_tabs);
            m_hbox->addWidget (m_view);
            setLayout (m_hbox);
            break;
            }
        }
    }
}
