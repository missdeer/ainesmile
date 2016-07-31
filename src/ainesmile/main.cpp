#include "qtsingleapplication.h"
#include <QNetworkProxy>
#include <QThreadPool>
#include <QStringList>
#include <QTranslator>
#include <QLibraryInfo>
#include <QUrl>
#include "config.h"
#include "mainwindow.h"

#ifdef Q_OS_MAC
#  define SHARE_PATH "/../Resources"
#else
#  define SHARE_PATH ""
#endif

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
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
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

    QTranslator translator;
    QTranslator qtTranslator;
    QStringList uiLanguages;
// uiLanguages crashes on Windows with 4.8.0 release builds
#if (QT_VERSION >= 0x040801) || (QT_VERSION >= 0x040800 && !defined(Q_OS_WIN))
    uiLanguages = QLocale::system().uiLanguages();
#else
    uiLanguages << QLocale::system().name();
#endif
    boost::property_tree::ptree& pt = Config::instance()->pt();
    QString overrideLanguage(pt.get<std::string>("General/OverrideLanguage", "").c_str());
    if (!overrideLanguage.isEmpty())
        uiLanguages.prepend(overrideLanguage);
    const QString &ainesmileTrPath = QCoreApplication::applicationDirPath()
            + QLatin1String(SHARE_PATH "/translations");
    foreach (QString locale, uiLanguages)
    {
#if (QT_VERSION >= 0x050000)
        locale = QLocale(locale).name();
#else
        locale.replace(QLatin1Char('-'), QLatin1Char('_')); // work around QTBUG-25973
#endif
        if (translator.load(QLatin1String("ainesmile_") + locale, ainesmileTrPath))
        {
            const QString &qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
            const QString &qtTrFile = QLatin1String("qt_") + locale;
            // Binary installer puts Qt tr files into aiensmileTrPath
            if (qtTranslator.load(qtTrFile, qtTrPath) || qtTranslator.load(qtTrFile, ainesmileTrPath))
            {
                a.installTranslator(&translator);
                a.installTranslator(&qtTranslator);
                a.setProperty("ainesmile_locale", locale);
                break;
            }
            translator.load(QString()); // unload()
        }
        else if (locale == QLatin1String("C") /* overrideLanguage == "English" */)
        {
            // use built-in
            break;
        }
        else if (locale.startsWith(QLatin1String("en")) /* "English" is built-in */)
        {
            // use built-in
            break;
        }
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
    w.show();

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

    w.setWindowTitle(QObject::tr("aiensmile"));

    QObject::connect(&a, SIGNAL(messageReceived(QString,QObject*)), &w, SLOT(onIPCMessageReceived(QString,QObject*)));
    QObject::connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    return a.exec();
}
