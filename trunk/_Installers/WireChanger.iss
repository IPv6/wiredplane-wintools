#include "_preprocess.iss"

[Setup]
AppName={#APP_NAME}
AppVerName={#APP_NAME} version {#APP_VER}
DefaultDirName={pf}\WiredPlane\{#APP_NAME}
DefaultGroupName={#APP_NAME}
UninstallDisplayName={#APP_NAME} (remove only)
UninstallDisplayIcon={app}\{#APP_NAME}.exe
LicenseFile={#APP_NAME}\license.rtf
Compression=lzma
SolidCompression=true
OutputDir=\_WPLabs\_Installers
OutputBaseFilename={#APP_NAME}
AppCopyright=WiredPlane labs
AppPublisherURL=http://www.wiredplane.com/ 
AlwaysShowDirOnReadyPage=true
AlwaysShowGroupOnReadyPage=true
ShowLanguageDialog=yes
WizardImageFile=_Stuff\wc_setup_big.bmp
WizardSmallImageFile=_Stuff\wc_setup_small.bmp

[Files]

Source: {#APP_AFNAME}\*; DestDir: {app}; Flags: recursesubdirs ignoreversion createallsubdirs uninsrestartdelete
Source: {#APP_AFNAME}\Wired_Screensaver.exe; DestDir: {win}; Flags: ignoreversion overwritereadonly

[Icons]
Name: {group}\Uninstall {#APP_NAME}; Filename: "{uninstallexe}"
Name: {group}\Run {#APP_NAME}; Filename: {app}\{#APP_NAME}.exe
Name: {group}\{#APP_NAME} Help; Filename: {app}\{#APP_NAME}_l0.chm
Name: {group}\{#APP_NAME} homepage; Filename: {app}\WiredPlane.url

[Registry] 
Root: HKCU; Subkey: "Software\WiredPlane\WireChanger"; ValueType: string; ValueName: "InstallPath3"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\WiredPlane\WireChanger"; ValueType: string; ValueName: "InstallPath3"; ValueData: "{app}"; Flags: uninsdeletekey

[UninstallDelete]
Type: filesandordirs; Name: "{app}\*.*"

[Run]
FileName: "{app}\readme.txt"; Description: "View readme"; Flags: nowait postinstall runmaximized unchecked shellexec
FileName: "{app}\{#APP_NAME}.exe"; Parameters: "-addclo=1"; Description: "Put clock on desktop"; Flags: postinstall runminimized unchecked
FileName: "{app}\{#APP_NAME}.exe"; Parameters: "-addcal=1"; Description: "Put calendar on desktop"; Flags: postinstall runminimized
FileName: "{app}\Wired_Screensaver.exe"; Parameters: "/i /t=5 /e=1 /d"; Description: "Install as slideshow screensaver"; Flags: nowait postinstall unchecked runminimized
FileName: "{app}\{#APP_NAME}.exe"; Description: "Start {#APP_NAME}"; Flags: nowait postinstall runmaximized

[UninstallRun]
Filename: "http://www.wiredplane.com/guestbook/uninstall_wirechanger.php?who={#APP_NAME}&ver={#APP_VER}&indate={ini:{app}\install.ini,Main,Date|Unknown}&affdat={ini:{app}\install.ini,Main,AffId|Unknown}"; Flags: shellexec nowait runmaximized
;{-#SaveToFile({#APP_NAME}.iss_test)}

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

function InitializeSetup(): Boolean;
begin
  Result := True;
  UserData := ExpandConstant('{reg:HKCU\Software\WiredPlane\WireChanger,InstallPathEF|none}');
  if UserData <> 'none' then begin
    MsgBox('If you are Elefun memeber, you should go to the membership area and download WireChanger from it. Installation cancelled.', mbError, MB_OK);
    Result := False;
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
  
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
   if CurUninstallStep = usPostUninstall then
	  if MsgBox('Do you want to remove all settings and notes too'+'?', mbConfirmation, MB_YESNO) = idYes then
	  begin
	    DelTree(UserData, True, True, True);
	  end;
  end;
end.
