# Microsoft Developer Studio Generated NMAKE File, Based on WP_RClickcp.dsp
!IF "$(CFG)" == ""
CFG=WP_RClickcp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to WP_RClickcp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "WP_RClickcp - Win32 Release" && "$(CFG)" != "WP_RClickcp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WP_RClickcp.mak" CFG="WP_RClickcp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WP_RClickcp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "WP_RClickcp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WP_RClickcp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\_Distr2\WKP_RClickcp\WP_RClickcp.wkp"


CLEAN :
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\WP_RClickcp.obj"
	-@erase "$(INTDIR)\WP_RClickcp.pch"
	-@erase "$(INTDIR)\WP_RClickcp.res"
	-@erase "$(OUTDIR)\WP_RClickcp.exp"
	-@erase "$(OUTDIR)\WP_RClickcp.lib"
	-@erase "..\..\..\..\_Distr2\WKP_RClickcp\WP_RClickcp.wkp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /w /W0 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENABLER_EXPORTS" /Fp"$(INTDIR)\WP_RClickcp.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\WP_RClickcp.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WP_RClickcp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib comdlg32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\WP_RClickcp.pdb" /machine:I386 /def:".\WP_RClickcp.def" /out:"\_Distr2\WKP_RClickcp\WP_RClickcp.wkp" /implib:"$(OUTDIR)\WP_RClickcp.lib" 
DEF_FILE= \
	".\WP_RClickcp.def"
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\WP_RClickcp.obj" \
	"$(INTDIR)\WP_RClickcp.res"

"..\..\..\..\_Distr2\WKP_RClickcp\WP_RClickcp.wkp" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WP_RClickcp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\Debug\Plugins\WP_RClickcp.wkp" "$(OUTDIR)\WP_RClickcp.bsc"


CLEAN :
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\WP_RClickcp.obj"
	-@erase "$(INTDIR)\WP_RClickcp.pch"
	-@erase "$(INTDIR)\WP_RClickcp.res"
	-@erase "$(INTDIR)\WP_RClickcp.sbr"
	-@erase "$(OUTDIR)\WP_RClickcp.bsc"
	-@erase "$(OUTDIR)\WP_RClickcp.exp"
	-@erase "$(OUTDIR)\WP_RClickcp.lib"
	-@erase "$(OUTDIR)\WP_RClickcp.pdb"
	-@erase "..\Debug\Plugins\WP_RClickcp.ilk"
	-@erase "..\Debug\Plugins\WP_RClickcp.wkp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENABLER_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\WP_RClickcp.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\WP_RClickcp.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WP_RClickcp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\WP_RClickcp.sbr"

"$(OUTDIR)\WP_RClickcp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\WP_RClickcp.pdb" /debug /machine:I386 /def:".\WP_RClickcp.def" /out:"..\Debug\Plugins\WP_RClickcp.wkp" /implib:"$(OUTDIR)\WP_RClickcp.lib" /pdbtype:sept 
DEF_FILE= \
	".\WP_RClickcp.def"
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\WP_RClickcp.obj" \
	"$(INTDIR)\WP_RClickcp.res"

"..\Debug\Plugins\WP_RClickcp.wkp" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("WP_RClickcp.dep")
!INCLUDE "WP_RClickcp.dep"
!ELSE 
!MESSAGE Warning: cannot find "WP_RClickcp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "WP_RClickcp - Win32 Release" || "$(CFG)" == "WP_RClickcp - Win32 Debug"
SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "WP_RClickcp - Win32 Release"

CPP_SWITCHES=/nologo /MT /w /W0 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENABLER_EXPORTS" /Fp"$(INTDIR)\WP_RClickcp.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\WP_RClickcp.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "WP_RClickcp - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENABLER_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\WP_RClickcp.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\WP_RClickcp.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\WP_RClickcp.cpp

!IF  "$(CFG)" == "WP_RClickcp - Win32 Release"


"$(INTDIR)\WP_RClickcp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\WP_RClickcp.pch"


!ELSEIF  "$(CFG)" == "WP_RClickcp - Win32 Debug"


"$(INTDIR)\WP_RClickcp.obj"	"$(INTDIR)\WP_RClickcp.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\WP_RClickcp.pch"


!ENDIF 

SOURCE=.\WP_RClickcp.rc

"$(INTDIR)\WP_RClickcp.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

