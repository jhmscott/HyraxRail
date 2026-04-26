/**
 * @file        config/credits.cpp
 * @brief       Dialog box for the third party software notices,
 *              and other credits
 * @author      Justin Scott
 * @date        2026-04-04
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/hovericon.hpp>
#include <ui/common/pointedwidget.hpp>
#include <ui/common/seperator.hpp>
#include <ui/common/utils.hpp>

#include <ui/config/credits.hpp>


#include <QApplication>
#include <QBoxLayout>
#include <QFile>
#include <QLabel>


namespace ui::config
{
static const char* const softwareName[] =
    {
    "Luicde Icons",
    "Posphur Icons",
    "DSEG Font Family"
    };


CreditsDialog::CreditsDialog (QWidget* parent) :
    QDialog (parent)
    {
    static const char* const creditsText[] =
        {
        "App icon by Rose Spencer-Spreeuw "
        "(<a href=\"https://www.linkedin.com/in/rose-spencer-spreeuw-82278a1a1/\">Linkedin</a>)"
        };
    static constexpr size_t numCredits = std::size (creditsText);

    QFont           font        = QApplication::font ();
    QFont           boldFont;

    QVBoxLayout*    layout      = new QVBoxLayout{ this };

    QVBoxLayout*    licLayout   = new QVBoxLayout{ this };
    QVBoxLayout*    creditLayout= new QVBoxLayout{ this };
    QWidget*        mainWidget  = new QWidget{ this };
    QVBoxLayout*    mainLayout  = new QVBoxLayout{ this };
    QHBoxLayout*    navLayout   = new QHBoxLayout{ this };
    QPushButton*    back        = new common::HoverIcon
        {
        QIcon{ ":/icons/misc/arrow-left.svg"},
        Qt::lightGray,
        common::HoverIcon::style::ROUNDED_RECT,
        this
        };


    m_licenses  = new QGroupBox{ "Third Party Software Notices", this };
    m_credits   = new QGroupBox{ "Credits", this };
    m_licViewer = new QTextBrowser{ this };
    m_navBar    = new QWidget{ this };
    m_licTitle  = new QLabel{ this };

    font.setPixelSize (16);

    boldFont = font;

    boldFont.setBold (true);

    for (int ii = 0; ii < NUM_SW_LICENSES; ++ii)
        {
        QPushButton* btn = new common::PointedButton{ softwareName[ii], this };

        btn->setFont (font);
        btn->setSizePolicy (QSizePolicy::Maximum,
                            QSizePolicy::Maximum);

        common::makeFrameless (*btn);

        connect (btn,
                &QPushButton::released,
                 this,
                 std::bind (&CreditsDialog::openLicense,
                            this,
                            static_cast<swLicense> (ii)));

        licLayout->addWidget (btn);

        if (NUM_SW_LICENSES - 1 != ii)
            {
            licLayout->addWidget (new common::Separator{ this });
            }
        }

    m_licenses->setLayout (licLayout);

    for (size_t ii = 0; ii < numCredits; ++ii)
        {
        QLabel* label = new QLabel{ this };

        label->setText                  (creditsText[ii]);
        label->setTextFormat            (Qt::RichText);
        label->setTextInteractionFlags  (Qt::TextBrowserInteraction);
        label->setOpenExternalLinks     (true);
        label->setFont                  (font);

        creditLayout->addWidget (label);

        if (numCredits - 1 != ii)
            {
            creditLayout->addWidget (new common::Separator{ this });
            }
        }

    m_credits->setLayout (creditLayout);

    m_licTitle->setFont (boldFont);

    back->setIconSize (QSize{ 20, 20 });

    navLayout->addWidget (back,         0, Qt::AlignLeft);
    navLayout->addWidget (m_licTitle,   0, Qt::AlignLeft);
    navLayout->setContentsMargins (0, 0, 0, 0);

    m_navBar->setLayout (navLayout);
    m_navBar->setContentsMargins (0, 0, 0, 0);

    m_navBar->setHidden (true);
    m_licViewer->setHidden (true);

    m_licenses->setSizePolicy (QSizePolicy::Minimum,
                               QSizePolicy::Maximum);
    m_credits->setSizePolicy  (QSizePolicy::Minimum,
                               QSizePolicy::Maximum);

    layout->addWidget (m_licenses);
    layout->addWidget (m_credits);
    layout->addWidget (m_navBar, 0, Qt::AlignLeft);
    layout->addWidget (m_licViewer);
    layout->setContentsMargins (0, 0, 0, 0);

    mainWidget->setSizePolicy (QSizePolicy::Minimum,
                               QSizePolicy::Maximum);

    mainWidget->setLayout (layout);
    mainWidget->setContentsMargins (0, 0, 0, 0);

    mainLayout->addWidget (mainWidget, 0, Qt::AlignTop);

    connect (back,
            &QPushButton::released,
             this,
            &CreditsDialog::back);

    setContentsMargins (0, 0, 0, 0);
    setWindowTitle (TITLE);
    setLayout (mainLayout);
    }

void CreditsDialog::openLicense (swLicense lic)
    {
    static const char* const licenseFiles[] =
        {
        ":/text/LICENSE_LUCIDE",
        ":/text/LICENSE_PHOSPHOR",
        ":/text/DSEG-LICENSE.txt"
        };
    QFile file (licenseFiles[lic]);

    if (file.open (QIODevice::ReadOnly))
        {
        m_licViewer->setPlainText   (file.readAll ());

        m_licViewer->show           ();

        m_navBar->setHidden         (false);
        m_licViewer->setHidden      (false);

        m_licenses->setHidden       (true);
        m_credits->setHidden        (true);

        m_licTitle->setText (softwareName[lic]);
        }
    }

void CreditsDialog::back ()
    {
    m_navBar->setHidden         (true);
    m_licViewer->setHidden      (true);

    m_licenses->setHidden       (false);
    m_credits->setHidden        (false);
    }

}
