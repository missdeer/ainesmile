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

void WindowListDialog::on_btnActivate_clicked()
{
//    QModelIndexList selected = ui->tableView->selectedIndexes();
//    Q_FOREACH(QModelIndex index, selected)
//    {
//        int row = index.row();
//        emit activateTab(fileList_.at(row));
//        break;
//    }
}

void WindowListDialog::on_btnSave_clicked()
{
    QStringList list;
//    QModelIndexList selected = ui->tableView->selectedIndexes();
//    Q_FOREACH(QModelIndex index, selected)
//    {
//        int row = index.row();
//        list << fileList_.at(row);
//    }

    if (!list.isEmpty())
        emit saveTab(list);
}

void WindowListDialog::on_btnCloseWindow_clicked()
{
    QStringList list;
//    QModelIndexList selected = ui->tableView->selectedIndexes();
//    Q_FOREACH(QModelIndex index, selected)
//    {
//        int row = index.row();
//        list << fileList_.at(row);
//    }

    if (!list.isEmpty())
        closeTab(list);
}

void WindowListDialog::on_btnSortTab_clicked()
{

}
