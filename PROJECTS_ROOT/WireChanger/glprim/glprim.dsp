# Microsoft Developer Studio Project File - Name="glprim" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=glprim - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "glprim.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "glprim.mak" CFG="glprim - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "glprim - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "glprim - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "glprim - Win32 Release"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_SKIP_MULTIMON_" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /pdb:none /machine:I386 /out:"\_WPLabs\_Installers\WireChanger\Template\EasyDraw.dll"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "glprim - Win32 Debug"

# PROP BASE Use_MFC 5
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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Debug\Template\EasyDraw.dll " /pdbtype:sept

!ENDIF 

# Begin Target

# Name "glprim - Win32 Release"
# Name "glprim - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\SmartWires\BitmapUtils\BMPUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SystemUtils\DataXMLSaver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\BitmapUtils\DelayLoadFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\BitmapUtils\Dib.cpp
# End Source File
# Begin Source File

SOURCE=.\GlGener.cpp
# End Source File
# Begin Source File

SOURCE=.\GlGener.h
# End Source File
# Begin Source File

SOURCE=.\glprim.cpp
# End Source File
# Begin Source File

SOURCE=.\glprim.def
# End Source File
# Begin Source File

SOURCE=.\glprim.rc
# End Source File
# Begin Source File

SOURCE=.\IEffects.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SystemUtils\InternetUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SmartWndSizer\SmartWndSizer.cpp
# SUBTRACT CPP /YX /Yc /Yu
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

SOURCE=..\..\SmartWires\SystemUtils\xml\XMLite.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\SmartWires\BitmapUtils\BMPUtil.h
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SystemUtils\DataXMLSaver.h
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\BitmapUtils\DelayLoadFuncs.h
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\BitmapUtils\Dib.h
# End Source File
# Begin Source File

SOURCE=.\glprim.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SmartWndSizer\SmartWndSizer.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SystemUtils\SupportClasses.h
# End Source File
# Begin Source File

SOURCE=..\..\SmartWires\SystemUtils\xml\XMLite.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\glprim.rc2
# End Source File
# End Group
# End Target
# End Project
