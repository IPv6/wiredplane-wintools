# Microsoft Developer Studio Project File - Name="WCRemind" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=WCRemind - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WCRemind.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WCRemind.mak" CFG="WCRemind - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WCRemind - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WCRemind - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WCRemind - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Od /D "NOSTUB_VC6" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 msimg32.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /out:"\_WPLabs\_Installers\WireChanger\WCRemind.cml"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "WCRemind - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "NOSTUB_VC6" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 msimg32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Debug\WCRemind.cml" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WCRemind - Win32 Release"
# Name "WCRemind - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AlertDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\BitmapUtils\BMPUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\CalendarDlg2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SystemUtils\DataXMLSaver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\BitmapUtils\DelayLoadFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\DLGEnterNumber.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SystemUtils\FileInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\MonthCalCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\OCalendar.cpp
# End Source File
# Begin Source File

SOURCE=.\PrivateProfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SmartWndSizer\ResizeableDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SystemUtils\SupportClasses.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SystemUtils\SupportClasses_m1.cpp
# End Source File
# Begin Source File

SOURCE=.\WCRemind.cpp
# End Source File
# Begin Source File

SOURCE=.\WCRemind.def
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AlertDialog.h
# End Source File
# Begin Source File

SOURCE=.\CalendarDlg2.h
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SystemUtils\DataXMLSaver.h
# End Source File
# Begin Source File

SOURCE=.\DLGEnterNumber.h
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SystemUtils\FileInfo.h
# End Source File
# Begin Source File

SOURCE=.\MonthCalCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\OCalendar.h
# End Source File
# Begin Source File

SOURCE=.\PrivateProfile.h
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SystemUtils\RemindClasses.h
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SmartWndSizer\ResizeableDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SystemUtils\SupportClasses.h
# End Source File
# Begin Source File

SOURCE=.\WCRemind.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\main.ico
# End Source File
# Begin Source File

SOURCE=.\WCRemind.rc
# End Source File
# Begin Source File

SOURCE=.\res\WCRemind.rc2
# End Source File
# End Group
# Begin Group "Calendar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\SmartWires\GUIClasses\Cal\WMDateCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\GUIClasses\Cal\WMDateCtrl.h
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\GUIClasses\Cal\WVCellData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\GUIClasses\Cal\WVCellData.h
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\GUIClasses\Cal\WVCellDataItem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\GUIClasses\Cal\WVCellDataItem.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
