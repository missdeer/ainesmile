#ifndef ENCODINGLISTDIALOG_H
#define ENCODINGLISTDIALOG_H

#include <QDialog>

namespace Ui
{
    class EncodingListDialog;
}

QT_FORWARD_DECLARE_CLASS(QListWidgetItem);

class EncodingListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EncodingListDialog(QWidget *parent = nullptr);
    ~EncodingListDialog();

    [[nodiscard]] QString selectedEncoding() const;
    [[nodiscard]] bool    withBOM() const;

private slots:
    void on_btnOK_clicked();

    void on_btnCancel_clicked();

    void on_listWidget_itemActivated(QListWidgetItem *item);

private:
    Ui::EncodingListDialog *ui;
    QString                 m_selectedEncoding;
    bool                    m_withBOM {false};
};

#endif // ENCODINGLISTDIALOG_H
