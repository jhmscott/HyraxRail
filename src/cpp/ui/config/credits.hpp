/**
 * @file        config/credits.hpp
 * @brief       Dialog box for the third party software notices,
 *              and other credits
 * @author      Justin Scott
 * @date        2026-04-04
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>


namespace ui::config
{
//////////////////////////////////////////////////////////////////////////////
/// Dialog to display 3rd party licenses and accreditation
///
//////////////////////////////////////////////////////////////////////////////
class CreditsDialog : public QDialog
    {
    Q_OBJECT
public:
    // Dialog box title
    static QString TITLE () { return tr ("Third Party Licenses & Credits"); };

    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    explicit CreditsDialog (QWidget* parent);

private:
    // 3rd Party licenses
    enum swLicense
        {
        SWLIC_LUCIDE,   ///< Lucide Icons
        SWLIC_POSPHUR,  ///< Phosphur Icons
        SWLIC_DSEG,     ///< DSEG font

        NUM_SW_LICENSES ///< Delimiter only
        };

    // Credit information
    struct creditPair
        {
        QString title;  ///< Title of this credit (brief)
        QString info;   ///< Full credit information
        };

    // Stores previously visited page
    using lastPage = std::variant<swLicense, creditPair>;

    QGroupBox*      m_licenses;                     ///< 3rd party licenses
    QGroupBox*      m_credits;                      ///< Special credits
    QTextBrowser*   m_licViewer;                    ///< License text viewer (Plain text)
    QTextBrowser*   m_creditViewer;                 ///< Credit "more info" viewer (HTML)
    QWidget*        m_navBar;                       ///< Navigation bar, title and back button
    QLabel*         m_licTitle;                     ///< Name of the license
    lastPage        m_lastPage = NUM_SW_LICENSES;   ///< Last license viewed when on main page

    //////////////////////////////////////////////////////////////////////////////
    /// Open the license or credit viewer text browser
    ///
    /// @param[in]  title       Title
    /// @param[in]  text        Text to display
    /// @param[in]  plainText   True if text is plain text
    ///                         False if text is HTML
    ///
    //////////////////////////////////////////////////////////////////////////////
    void openLicenseViewer (const QString& title, const QString& text, bool plainText);

private slots:
    //////////////////////////////////////////////////////////////////////////////
    /// Open a license file
    ///
    /// @param[in]  lic     Which license file to open
    ///
    //////////////////////////////////////////////////////////////////////////////
    void openLicense (swLicense lic);

    //////////////////////////////////////////////////////////////////////////////
    /// Open credit information
    ///
    /// @param[in]  credit      Credit to display
    ///
    //////////////////////////////////////////////////////////////////////////////
    void openCreditInfo (const creditPair& credit);

    //////////////////////////////////////////////////////////////////////////////
    /// Go to the previous page (home page)
    ///
    //////////////////////////////////////////////////////////////////////////////
    void back ();

    //////////////////////////////////////////////////////////////////////////////
    /// Go forward, back to the previous viewed license page
    ///
    //////////////////////////////////////////////////////////////////////////////
    void forward ();
    };

}