#include "preferencedialog.h"
#include "ui_preferencedialog.h"

PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    setFixedSize( size() );
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}
