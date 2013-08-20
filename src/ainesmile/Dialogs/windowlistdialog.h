#ifndef WINDOWLISTDIALOG_H
#define WINDOWLISTDIALOG_H

#include <QDialog>

namespace Ui {
class WindowListDialog;
}

class WindowListDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit WindowListDialog(QWidget *parent = 0);
    ~WindowListDialog();
    void setFileList(const QStringList& fileList)
    {
        fileList_ = fileList;
    }

signals:
    void activateTab(QString filePath);
    void closeTab(QStringList filePaths);
    void saveTab(QStringList filePaths);
    
private slots:
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
