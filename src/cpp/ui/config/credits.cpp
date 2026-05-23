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
#include <QShortcut>


namespace ui::config
{
static const char* const SOFTWARE_NAMES[] =
    {
    "Luicde Icons",
    "Posphur Icons",
    "DSEG Font Family"
    };


CreditsDialog::CreditsDialog (QWidget* parent) :
    QDialog (parent)
    {
    ASSERT_ARRAY_LENGTH (SOFTWARE_NAMES, NUM_SW_LICENSES);

    static const QString CREDITS_TEXT[] =
        {
        tr ("App icon by Rose Spencer-Spreeuw ") +
        "(<a href=\"https://www.linkedin.com/in/rose-spencer-spreeuw-82278a1a1/\">LinkedIn</a>)",
        "Localization Team"
        };
    static constexpr size_t NUM_CREDITS = std::size (CREDITS_TEXT);

    static const QString CREDITS_MORE_INFO[] =
        {
        "", // App Icon

        // Localization Team
        tr ("Long Dương (%1) : Vietnamese").arg (
            "<a href=\"https://github.com/longd1999\">GitHub</a>")
        };
    ASSERT_ARRAY_LENGTH (CREDITS_MORE_INFO, NUM_CREDITS);

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
        "misc/arrow-left",
        Qt::lightGray,
        common::HoverIcon::style::ROUNDED_RECT,
        this
        };

    // Setup main components

    m_licenses      = new QGroupBox{ tr ("Third Party Software Notices"), this };
    m_credits       = new QGroupBox{ tr ("Credits"), this };
    m_licViewer     = new QTextBrowser{ this };
    m_creditViewer  = new QTextBrowser{ this };
    m_navBar        = new QWidget{ this };
    m_licTitle      = new QLabel{ this };

    // Fonts

    font.setPixelSize (16);
    boldFont = font;
    boldFont.setBold (true);

    // 3rd party licenses

    for (int ii = 0; ii < NUM_SW_LICENSES; ++ii)
        {
        QPushButton* btn = new common::PointedButton{ SOFTWARE_NAMES[ii], this };

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

    // Credits

    for (size_t ii = 0; ii < NUM_CREDITS; ++ii)
        {
        QWidget* credit;

        if (CREDITS_MORE_INFO[ii].isEmpty ())
            {

            QLabel* label = new QLabel{ this };

            label->setText                  (CREDITS_TEXT[ii]);
            label->setTextFormat            (Qt::RichText);
            label->setTextInteractionFlags  (Qt::TextBrowserInteraction);
            label->setOpenExternalLinks     (true);

            credit = label;
            }
        else
            {
            QPushButton* btn = new common::PointedButton{ CREDITS_TEXT[ii], this };

            btn->setSizePolicy (QSizePolicy::Maximum,
                                QSizePolicy::Maximum);

            common::makeFrameless (*btn);

            connect (btn,
                    &QPushButton::released,
                     this,
                     std::bind (&CreditsDialog::openCreditInfo,
                                 this,
                                 creditPair{ CREDITS_TEXT[ii],
                                             CREDITS_MORE_INFO[ii] }));

            credit = btn;
            }


        credit->setFont (font);

        creditLayout->addWidget (credit);

        if (NUM_CREDITS - 1 != ii)
            {
            creditLayout->addWidget (new common::Separator{ this });
            }
        }

    // Components styling and sizing

    m_credits->setLayout (creditLayout);

    m_licTitle->setFont (boldFont);

    back->setIconSize (QSize{ 20, 20 });
    back->setShortcut (QKeySequence::Back);

    navLayout->addWidget (back,         0, Qt::AlignLeft);
    navLayout->addWidget (m_licTitle,   0, Qt::AlignLeft);
    navLayout->setContentsMargins (0, 0, 0, 0);

    m_navBar->setLayout (navLayout);
    m_navBar->setContentsMargins (0, 0, 0, 0);

    m_navBar->setHidden (true);
    m_licViewer->setHidden (true);
    m_creditViewer->setHidden (true);

    m_creditViewer->setOpenExternalLinks (true);
    m_creditViewer->setTextInteractionFlags (Qt::TextBrowserInteraction);

    m_licenses->setSizePolicy (QSizePolicy::Minimum,
                               QSizePolicy::Maximum);
    m_credits->setSizePolicy  (QSizePolicy::Minimum,
                               QSizePolicy::Maximum);

    // Add widgets to layout

    layout->addWidget (m_licenses);
    layout->addWidget (m_credits);
    layout->addWidget (m_navBar, 0, Qt::AlignLeft);
    layout->addWidget (m_licViewer);
    layout->addWidget (m_creditViewer);
    layout->setContentsMargins (0, 0, 0, 0);

    // Main widget styling and sizing

    mainWidget->setSizePolicy (QSizePolicy::Minimum,
                               QSizePolicy::Maximum);

    mainWidget->setLayout (layout);
    mainWidget->setContentsMargins (0, 0, 0, 0);

    mainLayout->addWidget (mainWidget, 0, Qt::AlignTop);

    // Keyboard shortcuts

    new QShortcut{ QKeySequence::Back,
                   this,    // parent
                   this,    // reciever
                  &CreditsDialog::back };

    new QShortcut{ QKeySequence::Backspace,
                   this,    // parent
                   this,    // reciever
                  &CreditsDialog::back };

    new QShortcut{ QKeySequence::Forward,
                   this,    // parent
                   this,    // reciever
                  &CreditsDialog::forward };
    // Signals

    connect (back,
            &QPushButton::released,
             this,
            &CreditsDialog::back);

    // Margins, title, layout

    setContentsMargins (0, 0, 0, 0);
    setWindowTitle (TITLE);
    setLayout (mainLayout);
    }

void CreditsDialog::openLicense (swLicense lic)
    {
    static const char* const LICENSE_FILES[] =
        {
        ":/text/LICENSE_LUCIDE",
        ":/text/LICENSE_PHOSPHOR",
        ":/text/DSEG-LICENSE.txt"
        };
    ASSERT_ARRAY_LENGTH (LICENSE_FILES, NUM_SW_LICENSES);

    QFile file (LICENSE_FILES[lic]);

    if (file.open (QIODevice::ReadOnly))
        {
        openLicenseViewer (SOFTWARE_NAMES[lic], file.readAll (), true);
        }

    m_lastPage = lic;
    }

void CreditsDialog::forward ()
    {
    if (std::holds_alternative<swLicense> (m_lastPage))
        {
        swLicense lic = std::get<swLicense> (m_lastPage);

        if (NUM_SW_LICENSES != lic)
            {
            openLicense (lic);
            }
        }
    else
        {
        openCreditInfo (std::get<creditPair> (m_lastPage));
        }
    }

void CreditsDialog::openLicenseViewer (const QString& title, const QString& text, bool plainText)
    {
    // There's a bug if you switch from HTML to plain text, it hyperlinks the entirey
    // of the plaintext. Just maintain two versions of the widget
    if (plainText)
        {
        m_licViewer->setPlainText (text);
        m_licViewer->show ();
        m_creditViewer->hide ();
        }
    else
        {
        m_creditViewer->setHtml (text);
        m_creditViewer->show ();
        m_licViewer->hide ();
        }


    m_navBar->setHidden         (false);
    m_licenses->setHidden       (true);
    m_credits->setHidden        (true);

    m_licTitle->setText         (title);
    }

void CreditsDialog::openCreditInfo (const creditPair& credit)
    {
    openLicenseViewer (credit.title, credit.info, false);

    m_lastPage = credit;
    }

void CreditsDialog::back ()
    {
    m_navBar->setHidden         (true);
    m_licViewer->setHidden      (true);
    m_creditViewer->setHidden   (true);

    m_licenses->setHidden       (false);
    m_credits->setHidden        (false);
    }

}
