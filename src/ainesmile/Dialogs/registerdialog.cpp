#include <QFile>
#include <QDesktopServices>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#endif
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>
#include "config.h"
#include "registerdialog.h"
#include "ui_registerdialog.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    setFixedSize( size() );

//    StupidCheck sc;
//    ui->edtPinCode->setText(sc.getPinCode());

    Config* cfg = Config::instance();
    QString username, licenseCode;
    cfg->loadLicenseInfo(username, licenseCode);
    ui->edtUsername->setText(username);
    ui->edtLicenseCode->setPlainText(licenseCode);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_buttonBox_accepted()
{
    if(ui->edtUsername->text().isEmpty() || ui->edtLicenseCode->toPlainText().isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please input username and license code."));
        reject();
    }
    else
    {
//        StupidCheck sc;
//        sc.save(ui->edtUsername->text(), ui->edtLicenseCode->toPlainText());

        QMessageBox::information(this, tr("Notice"), tr("Please restart ainesmile to verify the license."));
    }
}
