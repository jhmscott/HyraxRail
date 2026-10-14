/**
 * @file        common/formdialog.hpp
 * @brief       Test suite for the form dialog
 * @author      Justin Scott
 * @date        2026-08-14
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <testutils/ui.hpp>

#include <ui/common/formdialog.hpp>

#include <QtTest>


///////////////////////////////////////////////////////////////////////////////
/// Stub class for the FormDialog
///
/// @ingroup    STUB_CLASS
///
///////////////////////////////////////////////////////////////////////////////
class FormDialogStub: public ui::common::FormDialog
    {
public:
    FormDialogStub () : ui::common::FormDialog (NULL) {}

private:
    virtual bool hasAcceptableInput () const override { return true; }
    };

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the form dialog
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class FormDialogTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test that clicking cancel results in exec() returning Rejected
    ///
    /// @see    ui::common::FormDialog
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void cancelTest ()
        {
        FormDialogStub dlg;

        QTimer::singleShot (20,
            [&] () -> void
            {
            testutils::clickStandardButton (dlg, QDialogButtonBox::Cancel);
            });

        QCOMPARE (dlg.exec (), QDialog::Rejected);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test that clicking OK results in exec() returning Accepted
    ///
    /// @see    ui::common::FormDialog
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void okTest ()
        {
        FormDialogStub dlg;

        QTimer::singleShot (20,
            [&]() -> void
            {
            testutils::clickStandardButton (dlg, QDialogButtonBox::Ok);
            });

        QCOMPARE (dlg.exec (), QDialog::Accepted);
        }
    };

QTEST_MAIN (FormDialogTest)

#include "formdialogtest.moc"
