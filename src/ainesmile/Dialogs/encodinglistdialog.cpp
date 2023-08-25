#include <unicode/ucnv.h>

#include <QMessageBox>

#include "encodinglistdialog.h"
#include "ui_encodinglistdialog.h"

EncodingListDialog::EncodingListDialog(QWidget *parent) : QDialog(parent), ui(new Ui::EncodingListDialog)
{
    ui->setupUi(this);

    int32_t count = ucnv_countAvailable();
    for (int32_t i = 0; i < count; ++i)
    {
        const char *name = ucnv_getAvailableName(i);

        QStringList aliasList;
        UErrorCode  errorCode  = U_ZERO_ERROR;
        int32_t     aliasCount = ucnv_countAliases(name, &errorCode);
        if (U_SUCCESS(errorCode) && aliasCount > 0)
        {
            for (int32_t j = 1; j < aliasCount; ++j)
            {
                const char *alias = ucnv_getAlias(name, j, &errorCode);
                aliasList.append(QString::fromLatin1(alias));
            }
        }

        QString item = QString::fromLatin1(name);
        if (!aliasList.isEmpty())
        {
            item.append(" - " + aliasList.join('/'));
        }
        ui->listWidget->addItem(item);
    }
}

EncodingListDialog::~EncodingListDialog()
{
    delete ui;
}

QString EncodingListDialog::selectedCodec() const
{
    return m_selectedCodec;
}

void EncodingListDialog::on_btnOK_clicked()
{
    auto *item = ui->listWidget->currentItem();
    if (!item)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please select the expected encoding in list"), QMessageBox::Ok);
        return;
    }
    m_selectedCodec = item->text();
    m_withBOM       = ui->cbWithBOM->isChecked();
    QDialog::accept();
}

void EncodingListDialog::on_btnCancel_clicked()
{
    m_selectedCodec.clear();
    QDialog::reject();
}

bool EncodingListDialog::withBOM() const
{
    return m_withBOM;
}
