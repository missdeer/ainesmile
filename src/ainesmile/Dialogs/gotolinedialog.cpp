#include "stdafx.h"
#include "gotolinedialog.h"
#include "ui_gotolinedialog.h"

GotoLineDialog::GotoLineDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    line(0),
    ui(new Ui::GotoLineDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setValidator(new QIntValidator(0, 99999, this));
    setFixedSize( size() );
}

GotoLineDialog::~GotoLineDialog()
{
    delete ui;
}

void GotoLineDialog::on_buttonBox_accepted()
{
    line = ui->lineEdit->text().toInt();
}
