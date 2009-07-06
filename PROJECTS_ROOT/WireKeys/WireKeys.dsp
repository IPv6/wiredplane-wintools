# Microsoft Developer Studio Project File - Name="WireKeys" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WireKeys - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WireKeys.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WireKeys.mak" CFG="WireKeys - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WireKeys - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WireKeys - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WireKeys - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /Gi /GX /O1 /D "_DEBUG2_" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 User32.lib /nologo /subsystem:windows /pdb:none /machine:I386 /nodefaultlib:"winspool.lib" /nodefaultlib:"libcmt.lib" /out:"\_WPLabs\_Installers\WireKeys\WireKeys.exe"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "WireKeys - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WINNT" /D "_DEBUG2_" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_SPY_" /D "_AFXDLL" /FR"" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"WireKeys.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 User32.lib /nologo /subsystem:windows /pdb:"WireKeys.pdb" /debug /machine:I386 /nodefaultlib:"winspool.lib" /nodefaultlib:"libcmtd.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "WireKeys - Win32 Release"
# Name "WireKeys - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Dlg_AppInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_InfoWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\DLGClip.cpp
# End Source File
# Begin Source File

SOURCE=.\DLGClip.h
# End Source File
# Begin Source File

SOURCE=.\IconDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\keyboard.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\LikStubs.cpp
# End Source File
# Begin Source File

SOURCE=.\MiscUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\OleScript.cpp
# End Source File
# Begin Source File

SOURCE=.\WireKeys.cpp
# End Source File
# End Group
# Begin Group "Options"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLG_Chooser.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_EditMLine.cpp
# End Source File
# Begin Source File

SOURCE=.\OptsStub.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# Begin Source File

SOURCE=.\WKPluginEngine.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WireKeysDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Dlg_AppInfo.h
# End Source File
# Begin Source File

SOURCE=.\DLG_Chooser.h
# End Source File
# Begin Source File

SOURCE=.\DLG_InfoWindow.h
# End Source File
# Begin Source File

SOURCE=.\icondlg.h
# End Source File
# Begin Source File

SOURCE=.\WireKeysDlg.h
# End Source File
# End Group
# Begin Group "System.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\_externs.h
# End Source File
# Begin Source File

SOURCE=.\ItemModif.h
# End Source File
# Begin Source File

SOURCE=.\MiscUtils.h
# End Source File
# Begin Source File

SOURCE=.\WireKeys.h
# End Source File
# Begin Source File

SOURCE=.\WKPluginEngine.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\WireKeys.rc
# End Source File
# Begin Source File

SOURCE=.\res\WireKeys.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\_common.h
# End Source File
# Begin Source File

SOURCE=.\_defines.h
# End Source File
# Begin Source File

SOURCE=.\DLG_EditMLine.h
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

SOURCE=.\WKPlugin.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_aboss.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_abpro.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_addre.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_cddrive.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_del.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_dispprop.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_flash.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_help.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_hidde.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_options.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_pindw.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_pinud.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_procs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_qrun.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_resta.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_scrsa.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_shutd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ccopy.cur
# End Source File
# Begin Source File

SOURCE=.\res\cpaste.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursorsa.cur
# End Source File
# Begin Source File

SOURCE=.\res\ic_macro.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_ask.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_cpu.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_inf.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_macro.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_minim.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_mainw.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_mainx.ico
# End Source File
# Begin Source File

SOURCE=.\res\imagelis.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Intendo.ico
# End Source File
# Begin Source File

SOURCE=.\res\paste.ico
# End Source File
# End Group
# Begin Group "Extern"

# PROP Default_Filter ""
# Begin Group "SystemFuncs"

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

SOURCE=..\SmartWires\Atl\crt_cstringw.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\Atl\crt_cstringw.h
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

SOURCE=..\SmartWires\SoundUtils\SimpleMixer.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SoundUtils\SimpleMixer.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SystemInfo.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SystemInfo.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\Atl\file\verinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\Atl\file\verinfo.h
# End Source File
# End Group
# Begin Group "ImageFuncs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\BMPUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\BMPUtil.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\DelayLoadFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\DelayLoadFuncs.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\GUIClasses\Hyperlink.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\GUIClasses\Hyperlink.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\Videomod.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\Videomod.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\SmartWires\GUIClasses\FileDialogST.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\GUIClasses\FileDialogST.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\hotkeys.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\hotkeys.h
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

SOURCE=..\SmartWires\SystemUtils\SupportClasses.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SupportClasses.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SupportClasses_m1.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\GUIClasses\SYSTEMTR.CPP
# End Source File
# Begin Source File

SOURCE=..\SmartWires\GUIClasses\SYSTEMTR.H
# End Source File
# End Group
# Begin Group "Help"

# PROP Default_Filter "html"
# Begin Source File

SOURCE=.\Help\api.shtml
# End Source File
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

SOURCE=.\Help\hotkeys.shtml
# End Source File
# Begin Source File

SOURCE=.\Help\macros.shtml
# End Source File
# Begin Source File

SOURCE=.\Help\otherprods.shtml
# End Source File
# Begin Source File

SOURCE=.\Help\patterns.shtml
# End Source File
# Begin Source File

SOURCE=.\Help\plugins.shtml
# End Source File
# Begin Source File

SOURCE=.\Help\qruns.shtml
# End Source File
# Begin Source File

SOURCE=.\Help\tutorial.shtml
# End Source File
# End Group
# End Target
# End Project
