Name: 		Ainesmile
Version:	2013
Release:	2
Summary:	Ainesmile
Vendor:		DForD Software
License:	DForD Software License
Group:		Applications/Editor
BuildRoot:	/tmp/yum-Ainesmile

%description
Ainesmile is a powerful source code editor and browser.

%install
rm -rf $RPM_BUILD_ROOT
cp -r ${HOME}/Shareware/Ainesmile/AinesmileSetup/ $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr/bin/Ainesmile
/usr/bin/Ainesmile.app
/usr/share/Ainesmile/imports/QtWebKit/libqmlwebkitplugin.so
/usr/share/Ainesmile/imports/QtWebKit/qmldir
/usr/share/Ainesmile/libQtCore.so.4
/usr/share/Ainesmile/libQtCore.so.4.7.2
/usr/share/Ainesmile/libQtDeclarative.so.4
/usr/share/Ainesmile/libQtDeclarative.so.4.7.2
/usr/share/Ainesmile/libQtGui.so.4
/usr/share/Ainesmile/libQtGui.so.4.7.2
/usr/share/Ainesmile/libQtMultimedia.so.4
/usr/share/Ainesmile/libQtMultimedia.so.4.7.2
/usr/share/Ainesmile/libQtNetwork.so.4
/usr/share/Ainesmile/libQtNetwork.so.4.7.2
/usr/share/Ainesmile/libQtScript.so.4
/usr/share/Ainesmile/libQtScript.so.4.7.2
/usr/share/Ainesmile/libQtSql.so.4
/usr/share/Ainesmile/libQtSql.so.4.7.2
/usr/share/Ainesmile/libQtSvg.so.4
/usr/share/Ainesmile/libQtSvg.so.4.7.2
/usr/share/Ainesmile/libQtWebKit.so.4
/usr/share/Ainesmile/libQtWebKit.so.4.7.2
/usr/share/Ainesmile/libQtXml.so.4
/usr/share/Ainesmile/libQtXml.so.4.7.2
/usr/share/Ainesmile/libQtXmlPatterns.so.4
/usr/share/Ainesmile/libQtXmlPatterns.so.4.7.2
/usr/share/Ainesmile/plugins/bearer/libqgenericbearer.so
/usr/share/Ainesmile/plugins/codecs/libqcncodecs.so
/usr/share/Ainesmile/plugins/codecs/libqjpcodecs.so
/usr/share/Ainesmile/plugins/codecs/libqkrcodecs.so
/usr/share/Ainesmile/plugins/codecs/libqtwcodecs.so
/usr/share/Ainesmile/plugins/imageformats/libqgif.so
/usr/share/Ainesmile/plugins/imageformats/libqico.so
/usr/share/Ainesmile/plugins/imageformats/libqjpeg.so
/usr/share/Ainesmile/plugins/imageformats/libqmng.so
/usr/share/Ainesmile/plugins/imageformats/libqsvg.so
/usr/share/Ainesmile/plugins/imageformats/libqtiff.so
/usr/share/Ainesmile/plugins/inputmethods/libqimsw-multi.so
/usr/share/Ainesmile/plugins/sqldrivers/libqsqlite.so
/usr/share/Ainesmile/wqy-microhei.ttc

