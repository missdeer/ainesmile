Name: 		ainesmile
Version:	2013
Release:	2
Summary:	ainesmile
Vendor:		DForD Software
License:	DForD Software License
Group:		Applications/Editor
BuildRoot:	/tmp/yum-ainesmile

%description
ainesmile is a powerful source code editor and browser.

%install
rm -rf $RPM_BUILD_ROOT
cp -r ${HOME}/Shareware/ainesmile/ainesmileSetup/ $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
/usr/bin/ainesmile
/usr/bin/ainesmile.app
