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
public:
    // Dialog box title
    static inline const QString TITLE = "Third Party Licenses & Credits";

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

    QGroupBox*      m_licenses;                     ///< 3rd party licenses
    QGroupBox*      m_credits;                      ///< Special credits
    QTextBrowser*   m_licViewer;                    ///< License text viewer
    QWidget*        m_navBar;                       ///< Navigation bar, title and back button
    QLabel*         m_licTitle;                     ///< Name of the license
    swLicense       m_lastPage = NUM_SW_LICENSES;   ///< Last license viewed when on main page

private slots:
    //////////////////////////////////////////////////////////////////////////////
    /// Open a license file
    ///
    /// @param[in]  lic     Which license file to open
    ///
    //////////////////////////////////////////////////////////////////////////////
    void openLicense (swLicense lic);

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