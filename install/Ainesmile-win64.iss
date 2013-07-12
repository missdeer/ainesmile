; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppID={{8A57C9FF-E384-4B22-80AC-845E5E303B87}
AppName=Ainesmile 2013 - x86_64
AppVerName=Ainesmile 2013 - x86_64
AppPublisher=support@dfordsoft.com
AppPublisherURL=http://www.dfordsoft.com/ainesmile
AppSupportURL=http://www.dfordsoft.com/ainesmile
AppUpdatesURL=http://www.dfordsoft.com/ainesmile
DefaultDirName={pf}\Ainesmile
AllowNoIcons=true
OutputDir=.\
OutputBaseFilename=AinesmileSetup_x86_64
SolidCompression=true
Compression=lzma/Ultra
InternalCompressLevel=Ultra
VersionInfoVersion=2012
VersionInfoCompany=missdeer
VersionInfoDescription=Ainesmile 2013 - x86_64
VersionInfoTextVersion=Ainesmile 2013 - x86_64
VersionInfoCopyright=Copyright (c) 2010 - 2013 DForD Software. All rights reserved.
AppCopyright=Copyright (C) 2010 - 2013 DForD Software. All rights reserved.
PrivilegesRequired=none
AllowRootDirectory=true
AllowUNCPath=false
ShowLanguageDialog=auto
WizardImageFile=compiler:wizimage.bmp
WizardSmallImageFile=compiler:wizsmallimage.bmp
AppVersion=2012
UninstallDisplayName=Ainesmile 2013 - x86_64
ChangesAssociations=true
ChangesEnvironment=true
DefaultGroupName=Ainesmile
AppContact=support@dfordsoft.com
SetupIconFile=..\src\Ainesmile\rc\Ainesmile.ico
ArchitecturesInstallIn64BitMode=x86_64

[Languages]
Name: enu; MessagesFile: compiler:Default.isl
Name: chs; MessagesFile: compiler:Languages\ChineseSimp-12-5.1.11.isl
Name: cht; MessagesFile: compiler:Languages\ChineseTrad-2-5.1.11.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}

[Files]
Source: "..\src\Ainesmile\release\Ainesmile.exe"; DestDir: "{app}"
Source: "..\3rdparty\scintilla\bin\ScintillaEdit3.dll"; DestDir: "{app}"
Source: "..\..\..\mingw64\bin\libwinpthread-1.dll"; DestDir: "{app}"
Source: "..\..\..\mingw64\bin\libstdc++-6.dll"; DestDir: "{app}"
Source: "..\..\..\mingw64\bin\libgcc_s_seh-1.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\icudt51.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\icudt51.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\icuuc51.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\libEGL.dll.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\libGLESv2.dll.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\libiconv-2.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\libjbig-2.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\libjpeg-8.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\liblzma-5.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\libpcre16-0.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\libpng16-16.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\libsqlite3-0.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\libtiff-5.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\libxml2-2.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\libxslt-1.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5Core.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5Gui.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5Network.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5OpenGL.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5PrintSupport.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5Qml.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5Quick.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5Sensors.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5Sql.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5Svg.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5V8.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5WebKit.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5WebKitWidgets.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\Qt5Widgets.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\bin\zlib1.dll"; DestDir: "{app}"
Source: "..\..\..\Qt64-5.1.0\plugins\imageformats\qgif.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt64-5.1.0\plugins\imageformats\qico.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt64-5.1.0\plugins\imageformats\qjpeg.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt64-5.1.0\plugins\imageformats\qmng.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt64-5.1.0\plugins\imageformats\qsvg.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt64-5.1.0\plugins\imageformats\qtga.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt64-5.1.0\plugins\imageformats\qtiff.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt64-5.1.0\plugins\imageformats\qwbmp.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt64-5.1.0\plugins\platforms\qwindows.dll"; DestDir: "{app}\plugins\platforms"

[Icons]
Name: {commondesktop}\Ainesmile; Filename: {app}\Ainesmile.exe; WorkingDir: {app}; Tasks: desktopicon; Comment: Launch Ainesmile
Name: {group}\Ainesmile; Filename: {app}\Ainesmile.exe; WorkingDir: {app}; Comment: Launch Ainesmile
Name: {group}\{cm:UninstallProgram, Ainesmile}; Filename: {uninstallexe}; IconFilename: {app}\Ainesmile.exe; IconIndex: 0; 

[Messages]
BeveledLabel=DForD Software

[UninstallDelete]

[Registry]
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\Ainesmile.exe; ValueType: string; ValueData: {app}\Ainesmile.exe; Flags: uninsdeletekey
