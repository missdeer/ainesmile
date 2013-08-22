
#include "qtsingleapplication.h"
//#include <QApplication>
#include <QNetworkProxy>
#include <QThreadPool>
#include <QStringList>
#include "stupidcheck.h"
#include "nagdialog.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
#ifdef Q_OS_MAC
    // increase the number of file that can be opened in Qt Creator.
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);

    rl.rlim_cur = qMin((rlim_t)OPEN_MAX, rl.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rl);
#endif

    SharedTools::QtSingleApplication a(QLatin1String("ainesmile"), argc, argv);

    if (a.isRunning())
    {
        if (argc >= 2)
        {
            QString files;
            for(int i = 1; i < argc; i++)
            {
                files.append(argv[i]);
                files.append("\n");
            }
            a.sendMessage(files);
        }
        return 0;
    }

    const int threadCount = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(qMax(4, 2 * threadCount));

    Q_INIT_RESOURCE( ainesmile );

#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    // Make sure we honor the system's proxy settings
#if defined(Q_OS_UNIX)
    QUrl proxyUrl(QString::fromLatin1(qgetenv("http_proxy")));
    if (proxyUrl.isValid()) {
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, proxyUrl.host(),
                            proxyUrl.port(), proxyUrl.userName(), proxyUrl.password());
        QNetworkProxy::setApplicationProxy(proxy);
# if defined(Q_OS_MAC) // unix and mac
    } else {
        QNetworkProxyFactory::setUseSystemConfiguration(true);
# endif
    }
#else // windows
    QNetworkProxyFactory::setUseSystemConfiguration(true);
#endif

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

    QObject::connect(&a, SIGNAL(messageReceived(QString,QObject*)), &w, SLOT(onIPCMessageReceived(QString,QObject*)));
    QObject::connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    return a.exec();
}
