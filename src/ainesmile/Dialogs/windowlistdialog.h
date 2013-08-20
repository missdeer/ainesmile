#ifndef WINDOWLISTDIALOG_H
#define WINDOWLISTDIALOG_H

#include <QDialog>
#include <QList>

namespace Ui {
class WindowListDialog;
}

class WindowListDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit WindowListDialog(QWidget *parent = 0);
    ~WindowListDialog();
    void setFileList(const QStringList& fileList);

signals:
    void activateTab(int index);
    void closeTab(const QList<int>& indexes);
    void saveTab(const QList<int>& indexes);
    
private slots:
    void onTableItemSelectionChanged();

    void on_btnOK_clicked();

    void on_btnActivate_clicked();

    void on_btnSave_clicked();

    void on_btnCloseWindow_clicked();

    void on_btnSortTab_clicked();

private:
    Ui::WindowListDialog *ui;
    QStringList fileList_;
};

#endif // WINDOWLISTDIALOG_H
