/**
 * @file        mainwindow.hpp
 * @brief       Hyrax rail main application window
 * @author      Justin Scott
 * @date        2026-01-03
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <ui/dismiss.hpp>

#include <QtWidgets/QMainWindow>


#include <future>

namespace ui
{

///////////////////////////////////////////////////////////////////////////////
/// Main application window
///
///////////////////////////////////////////////////////////////////////////////
class MainWindow : public QMainWindow
    {
    Q_OBJECT

public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit MainWindow (QWidget *parent = NULL);

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~MainWindow ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle a back press
    ///
    /// @return     True if the back press should trigger a close
    ///             False if the back triggered something internally
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool backPress ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Progress the modal dismiss animation
    ///
    /// @param[in]  progress        Animation progress [0.0,1.0]
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void dismissModalAnimation (double progress);

    ///////////////////////////////////////////////////////////////////////////////
    /// Start dismissing the current modal dialog
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void dismissModalStart ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Cancel the modal dismiss action
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void dismissModalCancel ();
protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// Handle a change event
    ///
    /// @param[in]  event       Change event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void changeEvent (QEvent* event) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle a native, OS-specific event
    ///
    /// @param[in]  eventType   Event identifier
    /// @param[in]  message     Pointer to native OS message
    /// @param[out] result      Message processing result
    ///
    /// @return     true to stop QT handling of this event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool nativeEvent (const QByteArray& eventType,
                              void*             message,
                              qintptr*          result) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the window close event
    ///
    /// @param[in]  event       Close event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void closeEvent (QCloseEvent* event) override;

private:
    // Options for fast clock behaviour while app is not running
    enum class clockShutdownType
        {
        PAUSE,  ///< Starts the fast clock where it was when you closed the app
        RUN,    ///< On next startup, the time is extrapolated, as if the fast clock was left running
        NOT_SET ///< Not set
        };

    clockShutdownType   m_clockShutdownType = clockShutdownType::NOT_SET;   ///< Clock behaviour while shutdown
    bool                m_rememberType      = false;                        ///< True to remember the fast clock
                                                                            ///  behaviour while shutdown
    std::unique_ptr<DismissAnimation> m_dismiss = NULL; ///< Dismiss animation

    // Android colour scheme palettes
#if defined (Q_OS_ANDROID) || defined (DOXYGEN)
    QPalette            m_darkPalette;      ///< Dark mode
    QPalette            m_lightPalette;     ///< Light mode
#endif // defined (Q_OS_ANDROID) || defined (DOXYGEN)

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the main window and application titles
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTitle ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Handles a request to close the main window
    ///
    /// @return     True if the user allowed the cose
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool handleClose ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Save the fast clock settings to the user settings
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void saveFastClockSettings ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Restore the previous fast clock settings from user settings
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void restoreFastClockSettings ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the currently open modal dialog
    ///
    /// @return     Modal dialog, if one is open
    ///             NULL if not
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QDialog* getModal ();

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the request from the session manager to commit data
    ///
    /// @param[in]  manager     Session manager
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void commitDataRequest (QSessionManager& manager);

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle a change in the application state. Just logs it for debuggging
    ///
    /// @param[in]  state       New state
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void applicationStateChanged (Qt::ApplicationState state);

#if defined (Q_OS_ANDROID) || defined (DOXYGEN)
    ///////////////////////////////////////////////////////////////////////////////
    /// Defined on android to account for Qt not updating the palette in dark mode.
    /// Uses a palette based on the default from other platforms
    ///
    /// @param[in]  scheme      Colour scheme
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setPalette (Qt::ColorScheme scheme)
        { qApp->setPalette (Qt::ColorScheme::Dark == scheme ? m_darkPalette : m_lightPalette); }
#endif //  defined (Q_OS_ANDROID) || defined (DOXYGEN)
    };

} // namespace ui
