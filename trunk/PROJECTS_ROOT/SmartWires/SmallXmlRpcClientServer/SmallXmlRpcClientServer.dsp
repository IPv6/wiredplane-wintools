# Microsoft Developer Studio Project File - Name="SmallXmlRpcClientServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SMALLXMLRPCCLIENTSERVER - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SmallXmlRpcClientServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SmallXmlRpcClientServer.mak" CFG="SMALLXMLRPCCLIENTSERVER - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SmallXmlRpcClientServer - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SmallXmlRpcClientServer - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PROJECTS_ROOT/Tools/xml-rpc/SmallXmlRpcClientServer", KVUCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SmallXmlRpcClientServer - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ws2_32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"\PROJECTS_ROOT\UTIL\SORGANIZATORDLG\Debug\SmallXmlRpcClientServer.dll "

!ELSEIF  "$(CFG)" == "SmallXmlRpcClientServer - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_INCLUDEWINSOCK_" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"\PROJECTS_ROOT\LIB&DLL_OUT\SmallXmlRpcClientServer.dll " /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SmallXmlRpcClientServer - Win32 Release"
# Name "SmallXmlRpcClientServer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DLG_ClientTest.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_ServerTest.cpp
# End Source File
# Begin Source File

SOURCE=.\exports.cpp

!IF  "$(CFG)" == "SmallXmlRpcClientServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SmallXmlRpcClientServer - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SmallXmlRpcClientServer.cpp
# End Source File
# Begin Source File

SOURCE=.\SmallXmlRpcClientServer.def

!IF  "$(CFG)" == "SmallXmlRpcClientServer - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SmallXmlRpcClientServer - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SmallXmlRpcClientServer.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Utils.cpp
# End Source File
# Begin Source File

SOURCE=.\XmlRpcClient.cpp
# End Source File
# Begin Source File

SOURCE=.\XmlRpcCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\XmlRpcServer.cpp
# End Source File
# Begin Source File

SOURCE=.\Xpc.def

!IF  "$(CFG)" == "SmallXmlRpcClientServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SmallXmlRpcClientServer - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DLG_ClientTest.h
# End Source File
# Begin Source File

SOURCE=.\DLG_ServerTest.h
# End Source File
# Begin Source File

SOURCE=.\exports.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SmallXmlRpcClientServer.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Utils.h
# End Source File
# Begin Source File

SOURCE=.\XmlRpcClient.h
# End Source File
# Begin Source File

SOURCE=.\XmlRpcCommon.h
# End Source File
# Begin Source File

SOURCE=.\XmlRpcServer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\SmallXmlRpcClientServer.rc2
# End Source File
# End Group
# Begin Group "Extern"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\SmartWndSizer\SmartWndSizer.cpp

!IF  "$(CFG)" == "SmallXmlRpcClientServer - Win32 Release"

!ELSEIF  "$(CFG)" == "SmallXmlRpcClientServer - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\SmartWndSizer\SmartWndSizer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
