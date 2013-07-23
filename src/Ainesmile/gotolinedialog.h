#ifndef GOTOLINEDIALOG_H
#define GOTOLINEDIALOG_H

#include <QDialog>

namespace Ui {
class GotoLineDialog;
}

class GotoLineDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GotoLineDialog(QWidget *parent = 0);
    ~GotoLineDialog();
    
    int line;
private slots:
    void on_buttonBox_accepted();

private:
    Ui::GotoLineDialog *ui;
};

#endif // GOTOLINEDIALOG_H
