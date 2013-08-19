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
    
private slots:
    void on_btnOK_clicked();

private:
    Ui::WindowListDialog *ui;
};

#endif // WINDOWLISTDIALOG_H
