#include "stdafx.h"
#include "_common.h"
#include "Settings.h"
#include "_externs.h"
#include "DLG_Chooser.h"

BOOL ParsedInternetMem(CString sText);
typedef BOOL (WINAPI *_OpenOptionsDialog)(int iTab,const char* szDefTopic, CWnd* pParentWnd, int fpPostActionType, LPVOID pPostActionParam,CSettings* pobjSettings,CStub* pwk);
BOOL CSettings::OpenOptionsDialog(int iTab,const char* szDefTopic, CWnd* pParentWnd, int fpPostActionType, LPVOID pPostActionParam)
{_XLOG_
	__FLOWLOG;
	int iRes=-1;
	wk.pMainWnd=pMainDialogWindow;
	wk.theApp=&theApp;
	wk.afxModuleStateX=AfxGetStaticModuleState();
	wk.afxModuleStateX->m_hCurrentResourceHandle=AfxGetResourceHandle();
	static long lScrSaveInit=0;
	if(lScrSaveInit==0){_XLOG_
		lScrSaveInit=1;
		// Инициализируем пути к скринсевам
		wk.InitScreenSaverPaths();
	}
	ParsedInternetMem("");//Инициируем смарт кейворды
	// Должно быть единственное место обращения к вторым утилам
	HINSTANCE hUtils=LoadLibrary(GetAppFolder()+"WKUtils2.dll");
	DWORD dwErr=GetLastError();
	if(hUtils){_XLOG_
		_OpenOptionsDialog fp=(_OpenOptionsDialog)DBG_GETPROC(hUtils,"OpenOptions");
		if(fp){_XLOG_
			iRes=(*fp)(iTab,szDefTopic, pParentWnd, fpPostActionType, pPostActionParam,&objSettings,&wk);
		}
	}
	if(iRes==-1){_XLOG_
		AfxMessageBox(Format("%s, error=%i",_l("ERROR: Important files are missing. PLease reinstall application"),dwErr));
	}
	return iRes;
}

CIHotkey DefClipMapHK_HA_Preinit[MAX_CLIPS]={0};
CIHotkey DefActionMapHK_Preinit[LAST_ACTION_CODE+1]={393325,393288,393282,393295,131264,262267,393298,786504,786515,393251,131179,131181,524361,786509,0,0,393255,393253,393256,393254,/*-->20*/262336,524326,524328,0,0,0,0,0,0,0,0,0,0,0,0/*524363*/};
CIHotkey DefClipMapHK_Preinit[MAX_CLIPS*2+1]={0,0,131169,262241,131170,262242,131171,262243,0,0,0,0,0,0,0,0,0,0};
CIHotkey DefClipMapHK_H_Preinit[MAX_CLIPS]={524374,524385,524386,524387,000000,000000,000000,0,0,0,0,0,0,0,0,0,0};
CIHotkey DefDsSwMapHK_Preinit[MAX_DESKTOPS]={196657,196658,196659};

CIHotkey* CStub::getDefActionMapHK_Preinit()
{_XLOG_
	return DefActionMapHK_Preinit;
}

CIHotkey* CStub::getDefClipMapHK_Preinit()
{_XLOG_
	return DefClipMapHK_Preinit;
}

CIHotkey* CStub::getDefClipMapHK_H_Preinit()
{_XLOG_
	return DefClipMapHK_H_Preinit;
}

CIHotkey* CStub::getDefClipMapHK_HA_Preinit()
{_XLOG_
	return DefClipMapHK_HA_Preinit;
}

CIHotkey* CStub::getDefDsSwMapHK_Preinit()
{_XLOG_
	return DefDsSwMapHK_Preinit;
}

struct AFXx_MAINTAIN_STATE
{
	AFXx_MAINTAIN_STATE(AFX_MODULE_STATE* pNewState)
	{  
		m_hCurrentResourceHandle=0; 
		if(pNewState && pNewState->m_hCurrentResourceHandle){_XLOG_
			m_hCurrentResourceHandle=AfxGetResourceHandle();
			AfxSetResourceHandle(pNewState->m_hCurrentResourceHandle); 
		}
	};
	~AFXx_MAINTAIN_STATE()
	{  
		if(m_hCurrentResourceHandle){_XLOG_
			AfxSetResourceHandle(m_hCurrentResourceHandle); 
		}
	};
protected:
	HINSTANCE m_hCurrentResourceHandle;
};

// Активация измененных данных
CCriticalSection csApply;
BOOL CALLBACK CStub::ApplyOptionsFromDialog(BOOL bSaveAndClose,DWORD dwRes,CDLG_Options* dlgOpt)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	CSmartLock ql(csApply,TRUE,0);
	BOOL bRes=FALSE;
	if(dlgOpt->isOptionClassChanged(-1))
	{_XLOG_
		if(objSettings.iLikStatus==1){_XLOG_
			ShowRegNotice(DEFAULT_REGNOTICE3,1);
		}
	}
	if(dlgOpt->isOptionClassChanged(OC_PLUGACT)){_XLOG_
		// Там сброс хоткеев должен быть...
		dlgOpt->bAnyHotkeyWasChanged=TRUE;
	}
	BOOL bKeyRegRequired=(pMainDialogWindow && (dlgOpt->bAnyHotkeyWasChanged || objSettings.bStartWithOptions || wk.bHotkeyChangedMode));
	if(bKeyRegRequired){_XLOG_
		// отключаем горячие клавиши по номерам...
		DWORD dwt1=GetTickCount();
		pMainDialogWindow->UnRegisterMyHotKeys(FALSE);
		DWORD dwt2=GetTickCount();
		TRACE1("---------- Zamer1=%i\n",dwt2-dwt1);
	}
	DWORD dwt1a=GetTickCount();
	if(dwRes==IDOK){_XLOG_
		// Обрабатываем
		if(wk.g_aListOfCategs.GetSize()>0){// Категории
			for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
				if(objSettings.aQuickRunApps[i] && objSettings.aQuickRunApps[i]->lMacros<CATEG_MACR){_XLOG_
					int iCategNumberPrev=wk.FindQRunItemByID(objSettings.aQuickRunApps[i]->sCategoryID,&wk.g_aListOfCategs);
					if(objSettings.aQuickRunApps[i]->lTempCategNumber!=iCategNumberPrev){_XLOG_
						bRes=TRUE;
					}
					objSettings.aQuickRunApps[i]->sCategoryID=wk.g_aListOfCategs[objSettings.aQuickRunApps[i]->lTempCategNumber];
					if(wk.FindQRunItemByID(objSettings.aQuickRunApps[i]->sCategoryID)==-1){_XLOG_
						objSettings.aQuickRunApps[i]->sCategoryID="";
						bRes=TRUE;
					}
				}
			}
		}
		if(dlgOpt->isOptionClassChanged(OC_STARTUP)){_XLOG_
			StartupApplicationWithWindows(objSettings.bStartWithWindows);
		}
		if(dlgOpt->isOptionClassChanged(OC_DESKRES)){_XLOG_
			for(int i=0;i<objSettings.lDesktopSwitches;i++){_XLOG_
				objSettings.lResolutions[i]=objSettings.videoModesShort[objSettings.lChoosenResolutions[i]];
			}
		}
		if(dlgOpt->isOptionClassChanged(OC_PLUGACT) || wk.bForcePluginLoadUpdate){_XLOG_
			wk.bForcePluginLoadUpdate=0;
			objSettings.sActivePlugins="";
			for(int i=0;i<objSettings.plugins.GetSize();i++){_XLOG_
				CPlugin* oPlug=objSettings.plugins[i];
				if(oPlug){_XLOG_
					oPlug->bOptionsWasChanged=TRUE;
					char szFile[MAX_PATH]="C://", szPath[MAX_PATH]="";
					_splitpath(oPlug->sFile, NULL, szPath, szFile, NULL);
					oPlug->sFileName=szFile;
					// Перекрытие активности, случившееся по отличным причинам
					if(oPlug->bIsActiveOverrideForOption!=-1){_XLOG_
						oPlug->bIsActive=oPlug->bIsActiveOverrideForOption;
						oPlug->bIsActiveOverrideForOption=-1;
					}
					if(oPlug->bIsActive){_XLOG_
						// Запоминаем что он активен
						LoadPlugin(oPlug, TRUE);
					}else{_XLOG_
						UnloadPlugin(oPlug);
					}
					if(oPlug->fOptionsManager){_XLOG_
						(*oPlug->fOptionsManager)(OM_OPTIONS_SET,oPlug,0);
					}
					SavePluginOptions(oPlug);
					bRes=TRUE;
				}
			}
		}
		if(dlgOpt->isOptionClassChanged(OC_CLREGEXP)){_XLOG_
			if(objSettings.sRegexpTo.Find("\"")!=-1){_XLOG_
				objSettings.sRegexpTo.Replace("\\\"","\"");
				objSettings.sRegexpTo.Replace("\"","\\\"");
			}
			if(objSettings.sRegexpWhat.Find("\"")!=-1){_XLOG_
				objSettings.sRegexpWhat.Replace("\\\"","\"");
				objSettings.sRegexpWhat.Replace("\"","\\\"");
			}
		}
		if(dlgOpt->isOptionClassChanged(OC_SCRSAVE)){_XLOG_
			objSettings.sValidScreenSavers="";
			for(int i=0;i<objSettings.dirScreenSavers.GetSize();i++){_XLOG_
				if(objSettings.dirScreenSavers[i].bUse){_XLOG_
					objSettings.sValidScreenSavers+=Format("|%s|",objSettings.dirScreenSavers[i].sPath);
				}
			}
			InitScreenSaverPaths();
		}
		if(dlgOpt->isOptionClassChanged(OC_SPLASHC)){_XLOG_
			if(objSettings.lSplashTextColor==RGB(255,255,255)){_XLOG_
				objSettings.lSplashTextColor=RGB(254,254,254);
			}
			if(objSettings.lSplashBgColor==RGB(255,255,255)){_XLOG_
				objSettings.lSplashBgColor=RGB(254,254,254);
			}
			if(objSettings.lSplashTextColor==objSettings.lSplashBgColor){_XLOG_
				objSettings.lSplashBgColor=objSettings.lSplashBgColor-3-260;
			}
		}
		if(dlgOpt->isOptionClassChanged(OC_BOOST)){_XLOG_
			SetEvent(objSettings.hTimerEvent);
		}
		if((dlgOpt->isOptionClassChanged(OC_ICONTYPE) || dlgOpt->isOptionClassChanged(OC_SHOWICON)) && pMainDialogWindow){_XLOG_
			pMainDialogWindow->SetTrayTooltip();
		}
		if(dlgOpt->isOptionClassChanged(OC_SHOWICON) && pMainDialogWindow){_XLOG_
			ClearIcon(objSettings.hUDIcon);
			if(objSettings.lShowCPUInTrayIcon==0){_XLOG_
				objSettings.lShowCPUHistInTray=0;
			}
			SetEvent(objSettings.hTimerEvent);
			pMainDialogWindow->SetWindowIcon();
			pMainDialogWindow->PostMessage(LANG_CHANGE,0,0);
			if(objSettings.bShowIconInTray){_XLOG_
				pMainDialogWindow->m_STray.ShowIcon();
			}else{_XLOG_
				pMainDialogWindow->m_STray.HideIcon();
			}
		}
		if(dlgOpt->isOptionClassChanged(OC_LANG)){_XLOG_
			GetApplicationLang()=objSettings.lTempLang;
			dlgOpt->CommitData();// Делаем принудительный коммит, так как AddDefMacros меняет по живому!!!
			dlgOpt->bManualPreventCommit=1;
			AddDefMacros(0,0);
			bRes=TRUE;
		}
		if(wk.bCategsWereChanged){_XLOG_
			wk.bCategsWereChanged=0;
			bRes=TRUE;
		}
		if(dlgOpt->isOptionClassChanged(OC_LICDATA) || wk.lLikKeysPasted){_XLOG_
			wk.CheckLikKeys(NULL,NULL);
		}
		if(pMainDialogWindow && (dlgOpt->bAnyHotkeyWasChanged || wk.bHotkeyChangedMode)){// || dlgOpt.isOptionClassChanged(-1)
			pMainDialogWindow->GenerateBindingsFile();
		}
		objSettings.ApplySettings();
		objSettings.Save();
	}
	DWORD dwt2a=GetTickCount();
	TRACE1("---------- Zamer2=%i\n",dwt2a-dwt1a);
	if(bKeyRegRequired){_XLOG_
		// Включаем новые!
		DWORD dwt1=GetTickCount();
		pMainDialogWindow->RegisterMyHotKeys(FALSE);
		DWORD dwt2=GetTickCount();
		TRACE1("---------- Zamer3=%i\n",dwt2-dwt1);
	}
	return bRes;
}

long CStub::ChooseDlg(const char* szTitle,const char* szHelpString, CStringArray* pOptionList, int iStartElen, int iIconID, CWnd* pParent)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	CDLG_Chooser dlgChooser(szTitle, szHelpString, pOptionList, iStartElen, iIconID, pParent);
	return dlgChooser.DoModal();
}

CString CStub::GetQRunOptTitle(CQuickRunItem* pTemplate)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	CString sMacrosTitle;
	int lMacrosMask=pTemplate->lMacros;
	if(lMacrosMask==0){_XLOG_
		if(pTemplate->sItemPath.Find("http://")==-1){_XLOG_
			sMacrosTitle=((!isDefaultName(pTemplate->sItemName) && pTemplate->sItemName!="")?pTemplate->sItemName:Format("%s #%i",_l("Application"),pTemplate->lUniqueCount));
		}else{_XLOG_
			sMacrosTitle=((!isDefaultName(pTemplate->sItemName) && pTemplate->sItemName!="")?pTemplate->sItemName:Format("%s #%i",_l("Shortcut"),pTemplate->lUniqueCount));
		}
	}else if(lMacrosMask==1 || lMacrosMask==2 || lMacrosMask==3){_XLOG_
		sMacrosTitle=((!isDefaultName(pTemplate->sItemName) && pTemplate->sItemName!="")?pTemplate->sItemName:Format("%s #%i",_l("Macro"),pTemplate->lUniqueCount));
	}else if(lMacrosMask==CATEG_MACR){_XLOG_
		sMacrosTitle=((!isDefaultName(pTemplate->sItemName) && pTemplate->sItemName!="")?pTemplate->sItemName:Format("%s #%i",_l("Launch Set"),pTemplate->lUniqueCount));
	}
	if(pTemplate->qrHotkeys[0]->oHotKey.Present()){_XLOG_
		sMacrosTitle+=" [";
		sMacrosTitle+=GetIHotkeyName(pTemplate->qrHotkeys[0]->oHotKey);
		sMacrosTitle+="]";
	}
	return sMacrosTitle;
}

BOOL CStub::CreateMlineEdit(CWnd* pDialog,int iMode, CString& sText, CString sTitle)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	CDLG_EditMLine dlg(pDialog);
	if(sTitle!=""){_XLOG_
		dlg.sTitle=sTitle;
	}
	dlg.iMode=1;
	dlg.sText=sText;
	if(dlg.DoModal()==IDOK){_XLOG_
		sText=dlg.sText;
		return 1;
	}
	return 0;
}

int CStub::FindQRunItemByID(const char* szID, CStringArray* aList)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	if(aList){_XLOG_
		for(int i=0;i<aList->GetSize();i++){_XLOG_
			if(aList->GetAt(i)==szID){_XLOG_
				return i;
			}
		}
	}else{_XLOG_
		for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
			if(objSettings.aQuickRunApps[i] && objSettings.aQuickRunApps[i]->sUniqueID==szID){_XLOG_
				return i;
			}
		}
	}
	return -1;
}

const char* aHotActionsDsc[LAST_ACTION_CODE+1]=
{
	"Hot menu (items, related to currently active window)",
	"Hide active window to tray",
	"Bring offscreen window into view",
	"Switch On-Top state of active window",
	"Convert keyboard layout of the selected text",
	"Make screenshot and save it into file/open in editor",
	"Run selected text as from command string or calculate it",
	"Hide active window completely",
	"Start screen saver",
	"Shutdown/restart computer (double press to restart)",
	"Volume up",
	"Volume down",
	"Start browser",
	"Start email client",
	"Boss key (hide/terminate all/specified windows)",
	"Open/Close CD drive",
	"WinAmp - next song",
	"WinAmp - previous song",
	"WinAmp - start/stop",
	"WinAmp - pause/play",
	"Change case of the selected text",
	"Restore active window to previous size and position/Maximize",
	"Minimize active window",
	"Mute sound",
	"Lock computer",
	"Unhide all hidden windows",
	"Rollup/Unroll active window to title",
	"WinAmp - Forward 5 seconds",
	"WinAmp - Back 5 seconds",
	"Hide active window into floater",
	"Scroll window up",
	"Scroll window down",
	"Scroll window left",
	"Scroll window right",
	"Show preferences dialog",
	"Free physical memory",
	"Kill all applications from predefined list",
	"Close active application",
	"WinAmp - Show currently playing song",
	"WinAmp - Increase volume",
	"WinAmp - Decrease volume",
	"WinAmp - Change shuffling",
	"Switch to previous desktop",
	"Switch to next desktop",
	"Show time and date",
	"Switch keyboard layout",
	"Popup list of processes",
	"Clear clipboard histories",
	"Scroll one page up",
	"Scroll one page down",
	"Save desktop icons positions",
	"Restore desktop icons positions",
	"Switch monitor into stand-by mode",
	"Show/Hide WinAmp",
	"Convert last word to another layout",
	"Make active window totally transparent",
	"Extreme shutdown",
	"Context menu for selected text",
	"WinAmp - Start visualization plugin",
	"Show Shutdown/Restart dialog",
	"Display or hide the Start menu",
	"Display the System Properties dialog box",
	"Display the desktop",
	"Minimize all of the windows",
	"Open My Computer",
	"Search for a file or a folder",
	"Search for computers",
	"Display Windows Help",
	"Lock the keyboard",
	"Open the Run dialog box",
	"Open Utility Manager"
};

CString CStub::GetBasicHotkeyDsc(int iHK)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	if(strlen(aHotActionsDsc[iHK])>0){_XLOG_
		return _l(CString("DSC_")+TrimMessage(aHotActionsDsc[iHK],20),aHotActionsDsc[iHK]);
	}
	return "";
}

void CStub::RestartWK()
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	CString sExe=GetApplicationDir();
	sExe+=GetApplicationName();
	sExe+=".exe";
	int iRes=(int)::ShellExecute(0,"open",sExe,"-wait=yes",NULL,SW_SHOWNORMAL);
	pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_EXIT,0);
}

BOOL CALLBACK CStub::BuyWK(HIROW hData, CDLG_Options* pDialog)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	if(pMainDialogWindow){_XLOG_
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_DONATE,0);
	}
	return TRUE;
}

BOOL CALLBACK CStub::ClearClipHistory(const char* szClipHistName)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	for(int iClipH=0; iClipH<objSettings.sClipHistory[szClipHistName].GetSize(); iClipH++){_XLOG_
		objSettings.sClipHistory[szClipHistName].RemoveAll();
	}
	return TRUE;
}

CCriticalSection csClipBuffer;
BOOL CALLBACK CStub::ClearClipHistory(int iClip)
{_XLOG_
	SimpleLocker l(&csClipBuffer);
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	objSettings.sClipBuffer[iClip]="";
	return ClearClipHistory(GetClipHistoryName(iClip));
}

BOOL CALLBACK CStub::ClearAllClips(HIROW hData, CDLG_Options* pDialog)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	SimpleLocker l(&csClipBuffer);
	for(int iClip=0; iClip<objSettings.sClipBuffer.GetSize(); iClip++){_XLOG_
		ClearClipHistory(iClip);
	}
	return TRUE;
}

typedef BOOL (WINAPI *_DXSwitchOverlaySupport)(BOOL,const char*);
BOOL CALLBACK CStub::SwitchOverlaySupport(HIROW hData, CDLG_Options* pDialog)
{_XLOG_
	__FLOWLOG;
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	BOOL bRes=0;
	HINSTANCE hUtils=LoadLibrary(GetAppFolder()+"WKUtils.dll");
	if(hUtils){_XLOG_
		_DXSwitchOverlaySupport fp=(_DXSwitchOverlaySupport)DBG_GETPROC(hUtils,"DXSwitchOverlaySupport");
		if(fp){_XLOG_
			bRes=(*fp)(g_lOverlayState, GetApplicationDir());
		}
	}
	if(g_lOverlayState!=0){_XLOG_
		FreeLibrary(hUtils);
	}
	if(!bRes){_XLOG_
		Alert(_l("Overlay support can not be changed: DirectX unavailable"));
		return 0;
	}
	g_lOverlayState=1-g_lOverlayState;
	if(g_lOverlayState!=0){_XLOG_
		Alert(_l("Video windows support is enabled"));
	}else{_XLOG_
		Alert(_l("Video windows support is disabled"));
	}
	return TRUE;
}

BOOL CALLBACK CStub::SaveIconsPos(HIROW hData, CDLG_Options* pDialog)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	CString sRes="";
	if(!SaveDesktopIcons()){_XLOG_
		sRes=_l("Failed to save desktop icons positions")+"!";
	}else{_XLOG_
		sRes=_l("Desktop icons positions successfully saved")+"!";
	}
	SmartWarning(sRes,_l("Desktop icons"),&objSettings.lDisableDeskicoAlert,5000);
	return TRUE;
}

BOOL CALLBACK CStub::RestoreIconsPos(HIROW hData, CDLG_Options* pDialog)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	CString sRes="";
	if(RestoreDesktopIcons()<=0){_XLOG_
		sRes=_l("Failed to restore desktop icons positions")+"!";
	}else{_XLOG_
		sRes=_l("Desktop icons positions successfully restored")+"!";
	}
	SmartWarning(sRes,_l("Desktop icons"),&objSettings.lDisableDeskicoAlert,5000);
	return TRUE;
}

BOOL CALLBACK CStub::EditMacroFile(CString sFile)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	sFile.TrimLeft();
	sFile.TrimRight();
	if(sFile=="" || isUndefinedName(sFile)){_XLOG_
		Alert(_l("Type macro file name first")+"!");
		return FALSE;
	}
	CString sInline=ExtractInlineScript(sFile);
	if(sInline!=""){_XLOG_
		Alert(_l("Enter file name first")+"!");
		return FALSE;
	}
	CString sFullFile=GetMacrosFileName(sFile,TRUE);
	CString sEditor=GetExtensionAssociatedAction("txt","");
	if(sEditor=="" || !isFileExist(sEditor)){_XLOG_
		sEditor="notepad";
	}
	int iRes=(int)::ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,"open",sEditor,CString("\"")+sFullFile+CString("\""),NULL,SW_SHOWNORMAL);
	if(iRes>32){_XLOG_
		ClearReadFileCash();
		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK CStub::RemoveQRun(HIROW hData, CDLG_Options* pDialog)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
	static long lRemItem=0;
	if(lRemItem){_XLOG_
		return FALSE;
	}
	SimpleTracker Track(lRemItem);
	long lOptClassToDel=-1,lIDToDel=-1;
	long lID=long(hData);
	for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
		CQuickRunItem* pItem=objSettings.aQuickRunApps[i];
		if(pItem && pItem->lUniqueCount==lID){_XLOG_
			lOptClassToDel=pItem->lOptionClass;
			lIDToDel=i;
		}
	}
	CQuickRunItem* pItem=0;
	if(lOptClassToDel>=0){_XLOG_
		pItem=objSettings.aQuickRunApps[lIDToDel];
	}
	if(pItem==0){_XLOG_
		FLOG3("Deleting items NULL!!! %i: %s %i",lIDToDel,"none",lID);
		return 0;
	}
	DWORD dwRes=AskYesNo(_l("Do you want to remove")+"\r\n"+Format("'%s'?",TrimMessage(pItem->sItemName,40)),_l("Confirmation"),&objSettings.bAskForRemQ,GetAppWnd());
	if(dwRes!=IDYES){_XLOG_
		objSettings.bAskForRemQ=0;// Если NO, то в следующий раз спросим снова
		return FALSE;
	}
	if(pDialog){_XLOG_
		pDialog->m_OptionsList.LockWindowUpdate();
		pDialog->m_LList.LockWindowUpdate();
	}
	if(pItem){_XLOG_
		//FLOG3("Deleting items %i: %s %i",lIDToDel,pItem->sItemName,lID);
		FLOG3("~?0CQuickRunItem %i: %s %s",pItem->lUniqueCount,pItem->sItemName,pItem->sUniqueID);
		objSettings.aQuickRunApps[lIDToDel]=NULL;
		if(pDialog){_XLOG_
			if(pItem->lMacros==0 && hQRunType0){_XLOG_
				pDialog->MoveGlobalFocus(hQRunType0,TRUE);
			}
			if(pItem->lMacros==1 || ((pItem->lMacros==2 || pItem->lMacros==3) && hQRunType2)){_XLOG_
				pDialog->MoveGlobalFocus(hQRunType2,TRUE);
			}
			if(pItem->lMacros==CATEG_MACR && hQRunType3){_XLOG_
				wk.bCategsWereChanged=1;
				pDialog->CallApply(IDOK,FALSE);
				pDialog->MoveGlobalFocus(hQRunType3,TRUE);
			}
			pDialog->PopSomeDataFromScreen(lOptClassToDel);
			pDialog->Invalidate();
			// Зачищаем Данные в заголовке правой части
			HIROW hNowSelected=pDialog->m_LList.GetSelectedIRow();
			if(hNowSelected!=NULL){_XLOG_
				pDialog->MoveGlobalFocus(hNowSelected,TRUE);
			}
		}
		// Делетим упоминание о категории
		CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		for(int i=0;i<objSettings.aQuickRunApps.GetSize();i++){_XLOG_
			if(objSettings.aQuickRunApps[i] && (objSettings.aQuickRunApps[i]->sCategoryID==pItem->sUniqueID)){_XLOG_
				objSettings.aQuickRunApps[i]->sCategoryID="";
				wk.bCategsWereChanged=1;
			}
		}
		// Делетим все что относится к напоминаниям если есть
		HINSTANCE h=GetModuleHandle("wp_schedule.wkp");
		if(h){_XLOG_
			typedef int (WINAPI *_WKPluginDelEvent)(char szEvent[128]);
			_WKPluginDelEvent fp=(_WKPluginDelEvent)DBG_GETPROC(h,"WKPluginDelEvent");
			if(fp){_XLOG_
				for(int i=0;i<pItem->qrHotkeys.GetSize();i++){_XLOG_
					CQRunHotkeyItem* pQr=pItem->qrHotkeys[i];
					if(pQr){_XLOG_
						char szEvent[128]={0};
						strcpy(szEvent,pQr->oHotKey.GetActionId());
						(*fp)(szEvent);
					}
				}
			}
		}
		delete pItem;
		// Делетим флоатеры/иконки, если есть
		CSmartLock SL(&csInfoWnds,TRUE);
		for(int iHMap=0;iHMap<objSettings.aHidedWindowsMap.GetSize();iHMap++){_XLOG_
			if(objSettings.aHidedWindowsMap[iHMap] && objSettings.aHidedWindowsMap[iHMap]->Sets){_XLOG_
				if(objSettings.aHidedWindowsMap[iHMap]->Sets->hWnd==HWND(-lID)){_XLOG_
					objSettings.aHidedWindowsMap[iHMap]->PostMessage(WM_CLOSE,0,0);
				}
			}
		}
	}
	if(pDialog){_XLOG_
		pDialog->m_OptionsList.UnlockWindowUpdate();
		pDialog->m_LList.UnlockWindowUpdate();
	}
	// Чтобы вызывающий код не придирался
	return FALSE;
}

BOOL CStub::GetEntryDesc(CString& sContentNP, int iEntryCount, CString& sHotkeyTitleEx, CString& sDeclaration)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	CString sContent=sContentNP;
	// Урезаем лишнее
	while(sContent.Replace("\r","")>0){};
	while(sContent.Replace("\t"," ")>0){};
	while(sContent.Replace("  "," ")>0){};
	while(sContent.Replace(" (","(")>0){};
	int bRes=1;
	CString sStand=iEntryCount>1?Format("wkMain%i",iEntryCount):"wkMain";
	// Смотрим сначала есть ли такая функция в файлу
	int iPos0=sContent.Find(Format(" %s(",sStand));
	if(iPos0==-1){_XLOG_
		iPos0=sContent.Find(Format("\n%s(",sStand));
	}
	if(iPos0==-1){_XLOG_
		// Старое от макросов
		iPos0=sContent.Find(Format("<%s>",sStand));
	}
	if(iPos0!=-1){_XLOG_
		sDeclaration=CDataXMLSaver::GetInstringPart(Format("%s(",sStand),")",sContent);
		sHotkeyTitleEx=CDataXMLSaver::GetInstringPart(Format("%s description",sStand),"\n",sContent);
		if(sHotkeyTitleEx==""){_XLOG_
			int iPos2=ReverseFind(sContent,"//",-iPos0);
			if(iPos2!=-1){_XLOG_
				int iC=CountChars(sContent,"\n",iPos2,iPos0);
				if(iC==0){_XLOG_
					bRes=0;
					sHotkeyTitleEx="";
				}else if(iC<2){_XLOG_
					sHotkeyTitleEx=CDataXMLSaver::GetInstringPart("//","\n",sContent,iPos2);
				}
			}else{_XLOG_
				iPos2=ReverseFind(sContent,"'",-iPos0);
				if(iPos2!=-1){_XLOG_
					int iC=CountChars(sContent,"\n",iPos2,iPos0);
					if(iC==0){_XLOG_
						bRes=0;
						sHotkeyTitleEx="";
					}else if(iC<2){_XLOG_
						sHotkeyTitleEx=CDataXMLSaver::GetInstringPart("'","\n",sContent,iPos2);
					}
				}
			}
		}
	}else{_XLOG_
		bRes=0;
	}
	sDeclaration.TrimLeft(".\\/: \t\r\n()");
	sDeclaration.TrimRight(".\\/: \t\r\n()");
	while(sDeclaration.Replace(" ","")>0){};
	while(sDeclaration.Replace("\n","")>0){};
	sHotkeyTitleEx.TrimLeft(".\\/: \t\r\n");
	sHotkeyTitleEx.TrimRight(".\\/: \t\r\n");
	sHotkeyTitleEx.Replace(Format("<%s>: ",sStand),"");
	if(sHotkeyTitleEx=="" || strstr(DEF_MACROSTEMPLATE,sHotkeyTitleEx)!=0){_XLOG_
		sHotkeyTitleEx="Shortcut key";
	}
	return bRes;
}

CString CStub::GetMacrosFileName(CString sFile, BOOL bAutoGetModel)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	if(sFile.GetLength()<3 || !(sFile[0]=='\\' || sFile[2]=='\\' || sFile[1]==':')){_XLOG_
		sFile=CString(MACROS_FOLDER)+"\\"+sFile;
	}
	CString sMacroNameLower=sFile;
	sMacroNameLower.MakeLower();
	CString sFullFile=getFileFullName(sFile);
	if(sMacroNameLower.Find(".wkm")==-1){_XLOG_
		if(isFileExist(sFullFile+".wkm.js")){_XLOG_
			sFullFile+=".wkm.js";
			sFile+=".wkm.js";
		}else if(isFileExist(sFullFile+".wkm.vbs")){_XLOG_
			sFullFile+=".wkm.vbs";
			sFile+=".wkm.vbs";
		}else if(bAutoGetModel || isFileExist(sFullFile+".wkm")){_XLOG_
			sFullFile+=".wkm";
			sFile+=".wkm";
		}else if(isFileExist(sFullFile+".js")){_XLOG_
			sFullFile+=".js";
			sFile+=".js";
		}else if(isFileExist(sFullFile+".vbs")){_XLOG_
			sFullFile+=".vbs";
			sFile+=".vbs";
		}
	}
	if(!isFileExist(sFullFile)){_XLOG_
		CreateDirIfNotExist(sFullFile);
		CString sFileModel=Format("%s%s",GetApplicationDir(),sFile);
		if(!isFileExist(sFileModel)){_XLOG_
			sFileModel+=".wkm";
		}
		if(isFileExist(sFileModel)){_XLOG_
			::CopyFile(sFileModel,sFullFile,TRUE);
		}else if(bAutoGetModel){_XLOG_
			SaveFile(sFullFile,DEF_MACROSTEMPLATE);
		}
	}
	return sFullFile;
}

BOOL CStub::isUndefinedName(CString sName)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	return sName=="" || (sName.Left(1)=="<" && sName.Right(1)==">");
}

#define KSLEEP 25
BOOL CALLBACK CStub::CheckLikKeys(HIROW hData, CDLG_Options* pDialog)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	static long bLikKDialog=0;
	if(bLikKDialog>0){_XLOG_
		return FALSE;
	}
	SimpleTracker Track(objSettings.bDoModalInProcess);
	SimpleTracker TrackLik(bLikKDialog);
	if(pDialog){_XLOG_
		pDialog->CommitData();
	}
	objSettings.sLikKey.TrimLeft();
	objSettings.sLikKey.TrimRight();
	objSettings.sLikUser.TrimLeft();
	objSettings.sLikUser.TrimRight();
	if(objSettings.sLikKey.IsEmpty() || objSettings.sLikUser.IsEmpty()){_XLOG_
		return FALSE;
	}
	CString sError,sUsrName=objSettings.sLikUser;
	if(!isUserNameValid(sUsrName,sError)){_XLOG_
		Sleep(KSLEEP);
		Alert(_l("Invalid user name")+": "+_l(sError)+"!");
		return TRUE;// becouse of commit
	}
	BOOL bRestart=0;
	if(!isKeyValid(objSettings.sLikKey)){_XLOG_
		Sleep(KSLEEP);
		Alert(_l("Invalid registration key")+"!");
	}else{_XLOG_
		Sleep(KSLEEP);
		//Alert(_l("Application restarting to apply changes"),_l("Registration key accepted"));
		bRestart=1;
	}
	CString sLikFile=GetUserFolder()+LICENSE_KEY_FILE;
	CString szKey;
	szKey="user:[";
	szKey+=objSettings.sLikUser;
	szKey+="],";
	szKey+="key:[";
	szKey+=objSettings.sLikKey;
	szKey+="]";
	DWORD dwWritten=0;
	DWORD dwLikOpenType=CREATE_NEW;
	if(isFileExist(sLikFile)){_XLOG_
		dwLikOpenType=TRUNCATE_EXISTING;
	}
	HANDLE hLicenseFile=::CreateFile(sLikFile, GENERIC_WRITE, 0, NULL, dwLikOpenType, 0, NULL);
	if (hLicenseFile && hLicenseFile!=INVALID_HANDLE_VALUE){_XLOG_
		::WriteFile(hLicenseFile,szKey,strlen(szKey),&dwWritten,NULL);
		::CloseHandle(hLicenseFile);
	}
	if(bRestart && pMainDialogWindow){_XLOG_
		RestartWK();
	}
	return TRUE;
}

BOOL CALLBACK CStub::AddDefMacros(HIROW hData, CDLG_Options* pDialog)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	// Если при запуске небыло найдено ini-файла, показываем опции...
	CQuickRunItem* pTemplate=NULL;
	CString sCategID="";
	{// SysCateg
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("System programs");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("System programs");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=CATEG_MACR;
			pTemplate->sItemName=_l("System programs");
			pTemplate->sUniqueID="System programs";
			pTemplate->sItemPath="";
			sCategID=pTemplate->sUniqueID;
			objSettings.AddQrunT(pTemplate);
		}
	}	
	CString sMCategID="";
	{// SysCateg
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("File processing");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("File processing");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=CATEG_MACR;
			pTemplate->sItemName=_l("File processing");
			pTemplate->sUniqueID="File processing";
			pTemplate->sItemPath="";
			sMCategID=pTemplate->sUniqueID;
			objSettings.AddQrunT(pTemplate);
		}
	}
	CString sECategID="";
	{// Пустая категория...
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Miscellaneous");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Miscellaneous");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=CATEG_MACR;
			pTemplate->sItemName=_l("Miscellaneous");
			pTemplate->sUniqueID="Miscellaneous";
			pTemplate->sItemPath="";
			sECategID=pTemplate->sUniqueID;
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// Письмо по выделенному емайлу
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("New mail");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("New mail");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=1;
			pTemplate->sItemName=_l("New mail");
			pTemplate->sUniqueID="New mail";
			pTemplate->sItemPath="mailto:%SELECTED";
			pTemplate->sCategoryID=sECategID;
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// Поиск в гугле
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Search google");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Search google");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=1;
			pTemplate->sItemName=_l("Search google");
			pTemplate->sUniqueID="Search google";
			pTemplate->sItemPath="http://www.google.com/search?q=%SELECTED";
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// AddToFile
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Append to diary");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Append to diary");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=2;
			pTemplate->sItemName=_l("Append to diary");
			pTemplate->sUniqueID="Append to diary";
			pTemplate->sItemPath="AddToFile";
			pTemplate->sItemParams="C:\\Diary.txt";
			pTemplate->sCategoryID=sECategID;
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// MultilineExchange
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Swap line parts");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Swap line parts");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=2;
			pTemplate->sItemName=_l("Swap line parts");
			pTemplate->sUniqueID="Swap line parts";
			pTemplate->sItemPath="MultilineExchange";
			pTemplate->sItemParams="=";
			pTemplate->sCategoryID=sECategID;
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// Comment/Uncomment
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Comment/Uncomment");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Comment/Uncomment");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=2;
			pTemplate->sItemName=_l("Comment/Uncomment");
			pTemplate->sUniqueID="Comment/Uncomment";
			pTemplate->sItemPath="CommentMaster";
			pTemplate->sItemParams="/* */";
			pTemplate->sCategoryID=sECategID;
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// Drive info
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Drive info");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Drive info");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=2;
			pTemplate->sItemName=_l("Drive info");
			pTemplate->sUniqueID="Drive info";
			pTemplate->sItemPath="DriveInfo";
			pTemplate->sCategoryID=sMCategID;
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// RecursiveCopy
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Recursive copy");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Recursive copy");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=2;
			pTemplate->sItemName=_l("Recursive copy");
			pTemplate->sUniqueID="Recursive copy";
			pTemplate->sItemPath="RecursiveCopy";
			pTemplate->sItemParams="";
			pTemplate->sCategoryID=sMCategID;
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// TimeRename.wkm 
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Time Renamer");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Time Renamer");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=2;
			pTemplate->sItemName=_l("Time Renamer");
			pTemplate->sUniqueID="Time Renamer";
			pTemplate->sItemPath="TimeRename";
			pTemplate->sItemParams="";
			pTemplate->sCategoryID=sMCategID;
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// ConvertFolderToDrive
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Convert folder to drive");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Convert folder to drive");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=2;
			pTemplate->sItemName=_l("Convert folder to drive");
			pTemplate->sUniqueID="Convert folder to drive";
			pTemplate->sItemPath="AddFolderAsDrive";
			pTemplate->sItemParams="";
			pTemplate->sCategoryID=sMCategID;
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// Mouse control
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Mouse control");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Mouse control");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=2;
			pTemplate->sItemName=_l("Mouse control");
			pTemplate->sUniqueID="Mouse control";
			pTemplate->sItemPath="MouseControl";
			pTemplate->sItemParams="";
			pTemplate->sCategoryID=sECategID;
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// Desktop
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Desktop items");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Desktop items");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=0;
			pTemplate->sItemName=_l("Desktop items");
			pTemplate->sUniqueID="Desktop items";
			pTemplate->sItemPath=getDesktopPath();
			pTemplate->lParamType=2;
			pTemplate->lSearchInstanceFirst=0;
			pTemplate->lExpandFolderOrCheckInstance=1;
			pTemplate->sItemParams="";
			pTemplate->SetHotkey(0,CIHotkey(524368));
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// My documents
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("My documents");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("My documents");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=0;
			pTemplate->sItemName=_l("My documents");
			pTemplate->sUniqueID="My documents";
			pTemplate->sItemPath=isWin9x()?getMyDocPath():"::{450D8FBA-AD25-11D0-98A8-0800361B1103}";
			pTemplate->lParamType=2;
			pTemplate->lSearchInstanceFirst=0;
			pTemplate->sItemParams="";
			pTemplate->SetHotkey(0,CIHotkey(524377));
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// Empty recycle bin
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Empty recycle bin");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Empty recycle bin");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=0;
			pTemplate->sItemName=_l("Empty recycle bin");
			pTemplate->sUniqueID="Empty recycle bin";
			pTemplate->sItemPath="jscript:WireKeys.EmptyRecycleBin();";
			//pTemplate->sItemPath="::{645FF040-5081-101B-9F08-00AA002F954E}";
			//pTemplate->sItemParams="[ole:empty]";
			pTemplate->lParamType=1;
			pTemplate->lSearchInstanceFirst=0;
			objSettings.AddQrunT(pTemplate);
		}
	}
	{_XLOG_
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Internet Browser");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Internet Browser");
		}else if(hData){_XLOG_
			if(getBrowserExePath()!=""){// Default browser
				pTemplate=new CQuickRunItem();
				pTemplate->lMacros=0;
				pTemplate->sItemName=_l("Internet Browser");
				pTemplate->sUniqueID="Internet Browser";
				pTemplate->sItemPath=getBrowserExePath();
				pTemplate->lParamType=1;
				pTemplate->lSearchInstanceFirst=0;
				pTemplate->sItemParams="";
				pTemplate->qrHotkeys.SetAtGrow(0,new CQRunHotkeyItem(CIHotkey(DefActionMapHK_Preinit[RUN_DEFBROWSER])));
				objSettings.AddQrunT(pTemplate);
			}
		}
	}
	{_XLOG_
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Email client");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Email client");
		}else if(hData){_XLOG_
			if(getEmailExePath()!=""){// Default email client
				pTemplate=new CQuickRunItem();
				pTemplate->lMacros=0;
				pTemplate->sItemName=_l("Email client");
				pTemplate->sUniqueID="Email client";
				pTemplate->sItemPath=getEmailExePath();
				pTemplate->lParamType=1;
				pTemplate->lSearchInstanceFirst=1;
				pTemplate->sItemParams="";
				pTemplate->SetHotkey(0,CIHotkey(DefActionMapHK_Preinit[RUN_DEFEMAILCL]));
				objSettings.AddQrunT(pTemplate);
			}
		}
	}
	{_XLOG_
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Calculator");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Calculator");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=0;
			pTemplate->sItemName=_l("Calculator");
			pTemplate->sUniqueID="Calculator";
			pTemplate->sItemPath="%systemroot%\\system32\\calc.exe";
			pTemplate->lParamType=1;
			pTemplate->lSearchInstanceFirst=0;
			pTemplate->sItemParams="";
			pTemplate->SetHotkey(0,CIHotkey(524355));
			objSettings.AddQrunT(pTemplate);
		}
	}
	{// RegEdit
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Registry editor");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Registry editor");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=0;
			pTemplate->sItemName=_l("Registry editor");
			pTemplate->sUniqueID="Registry editor";
			pTemplate->sItemPath="%systemroot%\\regedit.exe";
			pTemplate->lSearchInstanceFirst=1;
			pTemplate->sItemParams="";
			pTemplate->SetHotkey(0,CIHotkey(524359));
			pTemplate->sCategoryID=sCategID;
			objSettings.AddQrunT(pTemplate);
		}
	}
	// Services
	{_XLOG_
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Services");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Services");
		}else if(hData){_XLOG_
			if(!objSettings.bUnderWindows98){_XLOG_
				pTemplate=new CQuickRunItem();
				pTemplate->lMacros=0;
				pTemplate->sItemName=_l("Services");
				pTemplate->sUniqueID="Services";
				pTemplate->sItemPath="%SystemRoot%\\system32\\services.msc";
				pTemplate->lSearchInstanceFirst=1;
				pTemplate->sItemParams="/s";
				pTemplate->SetHotkey(0,CIHotkey(524371));
				pTemplate->sCategoryID=sCategID;
				objSettings.AddQrunT(pTemplate);
			}
		}
	}
	{// CurDate
		//Если просто локализуем значение...
		int i=wk.FindQRunItemByID("Insert date");
		if(i!=-1){_XLOG_
			objSettings.aQuickRunApps[i]->sItemName=_l("Insert date");
		}else if(hData){_XLOG_
			pTemplate=new CQuickRunItem();
			pTemplate->lMacros=3;
			pTemplate->sItemName=_l("Insert date");
			pTemplate->sUniqueID="Insert date";
			pTemplate->sItemPath="%DATE";
			pTemplate->sItemParams="";
			pTemplate->SetHotkey(0,CIHotkey(262212));
			pTemplate->sCategoryID=sECategID;
			objSettings.AddQrunT(pTemplate);
		}
	}
	return TRUE;
}

BOOL CStub::isDefaultName(const char* szName)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	if(strlen(szName)>2 && szName[0]=='<' && szName[strlen(szName)-1]=='>'){_XLOG_
		return TRUE;
	}
	return FALSE;
}

#include "DLG_EditMLine.h"
BOOL WINAPI CStub::ExternalEnterFind(const char* szTitle, CString& sText,CWnd* pDialog)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	CDLG_EditMLine dlg(pDialog);
	dlg.iMode=1;
	dlg.sText=objSettings.sDefaultFind;
	dlg.sTitle=szTitle;
	if(dlg.DoModal()==IDOK){_XLOG_
		sText=dlg.sText;
		objSettings.sDefaultFind=dlg.sText;
		return 1;
	}
	DWORD dwErr=GetLastError();
	return 0;
}

int CStub::GetBuffersLang(CStringArray& aLangBufferNames, int& iDefLang)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	int iLang=0;
	CFileInfoArray dir;
	aLangBufferNames.RemoveAll();
	CString sStartDir=GetApplicationDir();
	dir.AddDir(sStartDir,"*.lng",FALSE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
	int iSize=dir.GetSize();
	if(iSize<=1){_XLOG_
		iSize=1;
		iDefLang=0;
		aLangBufferNames.Add("English");
		wk.sLangsNames="English";
	}else if(iSize>1){_XLOG_
		int i;
		for(i=0;i<iSize;i++){_XLOG_
			CString sLangName=GetLangName(i);
			aLangBufferNames.Add(sLangName);
		}
		wk.sLangsNames="";
		for(i=0;i<aLangBufferNames.GetSize();i++){_XLOG_
			wk.sLangsNames+=aLangBufferNames[i];
			wk.sLangsNames+="\t";
		}
		wk.sLangsNames.TrimRight();
	}
	return iSize;
}

void CStub::vAlert(const char* szText, const char* szTitle)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	Alert(szText, szTitle);
}
 
BOOL CALLBACK CStub::CallPlgFunction(HIROW hData, CDLG_Options* pDialog)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	DWORD lFuncCODE=(DWORD)hData;
	FORK(PluginOperation,lFuncCODE);
	return TRUE;
}

BOOL CStub::GetFullInfoFromHWND(HWND hwnd,DWORD& dwProcID,CProcInfo*& info,CString& sCommand, CString& sParameter)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	info=NULL;
	CString sCmdLine;
	GetWindowThreadProcessId(hwnd,&dwProcID);
	RefreshProcessListIfNeeded();
	GetProcessInfo(dwProcID,info);
	if(!info){_XLOG_
		return FALSE;
	}
	if(GetProcessComLine(dwProcID,sCmdLine)){_XLOG_
		ParseForShellExecute(sCmdLine, sCommand, sParameter, TRUE);
	}else{_XLOG_
		sCommand=sCmdLine;
	}
	if(sCommand==""){_XLOG_
		sCommand=info->sProcExePath;
	}
	return TRUE;
}

int CStub::GetIconNumByMacro(int lMacros, int iMacroParType,const char* szName)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	int iRes=0;
	if(szName && strstr(szName,"http://")!=0){_XLOG_
		iRes=22;
	}else{_XLOG_
		iRes=((lMacros==0)?65:((lMacros==1)?69:(lMacros==CATEG_MACR)?100:((lMacros==3)?19:70)));
		if(lMacros==0 && iMacroParType==2){_XLOG_
			iRes=5;
		}
	}
	return iRes;
}

CString CStub::GetQrunKeyDsc(CQuickRunItem* p,int j)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	return Format("@QRUN%s-%i",p->sUniqueID,j);
}

CString CStub::GetDesktopName(int i)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	CString sDeskTitle=_l("Desktop")+Format(" #%i",i+1);
	if(i==0){_XLOG_
		sDeskTitle=_l("Default desktop");
	}
	return sDeskTitle;
}

CString CStub::GetHKLName(int i)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	WORD dwLangID=LOWORD(objSettings.aKBLayouts[i]);
	return _l(Format("LANG_ID%04X",dwLangID));
}

const char* CStub::GetAppVers()
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	return GetAppVersion();
}

void CStub::vAsyncPlaySound(const char* szSoundName)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	AsyncPlaySound(szSoundName);
}

BOOL CALLBACK WK_EnumWindowsProc(HWND hwnd, LPARAM lParam);
BOOL CStub::vEnumWindowsProc(HWND hwnd, LPARAM lParam)
{_XLOG_
	//AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	return WK_EnumWindowsProc(hwnd, lParam);
}

BOOL CStub::isPluginLoaded(const char* szFileName, CPlugin** pOut, BOOL bResident)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	for(int i=0;i<objSettings.plugins.GetSize();i++){_XLOG_
		CPlugin* plg=objSettings.plugins[i];
		if(plg){_XLOG_
			if(stricmp(plg->sFileName,szFileName)==0){_XLOG_
				if(pOut){_XLOG_
					*pOut=plg;
				}
				// Если резидентный и выключен - значит его и нет!
				if(bResident && plg->bIsActive==0){
					return FALSE;
				}// Не резидентный всегда "активен" - его мождно загрузить
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CStub::InitScreenSaverPaths()
{_XLOG_
	__FLOWLOG;
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	CString sPath="%SystemRoot%\\";
	SubstEnvVariables(sPath);
	/*
	_GetFileDesc fp=0;
	HINSTANCE hUtils=LoadLibrary(GetAppFolder()+"WKUtils.dll");
	if(hUtils){_XLOG_
		fp=(_GetFileDesc)DBG_GETPROC(hUtils,"GetFileDescription");
	}
	*/
	objSettings.dirScreenSavers.RemoveAll();
	CFileInfoArray dirScreenSaversTmp;
	dirScreenSaversTmp.AddDir(sPath,"*.scr",FALSE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
	dirScreenSaversTmp.AddDir(sPath+"System32\\","*.scr",FALSE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
	for(int i=0;i<dirScreenSaversTmp.GetSize();i++){_XLOG_
		BOOL bIn=(objSettings.sValidScreenSavers.Find(Format("|%s|",dirScreenSaversTmp[i].GetFilePath()))!=-1);
		CString sDescr=dirScreenSaversTmp[i].GetFileName();
		//if(fp)
		{_XLOG_
			char sz[256]={0};
			GetFileDescription(dirScreenSaversTmp[i].GetFilePath(),sz);
			if(sz[0]){_XLOG_
				sDescr=sz;
			}
		}
		objSettings.dirScreenSavers.Add(CScrItem(sDescr,dirScreenSaversTmp[i].GetFilePath(),bIn));
	}
	/*
	if(hUtils){_XLOG_
		FreeLibrary(hUtils);
	}
	*/
}

CQuickRunItem* CALLBACK CStub::AddQRunFromHWnd(HIROW hData, CDLG_Options* pDialog)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	HWND hWindow=(HWND)hData;
	if(hWindow==NULL || !IsWindow(hWindow)){_XLOG_
		return FALSE;
	}
	// чтобы не повторялось!!!
	pDialog->m_pStartupInfo->fpPostAction=0;
	CQuickRunItem* pTemplate=new CQuickRunItem;
	pTemplate->lMacros=0;
	FillTemplateFromHWND(pTemplate, hWindow);
	CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
	objSettings.AddQrunT(pTemplate);
	return pTemplate;
}

CQuickRunItem* CStub::AddQRunTemplate(long lMacrosMask, const char* szFile,CQuickRunItem* pItem, BOOL bAdd)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	CQuickRunItem* pTemplate=new CQuickRunItem;
	if(pItem){_XLOG_
		pTemplate->Copy(pItem);
		pTemplate->sItemName=Format("%s %s",_l("Copy of"),pTemplate->sItemName);
		if(pTemplate->lMacros==2){_XLOG_
			// Копия файла!!!
			CString sFile=wk.GetMacrosFileName(pTemplate->sItemPath,FALSE);
			CString sContent;
			ReadFile(sFile,sContent);
			pTemplate->sItemPath=GetPathPart(sFile,1,1,0,0)+CString("Copy_")+GetPathPart(sFile,0,0,1,1);
			SaveFile(pTemplate->sItemPath,sContent);
		}
	}else{_XLOG_
		pTemplate->lMacros=lMacrosMask;
		if(pTemplate->lMacros==0){_XLOG_
			if(objSettings.lCreateQRunFrom==3){_XLOG_
				pTemplate->sItemName="<"+_l("Enter link name")+">";
				pTemplate->lParamType=1;//File
				pTemplate->sItemPath="http://";
			}else if(objSettings.lCreateQRunFrom==0){_XLOG_
				objSettings.sTopLevelWindowsTitles.RemoveAll();
				objSettings.sTopLevelWindowsHWnds.RemoveAll();
				::EnumWindows(WK_EnumWindowsProc,0);
				if(objSettings.sTopLevelWindowsHWnds.GetSize()==0){_XLOG_
					Alert(_l("No application`s windows was found\nOperation cancelled"),_l(PROGNAME": Quick-run"));
					return NULL;
				}
				long iBaseApp=wk.ChooseDlg(_l("New quick-run item"),_l("Select one of the running applications"),&objSettings.sTopLevelWindowsTitles, 0, IDR_MAINICO, NULL);
				if(iBaseApp==-1){_XLOG_
					return NULL;
				}
				FillTemplateFromHWND(pTemplate, objSettings.sTopLevelWindowsHWnds[iBaseApp]);
			}else{_XLOG_
				pTemplate->sItemName="<"+_l("Enter name")+">";
				if(objSettings.lCreateQRunFrom==1 || objSettings.lCreateQRunFrom==4){_XLOG_
					pTemplate->lParamType=1;//File
					if(szFile){_XLOG_
						pTemplate->sItemPath=szFile;
						char szN[MAX_PATH]={0};
						_splitpath(szFile,0,0,szN,0);
						pTemplate->sItemName=szN;
					}else{_XLOG_
						pTemplate->sItemPath="<"+_l("Enter command")+">";
					}
				}else{_XLOG_
					pTemplate->lParamType=2;//Folder
					if(szFile){_XLOG_
						pTemplate->sItemPath=szFile;
						char szN[MAX_PATH]={0};
						_splitpath(szFile,0,0,szN,0);
						pTemplate->sItemName=szN;
					}else{_XLOG_
						pTemplate->sItemPath="<"+_l("Enter folder")+">";
					}
				}
			}
		}else if(pTemplate->lMacros<CATEG_MACR){_XLOG_
			CString sMacroFile=szFile;
			CString sMacroName="<"+_l("Enter macro name")+">";
			if(szFile!=NULL){_XLOG_
				if(pTemplate->lMacros==2){_XLOG_
					// Пытаемся считать из файла
					CString sFile,sContent;
					sFile=wk.GetMacrosFileName(szFile,FALSE);
					ReadFileCashed(sFile,sContent);
					sMacroName=CDataXMLSaver::GetInstringPart("// Title:","\n",sContent);
					if(sMacroName==""){_XLOG_
						sMacroName=CDataXMLSaver::GetInstringPart("' Title:","\n",sContent);
					}
					if(sMacroName!=""){_XLOG_
						sMacroName.TrimLeft();
						sMacroName.TrimRight();
						sMacroName.Replace("\r","");
						sMacroName.Replace("\n","");
						sMacroName=_l(sMacroName);
					}
				}
				if(sMacroName==""){_XLOG_
					char szFileName[MAX_PATH]="";
					_splitpath(sMacroFile, NULL, NULL, szFileName, NULL);
					sMacroName=szFileName;
				}
			}
			if(pTemplate->lMacros==1){_XLOG_
				pTemplate->sItemName=sMacroName;
				pTemplate->sItemPath=(szFile!=NULL)?sMacroFile:("<"+_l("Enter macro command")+">");
			}else if(pTemplate->lMacros==3){_XLOG_
				pTemplate->sItemName=sMacroName;
				pTemplate->sItemPath=szFile;
			}else{_XLOG_
				pTemplate->sItemName=sMacroName;
				pTemplate->sItemPath=(szFile!=NULL)?sMacroFile:("<"+_l("Enter macro file name")+">");
			}
		}else{_XLOG_
			pTemplate->sItemName="<"+_l("Enter Launch Set name")+">";
			pTemplate->sItemPath="";
		}
	}
	if(bAdd){_XLOG_
		CSmartLock ql(objSettings.aQRunLock,TRUE,-1);
		objSettings.AddQrunT(pTemplate);
	}
	FLOG3("Add CQuickRunItem %i: %s %s",pTemplate->lUniqueCount,pTemplate->sItemName,pTemplate->sUniqueID);
	return pTemplate;
}

BOOL CStub::FillTemplateFromHWND(CQuickRunItem* pTemplate, HWND hwnd)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	if(hwnd==NULL || !IsWindow(hwnd)){_XLOG_
		return FALSE;
	}
	CWnd* pWnd=CWnd::FromHandle(hwnd);
	if(!pWnd){_XLOG_
		return FALSE;
	}
	pWnd->GetWindowText(pTemplate->sItemName);
	// Информация об приложении
	DWORD dwProcID=0;
	CProcInfo* info=NULL;
	CString sCommand, sParameter;
	if(wk.GetFullInfoFromHWND(hwnd,dwProcID,info,sCommand, sParameter)){_XLOG_
		pTemplate->sItemPath=sCommand;
		pTemplate->sItemParams=sParameter;
		if(info){// Если прога запускалась из домашнего каталога...
			CString sExe=info->sProcExePath;
			sCommand.MakeLower();
			sExe.MakeLower();
			if(sExe.Find(sCommand)!=-1){_XLOG_
				pTemplate->sItemPath=info->sProcExePath;
			}
		}
	}else{_XLOG_
		pTemplate->sItemPath="<unknown>";
	}
	char szFileName[MAX_PATH]="";
	_splitpath(pTemplate->sItemPath, NULL, NULL, szFileName, NULL);
	pTemplate->sItemName=szFileName;
	return TRUE;
}

CString CStub::vGetApplicationDir()
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	return GetApplicationDir();
}

CString CStub::vGetApplicationName()
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	return GetApplicationName();
}

long& CStub::vGetApplicationLang()
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	return GetApplicationLang();
}

HICON CStub::vLoadIcon(int ID)
{_XLOG_
	AFXx_MAINTAIN_STATE remeberState(afxModuleStateX);
	return ::AfxGetApp()->LoadIcon(ID);
}

CInPlaceHotKey*& CStub::GetHotkeyInputInProgress()
{_XLOG_
	static CInPlaceHotKey* pHk=0;
	return pHk;
};

