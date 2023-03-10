; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{2A5B39DF-FA8A-41AC-B1ED-FEA620AE9FFB}
AppName=ainesmile 2013 - x64
AppVerName=ainesmile 2013 - x64
AppPublisher=support@dfordsoft.com
AppPublisherURL=http://www.dfordsoft.com/ainesmile
AppSupportURL=http://www.dfordsoft.com/ainesmile
AppUpdatesURL=http://www.dfordsoft.com/ainesmile
DefaultDirName={pf}\ainesmile
AllowNoIcons=true
OutputDir=.\
OutputBaseFilename=ainesmileSetup_x64
SolidCompression=true
Compression=lzma/Ultra
InternalCompressLevel=Ultra
VersionInfoVersion=2012
VersionInfoCompany=missdeer
VersionInfoDescription=ainesmile 2013 - x64
VersionInfoTextVersion=ainesmile 2013 - x64
VersionInfoCopyright=Copyright (c) 2010 - 2013 DForD Software. All rights reserved.
AppCopyright=Copyright (C) 2010 - 2013 DForD Software. All rights reserved.
PrivilegesRequired=none
AllowRootDirectory=true
AllowUNCPath=false
ShowLanguageDialog=auto
WizardImageFile=compiler:wizimage.bmp
WizardSmallImageFile=compiler:wizsmallimage.bmp
AppVersion=2012
UninstallDisplayName=ainesmile 2013 - x64
ChangesAssociations=true
ChangesEnvironment=true
DefaultGroupName=ainesmile
AppContact=support@dfordsoft.com
SetupIconFile=..\src\ainesmile\rc\ainesmile.ico
ArchitecturesInstallIn64BitMode=x64

[Languages]
Name: enu; MessagesFile: compiler:Default.isl
Name: chs; MessagesFile: compiler:Languages\ChineseSimp-12-5.1.11.isl
Name: cht; MessagesFile: compiler:Languages\ChineseTrad-2-5.1.11.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}

[Files]
Source: "..\src\ainesmile\release\ainesmile.exe"; DestDir: "{app}"
Source: "..\3rdparty\scintilla\bin\ScintillaEdit3.dll"; DestDir: "{app}"
Source: "..\resource\*"; DestDir: "{app}"
Source: "..\..\..\mingw64\bin\libwinpthread-1.dll"; DestDir: "{app}"
Source: "..\..\..\mingw64\bin\libstdc++-6.dll"; DestDir: "{app}"
Source: "..\..\..\mingw64\bin\libgcc_s_seh-1.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\icudt52.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\icuin52.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\icuuc52.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\libeay32.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\libjpeg-8.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\libpcre16-0.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\libpng16-16.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\libtiff-5.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\Qt5Core.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\Qt5Gui.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\Qt5Network.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\Qt5Svg.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\Qt5Widgets.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\Qt5Xml.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\bin\zlib1.dll"; DestDir: "{app}"
Source: "..\..\..\Qt-5.2.0-x86_64\plugins\accessible\qtaccessiblewidgets.dll"; DestDir: "{app}\accessible"
Source: "..\..\..\Qt-5.2.0-x86_64\plugins\imageformats\qgif.dll"; DestDir: "{app}\imageformats"
Source: "..\..\..\Qt-5.2.0-x86_64\plugins\imageformats\qico.dll"; DestDir: "{app}\imageformats"
Source: "..\..\..\Qt-5.2.0-x86_64\plugins\imageformats\qjpeg.dll"; DestDir: "{app}\imageformats"
Source: "..\..\..\Qt-5.2.0-x86_64\plugins\imageformats\qmng.dll"; DestDir: "{app}\imageformats"
Source: "..\..\..\Qt-5.2.0-x86_64\plugins\imageformats\qsvg.dll"; DestDir: "{app}\imageformats"
Source: "..\..\..\Qt-5.2.0-x86_64\plugins\imageformats\qtga.dll"; DestDir: "{app}\imageformats"
Source: "..\..\..\Qt-5.2.0-x86_64\plugins\imageformats\qtiff.dll"; DestDir: "{app}\imageformats"
Source: "..\..\..\Qt-5.2.0-x86_64\plugins\imageformats\qwbmp.dll"; DestDir: "{app}\imageformats"
Source: "..\..\..\Qt-5.2.0-x86_64\plugins\platforms\qwindows.dll"; DestDir: "{app}\platforms"

[Icons]
Name: {commondesktop}\ainesmile; Filename: {app}\ainesmile.exe; WorkingDir: {app}; Tasks: desktopicon; Comment: Launch ainesmile
Name: {group}\ainesmile; Filename: {app}\ainesmile.exe; WorkingDir: {app}; Comment: Launch ainesmile
Name: {group}\{cm:UninstallProgram, ainesmile}; Filename: {uninstallexe}; IconFilename: {app}\ainesmile.exe; IconIndex: 0; 

[Messages]
BeveledLabel=DForD Software

[UninstallDelete]

[Registry]
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\ainesmile.exe; ValueType: string; ValueData: {app}\ainesmile.exe; Flags: uninsdeletekey
