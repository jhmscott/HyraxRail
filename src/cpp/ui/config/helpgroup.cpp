/**
 * @file        config/helpgroup.cpp
 * @brief       Group box for the application help and about pages
 * @author      Justin Scott
 * @date        2026-03-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/pointedwidget.hpp>
#include <ui/common/seperator.hpp>
#include <ui/common/utils.hpp>

#include <ui/config/credits.hpp>
#include <ui/config/helpgroup.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include <QTextBrowser>

#include <res/version.h>


#define HELPGROUP_ABOUT_MESSAGE         \
    PRODUCT_DESCRIPTION         "\n"    \
    "Version " VERSION_STRING   "\n"    \
    "Build " __DATE__           "\n"    \
    COPYRIGHT

namespace ui::config
{

class RichTextDialog : public QDialog
    {
public:
    enum class mode
        {
        MD,
        TXT
        };

    enum resize
        {
        NO_RESIZE   = 0x0,
        AUTO_WIDTH  = 0x01,
        AUTO_HEIGHT = 0x02,
        AUTO_RESIZE = AUTO_WIDTH | AUTO_HEIGHT
        };

     RichTextDialog (const char* title, const char* path, mode type, resize resize, QWidget* parent) :
        QDialog (parent)
        {
        QVBoxLayout*    layout = new QVBoxLayout{ this };
        QFile           file (path);

        setWindowTitle (title);

        if (file.open (QIODevice::ReadOnly))
            {
            QTextBrowser*   browser     = new QTextBrowser{ this };
            QString         contents    = file.readAll ();
            int             width       = 0;
            int             numLines    = 0;

            QFontMetrics    fontMetrics{ browser->font () };

            if (mode::TXT != type)
                {
                if (NO_RESIZE != resize)
                    {
                    qDebug () << "Auto-resize only supported for plain text";
                    resize = NO_RESIZE;
                    }
                }
            else if (NO_RESIZE != resize)
                {
                for (const auto& line : qTokenize (contents, tr ("\n")))
                    {
                    width = std::max (width,
                                      fontMetrics.
                                            horizontalAdvance (line.toString ()));
                    ++numLines;
                    }
                }

            if (mode::MD == type)
                {
                browser->setMarkdown (contents);
                }
            else
                {
                browser->setText (contents);
                }

            if ((resize & AUTO_WIDTH) > 0)
                {
                browser->setMinimumWidth (width);
                }

            if ((resize & AUTO_HEIGHT) > 0)
                {
                browser->setMinimumHeight (numLines * fontMetrics.height ());
                }

            layout->addWidget (browser);
            browser->show ();
            }

        setLayout (layout);
        }
    };


static void styleButton (QPushButton& btn)
    {
    QFont font = QApplication::font ();

    font.setPixelSize (16);

    btn.setFont (font);
    btn.setSizePolicy (QSizePolicy::Maximum,
                       QSizePolicy::Maximum);

    common::makeFrameless (btn);
    }

static QString escape (QString str)
    {
    return str.replace ("&", "&&");
    }

HelpGroup::HelpGroup (QWidget* parent) :
    QGroupBox ("Help", parent)
    {
    QVBoxLayout* layout     = new QVBoxLayout{ this };

    QPushButton* help       = new common::PointedButton{ "View Help",             this };
    QPushButton* aboutBtn   = new common::PointedButton{ "About Hyrax Rail",      this };
    QPushButton* aboutQtBtn = new common::PointedButton{ "About Qt",              this };
    QPushButton* licBtn     = new common::PointedButton{ "License Info",          this };
    QPushButton* creditsBtn = new common::PointedButton{ escape (CreditsDialog::TITLE), this};

    styleButton (*help);
    styleButton (*aboutBtn);
    styleButton (*aboutQtBtn);
    styleButton (*licBtn);
    styleButton (*creditsBtn);

    layout->addWidget (help);
    layout->addWidget (new common::Separator{ this });
    layout->addWidget (aboutBtn);
    layout->addWidget (new common::Separator{ this });
    layout->addWidget (aboutQtBtn);
    layout->addWidget (new common::Separator{ this });
    layout->addWidget (licBtn);
    layout->addWidget (new common::Separator{ this });
    layout->addWidget (creditsBtn);

    connect (aboutBtn,
            &QPushButton::released,
             this,
            &HelpGroup::about);

    connect (aboutQtBtn,
            &QPushButton::released,
             this,
            &QApplication::aboutQt);

    connect (help,
            &QPushButton::released,
             this,
            &HelpGroup::help);

    connect (licBtn,
            &QPushButton::released,
             this,
            &HelpGroup::license);

    connect (creditsBtn,
            &QPushButton::released,
             this,
            &HelpGroup::credits);

    setLayout (layout);
    }

void HelpGroup::about ()
    {
    QMessageBox::about (this,
                        "About",
                        HELPGROUP_ABOUT_MESSAGE);
    }

void HelpGroup::help ()
    {
    RichTextDialog dlg
        {
        "Help",
        ":/text/README.md",
        RichTextDialog::mode::MD,
        RichTextDialog::NO_RESIZE,
        this
        };

    dlg.exec ();
    }

void HelpGroup::license ()
    {
    RichTextDialog dlg
        {
        "License",
        ":/text/LICENSE",
        RichTextDialog::mode::TXT,
        RichTextDialog::AUTO_WIDTH,
        this
        };

    dlg.exec ();
    }

void HelpGroup::credits ()
    {
    CreditsDialog dlg{ this };

    dlg.exec ();
    }

}
