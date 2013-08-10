/**************************************************************************
**   Author: Fan Yang
**   Email: missdeer@gmail.com
**   License: see the license.txt file
**************************************************************************/

#ifndef NAGDIALOG_H
#define NAGDIALOG_H

#include <QDialog>

namespace Ui {
class NagDialog;
}

class NagDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NagDialog(QWidget *parent = 0);
    ~NagDialog();
    
private slots:
    void on_btnOK_clicked();

    void on_btnOrder_clicked();

    void on_btnRegister_clicked();

private:
    Ui::NagDialog *ui;
};

#endif // NAGDIALOG_H
