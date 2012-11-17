#include <QFile>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QMessageBox>
#include "SciLexer.h"
#include "codeeditpage.h"

CodeEditPage::CodeEditPage(QWidget *parent) :
    QWidget(parent)
  ,m_splitter(new QSplitter( Qt::Vertical, this))
  ,m_sciControlMaster(new ScintillaEditBase(this))
  ,m_sciControlSlave(new ScintillaEditBase(this))
  ,m_webView(new QWebView(this))
{
    Q_ASSERT(m_splitter);
    Q_ASSERT(m_sciControlMaster);
    Q_ASSERT(m_sciControlSlave);
    Q_ASSERT(m_webView);

    sptr_t docPtr = m_sciControlMaster->send(SCI_GETDOCPOINTER);
    m_sciControlSlave->send(SCI_SETDOCPOINTER, 0, docPtr);

    //webView->load(QUrl("http://www.dfordsoft.com"));
    m_splitter->addWidget(m_sciControlSlave);
    m_splitter->addWidget(m_sciControlMaster);
    m_splitter->addWidget(m_webView);

    QList<int> sizes;
    sizes << 0 << 0x7FFFF << 0;
    m_splitter->setSizes(sizes);

    QVBoxLayout* m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setMargin(0);
    m_mainLayout->addWidget(m_splitter);
    setLayout(m_mainLayout);
}

void CodeEditPage::openFile(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m_filePath = filePath;
        m_sciControlMaster->send(SCI_SETTEXT, 0, (sptr_t)file.readAll().data());
        m_sciControlMaster->send(SCI_EMPTYUNDOBUFFER, 0, 0);
        file.close();
    }
}

void CodeEditPage::saveFile(const QString &filePath)
{
    QFileInfo saveFileInfo(filePath);
    QFileInfo fileInfo(m_filePath);
    if (saveFileInfo != fileInfo)
    {
        m_filePath = filePath;
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qint64 len = m_sciControlMaster->send(SCI_GETTEXTLENGTH);
        char * szText = new char[len + 1];
        m_sciControlMaster->send(SCI_GETTEXT, len + 1, (sptr_t)szText);
        m_sciControlMaster->send(SCI_SETSAVEPOINT);
        qint64 size = file.write(szText, len);
        file.close();
        delete[] szText;

        if (size != len)
        {
            QMessageBox::warning(this, tr("Saving file failed:"),
                                 tr("Not all data is saved to file."));
        }
    }
}

const QString &CodeEditPage::getFilePath() const
{
    return m_filePath;
}

bool CodeEditPage::canClose()
{
    return !m_sciControlMaster->send(SCI_GETMODIFY);
}
