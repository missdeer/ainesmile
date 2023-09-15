#include "stdafx.h"

#include "config.h"
#include "mainwindow.h"

#if defined(Q_OS_WIN)
#    include <Windows.h>

#    include <shlobj.h>
#endif

QStringList sortUILanguages(const QStringList &languages)
{
    QStringList sortedLanguages;
    QStringList englishLanguages;

    for (const QString &lang : languages)
    {
        if (lang.startsWith(QStringLiteral("en")))
        {
            englishLanguages << lang;
        }
        else
        {
            sortedLanguages << lang;
        }
    }

    sortedLanguages << englishLanguages;
    return sortedLanguages;
}

void installTranslator(QTranslator &translator, QTranslator &qtTranslator)
{
    QDir translationDir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_MAC)
    translationDir.cdUp();
    translationDir.cd(QStringLiteral("Resources"));
#endif
    translationDir.cd(QStringLiteral("translations"));

    const QStringList uiLanguages = sortUILanguages(QLocale::system().uiLanguages());
    for (const auto &locale : uiLanguages)
    {
        QString qmFileName = QStringLiteral("ainesmile_%1.qm").arg(locale).replace('-', '_');
        if (translator.load(qmFileName, translationDir.absolutePath()))
        {
            QApplication::installTranslator(&translator);

            qDebug() << "all ui languages:" << uiLanguages << ", locale:" << locale << ", use qm file name:" << qmFileName;
            if (qtTranslator.load(QStringLiteral("qt_%1.qm").arg(locale).replace('-', '_'), translationDir.absolutePath()))
            {
                QApplication::installTranslator(&qtTranslator);
            }
            return;
        }

        qDebug() << "no translation file is matched for " << locale << " from " << translationDir.absolutePath();
    }
    for (const auto &locale : uiLanguages)
    {
        QString qmFileName = QStringLiteral("ainesmile_%1.qm").arg(QLocale(locale).name()).replace('-', '_');
        if (translator.load(qmFileName, translationDir.absolutePath()))
        {
            QApplication::installTranslator(&translator);

            qDebug() << "all ui languages:" << uiLanguages << ", locale:" << QLocale(locale).name() << ", use qm file name:" << qmFileName;
            if (qtTranslator.load(QStringLiteral("qt_%1.qm").arg(QLocale(locale).name()).replace('-', '_'), translationDir.absolutePath()))
            {
                QApplication::installTranslator(&qtTranslator);
            }
            return;
        }

        qDebug() << "no translation file is matched for " << locale << " from " << translationDir.absolutePath();
    }
}

#if defined(Q_OS_WIN)
void addExplorerContextMenu()
{
    const QString menuName = QCoreApplication::translate("main", "Edit with ainesmile");
    const QString appPath  = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

    QSettings menuNameSettings(QStringLiteral(R"(HKEY_CURRENT_USER\Software\Classes\*\shell\ainesmile)"), QSettings::NativeFormat);
    menuNameSettings.setValue(QStringLiteral("."), menuName);
    menuNameSettings.setValue(QStringLiteral("icon"), appPath);

    QSettings openCmdSettings(QStringLiteral(R"(HKEY_CURRENT_USER\Software\Classes\*\shell\ainesmile\command)"), QSettings::NativeFormat);
    openCmdSettings.setValue(QStringLiteral("."), QStringLiteral("\"%1\"").arg(appPath) + QStringLiteral(" \"%1\""));

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
}
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

    QTranslator translator;
    QTranslator qtTranslator;
    installTranslator(translator, qtTranslator);

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
#    if defined(Q_OS_WIN)
    addExplorerContextMenu();
#    endif
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

    mainWindow.setWindowTitle(QStringLiteral("aiensmile"));

    QObject::connect(&app, &QGuiApplication::lastWindowClosed, &app, &QCoreApplication::quit);
    return QCoreApplication::exec();
}
