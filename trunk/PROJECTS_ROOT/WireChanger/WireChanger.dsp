# Microsoft Developer Studio Project File - Name="WireChanger" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WireChanger - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WireChanger.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WireChanger.mak" CFG="WireChanger - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WireChanger - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WireChanger - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WireChanger - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Od /Gf /Gy /D "FART_VERSION" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Winmm.lib User32.lib ws2_32.lib /nologo /subsystem:windows /pdb:none /machine:I386 /nodefaultlib:"winspool.lib" /out:"\_WPLabs\_Installers\WireChanger\WireChanger.exe"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "WireChanger - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "FART_VERSION" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINNT" /D "_DEBUG2_" /D "_AFXDLL" /FR"" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"WireChanger.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Winmm.lib User32.lib ws2_32.lib /nologo /subsystem:windows /pdb:"WireChanger.pdb" /debug /machine:I386 /nodefaultlib:"winspool.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "WireChanger - Win32 Release"
# Name "WireChanger - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLG_InfoWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MiscUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\WireChanger.cpp

!IF  "$(CFG)" == "WireChanger - Win32 Release"

!ELSEIF  "$(CFG)" == "WireChanger - Win32 Debug"

# ADD CPP /D "_NOUNLIM_"

!ENDIF 

# End Source File
# End Group
# Begin Group "WChanger"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLG_AddLink.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_Chooser.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_Note.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_WChanger.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlWallpaper.cpp
# End Source File
# Begin Source File

SOURCE=.\HtmlWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\WallpaperUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\Webbrowser2.cpp
# End Source File
# End Group
# Begin Group "Options"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsTempl.cpp
# End Source File
# Begin Source File

SOURCE=.\WPaper.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WireChangerDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLG_AddLink.h
# End Source File
# Begin Source File

SOURCE=.\DLG_Chooser.h
# End Source File
# Begin Source File

SOURCE=.\DLG_InfoWindow.h
# End Source File
# Begin Source File

SOURCE=.\DLG_Note.h
# End Source File
# Begin Source File

SOURCE=.\DLG_WChanger.h
# End Source File
# Begin Source File

SOURCE=.\WireChangerDlg.h
# End Source File
# End Group
# Begin Group "System.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\_externs.h
# End Source File
# Begin Source File

SOURCE=.\HtmlWallpaper.h
# End Source File
# Begin Source File

SOURCE=.\HtmlWnd.h
# End Source File
# Begin Source File

SOURCE=.\MiscUtils.h
# End Source File
# Begin Source File

SOURCE=.\WallpaperUtils.h
# End Source File
# Begin Source File

SOURCE=.\Webbrowser2.h
# End Source File
# Begin Source File

SOURCE=.\WireChanger.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\WireChanger.rc
# End Source File
# Begin Source File

SOURCE=.\res\WireChanger.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\_common.h
# End Source File
# Begin Source File

SOURCE=.\_defines.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Macros.h
# End Source File
# Begin Source File

SOURCE=.\Settings.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WPaper.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bm_abpro.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_add.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_addar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_addfi.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_adver.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_clean.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_clock.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_del.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_del2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_edit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_edit2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_help.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_licke.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_more.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_newi.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_newth.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_noico.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_notedel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_noteok.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_options.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_prop.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_rem.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_remft.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_save.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_setn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_sort.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_sortb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_sync.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_syncb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_unreg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bn_clone.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bt_addir.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bt_sync.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cursorsa.cur
# End Source File
# Begin Source File

SOURCE=.\res\ddown_a.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico_tx.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ico_tx.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_ask.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_bg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon_bg.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_del.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_dow.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_fn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon_fn.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_fn2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon_fn2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_inf.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_rem.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_sav.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_swt.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_vi.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_vi2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_wc.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_maina.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_maintray.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_mainw.ico
# End Source File
# Begin Source File

SOURCE=.\res\imagelis.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Intendo.ico
# End Source File
# Begin Source File

SOURCE=.\res\nochwall.ico
# End Source File
# Begin Source File

SOURCE=.\res\remind_a.bmp
# End Source File
# Begin Source File

SOURCE=.\res\upnotify.ico
# End Source File
# End Group
# Begin Group "Extern"

# PROP Default_Filter ""
# Begin Group "BitmapUtils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\BMPUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\BMPUtil.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\CPreviewFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\CPreviewFileDialog.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\DelayLoadFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\DelayLoadFuncs.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\Dib.cpp
# End Source File
# End Group
# Begin Group "GUIClasses"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SmartWires\GUIClasses\DLG_Options.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\GUIClasses\DLG_Options.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\GUIClasses\IListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\GUIClasses\IListCtrl.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\MenuToolTip.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SmartWndSizer\SmartWndSizer.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SmartWndSizer\SmartWndSizer.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\GUIClasses\SYSTEMTR.CPP
# End Source File
# Begin Source File

SOURCE=..\SmartWires\GUIClasses\SYSTEMTR.H
# End Source File
# End Group
# Begin Group "SystemUtils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\AutoUpdate.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\AutoUpdate.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Lik\checksum.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Lik\checksum.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\DataXMLSaver.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\DataXMLSaver.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\FileInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\FileInfo.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\hotkeys.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\hotkeys.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\InternetUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\InternetUtils.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Lik\md5.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Lik\md5.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\NtSystemInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\math\parser.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\math\parser.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\regexp\regexp.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\regexp\regexp.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SupportClasses.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SupportClasses.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SystemInfo.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SystemInfo.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\XMLP.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\XMLP.h
# End Source File
# End Group
# Begin Group "Zip"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\adler32.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\compress.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\crc32.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\crc32.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\cryptunzip.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\cryptutil.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\cryptutil.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\cryptzip.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\deflate.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\deflate.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\gzio.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\infback.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\inffast.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\inffast.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\inffixed.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\inflate.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\inflate.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\inftrees.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\inftrees.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\trees.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\trees.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\uncompr.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\unzip.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\zconf.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\zip.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\ZipArchive.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\zlib.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\zutil.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Zip\zutil.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SupportClasses_m1.cpp
# End Source File
# End Group
# Begin Group "Help"

# PROP Default_Filter "html"
# Begin Source File

SOURCE=.\Help\faq.shtml
# End Source File
# Begin Source File

SOURCE=.\Help\features.shtml
# End Source File
# Begin Source File

SOURCE=.\Help\history.shtml
# End Source File
# Begin Source File

SOURCE=.\Help\overview.shtml
# End Source File
# Begin Source File

SOURCE=.\Help\preferences.shtml
# End Source File
# End Group
# Begin Source File

SOURCE=.\cximage_wc.cml
# End Source File
# End Target
# End Project
