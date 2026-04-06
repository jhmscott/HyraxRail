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

class CreditsDialog : public QDialog
    {
public:
    static inline const QString TITLE = "Third Party Licenses & Credits";

    explicit CreditsDialog (QWidget* parent);

private:
    enum swLicense
        {
        SWLIC_LUCIDE,
        SWLIC_POSPHUR,
        SWLIC_DSEG,

        NUM_SW_LICENSES
        };

    QGroupBox*      m_licenses;
    QGroupBox*      m_credits;
    QTextBrowser*   m_licViewer;
    QWidget*        m_navBar;
    QLabel*         m_licTitle;

    void openLicense (swLicense lic);

    void back ();
    };

}