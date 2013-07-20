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
//  ,m_webView(new QWebView(this))
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
 //   m_splitter->addWidget(m_webView);

    QList<int> sizes;
    sizes << 0 << 0x7FFFF << 0;
    m_splitter->setSizes(sizes);

    QVBoxLayout* m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setMargin(0);
    m_mainLayout->addWidget(m_splitter);
    setLayout(m_mainLayout);

    init();
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

bool CodeEditPage::canCut()
{
    return !m_sciControlMaster->send(SCI_GETSELECTIONEMPTY);
}

bool CodeEditPage::canCopy()
{
    return !m_sciControlMaster->send(SCI_GETSELECTIONEMPTY);
}

bool CodeEditPage::canPaste()
{
    return !!m_sciControlMaster->send(SCI_CANPASTE);
}

bool CodeEditPage::canUndo()
{
    return !!m_sciControlMaster->send(SCI_CANUNDO);
}

bool CodeEditPage::canRedo()
{
    return !!m_sciControlMaster->send(SCI_CANREDO);
}

bool CodeEditPage::isModified()
{
    return m_sciControlMaster->send(SCI_GETMODIFY);
}

void CodeEditPage::modified(int type, int position, int length, int linesAdded, const QByteArray &text, int line, int foldNow, int foldPrev)
{

}

void CodeEditPage::init()
{
    m_sciControlMaster->send(SCI_STYLERESETDEFAULT, 0, 0);
    m_sciControlMaster->send(SCI_STYLECLEARALL, 0, 0);
    m_sciControlMaster->send(SCI_CLEARDOCUMENTSTYLE, 0, 0);
    m_sciControlMaster->send(SCI_SETHSCROLLBAR, 1, 0);
    m_sciControlMaster->send(SCI_SETVSCROLLBAR, 1, 0);
    m_sciControlMaster->send(SCI_SETXOFFSET, 0, 0);
    m_sciControlMaster->send(SCI_SETSCROLLWIDTH, 1, 0);
    m_sciControlMaster->send(SCI_SETSCROLLWIDTHTRACKING, 1, 0);
    //m_sciControlMaster->send(SCI_ENDATLASTLINE, 1, 0);
    m_sciControlMaster->send(SCI_SETWHITESPACEFORE, 1, 0x808080);
    m_sciControlMaster->send(SCI_SETWHITESPACEBACK, 1, 0xFFFFFF);
    m_sciControlMaster->send(SCI_SETMOUSEDOWNCAPTURES, 1, 0);
    m_sciControlMaster->send(SCI_SETEOLMODE, SC_EOL_LF, 0);
    m_sciControlMaster->send(SCI_SETVIEWEOL, 0, 0);
    m_sciControlMaster->send(SCI_SETSTYLEBITS, 5, 0);
    m_sciControlMaster->send(SCI_SETCARETFORE, 0x0000FF, 0);
    m_sciControlMaster->send(SCI_SETCARETLINEVISIBLE, 1, 0);
    m_sciControlMaster->send(SCI_SETCARETLINEBACK, 0xFFFFD0, 0);
    //m_sciControlMaster->send(SCI_SETLINEBACKALPHA, 256, 0);
    m_sciControlMaster->send(SCI_SETCARETPERIOD, 500, 0);
    m_sciControlMaster->send(SCI_SETCARETWIDTH, 2, 0);
    m_sciControlMaster->send(SCI_SETCARETSTICKY, 0, 0);
    m_sciControlMaster->send(SCI_SETSELFORE, 1, 0xFFFFFF);
    m_sciControlMaster->send(SCI_SETSELBACK, 1, 0xC56A31);
    m_sciControlMaster->send(SCI_SETSELALPHA, 256, 0);
    m_sciControlMaster->send(SCI_SETSELEOLFILLED, 1, 0);
    m_sciControlMaster->send(SCI_SETADDITIONALSELECTIONTYPING, 1, 0);
    m_sciControlMaster->send(SCI_SETVIRTUALSPACEOPTIONS, SCVS_RECTANGULARSELECTION, 0);
    m_sciControlMaster->send(SCI_SETHOTSPOTACTIVEFORE, 0x0000FF, 0);
    m_sciControlMaster->send(SCI_SETHOTSPOTACTIVEBACK, 0xFFFFFF, 0);
    m_sciControlMaster->send(SCI_SETHOTSPOTACTIVEUNDERLINE, 1, 0);
    m_sciControlMaster->send(SCI_SETHOTSPOTSINGLELINE, 0, 0);
    m_sciControlMaster->send(SCI_SETCONTROLCHARSYMBOL, 0, 0);
    m_sciControlMaster->send(SCI_SETMARGINLEFT, 0, 3);
    m_sciControlMaster->send(SCI_SETMARGINRIGHT, 0, 3);
    m_sciControlMaster->send(SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
    m_sciControlMaster->send(SCI_SETMARGINWIDTHN, 0, 24);
    m_sciControlMaster->send(SCI_SETMARGINMASKN, 0, 0);
    m_sciControlMaster->send(SCI_SETMARGINSENSITIVEN, 0, 0);
    m_sciControlMaster->send(SCI_SETMARGINTYPEN, 1, SC_MARGIN_SYMBOL);
    m_sciControlMaster->send(SCI_SETMARGINWIDTHN, 1, 14);
    m_sciControlMaster->send(SCI_SETMARGINMASKN, 1, 33554431);
    m_sciControlMaster->send(SCI_SETMARGINSENSITIVEN, 1, 1);
    m_sciControlMaster->send(SCI_SETMARGINTYPEN, 2, SC_MARGIN_SYMBOL);
    m_sciControlMaster->send(SCI_SETMARGINWIDTHN, 2, 14);
    m_sciControlMaster->send(SCI_SETMARGINMASKN, 2, SC_MASK_FOLDERS); // -33554432)
    m_sciControlMaster->send(SCI_SETMARGINSENSITIVEN, 2, 1);

    m_sciControlMaster->send(SCI_SETFOLDMARGINCOLOUR, 1, 0xCDCDCD);
    m_sciControlMaster->send(SCI_SETFOLDMARGINHICOLOUR, 1, 0xFFFFFF);
    
    
    m_sciControlMaster->send(SCI_SETTABWIDTH, 4, 0);
    m_sciControlMaster->send(SCI_SETUSETABS, 0, 0);
    m_sciControlMaster->send(SCI_SETINDENT, 4, 0);
    m_sciControlMaster->send(SCI_SETTABINDENTS,   0,  0);
    m_sciControlMaster->send(SCI_SETBACKSPACEUNINDENTS,  0, 0);
    m_sciControlMaster->send(SCI_SETINDENTATIONGUIDES,  0,  0);
    m_sciControlMaster->send(SCI_SETHIGHLIGHTGUIDE,   1,  0);
    m_sciControlMaster->send(SCI_SETPRINTMAGNIFICATION,  1, 0);
    m_sciControlMaster->send(SCI_SETPRINTCOLOURMODE,  0, 0);
    m_sciControlMaster->send(SCI_SETPRINTWRAPMODE,  1,  0);
    
    //set_folder_style( sci )
    
    m_sciControlMaster->send(SCI_SETWRAPMODE, SC_WRAP_NONE, 0);
    m_sciControlMaster->send(SCI_SETWRAPVISUALFLAGS, SC_WRAPVISUALFLAG_NONE, 0);
    m_sciControlMaster->send(SCI_SETWRAPVISUALFLAGSLOCATION, SC_WRAPVISUALFLAGLOC_DEFAULT, 0);
    m_sciControlMaster->send(SCI_SETWRAPSTARTINDENT, 0, 0);

    m_sciControlMaster->send(SCI_SETLAYOUTCACHE, 2, 0);
    m_sciControlMaster->send(SCI_LINESSPLIT, 0, 0);
    m_sciControlMaster->send(SCI_SETEDGEMODE, 0, 0);
    m_sciControlMaster->send(SCI_SETEDGECOLUMN, 200, 0);
    m_sciControlMaster->send(SCI_SETEDGECOLOUR, 0xC0DCC0, 0);
        
    m_sciControlMaster->send(SCI_USEPOPUP, 0, 0);
    //m_sciControlMaster->send(SCI_SETUSEPALETTE, 1, 0);
    m_sciControlMaster->send(SCI_SETBUFFEREDDRAW, 1, 0);
    m_sciControlMaster->send(SCI_SETTWOPHASEDRAW, 1, 0);
    m_sciControlMaster->send(SCI_SETCODEPAGE, SC_CP_UTF8, 0);
    m_sciControlMaster->sends(SCI_SETWORDCHARS, 0, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
    m_sciControlMaster->send(SCI_SETZOOM,  1,  0);
    m_sciControlMaster->send(SCI_SETWHITESPACECHARS, 0, 0);
    m_sciControlMaster->send(SCI_SETMOUSEDWELLTIME,  2500,   0);

    m_sciControlMaster->send(SCI_SETSAVEPOINT, 0, 0);
    m_sciControlMaster->send(SCI_SETFONTQUALITY, SC_EFF_QUALITY_LCD_OPTIMIZED, 0 );

    //sci:SetEncoding( cfg:GetString("config/encoding") )
}

