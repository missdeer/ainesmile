#include <QApplication>
#include <QStringList>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Q_INIT_RESOURCE( Ainesmile );

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
    
    return a.exec();
}
