#include <QMessageBox>
#include <QTextCodec>

#include "encodinglistdialog.h"
#include "ui_encodinglistdialog.h"

EncodingListDialog::EncodingListDialog(QWidget *parent) : QDialog(parent), ui(new Ui::EncodingListDialog)
{
    ui->setupUi(this);

    auto codecs = QTextCodec::availableCodecs();
    for (const auto &codec : codecs)
    {
        ui->listWidget->addItem(QString(codec));
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
