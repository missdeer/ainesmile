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
    QString username = ui->edtUsername->toPlainText();
    QString pinCode = ui->edtPinCode->toPlainText();
    QFile file(":/public.pem");

    FILE* fp = fopen(":/public.pem", "r");
    if (fp)
    {
        RSA* rsa = PEM_read_RSAPublicKey(fp, NULL, NULL, NULL);
        if (rsa)
        {
            int len = RSA_size(rsa);
            unsigned char *sz = (unsigned char*)calloc(len, sizeof(unsigned char));
            memset(sz, 0, len);
            RSA_public_decrypt(licenseCode.size(), (const unsigned char *)licenseCode.c_str(), sz, rsa, RSA_PKCS1_OAEP_PADDING);

        }
        fclose(fp);
    }
}
