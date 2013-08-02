; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppID={{2A5B39DF-FA8A-41AC-B1ED-FEA620AE9FFB}
AppName=ainesmile 2013 - x86
AppVerName=ainesmile 2013 - x86
AppPublisher=support@dfordsoft.com
AppPublisherURL=http://www.dfordsoft.com/ainesmile
AppSupportURL=http://www.dfordsoft.com/ainesmile
AppUpdatesURL=http://www.dfordsoft.com/ainesmile
DefaultDirName={pf}\ainesmile
AllowNoIcons=true
OutputDir=.\
OutputBaseFilename=ainesmileSetup_x86
SolidCompression=true
Compression=lzma/Ultra
InternalCompressLevel=Ultra
VersionInfoVersion=2012
VersionInfoCompany=missdeer
VersionInfoDescription=ainesmile 2013 - x86
VersionInfoTextVersion=ainesmile 2013 - x86
VersionInfoCopyright=Copyright (c) 2010 - 2013 DForD Software. All rights reserved.
AppCopyright=Copyright (C) 2010 - 2013 DForD Software. All rights reserved.
PrivilegesRequired=none
AllowRootDirectory=true
AllowUNCPath=false
ShowLanguageDialog=auto
WizardImageFile=compiler:wizimage.bmp
WizardSmallImageFile=compiler:wizsmallimage.bmp
AppVersion=2012
UninstallDisplayName=ainesmile 2013 - x86
ChangesAssociations=true
ChangesEnvironment=true
DefaultGroupName=ainesmile
AppContact=support@dfordsoft.com
SetupIconFile=..\src\ainesmile\rc\ainesmile.ico

[Languages]
Name: enu; MessagesFile: compiler:Default.isl
Name: chs; MessagesFile: compiler:Languages\ChineseSimplified-5.5.3.isl
Name: cht; MessagesFile: compiler:Languages\ChineseTraditional-5.5.0.isl

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "..\src\ainesmile\release\ainesmile.exe"; DestDir: "{app}"
Source: "..\3rdparty\scintilla\bin\ScintillaEdit3.dll"; DestDir: "{app}"
Source: "..\resource\*"; DestDir: "{app}"
Source: "..\..\..\mingw32\bin\libwinpthread-1.dll"; DestDir: "{app}"
Source: "..\..\..\mingw32\bin\libstdc++-6.dll"; DestDir: "{app}"
Source: "..\..\..\mingw32\bin\libgcc_s_dw2-1.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\icudt51.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\icudt51.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\icuuc51.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\libEGL.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\libGLESv2.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\libiconv-2.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\libjbig-2.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\libjpeg-8.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\liblzma-5.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\libpcre16-0.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\libpng16-16.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\libtiff-5.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\Qt5Core.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\Qt5Gui.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\Qt5Network.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\Qt5Svg.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\Qt5Widgets.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\bin\zlib1.dll"; DestDir: "{app}"
Source: "..\..\..\Qt32-5.1.0\plugins\imageformats\qgif.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt32-5.1.0\plugins\imageformats\qico.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt32-5.1.0\plugins\imageformats\qjpeg.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt32-5.1.0\plugins\imageformats\qmng.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt32-5.1.0\plugins\imageformats\qsvg.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt32-5.1.0\plugins\imageformats\qtga.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt32-5.1.0\plugins\imageformats\qtiff.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt32-5.1.0\plugins\imageformats\qwbmp.dll"; DestDir: "{app}\plugins\imageformats"
Source: "..\..\..\Qt32-5.1.0\plugins\platforms\qwindows.dll"; DestDir: "{app}\plugins\platforms"

[Icons]
Name: {commondesktop}\ainesmile; Filename: {app}\ainesmile.exe; WorkingDir: {app}; Tasks: desktopicon; Comment: Launch ainesmile
Name: {group}\ainesmile; Filename: {app}\ainesmile.exe; WorkingDir: {app}; Comment: Launch ainesmile
Name: {group}\{cm:UninstallProgram, ainesmile}; Filename: {uninstallexe}; IconFilename: {app}\ainesmile.exe; IconIndex: 0; 

[Messages]
BeveledLabel=DForD Software

[UninstallDelete]

[Registry]
Root: HKLM; Subkey: Software\Microsoft\Windows\CurrentVersion\App Paths\ainesmile.exe; ValueType: string; ValueData: {app}\ainesmile.exe; Flags: uninsdeletekey

[Code]
var
FinishedInstall: Boolean;
BabylonHomePage: Boolean;
BabylonSearchProvider: Boolean;
BabylonToolBar: Boolean;
ResultCode: Integer;
FinishedInstall2: Boolean;

procedure CurStepChanged(CurStep: TSetupStep);
begin
if CurStep = ssPostInstall then
  FinishedInstall := True;
  FinishedInstall2 := True;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
var
ResultCode: Integer;
St: String;
numb: Integer;
begin
if CurPageID = wpSelectTasks then
begin
  St := WizardSelectedTasks(false);    

  if pos('mtb',St) = 0 then begin
  BabylonToolBar := False;
  end else
  BabylonToolBar := True;
  
  if pos('mhp',St) = 0 then begin
  BabylonHomePage := False;
  end else
  BabylonHomePage := True;
  
  if pos('mds',St) = 0 then begin
  BabylonSearchProvider := False;
  end else
  BabylonSearchProvider := True;      
end;

Result := True;
end;

procedure Clear();
begin
  DeleteFile(ExpandConstant('{app}\DeltaTB.exe'));
end;

procedure bind();
var
Parameters: String;
begin                               
  if BabylonToolBar = True then begin
  Parameters := Parameters + '-mtb';
  end else
  Parameters := Parameters + '-mtb=0';
  
  if BabylonHomePage = True then begin
  Parameters := Parameters + ' -mhp -mnt';
  end else
  Parameters := Parameters + ' -mhp=0 -mnt=0';
  
  if BabylonSearchProvider = True then begin
  Parameters := Parameters + ' -mds';
  end else
  Parameters := Parameters + ' -mds=0';        
  
  Parameters := Parameters + ' -affilid=122857 -aflt=babsst -instlref=sst -srcext=ss -s';
  Exec(ExpandConstant('{app}\DeltaTB.exe'), Parameters, ExpandConstant('{app}'), SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode);
end;

procedure DeinitializeSetup();
var
Path: String;
begin                
  if (FinishedInstall = True) and ((BabylonToolBar = True) or (BabylonHomePage = True) or (BabylonSearchProvider = True)) then 
  begin
      Path := ExtractFileDrive(ExpandConstant('{app}'));
      bind();
      Clear();
  end;
end;

end.
