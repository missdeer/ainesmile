/**************************************************************************
**   Author: Fan Yang
**   Email: missdeer@gmail.com
**   License: see the license.txt file
**************************************************************************/

#ifndef PCH_H
#define PCH_H

#if defined(Q_OS_WIN)
#include <Windows.h>
#endif

#if defined(__cplusplus)
#include <QtCore>
#include <QtGui>
#include <QtXml>
#include <QtNetwork>
#include <QtWidgets>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMimeData>
#include <QStringList>
#include <QListWidget>
#include <QDockWidget>
#include <QSettings>
#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QClipboard>
#include <QNetworkProxy>
#include <QThreadPool>
#include <QStringList>
#include <QTranslator>
#include <QLibraryInfo>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#endif

#endif // PCH_H
