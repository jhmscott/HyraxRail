/**
 * @file        config/helpgroup.cpp
 * @brief       Group box for the application help and about pages
 * @author      Justin Scott
 * @date        2026-03-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/pointedwidget.hpp>
#include <ui/common/schemeicon.hpp>
#include <ui/common/seperator.hpp>
#include <ui/common/utils.hpp>

#include <ui/config/credits.hpp>
#include <ui/config/helpgroup.hpp>

#include <utils/string.hpp>

#include <QApplication>
#include <QBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QTextBrowser>

#include <res/version.h>




namespace ui::config
{
///////////////////////////////////////////////////////////////////////////////
/// Simple dialog with a rich text browser
///
///////////////////////////////////////////////////////////////////////////////
class RichTextDialog : public common::SchemeDialog
    {
public:
    // Text mode
    enum class mode
        {
        MD,     ///< Markdown
        TXT     ///< Plain text
        };

    // Resize mode
    enum resize
        {
        NO_RESIZE   = 0x0,                      ///< Don't resize the dialog
        AUTO_WIDTH  = 0x01,                     ///< Resize the dialog to match the text browser's width
        AUTO_HEIGHT = 0x02,                     ///< Resize the dialog to match the text browser's height
        AUTO_RESIZE = AUTO_WIDTH | AUTO_HEIGHT  ///< Resize the dialog to match the text browser
        };

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  title       Dialog box title
    /// @param[in]  path        Path to file to view
    /// @param[in]  type        Type of text file
    /// @param[in]  resize      Resize mode
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    RichTextDialog (const QString&  title,
                    const char*     path,
                    mode            type,
                    resize          resize,
                    QWidget*        parent) :
        common::SchemeDialog (parent)
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
                for (const auto& line : qTokenize (contents, QString{ "\n" }))
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


///////////////////////////////////////////////////////////////////////////////
/// Style a button for the help menu
///
/// @param[in,out]  btn     Button to style
///
///////////////////////////////////////////////////////////////////////////////
static void styleButton (QPushButton& btn)
    {
    QFont font = QApplication::font ();

    font.setPixelSize (16);

    btn.setFont (font);
    btn.setSizePolicy (QSizePolicy::Maximum,
                       QSizePolicy::Maximum);

    common::makeFrameless (btn);
    }


HelpGroup::HelpGroup (QWidget* parent) :
    QGroupBox (parent)
    {
    QVBoxLayout* layout     = new QVBoxLayout{ this };

    m_help       = new common::PointedButton{ this };
    m_aboutBtn   = new common::PointedButton{ this };
    m_aboutQtBtn = new common::PointedButton{ this };
    m_licBtn     = new common::PointedButton{ this };
    m_creditsBtn = new common::PointedButton{ this };

    styleButton (*m_help);
    styleButton (*m_aboutBtn);
    styleButton (*m_aboutQtBtn);
    styleButton (*m_licBtn);
    styleButton (*m_creditsBtn);

    layout->addWidget (m_help);
    layout->addWidget (new common::Separator{ this });
    layout->addWidget (m_aboutBtn);
    layout->addWidget (new common::Separator{ this });
    layout->addWidget (m_aboutQtBtn);
    layout->addWidget (new common::Separator{ this });
    layout->addWidget (m_licBtn);
    layout->addWidget (new common::Separator{ this });
    layout->addWidget (m_creditsBtn);

    setLabels ();

    new QShortcut{ QKeySequence::HelpContents,
                   this, // parent
                   this, // reciever
                  &HelpGroup::help };

    connect (m_aboutBtn,
            &QPushButton::released,
             this,
            &HelpGroup::about);

    connect (m_aboutQtBtn,
            &QPushButton::released,
             this,
            &QApplication::aboutQt);

    connect (m_help,
            &QPushButton::released,
             this,
            &HelpGroup::help);

    connect (m_licBtn,
            &QPushButton::released,
             this,
            &HelpGroup::license);

    connect (m_creditsBtn,
            &QPushButton::released,
             this,
            &HelpGroup::credits);

    setLayout (layout);
    }

void HelpGroup::setLabels ()
    {
    setTitle (tr ("Help"));

    m_help->setText (tr ("View Help"));
    m_aboutBtn->setText (tr ("About Hyrax Rail"));
    m_aboutQtBtn->setText (tr ("About Qt"));
    m_licBtn->setText (tr ("License Info"));
    m_creditsBtn->setText (utils::str::escape (CreditsDialog::TITLE ()));
    }

void HelpGroup::about ()
    {
    const QString HELPGROUP_ABOUT_MESSAGE =
        tr (PRODUCT_DESCRIPTION)  + "<BR>"
        "Version " VERSION_STRING   "<BR>"
        "Build " __DATE__           "<BR>" +
        tr (COPYRIGHT)            + "<BR>"
        "Github : <a href='https://github.com/jhmscott/HyraxRail'>"
                          "https://github.com/jhmscott/HyraxRail</a>";

    QMessageBox msg{ this };

    msg.setWindowTitle (tr ("About"));
    msg.setTextFormat (Qt::RichText);
    msg.setText (HELPGROUP_ABOUT_MESSAGE);
    msg.setIconPixmap (qApp->windowIcon().pixmap (96, 96));

    msg.exec ();
    }

void HelpGroup::help ()
    {
    RichTextDialog dlg
        {
        tr ("Help"),
        ":/text/README.md",
        RichTextDialog::mode::MD,
        RichTextDialog::NO_RESIZE,
        this
        };

    dlg.setWindowIcon ("misc/question");
    dlg.exec ();
    }

void HelpGroup::license ()
    {
    RichTextDialog dlg
        {
        tr ("License"),
        ":/text/LICENSE",
        RichTextDialog::mode::TXT,
        RichTextDialog::AUTO_WIDTH,
        this
        };

    dlg.setWindowIcon ("misc/key");
    dlg.exec ();
    }

void HelpGroup::credits ()
    {
    CreditsDialog dlg{ this };

    dlg.exec ();
    }

}
