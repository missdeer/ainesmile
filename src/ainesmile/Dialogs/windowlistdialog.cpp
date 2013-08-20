#include "windowlistdialog.h"
#include "ui_windowlistdialog.h"

WindowListDialog::WindowListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WindowListDialog)
{
    ui->setupUi(this);
    setFixedSize( size() );
    connect(ui->tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(on_tableItemSelection_changed()));
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
    Q_FOREACH(const QString& filePath, fileList)
    {
//        QTableWidgetItem *newItem = new QTableWidgetItem(
//                    tr("%1").arg((row+1)*(column+1)));
//        tableWidget->setItem(row, column, newItem);
    }
}

void WindowListDialog::on_tableItemSelection_changed()
{

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
