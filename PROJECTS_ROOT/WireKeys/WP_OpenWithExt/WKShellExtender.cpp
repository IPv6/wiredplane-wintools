// WKShellExtender.cpp : Implementation of CWKShellExtender
#include "stdafx.h"
#include "WP_OPE.h"
#include "..\..\SmartWires\Atl\file\SADirRead.h"

#pragma comment(lib,"shell32")
extern DWORD dwFMapSize;
extern char szAddHere[128];
extern char szManageMenu[128];
extern char szSaveicons[128];
extern char szRestoreIcons[128];
extern char szFindLinkTarget[128];
extern char szRegisterDll[128];
extern char szUnregisterDll[128];
extern char szAssignHotkey[128];
extern char szCopyPaths[128];
extern char szCopyNames[128];
extern char szStartConsole[128];
extern char szFolderListing[128];
extern char szChangeIcon[128];
extern char szMountAsDrive[128];
extern char szPutInSendTo[128];
extern char szWKSpecials[128];
extern char szJumpToFolder[128];
extern char szAddPBar[128];
extern char szRemPBar[128];
extern char szConcat[128];
extern char szAssignOpen[128];
extern char	szShowDiff[128];
extern char	szCopyNFol[128];
extern char	szAdd2Path[128];
extern char szOpts[128];
extern char	szExit[128];
extern char	szFDelete[128];
extern char	szFPack[128];
extern char	szFUNPack[128];
extern COptions plgOptions;
extern char	g_CurrentFile[MAX_PATH];
extern long	bOnNT;
extern char g_GlobalStaticBuffer[MAX_PATH];
extern HWND hWKMainWnd;
BOOL SaveFile(const char* sStartDir, const char* sFileContent);
BOOL ReadFile(const char* szFileName, CCOMString& sFileContent);
BOOL AppendFile(const char* sStartDir, const char* sFileContent);
BOOL SetClipText(HWND hFore,const char* sTextToSetIntoClipA, WCHAR* sTextToSetIntoClipW, BOOL bWTrick);

BOOL DoWaitCounter(char* szData)
{
	int iCounter=100;
	while(szData[0]!=0 && iCounter>=0){
		Sleep(50);
		iCounter--;
	}
	if(iCounter<=0){
		return 0;
	}
	return 1;
}

BOOL IsConsoleApplication(const char* szFile)
{
	FILE* pFile=fopen(szFile,"rb");
	if(pFile){
		IMAGE_DOS_HEADER headDOS;
		memset(&headDOS,0,sizeof(headDOS));
		fread(&headDOS,1,sizeof(headDOS),pFile);
		fseek(pFile,headDOS.e_lfanew,SEEK_SET);
		IMAGE_NT_HEADERS headNT;
		memset(&headNT,0,sizeof(headNT));
		fread(&headNT,1,sizeof(headNT),pFile);
		fclose(pFile);
		if(headNT.OptionalHeader.Subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI){
			return TRUE;
		}
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CWKShellExtender
CCOMString CWKShellExtender::GetFirstValidFolder()
{
	if(m_szSelectedFiles.size()==0){
		return "";
	}
	CCOMString sRes=m_szSelectedFiles.front().c_str();
	if(isFolder(sRes) && sRes.Right(1)!="\\"){
		sRes+='\\';
	}
	char szD[MAX_PATH]={0},szP[MAX_PATH]={0},szN[MAX_PATH]={0},szE[MAX_PATH]={0};
	_splitpath(sRes,szD,szP,szN,szE);
	strcat(szD,szP);
	if(strchr(szD,' ')!=0){
		strcpy(szP,"\"");
		strcat(szP,szD);
		strcat(szP,"\"");
		return szP;
	}
	return szD;
}

HRESULT CWKShellExtender::Initialize ( LPCITEMIDLIST pidlFolder,
										 LPDATAOBJECT pDataObj,
										 HKEY hProgID )
{
	//FLOG("Initialize-CMD-start 1");ANDFLUSH;
	
	lDLLs=0;
	lWKPack=0;
	lLNKs=0;
	lFiles=0;
	lFolders=0;
	bBGFolder=0;
	bDesktop=0;
	hFore=GetForegroundWindow();
	//FLOG1("Initialize-CMD-%i",__LINE__);ANDFLUSH;
	if(plgOptions.bShowOnlyIfShift && GetAsyncKeyState(VK_SHIFT)>=0){
		//FLOG1("Initialize-CMD-%i",__LINE__);ANDFLUSH;
		return E_INVALIDARG;
	}
	//FLOG1("Initialize-CMD-%i",__LINE__);ANDFLUSH;
	if(plgOptions.bMenuBlocked || !plgOptions.dwWKThread){
		//FLOG1("Initialize-CMD-%i",__LINE__);ANDFLUSH;
		return E_INVALIDARG;
	}
	HRESULT hr = S_OK;
	//FLOG1("Initialize-CMD-%i",__LINE__);ANDFLUSH;
	if(pDataObj){
		FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		STGMEDIUM stg = { TYMED_HGLOBAL };
		HDROP     hDrop;
		
		// Look for CF_HDROP data in the data object.
		if ( FAILED( pDataObj->GetData ( &fmt, &stg )))
        {
			// Nope! Return an "invalid argument" error back to Explorer.
			//FLOG1("Initialize-CMD-%i",__LINE__);ANDFLUSH;
			return E_INVALIDARG;
        }
		
		// Get a pointer to the actual data.
		hDrop = (HDROP) GlobalLock ( stg.hGlobal );
		
		// Make sure it worked.
		if ( NULL == hDrop ){
			//FLOG1("Initialize-CMD-%i",__LINE__);ANDFLUSH;
			return E_INVALIDARG;
		}
		//FLOG1("Initialize-CMD-%i",__LINE__);ANDFLUSH;
		// Sanity check - make sure there is at least one filename.
		UINT uNumFiles = DragQueryFile ( hDrop, 0xFFFFFFFF, NULL, 0 );
		
		if ( 0 == uNumFiles )
        {
			GlobalUnlock ( stg.hGlobal );
			ReleaseStgMedium ( &stg );
			//FLOG1("Initialize-CMD-%i",__LINE__);ANDFLUSH;
			return E_INVALIDARG;
        }
		for(int i=0;i<uNumFiles;i++){
			// Get the name of the first file and store it in our member variable m_szFile.
			if ( 0 == DragQueryFile ( hDrop, i, m_szSelectedFile, MAX_PATH )){
				hr = E_INVALIDARG;
			};
			if(isFolder(m_szSelectedFile)){
				lFolders++;
			}else{
				lFiles++;
			}
			if(isDLL(m_szSelectedFile)){
				lDLLs++;
			}
			if(isLNK(m_szSelectedFile)){
				lLNKs++;
			}
			if(strlen(m_szSelectedFile)>7 && stricmp((const char*)m_szSelectedFile+strlen(m_szSelectedFile)-7,".wkpack")==0){
				lWKPack++;
			}
			m_szSelectedFiles.push_back(m_szSelectedFile);
			//FLOG1("Initialize-CMD-%i",__LINE__);ANDFLUSH;
		}
		GlobalUnlock ( stg.hGlobal );
		ReleaseStgMedium ( &stg );
	}else{// pidlFolder - PIDL каталога,  на котором был щелчок.  pDO - NULL, т.к. не было указано с чем работать. Мы получаем обычный путь к каталогу из его PIDL с помощью SHGetPathFromIDList() API.
		hr=SHGetPathFromIDList ( pidlFolder, m_szSelectedFile ) ? S_OK : E_INVALIDARG;
		if(hr==S_OK){
			if(isFolder(m_szSelectedFile)){
				lFolders++;
			}
			if(isDLL(m_szSelectedFile)){
				lDLLs++;
			}
			if(isLNK(m_szSelectedFile)){
				lLNKs++;
			}
			if(strlen(m_szSelectedFile)>7 && stricmp((const char*)m_szSelectedFile+strlen(m_szSelectedFile)-7,".wkpack")==0){
				lWKPack++;
			}
			m_szSelectedFiles.push_back(m_szSelectedFile);
			if(strcmp(m_szSelectedFile,getDesktopPath())==0){
				bDesktop=1;
			}
		}else{
			bBGFolder=1;
		}
	}
	//FLOG1("Initialize-CMD-%i",__LINE__);ANDFLUSH;
	if(!bDesktop){
		HWND hSubFolder=FindWindowEx(hFore,0,"SHELLDLL_DefView",0);
		if(hSubFolder){
			bBGFolder=1;
			if(FindWindowEx(hFore,0,"ReBarWindow32",0)==0 && FindWindowEx(hFore,0,"ToolbarWindow32",0)!=0){
				bBGFolder=2;
			}
		}else{
			HWND hSubFolder=FindWindowEx(hFore,0,"Snake List",0);
			if(hSubFolder){
				bBGFolder=1;
			}else{
				char szClass[128]="";
				::GetClassName(hFore,szClass,sizeof(szClass));
				if(stricmp(szClass,"TTOTAL_CMD")==0){
					bBGFolder=1;
				}
				HWND hSubFolder=FindWindowEx(hFore,0,"FolderView",0);
				if(hSubFolder){
					bBGFolder=1;
				}
			}
		}
	}
	//FLOG("Initialize-CMD-stop");ANDFLUSH;
    return hr;
}

BOOL bCritSectionInitok=0;
CRITICAL_SECTION csMainThread;
string_list m_szActions;
int AddAction(const char* szActName)
{
	EnterCriticalSection(&csMainThread);
	int iRes=m_szActions.size();
	m_szActions.push_back(szActName);
	LeaveCriticalSection(&csMainThread);
	return iRes;
}

LONG QueryBinaryValue(CRegKey& key, LPCTSTR pszValueName, void* pValue, ULONG* pnBytes)
{
	LONG lRes=0;
	DWORD dwType=0;
	lRes = ::RegQueryValueEx(key.m_hKey, pszValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pValue),	pnBytes);
	if (lRes != ERROR_SUCCESS)
		return lRes;
	if (dwType != REG_BINARY)
		return ERROR_INVALID_DATA;

	return ERROR_SUCCESS;
}

HMENU g_hmenu_ForRecents=0;
DWORD WINAPI FillRecents(UINT& uID)
{
	//FLOG("FillRecents-CMD");ANDFLUSH;
	// Iterate through files
	int nCount=0;
	while(1){
		HANDLE hRecents=CreateEvent(NULL,FALSE,FALSE,"WK_RECENTS_EVENT");
		g_CurrentFile[0]=0;
		int iRes=PostThreadMessage(plgOptions.dwWKThread,WM_FONTCHANGE,0,nCount);
		DWORD dwWRes=::WaitForSingleObject(hRecents,1500);
		CloseHandle(hRecents);
		if(g_CurrentFile[0]==0 || dwWRes!=WAIT_OBJECT_0){
			break;
		}
		if(!IsMenu(g_hmenu_ForRecents)){
			return 0;
		}
		int iID=AddAction(CCOMString("*")+g_CurrentFile);
		InsertMenu(g_hmenu_ForRecents, iID, MF_BYPOSITION, uID++, TrimMessage(g_CurrentFile,40));
		nCount++;
	}
	//FLOG("FillRecents-CMD-stop");ANDFLUSH;
	return 0;
}

HRESULT CWKShellExtender::QueryContextMenu ( HMENU hmenu, UINT  uMenuIndex, 
											   UINT  uidFirstCmd, UINT  uidLastCmd,
											   UINT  uFlags )
{
	//FLOG("QueryContextMenu-CMD");ANDFLUSH;
	
	if(plgOptions.bMenuBlocked || !plgOptions.dwWKThread || !IsWindow(hWKMainWnd)){
		return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );
	}
	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if ( uFlags & CMF_DEFAULTONLY )
		return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );
	if(bCritSectionInitok==0){
		InitializeCriticalSection(&csMainThread);
		bCritSectionInitok=1;
	}
	m_szActions.clear();
	// First, create and populate a submenu.
	HMENU hSubmenu = 0;
	if ((uFlags & CMF_RESERVED)==CMF_RESERVED){
		hSubmenu = hmenu;
	}
	UINT uID = uidFirstCmd;
	if((bBGFolder && !bDesktop) || ((uFlags & CMF_RESERVED)==CMF_RESERVED)){
		if ((uFlags & CMF_RESERVED)!=CMF_RESERVED || hSubmenu==0){
			hSubmenu = CreatePopupMenu();
		}
		// фавориты...
		int iCI=0;
		while(plgOptions.szCustomItems[iCI][0]!=0 && iCI<MAX_CITEMS){
			char cLast=plgOptions.szCustomItems[iCI][strlen(plgOptions.szCustomItems[iCI])-1];
			if(cLast!='\\' && cLast!='/'){
				char szD[MAX_PATH]={0},szP[MAX_PATH]={0},szN[MAX_PATH]={0},szE[MAX_PATH]={0};
				_splitpath(plgOptions.szCustomItems[iCI],szD,szP,szN,szE);
				strcat(szD,szP);
				strcpy(plgOptions.szCustomItems[iCI],szD);
			}
			InsertMenu ( /*plgOptions.lAddToRoot?hmenu:hSubmenu*/hSubmenu, AddAction(CCOMString("*")+plgOptions.szCustomItems[iCI]), MF_BYPOSITION, uID++, TrimMessage(plgOptions.szCustomItems[iCI],40));
			iCI++;
		}
		if(iCI<MAX_CITEMS && lFolders==1){
			InsertMenu ( hSubmenu, AddAction("addcur"), MF_BYPOSITION, uID++, szAddHere);
		}
		if(iCI>=0 || lFolders==1){
			InsertMenu ( hSubmenu, AddAction("manage"), MF_BYPOSITION, uID++, szManageMenu);
		}
		// Список рисентов
		if(!plgOptions.bShowOnlyIfShift2 || ((uFlags & CMF_RESERVED)==CMF_RESERVED) || (plgOptions.bShowOnlyIfShift2 && GetAsyncKeyState(VK_SHIFT)<0)){
			if(plgOptions.dwRItemsToGet>0){// Список рисентов...
				if(GetMenuItemCount(hSubmenu)>0){
					InsertMenu ( hSubmenu, AddAction(""), MF_BYPOSITION|MF_SEPARATOR, uID++, _T("") );
				}
				g_hmenu_ForRecents=hSubmenu;
				FillRecents(uID);
			}
		}
		if ((uFlags & CMF_RESERVED)!=CMF_RESERVED){
			if(bBGFolder==2){
				InsertMenu ( hSubmenu, AddAction(""), MF_BYPOSITION|MF_SEPARATOR, uID++, _T("") );
				InsertMenu ( hSubmenu, AddAction("addpbar"), MF_BYPOSITION, uID++, szAddPBar);
				InsertMenu ( hSubmenu, AddAction("rempbar"), MF_BYPOSITION, uID++, szRemPBar);
			}
			// Insert the submenu into the ctx menu provided by Explorer.
			MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
			mii.fMask = MIIM_SUBMENU |  MIIM_ID | 0x00000040;//MIIM_STRING |
			mii.wID = uID++;
			mii.hSubMenu = hSubmenu;
			mii.dwTypeData = szJumpToFolder;
			InsertMenuItem( hmenu, 0/*plgOptions.bSetInFirst==0?0:uMenuIndex*/, TRUE, &mii );
			AddAction("");//Это для InsertMenuItem
		}
	}
	if ((uFlags & CMF_RESERVED)!=CMF_RESERVED){
		HMENU hSubmenu = CreatePopupMenu();
		if(bDesktop){
			InsertMenu ( hSubmenu, AddAction("saveicons"), MF_BYPOSITION, uID++, szSaveicons);
			InsertMenu ( hSubmenu, AddAction("loadicons"), MF_BYPOSITION, uID++, szRestoreIcons);
		}
		if(lLNKs==1){
			InsertMenu ( hSubmenu, AddAction("findlnk"), MF_BYPOSITION, uID++, szFindLinkTarget);
		}
		if(lDLLs>0){
			InsertMenu ( hSubmenu, AddAction("regdll"), MF_BYPOSITION, uID++, szRegisterDll);
			InsertMenu ( hSubmenu, AddAction("unregdll"), MF_BYPOSITION, uID++, szUnregisterDll);
		}
		if(lFolders==1 || lFiles==1){
			InsertMenu ( hSubmenu, AddAction("ahotk"), MF_BYPOSITION, uID++, szAssignHotkey);
		}
		if(lFolders>0){
			InsertMenu ( hSubmenu, AddAction("cpnopa"), MF_BYPOSITION, uID++, szCopyNFol);
		}
		if(lWKPack==1){
			InsertMenu ( hSubmenu, AddAction("unpack"), MF_BYPOSITION, uID++, szFUNPack);
		}
		if(lFiles>1){
			if(plgOptions.bExtrastuff){
				InsertMenu ( hSubmenu, AddAction("pack"), MF_BYPOSITION, uID++, szFPack);
			}
			InsertMenu ( hSubmenu, AddAction("concat"), MF_BYPOSITION, uID++, szConcat);
		}
		//InsertMenu ( hSubmenu, AddAction("asshand"), MF_BYPOSITION, uID++, szAssignOpen);
		InsertMenu ( hSubmenu, AddAction("copypaths"), MF_BYPOSITION, uID++, szCopyPaths);
		InsertMenu ( hSubmenu, AddAction("copynames"), MF_BYPOSITION, uID++, szCopyNames);
		InsertMenu ( hSubmenu, AddAction("cmdexe"), MF_BYPOSITION, uID++, szStartConsole);
		if(lFolders>0){
			InsertMenu ( hSubmenu, AddAction("foldlist"), MF_BYPOSITION, uID++, szFolderListing);
			if(lFolders==1){
				InsertMenu ( hSubmenu, AddAction("chicon"), MF_BYPOSITION, uID++, szChangeIcon);
				InsertMenu ( hSubmenu, AddAction("mount"), MF_BYPOSITION, uID++, szMountAsDrive);
				InsertMenu ( hSubmenu, AddAction("sendto"), MF_BYPOSITION, uID++, szPutInSendTo);
			}
		}
		if(plgOptions.bExtrastuff){
			if(lFiles==2){
				InsertMenu ( hSubmenu, AddAction("showdif"), MF_BYPOSITION, uID++, szShowDiff);
			}
			if(lFolders==1){
				InsertMenu ( hSubmenu, AddAction("Add2Path"), MF_BYPOSITION, uID++, szAdd2Path);
			}
			if(lFiles==1){
				InsertMenu ( hSubmenu, AddAction("delnow"), MF_BYPOSITION, uID++, szFDelete);
			}
		}
		// Insert the submenu into the ctx menu provided by Explorer.
		MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
		mii.fMask = MIIM_SUBMENU |  MIIM_ID | 0x00000040;//MIIM_STRING |
		mii.wID = uID++;
		mii.hSubMenu = hSubmenu;
		mii.dwTypeData = szWKSpecials;
		InsertMenuItem ( hmenu, 0/*plgOptions.bSetInFirst==0?0:uMenuIndex*/, TRUE, &mii );
		AddAction("");//Это для InsertMenuItem
		// Категории...
		int iTop=0;
		DWORD dwResult=0;
		CFileMap fMap;
		fMap.Create("WK_EXTERN_STUFF_CATEGS",DWORD(MAX_PATH));
		UINT iWM_EXTERN=RegisterWindowMessage("WK_EXTERN_STUFF");//WM_USER+14;//EXTERNSTUFF;//
		while(1){
			dwResult=0;
			SendMessageTimeout(hWKMainWnd,iWM_EXTERN,(WPARAM)(0x10000000),(LPARAM)(iTop),SMTO_ABORTIFHUNG,500,&dwResult);
			if(dwResult!=1){
				break;
			}
			CFileMap fMap2("WK_EXTERN_STUFF_CATEGS",MAX_PATH);
			char* szData=(char*)fMap2.Base();
			if(szData && *szData){
				HMENU hSubmenu = CreatePopupMenu();
				{// Пункты меню!!!
					CFileMap fMapX;
					fMapX.Create("WK_EXTERN_STUFF_CATEGITEMS",DWORD(MAX_PATH));
					DWORD dwResultX=0;
					int iTopX=0;
					while(1){
						dwResultX=0;
						SendMessageTimeout(hWKMainWnd,iWM_EXTERN,(WPARAM)(0x20000000|iTop),(LPARAM)(iTopX),SMTO_ABORTIFHUNG,500,&dwResultX);
						if(dwResultX!=1){
							break;
						}
						CFileMap fMap2X("WK_EXTERN_STUFF_CATEGITEMS",MAX_PATH);
						char* szDataX=(char*)fMap2X.Base();
						if(szDataX && *szDataX){
							char* szCode=strchr(szDataX,'\t');
							szCode[0]=0;
							char szPreparedCode[256]="$";
							strcat(szPreparedCode,szCode+1);
							//FLOG2("Adding %s:%s",szPreparedCode,szDataX);
							InsertMenu ( hSubmenu, AddAction(szPreparedCode), MF_BYPOSITION, uID++, szDataX);
						}
						iTopX++;
					}
				}
				MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
				mii.fMask = MIIM_SUBMENU |  MIIM_ID | 0x00000040;
				mii.wID = uID++;
				mii.hSubMenu = hSubmenu;
				mii.dwTypeData = szData;
				InsertMenuItem ( hmenu, 0/*plgOptions.bSetInFirst==0?0:uMenuIndex*/, TRUE, &mii );
				AddAction("");//Это для InsertMenuItem
			}else{
				break;
			}
			iTop++;
		}	
	}else{
		InsertMenu ( hSubmenu, AddAction(""), MF_BYPOSITION|MF_SEPARATOR, uID++, _T("") );
		InsertMenu ( hSubmenu, AddAction("optss"), MF_BYPOSITION, uID++, szOpts);
		InsertMenu ( hSubmenu, AddAction("exit"), MF_BYPOSITION, uID++, szExit);
	}
	//DeleteCriticalSection(&csMainThread);
	//FLOG("QueryContextMenu-CMD-stop");ANDFLUSH;
	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, uID - uidFirstCmd );
}

HRESULT CWKShellExtender::GetCommandString ( UINT  idCmd,      UINT uFlags,
											   UINT* pwReserved, LPSTR pszName,
											   UINT  cchMax )
{
	//FLOG("GetCommandString-CMD");ANDFLUSH;
    return E_INVALIDARG;
}

typedef HRESULT (STDAPICALLTYPE *_DllRegisterServer)();
typedef HRESULT (STDAPICALLTYPE *_DllUnregisterServer)();
HRESULT CWKShellExtender::InvokeCommand ( LPCMINVOKECOMMANDINFO pCmdInfo )
{
	//FLOG("INVOKE-CMD");ANDFLUSH;
	
	if(plgOptions.bMenuBlocked || !plgOptions.dwWKThread){
		return E_INVALIDARG;
	}
	CSimpleTracker cl(plgOptions.bMenuBlocked);
    // If lpVerb really points to a string, ignore this function call and bail out.
    if ( 0 != HIWORD( pCmdInfo->lpVerb )){
        return E_INVALIDARG;
	}
	int iIndex=LOWORD( pCmdInfo->lpVerb );
	CCOMString sAction="";
	for(int i=0;i<m_szActions.size();i++,iIndex--){
		if(iIndex==0){
			sAction=m_szActions[i].c_str();
			break;
		}
	}
	CCOMString sFile;
	CCOMString sTextToSetIntoClip;
	if(sAction.CompareNoCase("optss")==0){
		PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,WPARAM(hFore),16);
		return NOERROR;
	}
	if(sAction.CompareNoCase("exit")==0){
		HANDLE hRecents=CreateEvent(NULL,FALSE,FALSE,"WK_RECENTS_EVENT");
		int iRes=PostThreadMessage(plgOptions.dwWKThread,WM_TIMECHANGE,0,0);
		::WaitForSingleObject(hRecents,500);
		CloseHandle(hRecents);
		return NOERROR;
	}
	if(sAction.CompareNoCase("pack")==0){
		PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,11);
		CCOMString sFileOut;
		for(int f=0;f<m_szSelectedFiles.size();f++){
			sFile=m_szSelectedFiles[f].c_str();
			if(sFileOut==""){
				sFileOut=sFile+".wkpack";
			}
			DWORD dwSize=GetFileSize((const char*)sFile);
			if(dwSize!=0){
				FILE* f=fopen((const char*)sFile,"rb");
				if(f!=NULL){
					// =======
					char* szData = new char[dwSize+1];
					memset(szData,0,dwSize+1);
					DWORD dwReadedBytes=fread(szData,1,dwSize,f);
					fclose(f);
					// =======
					FILE* f2=fopen((const char*)sFileOut,"a+b");
					CCOMString sDelimiter="%STOPMARK_00%";
					int Cnt=0;
					while(strstr(szData,sDelimiter)!=0){
						sDelimiter.Format("%STOPMARK_%02i%",Cnt++);
					}
					char szN[MAX_PATH]={0};
					char szE[MAX_PATH]={0};
					_splitpath((const char*)sFile,0,0,szN,szE);
					CCOMString sBeginning;
					sBeginning.Format("<FILE NAME='%s%s' FINISH='%s'>\n",szN,szE,(const char*)sDelimiter);
					fwrite(sBeginning,1,sBeginning.GetLength(),f2);
					fwrite(szData,1,dwReadedBytes,f2);
					sBeginning.Format("\n</FILE NAME='%s%s' FINISH='%s'>\n\n",szN,szE,(const char*)sDelimiter);
					fwrite(sBeginning,1,sBeginning.GetLength(),f2);
					fclose(f2);
					delete[] szData;
				}
			}
		}
		sAction="";
		PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,12);
	}
	if(sAction.CompareNoCase("concat")==0){
		PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,11);
		CCOMString sFileOut;
		for(int f=0;f<m_szSelectedFiles.size();f++){
			sFile=m_szSelectedFiles[f].c_str();
			if(sFileOut==""){
				sFileOut=sFile+".full";
			}
			DWORD dwSize=GetFileSize((const char*)sFile);
			if(dwSize!=0){
				FILE* f=fopen((const char*)sFile,"rb");
				if(f!=NULL){
					char* szData = new char[dwSize+1];
					memset(szData,0,dwSize+1);
					DWORD dwReadedBytes=fread(szData,1,dwSize,f);
					fclose(f);
					FILE* f2=fopen((const char*)sFileOut,"a+b");
					fwrite(szData,1,dwReadedBytes,f2);
					fclose(f2);
					delete[] szData;
				}
			}
		}
		sAction="";
		PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,12);
	}
	if(sAction.CompareNoCase("asshand")==0){
		for(int f=0;f<m_szSelectedFiles.size();f++){
			sFile=m_szSelectedFiles[f].c_str();
		}
		//???
		sAction="";
	}
	if(sAction.CompareNoCase("showdif")==0){
		PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,11);
		CCOMString sFile1=(m_szSelectedFiles.front()).c_str();
		CCOMString sFile2=(m_szSelectedFiles.back()).c_str();
		if(IsConsoleApplication(plgOptions.szDiffEditor) || strstr(plgOptions.szDiffEditor,"fc.exe")!=NULL){
			char szSendToPath[MAX_PATH]={0};
			GetTempPath(sizeof(szSendToPath),szSendToPath);
			CCOMString sCmd="del /F /Q FileDiff.txt\r\n";
			sCmd+="\"";
			sCmd+=plgOptions.szDiffEditor;
			sCmd+="\" \"";
			sCmd+=sFile1.GetString();
			sCmd+="\" \"";
			sCmd+=sFile2.GetString();
			sCmd+="\"";
			sCmd+=" >FileDiff.txt\r\n";
			sCmd+="start FileDiff.txt";
			CCOMString sTempBat=szSendToPath;
			sTempBat+="GetFileDifference.bat";
			SaveFile(sTempBat.GetString(),(const char*)sCmd);
			::ShellExecute(NULL,"open",sTempBat.GetString(),"",szSendToPath,SW_HIDE);
		}else{
			CCOMString sCmd="\"";
			sCmd+=plgOptions.szDiffEditor;
			sCmd+="\" \"";
			sCmd+=sFile1.GetString();
			sCmd+="\" \"";
			sCmd+=sFile2.GetString();
			sCmd+="\"";
			//MessageBox(0,sCmd.GetString(),"DEBUG",0);
			//::ShellExecute(NULL,NULL,plgOptions.szDiffEditor,sCmd.GetString(),"",SW_HIDE);
			::WinExec(sCmd.GetString(),SW_SHOWNORMAL);
		}
		sAction="";
		PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,12);
	}
	if(sAction!=""){
		if(sAction.CompareNoCase("addcur")==0 && pCmdInfo->cbSize==0){
			PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,9);
		}else if(sAction.CompareNoCase("manage")==0){
			//memset(plgOptions.szCustomItems,0,MAX_CITEMS*MAX_PATH);
			PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,9);
		}else if(sAction[0]=='*'){
			strcpy(g_CurrentFile,((const char*)sAction)+1);
			PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,WPARAM(hFore),6);
		}else if(sAction[0]=='$'){
			strcpy(g_CurrentFile,((const char*)sAction)+1);
			//FLOG1("Executing %s",g_CurrentFile);
			dwFMapSize=m_szSelectedFiles.size()*MAX_PATH+100;
			CFileMap fMap;
			fMap.Open("WK_SHELL_DOMACROS",true,dwFMapSize);
			char* szData=(char*)fMap.Base();
			memset(szData,0,dwFMapSize);
			if(szData){
				int iType=plgOptions.bDobyone;
				if(GetAsyncKeyState(VK_SHIFT)<0){
					iType=!iType;
				}
				if(!iType){
					for(int f=0;f<m_szSelectedFiles.size();f++){
						if(!DoWaitCounter(szData)){
							break;
						}
						strcpy(szData,m_szSelectedFiles[f].c_str());
						PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,WPARAM(hFore),14);
					}
				}else{
					DoWaitCounter(szData);
					char* szPos=szData;
					for(int f=0;f<m_szSelectedFiles.size();f++){
						if(szPos!=szData){
							szPos[0]='\n';
							szPos++;
						}
						int iLen=strlen(m_szSelectedFiles[f].c_str());
						memcpy(szPos,m_szSelectedFiles[f].c_str(),iLen);
						szPos+=iLen;
					}
					PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,WPARAM(hFore),14);
				}
				// Ждем очистки!!!
				DoWaitCounter(szData);
			}
		}else for(int f=0;f<m_szSelectedFiles.size();f++){
			sFile=m_szSelectedFiles[f].c_str();
			//Простые вещи обрабатываем по месту...
			if(sAction.CompareNoCase("saveicons")==0){
				PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,1);
			}
			if(sAction.CompareNoCase("loadicons")==0){
				PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,2);
			}
			if(sAction.CompareNoCase("regdll")==0){
				HRESULT hr=S_FALSE;
				HINSTANCE hDLL=LoadLibrary(sFile.GetString());
				if(hDLL){
					_DllRegisterServer fpReg=(_DllRegisterServer)GetProcAddress(hDLL, "DllRegisterServer");
					if(fpReg){
						hr=fpReg();
					}
				}
				if(hr!=S_OK){
					MessageBox(0,"Error: Failed to register DLL","DLL registration",0);
				}else{
					MessageBox(0,"DLL Registered successfully","DLL registration",0);
				}
			}
			if(sAction.CompareNoCase("unregdll")==0){
				HRESULT hr=S_FALSE;
				HINSTANCE hDLL=LoadLibrary(sFile.GetString());
				if(hDLL){
					_DllRegisterServer fpReg=(_DllUnregisterServer)GetProcAddress(hDLL, "DllUnregisterServer");
					if(fpReg){
						hr=fpReg();
					}
				}
				if(hr!=S_OK){
					MessageBox(0,"Error: Failed to unregister DLL","DLL unregistration",0);
				}else{
					MessageBox(0,"DLL Unregistered successfully","DLL unregistration",0);
				}
			}
			if(sAction.CompareNoCase("ahotk")==0){
				strcpy(g_CurrentFile,sFile.GetString());
				PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,3);
			}
			if(sAction.CompareNoCase("delnow")==0){
				strcpy(g_CurrentFile,sFile.GetString());
				PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,15);
			}
			if(sAction.CompareNoCase("Add2Path")==0){
				strcpy(g_CurrentFile,sFile.GetString());
				PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,13);
			}
			if(sAction.CompareNoCase("cpnopa")==0){
				strcpy(g_CurrentFile,sFile.GetString());
				PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,10);
			}
			if(sAction.CompareNoCase("foldlist")==0){
				PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,11);
				char szSendToPath[MAX_PATH]={0};
				GetTempPath(sizeof(szSendToPath),szSendToPath);
				CCOMString sCmd="del /F /Q FolderListing.txt\r\n";
				sCmd+="cmd.exe /U /C ";
				sCmd+="dir /S \"";
				sCmd+=sFile.GetString();
				sCmd+="\" >FolderListing.txt\r\n";
				sCmd+="start ";
				sCmd+=plgOptions.szListingEditor;
				sCmd+=" FolderListing.txt";
				CCOMString sTempBat=szSendToPath;
				sTempBat+="GetFolderListing.bat";
				SaveFile(sTempBat.GetString(),(const char*)sCmd);
				::ShellExecute(NULL,"open",sTempBat.GetString(),"",szSendToPath,SW_HIDE);
				PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,12);
			}
			if(sAction.CompareNoCase("sendto")==0){
				char szSendToPath[MAX_PATH]={0},szN[MAX_PATH]={0};
				GetSpecialFolderPath(szSendToPath,CSIDL_SENDTO);
				CCOMString sDsc="Link to ";
				sDsc+=sFile;
				sDsc+=", created by WireKeys";
				_splitpath(sFile.GetString(),0,0,szN,0);
				strcat(szSendToPath,"\\Copy to ");
				strcat(szSendToPath,szN);
				strcat(szSendToPath,".lnk");
				CreateLink(sFile.GetString(),szSendToPath,sDsc.GetString(),"");
			}
			if(sAction.CompareNoCase("chicon")==0){
				strcpy(g_CurrentFile,sFile.GetString());
				PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,4);
			}
			if(sAction.CompareNoCase("mount")==0){
				char cDisk[]="Z:";
				char szDiskPath[16]={0};
				strcpy(szDiskPath,cDisk);
				strcat(szDiskPath,"\\");
				while(cDisk[0]>'C' && GetDriveType(szDiskPath)!=DRIVE_NO_ROOT_DIR){
					cDisk[0]--;
				}
				if(cDisk[0]>'C'){
					CCOMString sParams=cDisk;
					sParams+=" \"";
					sParams+=sFile.GetString();
					sParams+="\"";
					::ShellExecute(NULL,"open","subst.exe",sParams.GetString(),0,SW_HIDE);
				}
			}
			if(sAction.CompareNoCase("addpbar")==0){
				strcpy(g_CurrentFile,sFile.GetString());
				PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,7);
			}
			if(sAction.CompareNoCase("rempbar")==0){
				PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,8);
			}
			if(sAction.CompareNoCase("addcur")==0){
				int iCI=0;
				while(plgOptions.szCustomItems[iCI][0]!=0){
					iCI++;
				}
				if(iCI<MAX_CITEMS){
					strcpy(plgOptions.szCustomItems[iCI],sFile.GetString());
					if(isFolder(plgOptions.szCustomItems[iCI])){
						char cLast=plgOptions.szCustomItems[iCI][strlen(plgOptions.szCustomItems[iCI])-1];
						if(cLast!='\\' && cLast!='/'){
							strcat(plgOptions.szCustomItems[iCI],"\\");
						}
					}
					PostThreadMessage(plgOptions.dwWKThread,WM_COMMAND,0,9);
				}
			}
			if(sAction.CompareNoCase("cmdexe")==0){
				if(isFolder(sFile.GetString()) && sFile.GetString()[strlen(sFile.GetString())-1]!='\\'){
					sFile+="\\";
				}
				char szD[MAX_PATH]={0},szP[MAX_PATH]={0},szN[MAX_PATH]={0},szE[MAX_PATH]={0};
				_splitpath(sFile.GetString(),szD,szP,szN,szE);
				strcat(szD,szP);
				::ShellExecute(NULL,"open","cmd.exe",0,szD,SW_SHOWNORMAL);
			}
			if(sAction.CompareNoCase("copypaths")==0){
				sTextToSetIntoClip+=sFile.GetString();
				sTextToSetIntoClip+="\r\n";
			}
			if(sAction.CompareNoCase("copynames")==0){
				char szD[MAX_PATH]={0},szP[MAX_PATH]={0},szN[MAX_PATH]={0},szE[MAX_PATH]={0};
				_splitpath(sFile.GetString(),szD,szP,szN,szE);
				strcat(szN,szE);
				sTextToSetIntoClip+=szN;
				sTextToSetIntoClip+="\r\n";
			}
			if(sAction.CompareNoCase("findlnk")==0){
				char szD[MAX_PATH]={0};
				char szP[MAX_PATH]={0};
				char strTmpFile[MAX_PATH]={0};			
				if (SUCCEEDED(UnpackLink(sFile.GetString(),strTmpFile))){
					_splitpath(strTmpFile,szD,szP,0,0);
					CCOMString sFolder=szD;
					sFolder+=szP;
					::ShellExecute(NULL,"open",sFolder.GetString(),0,0,SW_SHOWNORMAL);
				}
			}
		}
	}
	if(sTextToSetIntoClip!=""){
		sTextToSetIntoClip.TrimLeft();
		sTextToSetIntoClip.TrimRight();
		SetClipText(hFore,sTextToSetIntoClip.GetString(),0,0);
	}
	//FLOG("INVOKE-CMD-stop");ANDFLUSH;
	return NOERROR;
}

/*
Does anyone know the parameters to the undocumented shell32.dll routine called
OpenAs_RunDLL?  It is the routine that displays the Open With dialog that
allows you to create an association for unregistered file types.  I have been
able to surmise that it takes 4 LONG parameters as follows:
param1:  HWND  - parent window
param2:  INSTANCE
param3:  LPTSTR - path of file/document you are trying to create an association
with
param4: nShowCommand

*/