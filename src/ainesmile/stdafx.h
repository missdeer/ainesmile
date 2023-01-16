/**************************************************************************
**   Author: Fan Yang
**   Email: missdeer@gmail.com
**   License: see the license.txt file
**************************************************************************/

#ifndef PCH_H
#define PCH_H

#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>
#include <Shellapi.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <Shlobj.h>
#include <strsafe.h>
#endif

#if defined(__cplusplus)
#    include <boost/lexical_cast.hpp>
#    include <boost/property_tree/json_parser.hpp>
#    include <boost/property_tree/xml_parser.hpp>

#    include <QAction>
#    include <QApplication>
#    include <QClipboard>
#    include <QCloseEvent>
#    include <QDateTime>
#    include <QDesktopServices>
#    include <QDockWidget>
#    include <QFile>
#    include <QFileDialog>
#    include <QFileInfo>
#    include <QInputDialog>
#    include <QLibraryInfo>
#    include <QListWidget>
#    include <QMessageBox>
#    include <QMimeData>
#    include <QNetworkProxy>
#    include <QSettings>
#    include <QStringList>
#    include <QThreadPool>
#    include <QTranslator>
#    include <QUrl>
#    include <QVBoxLayout>
#    include <QtNetwork>
#    include <QtWidgets>
#    include <QtXml>
#endif

#endif // PCH_H
