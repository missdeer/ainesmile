#include "windowlistdialog.h"
#include "ui_windowlistdialog.h"

WindowListDialog::WindowListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WindowListDialog)
{
    ui->setupUi(this);
    setFixedSize( size() );
}

WindowListDialog::~WindowListDialog()
{
    delete ui;
}

void WindowListDialog::on_btnOK_clicked()
{
    close();
}
