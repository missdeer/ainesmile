/**************************************************************************
**   Author: Fan Yang
**   Email: missdeer@gmail.com
**   License: see the license.txt file
**************************************************************************/
#include <QDesktopServices>
#include <QUrl>
#include "registerdialog.h"
#include "nagdialog.h"
#include "ui_nagdialog.h"

NagDialog::NagDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NagDialog)
{
    ui->setupUi(this);
}

NagDialog::~NagDialog()
{
    delete ui;
}

void NagDialog::on_btnOK_clicked()
{
    accept();
}

void NagDialog::on_btnOrder_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.dfordsoft.com/ainesmile-buy.htm"));
}

void NagDialog::on_btnRegister_clicked()
{
    RegisterDialog dlg(this);
    dlg.exec();
}
