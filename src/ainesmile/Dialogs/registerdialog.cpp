#include <QFile>
#include <QDesktopServices>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#endif
#include <QLineEdit>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/bn.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include "util.hpp"
#include "registerdialog.h"
#include "ui_registerdialog.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    setFixedSize( size() );

    uint64 peerId = utility::utilities::get_local_peer_id();
    Q_ASSERT(peerId);
    std::stringstream ss;
    ss << std::setw(16) << std::setfill('0') << std::hex << peerId;
    ui->edtPinCode->setText(QString::fromStdString(ss.str()));
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_buttonBox_accepted()
{
    std::string licenseCode = ui->edtLicenseCode->toPlainText().toStdString();
    QFile resFile(":/public.pem");
    resFile.open(QFile::ReadOnly | QFile::Text);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QString tempPath = QDesktopServices::storageLocation(QDesktopServices::TempLocation);
#else
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
#endif
    tempPath.append("/aspub.pem");
    QFile tempFile(tempPath);
    tempFile.open(QFile::WriteOnly | QFile::Text);
    tempFile.write(resFile.readAll());
    tempFile.close();
    resFile.close();

    FILE* fp = fopen(tempPath.toStdString().c_str(), "r");
    if (fp)
    {
        RSA* rsa = PEM_read_RSAPublicKey(fp, NULL, NULL, NULL);
        if (rsa)
        {
            int len = RSA_size(rsa);
            unsigned char *sz = (unsigned char*)calloc(len, sizeof(unsigned char));
            memset(sz, 0, len);
            RSA_public_decrypt(licenseCode.size(), (const unsigned char *)licenseCode.c_str(), sz, rsa, RSA_PKCS1_OAEP_PADDING);
            ui->edtUsername->text();
            ui->edtPinCode->text();
        }
        fclose(fp);
    }
}
