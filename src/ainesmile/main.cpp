#include "stdafx.h"

#include "config.h"
#include "mainwindow.h"

#ifdef Q_OS_MAC
#    define SHARE_PATH "/../Resources"
#else
#    define SHARE_PATH ""
#endif

int main(int argc, char *argv[])
{
#ifndef Q_OS_WIN
    // increase the number of file that can be opened in Qt Creator.
    struct rlimit rl;
    getrlimit(RLIMIT_NOFILE, &rl);

    rl.rlim_cur = qMin((rlim_t)OPEN_MAX, rl.rlim_max);
    setrlimit(RLIMIT_NOFILE, &rl);
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QCoreApplication::setOrganizationDomain(QStringLiteral("dfordsoft.com"));
    QCoreApplication::setOrganizationName(QStringLiteral("DForD Software"));
    QCoreApplication::setApplicationName(QStringLiteral("ainesmile"));

    QApplication app(argc, argv);
    QTranslator  translator;
    QTranslator  qtTranslator;
    QStringList  uiLanguages;

    uiLanguages                                   = QLocale::system().uiLanguages();
    boost::property_tree::ptree &pt               = Config::instance()->pt();
    QString                      overrideLanguage = QString::fromStdString(pt.get<std::string>("General/OverrideLanguage", ""));
    if (!overrideLanguage.isEmpty())
    {
        uiLanguages.prepend(overrideLanguage);
    }
    const QString &ainesmileTrPath = QCoreApplication::applicationDirPath() + QLatin1String(SHARE_PATH "/translations");
    for (auto &locale : uiLanguages)
    {
        locale = QLocale(locale).name();
        if (translator.load(QLatin1String("ainesmile_") + locale, ainesmileTrPath))
        {
            const QString &qtTrPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
            const QString &qtTrFile = QLatin1String("qt_") + locale;
            // Binary installer puts Qt tr files into aiensmileTrPath
            if (qtTranslator.load(qtTrFile, qtTrPath) || qtTranslator.load(qtTrFile, ainesmileTrPath))
            {
                QApplication::installTranslator(&translator);
                QApplication::installTranslator(&qtTranslator);
                app.setProperty("ainesmile_locale", locale);
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

    Q_INIT_RESOURCE(ainesmile);

    // Make sure we honor the system's proxy settings
#if defined(Q_OS_UNIX)
    QUrl proxyUrl(QString::fromLatin1(qgetenv("http_proxy")));
    if (proxyUrl.isValid())
    {
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, proxyUrl.host(), proxyUrl.port(), proxyUrl.userName(), proxyUrl.password());
        QNetworkProxy::setApplicationProxy(proxy);
#    if defined(Q_OS_MAC) // unix and mac
    }
    else
    {
        QNetworkProxyFactory::setUseSystemConfiguration(true);
#    endif
    }
#else // windows
    QNetworkProxyFactory::setUseSystemConfiguration(true);
#endif

    MainWindow mainWindow;
    g_mainWindow = &mainWindow;
    mainWindow.show();

    if (argc >= 2)
    {
        QStringList files;
        for (int i = 1; i < argc; i++)
        {
            files << QString(argv[i]);
        }
        mainWindow.openFiles(files);
    }
    else
    {
        mainWindow.newDocument();
    }

    mainWindow.setWindowTitle(QObject::tr("aiensmile"));

    QObject::connect(&app, &QGuiApplication::lastWindowClosed, &app, &QCoreApplication::quit);
    return QCoreApplication::exec();
}
