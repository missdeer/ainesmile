#include "findreplacedialog.h"
#include "ui_findreplacedialog.h"

FindReplaceDialog::FindReplaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindReplaceDialog)
{
    ui->setupUi(this);
    setFixedSize( size() );
}

FindReplaceDialog::~FindReplaceDialog()
{
    delete ui;
}
