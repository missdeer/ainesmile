/********************************************************************************
** Form generated from reading UI file 'aboutdialog.ui'
**
** Created: Tue Jul 24 21:56:55 2012
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_AboutDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *labelTitle;
    QLabel *labelIntroduce;
    QLabel *logoLabel;

    void setupUi(QDialog *AboutDialog)
    {
        if (AboutDialog->objectName().isEmpty())
            AboutDialog->setObjectName(QString::fromUtf8("AboutDialog"));
        AboutDialog->resize(403, 192);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(AboutDialog->sizePolicy().hasHeightForWidth());
        AboutDialog->setSizePolicy(sizePolicy);
        AboutDialog->setSizeGripEnabled(true);
        AboutDialog->setModal(true);
        buttonBox = new QDialogButtonBox(AboutDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(300, 150, 81, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);
        labelTitle = new QLabel(AboutDialog);
        labelTitle->setObjectName(QString::fromUtf8("labelTitle"));
        labelTitle->setGeometry(QRect(160, 30, 231, 41));
        QFont font;
        font.setPointSize(36);
        font.setBold(true);
        font.setWeight(75);
        labelTitle->setFont(font);
        labelIntroduce = new QLabel(AboutDialog);
        labelIntroduce->setObjectName(QString::fromUtf8("labelIntroduce"));
        labelIntroduce->setGeometry(QRect(160, 80, 231, 51));
        labelIntroduce->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        labelIntroduce->setWordWrap(true);
        logoLabel = new QLabel(AboutDialog);
        logoLabel->setObjectName(QString::fromUtf8("logoLabel"));
        logoLabel->setGeometry(QRect(20, 20, 131, 141));
        logoLabel->setPixmap(QPixmap(QString::fromUtf8(":/rc/Ainesmile.png")));

        retranslateUi(AboutDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), AboutDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), AboutDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *AboutDialog)
    {
        AboutDialog->setWindowTitle(QApplication::translate("AboutDialog", "About", 0, QApplication::UnicodeUTF8));
        labelTitle->setText(QApplication::translate("AboutDialog", "Ainesmile", 0, QApplication::UnicodeUTF8));
        labelIntroduce->setText(QApplication::translate("AboutDialog", "<html><head/><body><p>A powerful text editor which is extremely optimized for programmers. </p></body></html>", 0, QApplication::UnicodeUTF8));
        logoLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTDIALOG_H
