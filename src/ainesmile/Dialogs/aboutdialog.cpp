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
#if defined(Q_OS_MAC)
        date = QString::fromUtf8(d);
#else
        date = QString::fromLatin1(d);
#endif
        date = date.replace("\n", " ");
        fileDate.close();
    }

    QString revision;
    QFile fileRevision(":/REVISION");
    if (fileRevision.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray r = fileRevision.readAll();
#if defined(Q_OS_MAC)
        revision = QString::fromUtf8(r);
#else
        revision = QString::fromLatin1(r);
#endif
        fileRevision.close();
    }

    ui->labelBuildInfo->setText(tr("Build at ") + date + tr("\nRev ") + revision);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
