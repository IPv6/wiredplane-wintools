# Microsoft Developer Studio Project File - Name="WireNote" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WireNote - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WireNote.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WireNote.mak" CFG="WireNote - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WireNote - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WireNote - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WireNote - Win32 Release"

# PROP BASE Use_MFC 6
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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Z7 /Od /Gf /Gy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_DEBUG2_" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Winmm.lib User32.lib ws2_32.lib /nologo /subsystem:windows /pdb:none /machine:I386 /out:"\_WPLabs\_Installers\WireNote\WireNote.exe"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "WireNote - Win32 Debug"

# PROP BASE Use_MFC 6
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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "WINNT" /D "_DEBUG2_" /FR"" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"WireNote.bsc"
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 User32.lib ws2_32.lib Winmm.lib /nologo /subsystem:windows /pdb:"WireNote.pdb" /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "WireNote - Win32 Release"
# Name "WireNote - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLG_Find.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_InfoWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_SimpleEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemModif.cpp
# End Source File
# Begin Source File

SOURCE=.\LogActions.cpp
# End Source File
# Begin Source File

SOURCE=.\MiscUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\NoteEditCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\NoteEditCtrl.h
# End Source File
# Begin Source File

SOURCE=.\WireNote.cpp

!IF  "$(CFG)" == "WireNote - Win32 Release"

!ELSEIF  "$(CFG)" == "WireNote - Win32 Debug"

# ADD CPP /D "_NOUNLIM_"

!ENDIF 

# End Source File
# End Group
# Begin Group "Addrbook"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AddrBook.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_Addrbook.cpp
# End Source File
# Begin Source File

SOURCE=.\NewPerson.cpp
# End Source File
# End Group
# Begin Group "Notes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLG_Note.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_NoteSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Notesbook.cpp
# End Source File
# End Group
# Begin Group "Tasks"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLG_Newtask.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskHistory.cpp
# End Source File
# End Group
# Begin Group "Reminders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLG_Reminder.cpp
# End Source File
# Begin Source File

SOURCE=.\Reminders.cpp
# End Source File
# End Group
# Begin Group "Options"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLG_About.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_ChooseDate.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_Chooser.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_OptionStyles.cpp
# End Source File
# Begin Source File

SOURCE=.\Settings.cpp
# End Source File
# End Group
# Begin Group "Messages"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Emailbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Filebox.cpp
# End Source File
# Begin Source File

SOURCE=.\Mailslot.cpp
# End Source File
# Begin Source File

SOURCE=.\MessageWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\Netbios.cpp
# End Source File
# Begin Source File

SOURCE=.\Rssreader.cpp
# End Source File
# Begin Source File

SOURCE=.\Rssreader.h
# End Source File
# Begin Source File

SOURCE=.\XmlRpc.cpp
# End Source File
# End Group
# Begin Group "AntiSpam"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AntiSpamAccount.cpp
# End Source File
# Begin Source File

SOURCE=.\AntiSpamAccount.h
# End Source File
# Begin Source File

SOURCE=.\DLG_AntiSpam.cpp
# End Source File
# Begin Source File

SOURCE=.\DLG_AntiSpam.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Pop3.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Pop3.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\WireNoteDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DLG_About.h
# End Source File
# Begin Source File

SOURCE=.\DLG_Addrbook.h
# End Source File
# Begin Source File

SOURCE=.\DLG_ChooseDate.h
# End Source File
# Begin Source File

SOURCE=.\DLG_Chooser.h
# End Source File
# Begin Source File

SOURCE=.\DLG_Find.h
# End Source File
# Begin Source File

SOURCE=.\DLG_InfoWindow.h
# End Source File
# Begin Source File

SOURCE=.\DLG_Newtask.h
# End Source File
# Begin Source File

SOURCE=.\DLG_Note.h
# End Source File
# Begin Source File

SOURCE=.\DLG_NoteSettings.h
# End Source File
# Begin Source File

SOURCE=.\DLG_OptionStyles.h
# End Source File
# Begin Source File

SOURCE=.\DLG_Reminder.h
# End Source File
# Begin Source File

SOURCE=.\DLG_SimpleEdit.h
# End Source File
# Begin Source File

SOURCE=.\NewPerson.h
# End Source File
# Begin Source File

SOURCE=.\WireNoteDlg.h
# End Source File
# End Group
# Begin Group "System.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\_externs.h
# End Source File
# Begin Source File

SOURCE=.\Emailbox.h
# End Source File
# Begin Source File

SOURCE=.\Filebox.h
# End Source File
# Begin Source File

SOURCE=.\ItemModif.h
# End Source File
# Begin Source File

SOURCE=.\LogActions.h
# End Source File
# Begin Source File

SOURCE=.\Mailslot.h
# End Source File
# Begin Source File

SOURCE=.\MiscUtils.h
# End Source File
# Begin Source File

SOURCE=.\Netbios.h
# End Source File
# Begin Source File

SOURCE=.\WireNote.h
# End Source File
# Begin Source File

SOURCE=.\XmlRpc.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\WireNote.rc
# End Source File
# Begin Source File

SOURCE=.\res\WireNote.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\_common.h
# End Source File
# Begin Source File

SOURCE=.\_defines.h
# End Source File
# Begin Source File

SOURCE=.\AddrBook.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Macros.h
# End Source File
# Begin Source File

SOURCE=.\MessageWrapper.h
# End Source File
# Begin Source File

SOURCE=.\Notesbook.h
# End Source File
# Begin Source File

SOURCE=.\Reminders.h
# End Source File
# Begin Source File

SOURCE=.\Settings.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TaskHistory.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ab_nms1ico.ico
# End Source File
# Begin Source File

SOURCE=.\res\ab_nms2.ico
# End Source File
# Begin Source File

SOURCE=.\res\ab_nms3.ico
# End Source File
# Begin Source File

SOURCE=.\res\ab_nms4.ico
# End Source File
# Begin Source File

SOURCE=.\res\adbook.ico
# End Source File
# Begin Source File

SOURCE=.\res\addrbook_away.ico
# End Source File
# Begin Source File

SOURCE=.\res\addrbook_busy.ico
# End Source File
# Begin Source File

SOURCE=.\res\addrbook_dnd.ico
# End Source File
# Begin Source File

SOURCE=.\res\addtoadd.ico
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_abpro.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_actrem.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_adrbo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_adrbo2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_creatnew.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_del.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_delpe.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_delpers.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_delta.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_edit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_edita.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_eform.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_eform1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_find.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_help.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_inm7.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_inmes.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_licke.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_logo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_mescn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_msg_e.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_msg_i.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_msg_i1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_msg_i2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_msg_i3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_msg_i4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_msg_i5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_msg_i6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_msg_in.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_msg_new.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_msg_o.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_msg_s.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_msglog.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_newme2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_newmesm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_newno.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_newpe.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_newre.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_newta.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_newtd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_newto.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_nfold.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_nlink.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_notchc.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_notcopy.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_notdel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_notes.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_notreply.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_options.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_paway.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_pbusy.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_pdnd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_pgrou.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_pindw.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_pinua.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_pinud.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_poffl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_ponli.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_popta.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_print.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_punko.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_recen.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_refre.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_save.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_share.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_skrep.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_tdcur.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_tdundo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_todod.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_todoi.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bm_wch.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_abprog.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_inm2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_inm3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_inm4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_inm5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_inm6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_newmsg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_newmsg2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_notchc.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_notdel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_note2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp_pcop.bmp
# End Source File
# Begin Source File

SOURCE=.\res\clock_bg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\copytocl.ico
# End Source File
# Begin Source File

SOURCE=.\res\cur_save.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursorsa.cur
# End Source File
# Begin Source File

SOURCE=.\res\ico_bg.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico_mes.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico_mes_in.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico_rem.ico
# End Source File
# Begin Source File

SOURCE=.\res\icoMesErr.ico
# End Source File
# Begin Source File

SOURCE=.\res\icoMesOk.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_ask.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_inf.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_m2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_m3.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_m4.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_m5.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_m6.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_m7.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_mai.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_minim.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_not.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_wc.ico
# End Source File
# Begin Source File

SOURCE=.\res\imagelis.bmp
# End Source File
# Begin Source File

SOURCE=..\WireChanger\res\Intendo.ico
# End Source File
# Begin Source File

SOURCE=..\WireKeys\res\Intendo.ico
# End Source File
# Begin Source File

SOURCE=.\res\Intendo.ico
# End Source File
# Begin Source File

SOURCE=.\res\maintool.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pinned_u.bmp
# End Source File
# Begin Source File

SOURCE=.\res\upnotify.ico
# End Source File
# Begin Source File

SOURCE=.\res\WireNote.ico
# End Source File
# Begin Source File

SOURCE=.\res\WireNote.ico
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

SOURCE=..\SmartWires\BitmapUtils\DelayLoadFuncs.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\DelayLoadFuncs.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\BitmapUtils\MenuToolTip.h
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
# Begin Group "Lik"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Lik\checksum.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Lik\checksum.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Lik\EXECryptor.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Lik\EXECryptor.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Lik\md5.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Lik\md5.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Lik\Stdlik.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\Lik\Stdlik.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\AutoUpdate.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\AutoUpdate.h
# End Source File
# Begin Source File

SOURCE=.\cncb.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\cncb.h
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

SOURCE=..\SmartWires\SystemUtils\SupportClasses.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SupportClasses.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SupportClasses_m1.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SysService.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SysService.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SystemInfo.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\SystemInfo.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\xml\XMLite.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SystemUtils\xml\XMLite.h
# End Source File
# End Group
# Begin Group "Xpc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\SmartWires\SmallXmlRpcClientServer\exports.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SmallXmlRpcClientServer\exports.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SmallXmlRpcClientServer\resource.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SmallXmlRpcClientServer\Utils.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SmallXmlRpcClientServer\Utils.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SmallXmlRpcClientServer\XmlRpcClient.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SmallXmlRpcClientServer\XmlRpcClient.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SmallXmlRpcClientServer\XmlRpcCommon.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SmallXmlRpcClientServer\XmlRpcCommon.h
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SmallXmlRpcClientServer\XmlRpcServer.cpp
# End Source File
# Begin Source File

SOURCE=..\SmartWires\SmallXmlRpcClientServer\XmlRpcServer.h
# End Source File
# End Group
# End Group
# Begin Group "Help"

# PROP Default_Filter "html"
# Begin Source File

SOURCE=.\Help\dsc_network.shtml
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

SOURCE=.\Help\overview.shtml
# End Source File
# Begin Source File

SOURCE=.\Help\preferences.shtml
# End Source File
# End Group
# End Target
# End Project
