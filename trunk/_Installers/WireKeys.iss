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
WizardImageFile=_Stuff\wk_setup_big.bmp
WizardSmallImageFile=_Stuff\wc_setup_small.bmp

[Files]

Source: {#APP_AFNAME}\*; DestDir: {app}; Flags: recursesubdirs ignoreversion createallsubdirs uninsrestartdelete

[Icons]
Name: {group}\Uninstall {#APP_NAME}; Filename: "{uninstallexe}"
Name: {group}\Run {#APP_NAME}; Filename: {app}\{#APP_NAME}.exe
Name: {group}\{#APP_NAME} Help; Filename: {app}\{#APP_NAME}_l0.chm
Name: {group}\{#APP_NAME} homepage; Filename: {app}\WiredPlane.url

[Registry] 
Root: HKCU; Subkey: "Software\WiredPlane\WireKeys"; ValueType: string; ValueName: "InstallPath3"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\WiredPlane\WireKeys"; ValueType: string; ValueName: "InstallPath3"; ValueData: "{app}"; Flags: uninsdeletekey

[UninstallDelete]
Type: filesandordirs; Name: "{app}\*.*"

[Run]
FileName: "{app}\readme.txt"; Description: "View readme"; Flags: nowait postinstall runmaximized unchecked shellexec
FileName: "{app}\history.html"; Description: "Latest changes"; Flags: nowait postinstall runmaximized unchecked shellexec
FileName: "{app}\{#APP_NAME}.exe"; Description: "Start {#APP_NAME}"; Flags: nowait postinstall runmaximized

[UninstallRun]
Filename: "http://www.wiredplane.com/guestbook/uninstall_wirekeys.php?who={#APP_NAME}&ver={#APP_VER}&indate={ini:{app}\install.ini,Main,Date|Unknown}&affdat={ini:{app}\install.ini,Main,AffId|Unknown}"; Flags: shellexec nowait runmaximized
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
    SetIniString('Main','AffId',ExpandConstant('{#APP_AFNAME}'),ExpandConstant('{app}\install.ini'));
    SetIniString('Main','Date',ExpandConstant('{code:InstallDate}'),ExpandConstant('{app}\install.ini'));
  end;
   if CurStep = ssInstall then begin

	DeleteFile(ExpandConstant('{app}\Plugins\wkatexth._p'));
    RenameFile(ExpandConstant('{app}\Plugins\wkatexth.dll'),ExpandConstant('{app}\Plugins\wkatexth._p'));

    DeleteFile(ExpandConstant('{app}\Plugins\wkkeyh._p'));
    RenameFile(ExpandConstant('{app}\Plugins\wkkeyh.dll'),ExpandConstant('{app}\Plugins\wkkeyh._p'));

    DeleteFile(ExpandConstant('{app}\Plugins\WP_WTraits._p'));
    RenameFile(ExpandConstant('{app}\Plugins\WP_WTraits.wkp'),ExpandConstant('{app}\Plugins\WP_WTraits._p'));

    DeleteFile(ExpandConstant('{app}\Plugins\WP_OpenWithExt._p'));
    RenameFile(ExpandConstant('{app}\Plugins\WP_OpenWithExt.wkp'),ExpandConstant('{app}\Plugins\WP_OpenWithExt._p'));
    
    MainWnd := FindWindowByWindowName('WK_ROOT');
    if(MainWnd <> 0) then
    begin
      //PostMessage(MainWnd, 16, 0, 0);//0x0010 - WM_CLOSE
      if MsgBox('WireKeys is currently running. Close WireKeys and then continue. Continue?', mbConfirmation, MB_YESNO) <> idYes then
        begin
          Abort();
    	  end;
 	  end;
  end;
end;

function InitializeUninstall(): Boolean;
var
  ResultCode: Integer;
  MainWnd: HWND;
begin
  UserData := ExpandConstant('{ini:{app}\install.ini,Main,UserData|Unknown}');
  MainWnd := FindWindowByWindowName('WK_ROOT');
  if(MainWnd <> 0) then
  begin
    //PostMessage(MainWnd, 16, 0, 0);//0x0010 - WM_CLOSE
    if MsgBox('WireKeys is currently running. Close WireKeys and then continue. Continue?', mbConfirmation, MB_YESNO) <> idYes then
    begin
      Abort();
    end;
  end;
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
