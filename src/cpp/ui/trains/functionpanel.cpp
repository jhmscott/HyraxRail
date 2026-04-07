/**
 * @file        trains/functionpanel.cpp
 * @brief       Panel containing a grid of function buttons for
 *              a given locomotive
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/trains/functionpanel.hpp>
#include <ui/common/autogrid.hpp>
#include <ui/common/pointedwidget.hpp>

#include <QApplication>
#include <QPainter>
#include <QStyleOption>

namespace ui::trains
{
class FunctionButton : public common::PointedButton
    {
public:
    FunctionButton (uint8_t id, QWidget* parent) :
        common::PointedButton (QString::number (id), parent)
        {}

    void setInstanceNum (int instanceNum) { m_instanceNum = instanceNum; }
protected:
    virtual void paintEvent (QPaintEvent* event) override
        {
        QPushButton::paintEvent (event);

        if (0 != m_instanceNum)
            {
            QPainter painter {this};

            QFont font = QApplication::font ();

            QStyleOptionButton option;
            option.initFrom (this);

            QPoint bottomRight = option.rect.bottomRight ();

            bottomRight.rx () -= 10;
            bottomRight.ry () -= 10;

            font.setPixelSize (9);


            painter.setFont (font);
            painter.drawText (bottomRight,
                              QString::number (m_instanceNum));
            }
        }
private:
    int m_instanceNum = 0;
    };

FunctionPanel::FunctionPanel (vAlignment align, QWidget* parent) :
    QWidget (parent)
    {
    common::AutoGridLayout* mainLayout =
        new common::AutoGridLayout{ common::AutoGridLayout::expand::COLUMN_FIRST,
                                    8,
                                    this };

    for (uint8_t ii = 0; ii < std::size (m_btns); ++ii)
        {
        FunctionButton* btn = new FunctionButton{ ii, this };

        btn->setCheckable (true);

        mainLayout->addWidget (btn);

        if (vAlignment::LEFT == align)
            {
            mainLayout->setAlignment (Qt::AlignLeft | Qt::AlignTop);
            }
        else
            {
            mainLayout->setAlignment (Qt::AlignRight | Qt::AlignTop);
            }

        btn->setFixedSize (50, 50);
        btn->setIconSize (QSize{ 30, 30 });

        connect (btn,
                &QPushButton::toggled,
                 this,
                 std::bind (&FunctionPanel::buttonPressed,
                            this,
                            ii,
                            std::placeholders::_1));

        m_btns[ii] = btn;
        }

    setLayout (mainLayout);
    }

void FunctionPanel::setLocomotive (const layout::Locomotive& loco)
    {
    static std::array<QIcon, layout::funcInfo::NUM_TRUE_ICONS> icons =
        {
        // lights
        QIcon{":/icons/functions/headlights.svg"},        ///< ICON_FUNC_LIGHT_HEADLIGHT,
        QIcon{":/icons/functions/lightbulb.svg"},         ///< ICON_FUNC_LIGHT_CAB

        // Sound
        QIcon{":/icons/functions/megaphone.svg"},         ///< ICON_FUNC_SOUND_HORN
        QIcon{":/icons/functions/tire.svg"},              ///< ICON_FUNC_SOUND_BRAKES
        QIcon{":/icons/functions/plugs-connected.svg"},   ///< ICON_FUNC_SOUND_COUPLING
        QIcon{":/icons/functions/speaker-high.svg"},      ///< ICON_FUNC_SOUND_GENERIC
        QIcon{":/icons/functions/engine.svg"},            ///< ICON_FUNC_SOUND_OPERATING

        // Misc
        QIcon{":/icons/functions/radical.svg"},
        QIcon{":/icons/functions/chart-line-up.svg"},
        QIcon{":/icons/functions/snail.svg"},
        };

    m_loco = loco;

    auto functions = m_loco.getFunctions ();

    // Start by hiding all the buttons
    std::for_each (m_btns.begin (),
                   m_btns.end (),
                   std::mem_fn (&QPushButton::hide));

    for (auto it = functions.begin (); functions.end () != it; ++it)
        {
        auto& [name, icon, idx, state] = *it;

        if (layout::funcInfo::ICON_FUNC_NUMBER == icon)
            {
            m_btns[idx]->setIcon ({});
            m_btns[idx]->setText (QString::number (idx));
            m_btns[idx]->setInstanceNum (0);
            }
        else
            {
            auto sameIcon =
                [icon] (const layout::funcInfo& funcInfo) -> bool
                {
                return funcInfo.icon == icon;
                };

            int totalCount = std::count_if (functions.begin (),
                                            functions.end (),
                                            sameIcon);
            int instanceNum = std::count_if (functions.begin (),
                                             it + 1,
                                             sameIcon);

            if (totalCount > 1)
                {
                m_btns[idx]->setInstanceNum (instanceNum);
                }
            else
                {
                m_btns[idx]->setInstanceNum (0);
                }

            m_btns[idx]->setIcon (icons[icon]);
            m_btns[idx]->setText ("");
            }

        if (name.empty ())
            {
            m_btns[idx]->setToolTip (QString::asprintf ("Function %d", idx));
            }
        // Function 0 is pretty universally the main headlight
        else if (0 == idx)
            {
            m_btns[idx]->setToolTip (QString::asprintf ("Function %d: Headlights", idx));
            }
        else
            {
            m_btns[idx]->setToolTip (QString::asprintf ("Function %d: %s",
                                                        idx,
                                                        name.c_str ()));
            }

        m_btns[idx]->setChecked (state);
        // Show buttons for functions we are using
        m_btns[idx]->show ();
        }

    // refresh the layout
    layout ()->update ();
    }

void FunctionPanel::clear ()
    {
    m_loco = {};

    std::for_each (m_btns.begin (),
                   m_btns.end (),
                   std::mem_fn (&QPushButton::hide));
    }


}