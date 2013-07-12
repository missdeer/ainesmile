Name: 		Ninayan
Version:	2011.3
Release:	2
Summary:	DForD Ninayan
Vendor:		DForD Software
License:	DForD Software License
Group:		Applications/Communications
BuildRoot:	/tmp/yum-ninayan

%description
Ninayan is a powerful information getting and reading tool based on SNS services.


%install
rm -rf $RPM_BUILD_ROOT
cp -r ${HOME}/Shareware/ninayan/NinayanSetup/ $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr/bin/Ninayan
/usr/bin/Ninayan.app
/usr/share/Ninayan/imports/QtWebKit/libqmlwebkitplugin.so
/usr/share/Ninayan/imports/QtWebKit/qmldir
/usr/share/Ninayan/libQtCore.so.4
/usr/share/Ninayan/libQtCore.so.4.7.2
/usr/share/Ninayan/libQtDeclarative.so.4
/usr/share/Ninayan/libQtDeclarative.so.4.7.2
/usr/share/Ninayan/libQtGui.so.4
/usr/share/Ninayan/libQtGui.so.4.7.2
/usr/share/Ninayan/libQtMultimedia.so.4
/usr/share/Ninayan/libQtMultimedia.so.4.7.2
/usr/share/Ninayan/libQtNetwork.so.4
/usr/share/Ninayan/libQtNetwork.so.4.7.2
/usr/share/Ninayan/libQtScript.so.4
/usr/share/Ninayan/libQtScript.so.4.7.2
/usr/share/Ninayan/libQtSql.so.4
/usr/share/Ninayan/libQtSql.so.4.7.2
/usr/share/Ninayan/libQtSvg.so.4
/usr/share/Ninayan/libQtSvg.so.4.7.2
/usr/share/Ninayan/libQtWebKit.so.4
/usr/share/Ninayan/libQtWebKit.so.4.7.2
/usr/share/Ninayan/libQtXml.so.4
/usr/share/Ninayan/libQtXml.so.4.7.2
/usr/share/Ninayan/libQtXmlPatterns.so.4
/usr/share/Ninayan/libQtXmlPatterns.so.4.7.2
/usr/share/Ninayan/plugins/bearer/libqgenericbearer.so
/usr/share/Ninayan/plugins/codecs/libqcncodecs.so
/usr/share/Ninayan/plugins/codecs/libqjpcodecs.so
/usr/share/Ninayan/plugins/codecs/libqkrcodecs.so
/usr/share/Ninayan/plugins/codecs/libqtwcodecs.so
/usr/share/Ninayan/plugins/imageformats/libqgif.so
/usr/share/Ninayan/plugins/imageformats/libqico.so
/usr/share/Ninayan/plugins/imageformats/libqjpeg.so
/usr/share/Ninayan/plugins/imageformats/libqmng.so
/usr/share/Ninayan/plugins/imageformats/libqsvg.so
/usr/share/Ninayan/plugins/imageformats/libqtiff.so
/usr/share/Ninayan/plugins/inputmethods/libqimsw-multi.so
/usr/share/Ninayan/plugins/sqldrivers/libqsqlite.so
/usr/share/Ninayan/wqy-microhei.ttc

