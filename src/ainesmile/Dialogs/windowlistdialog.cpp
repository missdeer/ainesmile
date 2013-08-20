#include <QFile>
#include <QFileInfo>
#include "windowlistdialog.h"
#include "ui_windowlistdialog.h"

WindowListDialog::WindowListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WindowListDialog)
{
    ui->setupUi(this);
    setFixedSize( size() );
    connect(ui->tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(onTableItemSelectionChanged()));
}

WindowListDialog::~WindowListDialog()
{
    delete ui;
}

void WindowListDialog::setFileList(const QStringList &fileList)
{
    fileList_ = fileList;
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(fileList.size());
    ui->tableWidget->setColumnCount(3);
    QStringList headers;
    headers << tr("File")
            << tr("Path")
            << tr("Type");
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->setColumnWidth(0, 150);
    ui->tableWidget->setColumnWidth(1, 300);
    for (int i = 0; i < fileList.size(); i++)
    {
        const QString& filePath = fileList.at(i);
        if (QFile::exists(filePath))
        {
            QFileInfo fi(filePath);
            QTableWidgetItem *fileNameItem = new QTableWidgetItem(fi.fileName());
            ui->tableWidget->setItem(i, 0, fileNameItem);
            QTableWidgetItem *filePathItem = new QTableWidgetItem(fi.absolutePath());
            ui->tableWidget->setItem(i, 1, filePathItem);
            QTableWidgetItem *fileTypeItem = new QTableWidgetItem(fi.suffix());
            ui->tableWidget->setItem(i, 2, fileTypeItem);
        }
        else
        {
            QTableWidgetItem *fileNameItem = new QTableWidgetItem(filePath);
            ui->tableWidget->setItem(i, 0, fileNameItem);
        }
    }
}

void WindowListDialog::onTableItemSelectionChanged()
{
    QList<QTableWidgetItem*> selected = ui->tableWidget->selectedItems();
    ui->btnActivate->setEnabled(selected.size() == 3);
    ui->btnCloseWindow->setEnabled(!selected.isEmpty());
    ui->btnSave->setEnabled(!selected.isEmpty());
}

void WindowListDialog::on_btnOK_clicked()
{
    close();
}

void WindowListDialog::on_btnActivate_clicked()
{
    QList<QTableWidgetItem*> selected = ui->tableWidget->selectedItems();
    Q_FOREACH(QTableWidgetItem *item, selected)
    {
        break;
    }
}

void WindowListDialog::on_btnSave_clicked()
{
    QStringList list;
    QList<QTableWidgetItem*> selected = ui->tableWidget->selectedItems();
    Q_FOREACH(QTableWidgetItem *item, selected)
    {
    }

    if (!list.isEmpty())
        emit saveTab(list);
}

void WindowListDialog::on_btnCloseWindow_clicked()
{
    QStringList list;
    QList<QTableWidgetItem*> selected = ui->tableWidget->selectedItems();
    Q_FOREACH(QTableWidgetItem *item, selected)
    {
    }

    if (!list.isEmpty())
        closeTab(list);
}

void WindowListDialog::on_btnSortTab_clicked()
{

}
