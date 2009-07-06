#include "_preprocess.iss"

[Setup]
AppName={#APP_NAME}
AppVerName={#APP_NAME} version {#APP_VER}
DefaultDirName={pf}\Elefun\{#APP_NAME}
DefaultGroupName={#APP_NAME}
UninstallDisplayName={#APP_NAME} (remove only)
UninstallDisplayIcon={app}\{#APP_NAME}.exe
LicenseFile={#APP_NAME}\license.rtf
Compression=lzma
SolidCompression=true
OutputDir=\_WPLabs\_Installers
OutputBaseFilename={#APP_NAME}
AppCopyright=Elefun multimedia
AppPublisherURL=http://www.elefun-desktops.com/ 
AlwaysShowDirOnReadyPage=true
AlwaysShowGroupOnReadyPage=true
ShowLanguageDialog=yes
WizardImageFile=_Stuff\sa_setup_big.bmp
WizardSmallImageFile=_Stuff\sa_setup_small.bmp
SetupIconFile=_Stuff\sa_setup_exe.ico

[Files]

Source: {#APP_AFNAME}\*; DestDir: {app}; Flags: recursesubdirs ignoreversion createallsubdirs uninsrestartdelete

[Icons]
Name: {group}\Uninstall {#APP_NAME}; Filename: "{uninstallexe}"
Name: {group}\Run {#APP_NAME}; Filename: {app}\{#APP_NAME}.exe
Name: {group}\{#APP_NAME} Help; Filename: {app}\help.html

[Registry] 
Root: HKCU; Subkey: "Software\Elefun\ScreenArt"; ValueType: string; ValueName: "InstallPath3"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\Elefun\ScreenArt"; ValueType: string; ValueName: "InstallPath3"; ValueData: "{app}"; Flags: uninsdeletekey

[UninstallDelete]
Type: filesandordirs; Name: "{app}\*.*"

[Run]
FileName: "{app}\{#APP_NAME}.exe"; Description: "Start {#APP_NAME}"; Flags: nowait postinstall runmaximized

[Code]
var
  UserData: String;

function InstallDate(Param: String): String;
begin
  Result := GetDateTimeString('dd.mm.yyyy',#0, #0);
end;

procedure CurStepChanged(CurStep: TSetupStep);
var ResultCode: Integer;
    Pars: String;
    MainWnd: HWND;
begin
   if CurStep = ssPostInstall then begin
    Exec(ExpandConstant('{app}\{#APP_NAME}.exe'), '-console=yes -wpsave=yes -affid='+ExpandConstant('{#APP_AFNAME}')+' -date='+ExpandConstant('{code:InstallDate}'), '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  end;
   if CurStep = ssInstall then begin
    MainWnd := FindWindowByWindowName('WC_ROOT');
    PostMessage(MainWnd, 16, 0, 0);//0x0010 - WM_CLOSE
  end;
end;

function InitializeUninstall(): Boolean;
var
  ResultCode: Integer;
  MainWnd: HWND;
begin
  Exec(ExpandConstant('{app}\{#APP_NAME}.exe'), '-console=yes -wprestore=yes', '', SW_SHOWMINIMIZED, ewWaitUntilTerminated, ResultCode)
  UserData := ExpandConstant('{ini:{app}\install.ini,Main,UserData|Unknown}');
  MainWnd := FindWindowByWindowName('WC_ROOT');
  SendMessage(MainWnd, 16, 0, 0);//0x0010 - WM_CLOSE
  Result := True;
end;
  
