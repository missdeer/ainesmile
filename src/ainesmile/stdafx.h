/**************************************************************************
**   Author: Fan Yang
**   Email: missdeer@gmail.com
**   License: see the license.txt file
**************************************************************************/

#ifndef PCH_H
#define PCH_H

#if defined(_WIN32)
#    define NOMINMAX
#    include <Windows.h>

#    include <Shellapi.h>
#    include <Shlobj.h>
#    include <shlguid.h>
#    include <shobjidl.h>
#    include <strsafe.h>
#endif

#if defined(__cplusplus)
#    include <algorithm>
#    include <boost/lexical_cast.hpp>
#    include <boost/property_tree/json_parser.hpp>
#    include <boost/property_tree/xml_parser.hpp>
#    include <boost/scope_exit.hpp>
#    include <unicode/ucnv.h>
#    include <unicode/ucsdet.h>
#    include <unicode/utext.h>

#    include <QAction>
#    include <QApplication>
#    include <QClipboard>
#    include <QCloseEvent>
#    include <QDateTime>
#    include <QDesktopServices>
#    include <QDockWidget>
#    include <QDomDocument>
#    include <QEvent>
#    include <QFile>
#    include <QFileDialog>
#    include <QFileInfo>
#    include <QInputDialog>
#    include <QLibraryInfo>
#    include <QListWidget>
#    include <QMenu>
#    include <QMessageBox>
#    include <QMimeData>
#    include <QNetworkProxy>
#    include <QRegularExpression>
#    include <QSaveFile>
#    include <QSettings>
#    include <QStandardPaths>
#    include <QStringList>
#    include <QTextStream>
#    include <QThreadPool>
#    include <QTranslator>
#    include <QUrl>
#    include <QVBoxLayout>

#    ifndef Q_MOC_RUN
#        if defined(emit)
#            undef emit
#            include <oneapi/tbb.h>
#            define emit // restore the macro definition of "emit", as it was defined in gtmetamacros.h
#        else
#            include <oneapi/tbb.h>
#        endif // defined(emit)
#    endif     // Q_MOC_RUN
#endif

#endif // PCH_H
