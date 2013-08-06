#include <QFile>
#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setFixedSize( size() );

    QString date;
    QFile fileDate(":/DATE");
    if (fileDate.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray d = fileDate.readAll();
        date = QString::fromUtf8(d);
        date = date.replace("\n", " ");
        fileDate.close();
    }

    QString revision;
    QFile fileRevision(":/REVISION");
    if (fileRevision.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray r = fileRevision.readAll();
        revision = QString::fromUtf8(r);
        fileRevision.close();
    }

    ui->labelBuildInfo->setText(tr("Build at ") + date + tr("\nRev ") + revision);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
