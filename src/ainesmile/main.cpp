#include <QApplication>
#include <QStringList>
#include "stupidcheck.h"
#include "nagdialog.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Q_INIT_RESOURCE( ainesmile );

    MainWindow w;
    w.showMaximized();

    if (argc >= 2)
    {
        QStringList files;
        for(int i = 1; i < argc; i++)
        {
            files << QString(argv[i]);
        }
        w.openFiles(files);
    }
    else
    {
        w.newDocument();
    }


    StupidCheck sc;
    if (sc.isStandard())
    {
        w.setWindowTitle(QObject::tr("ainesmile - Standard Edition"));
    }
    else if (sc.isProfessional())
    {
        w.setWindowTitle(QObject::tr("ainesmile - Professional Edition"));
    }
    else if (sc.isDeluxe())
    {
        w.setWindowTitle(QObject::tr("ainesmile - Deluxe Edition"));
    }
    else
    {
        w.setWindowTitle(QObject::tr("aiensmile - Unregister"));
        NagDialog dlg(&w);
        dlg.exec();
    }

    return a.exec();
}
