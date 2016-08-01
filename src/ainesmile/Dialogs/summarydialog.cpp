#include "stdafx.h"
#include "summarydialog.h"
#include "ui_summarydialog.h"

SummaryDialog::SummaryDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::SummaryDialog)
{
    ui->setupUi(this);
    setFixedSize( size() );
}

SummaryDialog::~SummaryDialog()
{
    delete ui;
}
