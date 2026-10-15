/**
 * @file        common/elidedlabel.hpp
 * @brief       Label that automatically elides text when there isn't enough space
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <QLabel>

namespace ui::common
{

///////////////////////////////////////////////////////////////////////////////
/// Label that automatically elides text when there isn't enough space
///
///////////////////////////////////////////////////////////////////////////////
class ElidedLabel : public QLabel
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  text        Label text
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit ElidedLabel (const QString& text, QWidget* parent = NULL);

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// Handle widget painting
    ///
    /// @param[in]  event       Paint event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void paintEvent (QPaintEvent* event) override;
    };

} // namespace ui::common
