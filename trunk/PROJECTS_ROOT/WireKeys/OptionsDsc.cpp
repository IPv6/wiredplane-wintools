http://gamerotor.com/node/247#comment-20
http://gamerotor.com/node/249#comment-19



// Settings.cpp: implementation of the CSettings class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "_common.h"
#include "Settings.h"
#include "DLG_Chooser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CStub* pwk=0;
CSettings* psk=0;
extern CString sSafeChar;
extern CInPlaceHotKey* lHotkeyInputInProgress;
BOOL CALLBACK AddQRunFromFile(HIROW hData, CDLG_Options* pDialog);
BOOL CALLBACK AddQRunFromHWnd(HIROW hData, CDLG_Options* pDialog);
BOOL OpenOptionsDialog(int iTab,const char* szDefTopic, CWnd* pParentWnd, FP_OPTIONACTION fpPostActionType, LPVOID pPostActionParam);
BOOL WINAPI OpenOptions(int iTab,const char* szDefTopic, CWnd* pParentWnd, int iPostActionType, LPVOID pPostActionParam,CSettings* pobjSettings,CStub* pWKStub)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	FP_OPTIONACTION fpPostActionType=0;
	if(iPostActionType==0){
		fpPostActionType=AddQRunFromFile;
	}
	if(iPostActionType==1){
		fpPostActionType=AddQRunFromHWnd;
	}
	psk=pobjSettings;
	pwk=pWKStub;
	pwk->GetHotkeyInputInProgress()=lHotkeyInputInProgress;
	sSafeChar=psk->sSafeChar;
	SetAppDir(pwk->vGetApplicationDir());
	SetAppName(pwk->vGetApplicationName());
	GetApplicationLang()=pwk->vGetApplicationLang();
	WKIHOTKEYS_CheckForExternalDLL()=TRUE;
	int iRes=OpenOptionsDialog(iTab,szDefTopic, pParentWnd, fpPostActionType, pPostActionParam);
	return iRes;
}


#include "..\SmartWires\GUIClasses\WindowDescriber.h"

int iQAppCount=0;
int iQMacCount=0;
CString sDesktopWarning="";
CString g_ListOfCategs;
BOOL bFileDialogOpened=FALSE;
BOOL OpenFileDialog(const char* szExtension, LPVOID lpData, CDLG_Options* pDialog, BOOL bSkipFileCheck=FALSE, BOOL bWithCString=FALSE, BOOL bOpenDialog=TRUE);
CStringArray aWndDescriptions;
void ShowIntroduction(CWnd* pDialog)
{
	aWndDescriptions.RemoveAll();
	aWndDescriptions.Add("TEXT[["
		"Вы впервые открыли настройки этой программы. Хотя настроек весьма"
		" много (как и доступных возможностей), не стоит пугаться :) Эта подсказка даст Вам общее представление о них."
		" \n\nВсе настройки распределены по категориям. Окно настроек разделено на две части."
		" В левой части находится список категорий, в правой - параметры (настройки) текущей выбранной категории."
		" Вам нет необходимости менять те параметры, которые Вам неинтересны."
		" Обратите внимание также на нижнюю половину правой части - при выделении параметра там отображается его подробное описание и возможные значения с примерами."
		" Границы между всеми частями этого окна можно перетаскивать мышью. В меню этого окна находятся некоторые часто используемые возможности, которые можно вызывать не углубляясь в дерево настроек, прямо оттуда."
		"]] FOOTER[[Для перехода ко 2ой странице кликните на это окно]] TITLE[[Немного информации. Шаг 1/3]] ID[[SCREEN]] X[[-10]] Y[[10]] H[[AUTO]] WAIT[[90000]]");//ID[[1190]] 
	aWndDescriptions.Add("TEXT[["
		"Основным предназначением программы WireKeys (как ясно из названия) является задание горячих клавиш различным действиям."
		" Каждое доступное действие имеет параметр-настройку (обычно это самая первая строка) со значком клавиатуры - это и есть место для задания комбинаций клавиш, нажатие которой приведет к активации действия."
		" Для задания горячей клавиши кликните дважды на этой настройке, после чего просто нажимайте желаемую комбинацию (все клавиши комбинации одновременно)."
		" Нажав правой кнопкой мыши на поле комбинации вы откроете дополнительное меню, где составные части горячей клавиши можно выбрать не нажимая их на клавиатуре. Также в этом меню можно выбирать специальные кнопки и возможности (углы экрана, жесты мыши и т.п.) и особенности поведения комбинации."
		" Действия также можно активировать по таймеру и другими путями. Раскройте список дополнительных возможностей активации"
		" нажав на плюсик слева от настройки горячей клавиши."
		"]] FOOTER[[Для перехода к 3ей странице кликните на это окно]] TITLE[[Немного информации. Шаг 2/3]] ID[[SCREEN]] X[[-10]] Y[[10]] H[[AUTO]] WAIT[[90000]]");//ID[[1190]] 
	aWndDescriptions.Add("TEXT[["
		"Напоследок хочется добавить что WireKeys использует понятие 'горячей клавиши' намного шире чем это принято."
		" Вы можете использовать в комбинациях такие вещи как: факт двойного нажатия заданной кнопки клавиатуры, клик на определенную часть активного окна (кнопка минимизации или к примеру заголовок окна), определенный жест мышью или кнопка IR пульта и многое другое. Некоторые действия имеют альтернативный режим, активируемый при двойном нажатии на горячую клавишу действия. Например при нажатии комбинации снятия скриншота будет сохранен весь экран, а при двойном нажатии этой комбинации - только активное окно или его часть."
		" \n\nКроме этого WireKeys обладает развитыми средствами автоматизации, позволяя манипулировать окнами, файлами и текстом (выделенным в активном приложении, к примеру) из макросов и из скриптов."
		" \n\nПри работе с программой вовсе не обязательно обращаться к справке, многие параметры-настройки говорят сами за себя."
		" Но потратив на справку 15 минут Вашего времени Вы сможете выполнять многие рутинные операции намного быстрее;"
		" либо сможете автоматизировать их так, что Вашего участия в них больше не потребуется. Для начало прочитайте раздел помощи 'Обучение'."
		" А любые возникающие вопросы смело задавайте у нас на форуме:\nhttp://www.wiredplane.com/forum/index.php"
		"\nУдачи!\n\nРазинков Илья, WiredPlane.com"
		"]] FOOTER[[Для закрытия подсказки кликните на это окно]] TITLE[[Немного информации. Шаг 3/3]] ID[[SCREEN]] X[[-10]] Y[[10]] H[[AUTO]] WAIT[[90000]]");//ID[[1190]] 
	StartWndDescriber(pwk->pMainWnd, pDialog, &aWndDescriptions);
}

BOOL CALLBACK ChooseDirEx(HIROW hData, CDLG_Options* pDialog, CString* sOut=NULL)
{
	if(bFileDialogOpened){
		return FALSE;
	}
	BOOL bRes=FALSE;
	bFileDialogOpened=TRUE;
	char szDir[MAX_PATH]="";
	if(pDialog && hData){
		strcpy(szDir,pDialog->m_OptionsList.GetIItemText(hData,1));
	}else{
		strcpy(szDir,psk->sScrshotDir);
	}
	if(GetFolder (_l("Choose directory"), szDir, NULL, pDialog?pDialog->GetSafeHwnd():NULL)){
		bRes=TRUE;
		if(szDir[strlen(szDir)-1]!='\\'){
			strcat(szDir,"\\");
		}
		if(pDialog && hData){
			pDialog->m_OptionsList.SetIItemText(hData,1,szDir);
			FLNM_ROWSTATE flnmRowState;
			flnmRowState.bCollapsed= FALSE;
			flnmRowState.dwData= 0;
			flnmRowState.strText=szDir;
			flnmRowState.hIRow= hData;
			flnmRowState.iIItem=1;
			LRESULT result;
			pDialog->OnEndEdit((NMHDR*)(&flnmRowState), &result);
		}
	}
	bFileDialogOpened=FALSE;
	if(sOut){
		*sOut=szDir;
	}
	return bRes;
}

//------------------------------
BOOL CALLBACK EditSmartKeywords(HIROW hData, CDLG_Options* pDialog)
{
	CString sOutFile=GetUserFolder()+"smartkeywords.ini";
	::ShellExecute(NULL,"open","notepad.exe",sOutFile,NULL,SW_SHOWNORMAL);
	return TRUE;
}

BOOL CALLBACK ClearAllClips(HIROW hData, CDLG_Options* pDialog)
{
	return pwk->ClearAllClips(hData, pDialog);
}

BOOL CALLBACK CheckLikKeys(HIROW hData, CDLG_Options* pDialog)
{
	return pwk->CheckLikKeys(hData, pDialog);
}

BOOL CALLBACK BuyWK(HIROW hData, CDLG_Options* pDialog)
{
	return pwk->BuyWK(hData, pDialog);
}

BOOL CALLBACK AddDefMacros(HIROW hData, CDLG_Options* pDialog)
{
	return pwk->AddDefMacros(hData, pDialog);
}

BOOL CALLBACK RestoreIconsPos(HIROW hData, CDLG_Options* pDialog)
{
	return pwk->RestoreIconsPos(hData, pDialog);
}

BOOL CALLBACK SaveIconsPos(HIROW hData, CDLG_Options* pDialog)
{
	return pwk->SaveIconsPos(hData, pDialog);
}

BOOL CALLBACK SwitchOverlaySupport(HIROW hData, CDLG_Options* pDialog)
{
	return pwk->SwitchOverlaySupport(hData, pDialog);
}

BOOL CALLBACK RemoveQRun(HIROW hData, CDLG_Options* pDialog)
{
	return pwk->RemoveQRun(hData, pDialog);
}

BOOL CALLBACK ChooseDir(HIROW hData, CDLG_Options* pDialog)
{
	return ChooseDirEx(hData, pDialog, 0);
}

BOOL CALLBACK ChooseAnyFile(HIROW hData, CDLG_Options* pDialog)
{
	return OpenFileDialog("Any files (*.*)|*.*||",hData,pDialog);
}

BOOL CALLBACK ChooseIcoFile(HIROW hData, CDLG_Options* pDialog)
{
	CString sFile=pDialog->m_OptionsList.GetIItemText(hData,1);
	if(sFile==""){
		pDialog->m_OptionsList.SetIItemText(hData,1,Format("%sIcons\\*.ico",GetApplicationDir()));
	}
	return OpenFileDialog("Icons files (*.ico)|*.ico||",hData,pDialog);
}

BOOL CALLBACK CallPluginOptions(HIROW hData, CDLG_Options* pDialog)
{
	SimpleTracker lc(psk->lInPlugAction);
	int iRes=FALSE;
	int iPlugin=int(hData);
	CPlugin* plg=psk->plugins[iPlugin];
	if(plg && plg->fOptionsFunction){
		if(pDialog){
			pDialog->CommitData();
		}
		if(plg->fOptionsManager){
			// Сначала переносим настройки если что...
			(*plg->fOptionsManager)(OM_OPTIONS_SET,plg,0);
		}
		(*plg->fOptionsFunction)(pDialog?pDialog->GetSafeHwnd():(pwk->pMainWnd?pwk->pMainWnd->GetSafeHwnd():NULL));
		if(pDialog){
			pDialog->PushSomeDataOnScreen(OC_PLUGACT);
		}
	}
	return iRes;
}


BOOL CALLBACK EnablePlugin(HIROW hData, CDLG_Options* pDialog)
{
	if(pDialog){
		pDialog->m_OptionsList.SetIItemCheck(hData, 0, TRUE);
		pDialog->CallApply(IDOK,FALSE);
	}
	return TRUE;
}

BOOL CALLBACK DeinstallPlugin(HIROW hData, CDLG_Options* pDialog)
{
	CPlugin* plg=(CPlugin*)hData;
	CString sFileName=plg->sFile;
	if(isFileExist(sFileName)){
		CString sFileNameNew=GetPathPart(sFileName,1,1,1,0)+".deleted";
		if(isFileExist(sFileNameNew)){
			if(!DeleteFile(sFileNameNew)){
				pwk->vAlert(_l("Plugin can not be deinstalled. Disable plugin and restart computer first"),_l("Warning"));
				return 0;
			}
		}
		if(!MoveFile(sFileName,sFileNameNew)){
			pwk->vAlert(_l("Plugin can not be deinstalled. Disable plugin and restart computer first"),_l("Warning"));
			return 0;
		}
	}
	pwk->vAlert(_l("Restart "PROGNAME" to finish deinstallation"),_l("Warning"));
	return TRUE;
}

BOOL CALLBACK DisablePlugin(HIROW hData, CDLG_Options* pDialog)
{
	if(psk->lInPlugAction){
		pwk->vAlert(_l("Plugin`s dialog opened or plugin is in action.\nAll plugins must be idle before disabling any plugin"),_l("Warning"));
		return FALSE;
	}
	if(pDialog){
		pDialog->m_OptionsList.SetIItemCheck(hData, 0, FALSE);
		pDialog->CallApply(IDOK,FALSE);
	}
	return TRUE;
}

BOOL CALLBACK ChooseWkmFile(HIROW hData, CDLG_Options* pDialog)
{
	return OpenFileDialog(PROGNAME" macro files (*.wkm,*.js,*.vbs)|*.wkm;*.js;*.vbs||",hData,pDialog);
}

BOOL CALLBACK ChooseAnyFile_Macros(HIROW hData, CDLG_Options* pDialog)
{
	return pwk->EditMacroFile(pDialog->m_OptionsList.GetIItemText(hData,1));
}

BOOL CALLBACK ChooseWavFile(HIROW hData, CDLG_Options* pDialog)
{
	return OpenFileDialog("Wav files (*.wav)|*.wav||",hData,pDialog);
}

int CountPluginOption(CPlugin* plg,DWORD dwPosition);
BOOL CALLBACK CallPlgFunction(HIROW hData, CDLG_Options* pDialog)
{
	if(pDialog){
		pDialog->CommitData();
	}
	pwk->CallPlgFunction(hData, pDialog);
	return TRUE;
}

BOOL ExtractPluginActionDscs(WKPluginFunctionDscEx* pFunc,CString& sName,CString& sDsc)
{
	sName=pFunc->szItemName;
	sDsc="";
	int iDscPos=sName.Find("\t");
	if(iDscPos!=-1){
		sDsc=sName.Mid(iDscPos+1);
		sDsc=sDsc.SpanExcluding("\t");
		sName=sName.Left(iDscPos);
	}
	return TRUE;
}

BOOL AddPluginOption(HIROW hParent, CDLG_Options* pDialog, CPlugin* plg, DWORD dwPosition, BOOL bStrict)
{
	if(!plg){
		return FALSE;
	}
	if(!plg->bIsActive){
		return FALSE;
	}
	int iRes=CountPluginOption(plg,dwPosition);
	if(iRes==0){
		return FALSE;
	}else{
		if(dwPosition!=0){
			pDialog->Option_AddTitle(hParent,_l("Options, provided by")+" "+_l("plugin")+" '"+plg->_l(plg->sTitle)+"'",61);
		}
	}
	CString sName;
	for(int i=0;i<(plg->options.GetSize());i++){
		CPluginOption* pValue=plg->options[i];
		if(pValue){
			if((pValue->m_dwOptionPosition==dwPosition || (!bStrict && dwPosition==0)) && pValue->m_sOptionTitle!=""){
				HIROW hThisParent=hParent;
				if(pValue->m_pParent!=""){
					CPluginOption* pParValue=plg->GetOptionByName(pValue->m_pParent);
					if(pParValue && pParValue->m_hThis){
						hThisParent=pParValue->m_hThis;
					}
				}
				if(pValue->m_pOption){
					switch(pValue->m_pOption->iOptionType){
					case OPTTYPE_BOOL:
						pValue->m_hThis=pDialog->Option_AddBOOL(hThisParent,pValue->m_sOptionTitle,&pValue->m_lValue,((COptionBOOL*)pValue->m_pOption)->m_Value_Def,0,OC_PLUGACT);
						break;
					case OPTTYPE_NUMBER:
						pValue->m_hThis=pDialog->Option_AddNString(hThisParent,pValue->m_sOptionTitle,&pValue->m_lValue,((COptionNumber*)pValue->m_pOption)->m_Value_Def,((COptionNumber*)pValue->m_pOption)->m_dwMin,((COptionNumber*)pValue->m_pOption)->m_dwMax,OC_PLUGACT);
						break;
					case OPTTYPE_COMBO:
						pValue->m_hThis=pDialog->Option_AddCombo(hThisParent,pValue->m_sOptionTitle,&pValue->m_lValue,((COptionCombo*)pValue->m_pOption)->m_Value_Def,pValue->m_sValue,OC_PLUGACT);
						break;
					case OPTTYPE_STRING:
						{
							HIROW hStr=pDialog->Option_AddString(hThisParent,pValue->m_sOptionTitle,&pValue->m_sValue,((COptionString*)pValue->m_pOption)->m_Value_Def,OC_PLUGACT);
							if(pValue->m_lValue==1){
								pDialog->Option_AddActionToEdit(hStr, ChooseAnyFile, hStr, OC_PLUGACT, 94);
							}
							if(pValue->m_lValue==2){
								pDialog->Option_AddActionToEdit(hStr, ChooseDir, hStr, OC_PLUGACT, 94);
							}
							pValue->m_hThis=hStr;
							break;
						}
					case OPTTYPE_ACTION:
						{
							long lCode=long(((COptionAction*)pValue->m_pOption)->functionParam);
							if(lCode>=0 && lCode<plg->funcs.GetSize()){
								DWORD dwCode=plg->funcs[lCode].dwCommandCode;
								pValue->m_hThis=pDialog->Option_AddAction(hThisParent, pValue->m_sOptionTitle, CallPlgFunction, (HIROW)dwCode, OC_PLUGACT);
							}else{
								pValue->m_hThis=pDialog->Option_AddAction(hThisParent, pValue->m_sOptionTitle, CallPluginOptions, (HIROW)(plg->lIndex), OC_PLUGACT);
							}
						}
						break;
					case OPTTYPE_HOTKEY:
						{
							long lCode=pValue->m_lValue;
							pValue->m_hThis=pDialog->Option_AddHotKey(hThisParent,pValue->m_sOptionTitle,&plg->funcs[lCode].oRealHotkey,plg->funcs[lCode].oRealDefHotkey,OC_PLUGACT);
						}
						break;
					case OPTTYPE_COLOR:
						{
							pValue->m_hThis=pDialog->Option_AddColor(hThisParent,pValue->m_sOptionTitle,&pValue->m_lValue,((COptionNumber*)pValue->m_pOption)->m_Value_Def, OC_PLUGACT, 99);
						}
						break;
					}
				}else{
					if(pValue->sName.Find("folder")==-1){
						pValue->m_hThis=pDialog->Option_AddTitle(hThisParent,pValue->m_sOptionTitle,86);
					}else{
						pValue->m_hThis=pDialog->Option_AddTitle(hThisParent,pValue->m_sOptionTitle,5);
					}
				}
			}
		}
	}
	return TRUE;
}


BOOL AddPluginsOption(HIROW hParent, CDLG_Options* pDialog, DWORD dwPosition)
{
	if(psk->plugins.GetSize()>0){
		for(int i=0;i<psk->plugins.GetSize();i++){
			CPlugin* plg=psk->plugins[i];
			if(plg && plg->bIsActive){
				AddPluginOption(hParent, pDialog, plg, dwPosition, FALSE);
			}
		}
	}
	return TRUE;
}

BOOL AddPluginHotkeys(CString sTitleRaw, CString sNameRaw, int iPlgIndex, HIROW hParent, CDLG_Options* pDialog, CPlugin* plg, BOOL bStrict)
{
	CString sTitle=plg->_l(plg->sTitle);
	DWORD dwIcon=(plg->iPluginIconNum==-1)?63:plg->iPluginIconNum;
	if(sNameRaw==""){
		if(plg->sDescription!=""){
			pDialog->Option_AddTitle(hParent,plg->_l("DSCPLG_"+plg->sFileName,plg->sDescription),61);
		}
		if(plg->sAuthors!=""){
			pDialog->Option_AddTitle(hParent,_l("Author")+": "+plg->sAuthors,54);
		}
	}
	HIROW hEnaDisa=pDialog->Option_AddBOOL(hParent, sNameRaw==""?Format("%s %s",_l("Load"),/*sTitle,*/_l("at startup")):sTitleRaw, &plg->bIsActive, plg->bIsActive, 0, OC_PLUGACT);
	if(!plg->bIsActive){
		if(sNameRaw!=""){
			pDialog->Option_AddTitle(hParent,_l("Enable")+" "+sNameRaw+", "+_l("Reopen preferences"),86);
		}else{
			pDialog->Option_AddTitle(hParent,_l("Caution")+": "+_l("Plugin is disabled","Plugin is disabled, no options available"),37);
			pDialog->Option_AddAction(hParent,_l("Enable")+" '"+sTitle+"'", EnablePlugin, hEnaDisa);
		}
	}else{
		for(int j=0;j<plg->funcs.GetSize();j++){
			WKPluginFunctionDscEx* pFunc=&plg->funcs[j];
			if(pFunc && pFunc->dwDefaultHotkey!=-1 && strlen(pFunc->szItemName)>0 && (pFunc->dwItemMenuPosition & WKITEMPOSITION_OPTFLOATS)==0){
				CString sName=pFunc->szItemName,sDsc="";
				ExtractPluginActionDscs(pFunc,sName,sDsc);
				pDialog->Option_AddHotKey(hParent,plg->_l(sName)+(sDsc!=""?(CString("\t")+plg->_l(sDsc)):""),&pFunc->oRealHotkey,pFunc->oRealDefHotkey,OC_PLUGACT);
			}
		}
		AddPluginOption(hParent,pDialog,plg,0,bStrict);
		if(sNameRaw==""){
			if(plg->fOptionsFunction!=NULL){
				pDialog->Option_AddAction(hParent,_l("Show additional preferences"), CallPluginOptions, HIROW(iPlgIndex));
			}
			pDialog->Option_AddAction(hParent,_l("Disable")+" '"+sTitle+"'", DisablePlugin, hEnaDisa);
		}
	}
	if(sNameRaw==""){
		pDialog->Option_AddAction(hParent,_l("Deinstall")+" '"+sTitle+"'", DeinstallPlugin, (HIROW)plg);
	}
	return plg->bIsActive;
}

BOOL CALLBACK HowGetLikKeys(HIROW hData, CDLG_Options* pDialog)
{
	ShowHelp("Registration");
	return TRUE;
}

BOOL CALLBACK InitSSs(HIROW hData, CDLG_Options* pDialog)
{
	pwk->InitScreenSaverPaths();
	pDialog->CallApply(IDOK,FALSE);
	return 0;
}

BOOL CALLBACK TimeDateFormats(HIROW hData, CDLG_Options* pDialog)
{
	ShowHelp("Patterns");
	return TRUE;
}

int FindQRunItemByName(const char* szName)
{
	for(int i=0;i<psk->aQuickRunApps.GetSize();i++){
		if(psk->aQuickRunApps[i] && psk->aQuickRunApps[i]->sItemName==szName){
			return i;
		}
	}
	return -1;
}

BOOL CALLBACK TestSound(HIROW hData, CDLG_Options* pDialog)
{
	if(!pDialog){
		return FALSE;
	}
	CString sFile=pDialog->m_OptionsList.GetIItemText(hData,1);
	if(sFile!=""){
		pwk->vAsyncPlaySound(sFile);
	}
	return TRUE;
}

BOOL CALLBACK ListHotkeys(HIROW hData, CDLG_Options* pDialog)
{
	if(pDialog){
		pDialog->CommitData();
	}
	if(pwk->pMainWnd){
		pwk->pMainWnd->PostMessage(WM_COMMAND,ID_SYSTRAY_CURRENTKEYBINDINGS,0);
	}
	return TRUE;
}

void MakeBackup(CString sFile)
{
	CString sBaseDir=GetUserFolder();
	CFileInfoArray dir;
	dir.AddDir(sBaseDir,"*.conf",TRUE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
	CString sCabFileContent;
	for (int i=0;i<dir.GetSize();i++){_XLOG_
		CString sFileIn=dir[i].GetFilePath();
		AddToTextCab(sCabFileContent,sFileIn,sBaseDir);
	}
	SaveFile(sFile,sCabFileContent);
	//AfxMessageBox(_l("Done")+"!");
}

BOOL CALLBACK BackupFromFile(HIROW hData, CDLG_Options* pDialog)
{
	if(psk->iLikStatus<2)
	{
		AfxMessageBox(_l("This feature can be used in registered version only")+"\n"+_l("Please register")+"!");
		return FALSE;
	}
	if(!pDialog){
		psk->sBackupFile="";
	}
	CString sFile=psk->sBackupFile;
	if(hData && pDialog){
		sFile=pDialog->m_OptionsList.GetIItemText(hData,1);
	}
	if(sFile==""){
		sFile=getFileFullName("Backups",TRUE);
		CreateDirIfNotExist(sFile);
		sFile+="wkbackup.wkb3";
	}
	if(!OpenFileDialog(PROGNAME" backup (*.wkb3)|*.wkb3|Any files (*.*)|*.*||",&sFile,pDialog,FALSE,TRUE)){
		return FALSE;
	}
	if(sFile!="" && isFileExist(sFile)){
		psk->sBackupFile=sFile;
		psk->lSkipSavings=1;
		//psk->Load(sFile);
		CString sCabContent;
		ReadFile(sFile,sCabContent);
		if(ExtractFromTextCab(sCabContent,GetUserFolder())){
			AfxMessageBox(_l("Configuration restored")+"!"+"\n"+_l(PROGNAME" will exit now to apply changes"));
			if(pDialog){
				pDialog->SendMessage(WM_COMMAND,IDCANCEL,0);
			}
			if(pwk->pMainWnd){
				pwk->pMainWnd->PostMessage(WM_COMMAND,ID_SYSTRAY_EXIT,0);
			}
			CString sExe=GetApplicationDir();
			sExe+=GetApplicationName();
			sExe+=".exe";
			int iRes=(int)::ShellExecute(0,"open",sExe,"-wait=yes",NULL,SW_SHOWNORMAL);
		}else{
			AfxMessageBox(_l("File is not valid backup file. Please choose another"));
		}
	}
	return TRUE;
}

BOOL CALLBACK BackupToFile(HIROW hData, CDLG_Options* pDialog)
{
	if(!pDialog){
		psk->sBackupFile="";
	}else{
		pDialog->CommitData();
	}
	CString sFile=psk->sBackupFile;
	BOOL bAskForFile=TRUE;
	if(isFileExist(sFile)){
		if(AfxMessageBox(Format("%s ('%s')?\n%s. %s.",_l("Overwrite existing backup"),TrimMessage(sFile,40,3),_l("Yes to overwrite"),_l("No to choose another file")),MB_YESNO|MB_ICONQUESTION)==IDYES){
			bAskForFile=FALSE;
		}
	}
	if(bAskForFile){
		if(sFile==""){
			sFile=getFileFullName("Backups",TRUE);
			CreateDirIfNotExist(sFile);
			sFile+="wkbackup.wkb3";
		}
		if(!OpenFileDialog(PROGNAME" backup (*.wkb3)|*.wkb3|Any files (*.*)|*.*||",&sFile,pDialog,FALSE,TRUE,FALSE)){
			return FALSE;
		}
	}
	if(sFile!=""){
		psk->sBackupFile=sFile;
		if(pDialog){
			pDialog->PushSomeDataOnScreen(OC_BACKUP);
		}
		MakeBackup(sFile);
		AfxMessageBox(_l("Configuration backed-up")+"!");
	}
	return TRUE;
}

BOOL CALLBACK TestQRun(HIROW hData, CDLG_Options* pDialog)
{
	static long lRemItem=0;
	if(lRemItem){
		return FALSE;
	}
	SimpleTracker Track(lRemItem);
	//
	if(!pDialog || !pwk->pMainWnd){
		return FALSE;
	}
	long lID=long(hData);
	CSmartLock ql(psk->aQRunLock,TRUE,-1);
	for(int i=0;i<psk->aQuickRunApps.GetSize();i++){
		if(psk->aQuickRunApps[i] && psk->aQuickRunApps[i]->lUniqueCount==lID){
			pDialog->CommitData(psk->aQuickRunApps[i]->lOptionClass);
			pwk->pMainWnd->PostMessage(WM_COMMAND,WM_USER+QRUNAPPLICATION+i,0);
			break;
		}
	}
	return TRUE;
}

BOOL FillTemplateFromHWND(CQuickRunItem* pTemplate, HWND hwnd)
{
	return pwk->FillTemplateFromHWND(pTemplate, hwnd);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	return pwk->vEnumWindowsProc(hwnd, lParam);
}

// Сдвигание...
BOOL CALLBACK MoveQRunDown(HIROW hData, CDLG_Options* pDialog)
{
	CSmartLock ql(psk->aQRunLock,TRUE,-1);
	static long lRemItem=0;
	if(lRemItem || !pDialog){
		return FALSE;
	}
	SimpleTracker Track(lRemItem);
	long lID=long(hData),lItem1=-1,lItem2=-1;
	CQuickRunItem* pItem=NULL;
	CQuickRunItem* pItem2=NULL;

	for(int i=0;i<psk->aQuickRunApps.GetSize();i++){
		if(psk->aQuickRunApps[i] && psk->aQuickRunApps[i]->lUniqueCount==lID){
			lItem1=i;
			pItem=psk->aQuickRunApps[i];
			break;
		}
	}
	if(pItem>=0){
		// Ищем предыдущую
		HIROW hNext=pDialog->m_LList.GetNextIRow(pItem->hLeftPanePosition);
		if(hNext){
			for(int i=0;i<psk->aQuickRunApps.GetSize();i++){
				if(psk->aQuickRunApps[i] && psk->aQuickRunApps[i]->hLeftPanePosition==hNext){
					lItem2=i;
					pItem2=psk->aQuickRunApps[i];
					break;
				}
			}
		}
	}
	if(pItem && pItem2 && pItem!=pItem2 && lItem2!=lItem1){
		psk->aQuickRunApps[lItem2]=pItem;
		psk->aQuickRunApps[lItem1]=pItem2;
		HIROW dwData=(HIROW)pDialog->m_LList.GetIItemData(pItem->hLeftPanePosition,0);
		HIROW dwData2=(HIROW)pDialog->m_LList.GetIItemData(pItem2->hLeftPanePosition,0);
		CString sText=pDialog->m_LList.GetIItemText(pItem->hLeftPanePosition,0);
		int iIcon=pDialog->m_LList.GetIItemImage(pItem->hLeftPanePosition,0);
		HIROW hNewLeftPane=pDialog->m_LList.InsertIRow(pDialog->m_LList.GetParentIRow(pItem2->hLeftPanePosition),pItem2->hLeftPanePosition,sText,iIcon);
		pDialog->m_LList.DeleteIRow(pItem->hLeftPanePosition);
		pItem->hLeftPanePosition=hNewLeftPane;
		// Восстанавливаем целостность
		pDialog->m_LList.SetIItemData(pItem->hLeftPanePosition,0,(DWORD)dwData);
		pDialog->m_LList.SetIItemData(pItem2->hLeftPanePosition,0,(DWORD)dwData2);
		pDialog->aFromRtoL.SetAt(dwData,pItem->hLeftPanePosition);
		pDialog->aFromRtoL.SetAt(dwData2,pItem2->hLeftPanePosition);
		pDialog->m_LList.SelectIRow(pItem->hLeftPanePosition,TRUE);
	}
	return FALSE;
}

BOOL CALLBACK MoveFocusToQRun(HIROW hData, CDLG_Options* pDialog)
{
	CSmartLock ql(psk->aQRunLock,TRUE,-1);
	static long lToQRun=0;
	if(lToQRun || !pDialog){
		return FALSE;
	}
	SimpleTracker Track(lToQRun);
	CQuickRunItem* pItem=NULL;
	long lID=long(hData),lItem1=-1;
	for(int i=0;i<psk->aQuickRunApps.GetSize();i++){
		if(psk->aQuickRunApps[i] && psk->aQuickRunApps[i]->lUniqueCount==lID){
			lItem1=i;
			pItem=psk->aQuickRunApps[i];
			break;
		}
	}
	if(pItem){
		pDialog->MoveGlobalFocus(pItem->hLeftPanePosition,TRUE);
	}else{
		pwk->vAlert(_l("This item was deleted already")+"!");
	}
	return FALSE;
}

BOOL CALLBACK MoveQRunUp(HIROW hData, CDLG_Options* pDialog)
{
	CSmartLock ql(psk->aQRunLock,TRUE,-1);
	static long lRemItem=0;
	if(lRemItem || !pDialog){
		return FALSE;
	}
	SimpleTracker Track(lRemItem);
	long lID=long(hData),lItem1=-1,lItem2=-1;
	CQuickRunItem* pItem=NULL;
	CQuickRunItem* pItem2=NULL;

	for(int i=0;i<psk->aQuickRunApps.GetSize();i++){
		if(psk->aQuickRunApps[i] && psk->aQuickRunApps[i]->lUniqueCount==lID){
			lItem1=i;
			pItem=psk->aQuickRunApps[i];
			break;
		}
	}
	if(pItem>=0){
		// Ищем предыдущую
		HIROW hNext=pDialog->m_LList.GetPrevIRow(pItem->hLeftPanePosition);
		if(hNext){
			for(int i=0;i<psk->aQuickRunApps.GetSize();i++){
				if(psk->aQuickRunApps[i] && psk->aQuickRunApps[i]->hLeftPanePosition==hNext){
					lItem2=i;
					pItem2=psk->aQuickRunApps[i];
					break;
				}
			}
		}
	}
	if(pItem && pItem2 && pItem!=pItem2 && lItem2!=lItem1){
		psk->aQuickRunApps[lItem2]=pItem;
		psk->aQuickRunApps[lItem1]=pItem2;
		HIROW dwData=(HIROW)pDialog->m_LList.GetIItemData(pItem->hLeftPanePosition,0);
		HIROW dwData2=(HIROW)pDialog->m_LList.GetIItemData(pItem2->hLeftPanePosition,0);
		CString sText=pDialog->m_LList.GetIItemText(pItem2->hLeftPanePosition,0);
		int iIcon=pDialog->m_LList.GetIItemImage(pItem2->hLeftPanePosition,0);
		HIROW hNewLeftPane=pDialog->m_LList.InsertIRow(pDialog->m_LList.GetParentIRow(pItem->hLeftPanePosition),pItem->hLeftPanePosition,sText,iIcon);
		pDialog->m_LList.DeleteIRow(pItem2->hLeftPanePosition);
		pItem2->hLeftPanePosition=hNewLeftPane;
		// Восстанавливаем целостность
		pDialog->m_LList.SetIItemData(pItem->hLeftPanePosition,0,(DWORD)dwData);
		pDialog->m_LList.SetIItemData(pItem2->hLeftPanePosition,0,(DWORD)dwData2);
		pDialog->aFromRtoL.SetAt(dwData,pItem->hLeftPanePosition);
		pDialog->aFromRtoL.SetAt(dwData2,pItem2->hLeftPanePosition);
		pDialog->m_LList.SelectIRow(pItem->hLeftPanePosition,TRUE);
	}
	return FALSE;
}
#include "DLG_EditMLine.h"
int iUseCategAliases=0;
CMap<CString, const char*,HIROW,HIROW> g_aCategRowsMap;
BOOL CALLBACK DuplQRun(HIROW hData, CDLG_Options* pDialog);
CArray<HIROW,HIROW> g_aCategsL2R;
CArray<long,long> g_aCategsL2R_targs;
HIROW hQCateg=NULL;
CQuickRunItem* AddQRunTemplate(long lMacrosMask=0, const char* szFile=NULL,CQuickRunItem* pItem=0)
{
	return pwk->AddQRunTemplate(lMacrosMask, szFile,pItem,1);
}

BOOL CALLBACK AddQRunEx(HIROW hData, CDLG_Options* pDialog, CQuickRunItem* pTemplate, long lMacrosMask=0, BOOL bMoveFocus=TRUE)
{
	if(!pDialog){
		return FALSE;
	}
	static long lAddItem=0;
	if(lAddItem){
		return FALSE;
	}
	BOOL bDefaultValues=FALSE;
	SimpleTracker Track(lAddItem);
	BOOL bNeedManualAdd=0;
	{// Запрос имени и выход в случае проблем
		if(pTemplate==NULL){
			pDialog->CommitData(OC_QRUNFROM);
			pTemplate=pwk->AddQRunTemplate(lMacrosMask,0,0,0);
			bNeedManualAdd=1;
			if(pTemplate==NULL){
				return FALSE;
			}
		}else{
			if(pTemplate->lMacros!=lMacrosMask){
				// Для других макросов - свои заморочки
				return FALSE;
			}
		}
		pTemplate->lOptionClass=OC_STARQAPP+iQAppCount;
		if(pwk->isDefaultName(pTemplate->sItemName)){
			bDefaultValues=TRUE;
			if(pTemplate->lMacros==0){
				if(pTemplate->lParamType==2){
					if(ChooseDirEx(NULL,pDialog,&pTemplate->sItemPath)){
						CString sTestPath=pTemplate->sItemPath;
						sTestPath.TrimRight("\\/ ,.");
						int iSlashPos=sTestPath.ReverseFind('\\');
						if(iSlashPos!=-1){
							pTemplate->sItemName=sTestPath.Mid(iSlashPos+1);
						}
					}else{
						// Имя нам так и не задали - выходим!
						return FALSE;
					}
				}else{
					if(pTemplate->sItemPath.Find("http://")==-1 && psk->lCreateQRunFrom!=4){
						if(OpenFileDialog("Any files (*.*)|*.*||",&pTemplate->sItemPath,pDialog,FALSE,TRUE)){
							char szFile[MAX_PATH]="";
							_splitpath(pTemplate->sItemPath,NULL,NULL,szFile,NULL);
							pTemplate->sItemName=szFile;
						}else{
							// Имя нам так и не задали - выходим!
							return FALSE;
						}
					}
				}
			}
		}
		// Запрашиваем имя если еще не...
		if(pwk->isDefaultName(pTemplate->sItemName)){
			CString sBackup=pwk->GetQRunOptTitle(pTemplate);
			int iRes=pwk->CreateMlineEdit(pDialog,1,pTemplate->sItemName);
			if(iRes){
				pTemplate->sItemName.TrimLeft();
				pTemplate->sItemName.TrimRight();
				if(pwk->isDefaultName(pTemplate->sItemName)){
					pTemplate->sItemName="";
				}else if(lMacrosMask==2){
					pTemplate->sItemPath=pTemplate->sItemName;
					MakeSafeFileName(pTemplate->sItemPath,psk->sSafeChar.GetLength()?psk->sSafeChar[0]:0);
				}
			}else{
				pTemplate->sItemName="";
			}
			if(pTemplate->sItemName=="" || pwk->isDefaultName(pTemplate->sItemName)){
				// Имя нам так и не задали - выходим!
				// Нужно удалить из	// objSettings.aQuickRunApps.Add(pTemplate);
				return FALSE;
			}
			if(pTemplate->sItemName==""){
				pTemplate->sItemName=sBackup;
			}
		}
	}
	// Добавляем пункты к диалогу настроек
	if(bNeedManualAdd){
		// Если солздали по месту - добавляем!
		psk->AddQrunT(pTemplate);
	}
	iQAppCount++;// Считаем все
	// Начинаем прицеплять к дереву
	CString sMacrosTitle=pwk->GetQRunOptTitle(pTemplate);
	if(pTemplate->lMacros==1 || pTemplate->lMacros==2 || pTemplate->lMacros==3){
		iQMacCount++;// Считаем только макросы
	}
	int iIcon=pwk->GetIconNumByMacro(pTemplate->lMacros,pTemplate->lParamType,pTemplate->sItemPath);
	HIROW hParentData=hData;
	int iCategNumber=-1;
	// Формируем парентовую категорию если нужно
	if(pTemplate->sCategoryID!="" && (iCategNumber=pwk->FindQRunItemByID(pTemplate->sCategoryID))!=-1){
		CQuickRunItem* pCateg=psk->aQuickRunApps[iCategNumber];
		CString sParentKey=Format("%08x_%s",hData,pTemplate->sCategoryID);
		if(!g_aCategRowsMap.Lookup(sParentKey,hParentData)){
			// Добавляем!
			HIROW hDummyCateg=0;
			CString sCategTitleKey=Format("TITLE%08x",hData);
			if(!g_aCategRowsMap.Lookup(sCategTitleKey,hDummyCateg)){
				//pDialog->Option_AddTitle(hData,_l("Quickly move to Launch Set`s options"),61);
				g_aCategRowsMap.SetAt(sCategTitleKey,0);
			}
			hDummyCateg=pDialog->Option_AddAction(hQCateg/*hData*/,_l("Edit Launch Set")+" '"+pCateg->sItemName+"'", MoveFocusToQRun, HIROW(pCateg->lUniqueCount));
			HIROW hCategRow=pDialog->Option_AddAlias(hDummyCateg, pCateg->sItemName+"...", pwk->GetIconNumByMacro(pCateg->lMacros), hParentData, TRUE);
			g_aCategsL2R.Add(hCategRow);
			g_aCategsL2R_targs.Add(pCateg->lUniqueCount);
			//pDialog->SetLAliasTarget(hCategRow, hData);
			hParentData=hDummyCateg;
			g_aCategRowsMap.SetAt(sParentKey,hParentData);
		}
	}
	// Загловок итема - корень всех его настроек
	HIROW hNewItemTitle=pDialog->Option_AddString(FL_ROOT/*hData*/, ISIN2(pTemplate->lMacros,2,3)?_l("Macro title"):_l("Item title"), &(pTemplate->sItemName), pTemplate->sItemName, pTemplate->lOptionClass);
	pTemplate->hLeftPanePosition=pDialog->Option_AddAlias(hNewItemTitle, sMacrosTitle, iIcon, hParentData);
	pTemplate->hThisItemParent=hData;
	pTemplate->hThisItemRoot=hNewItemTitle;
	pDialog->m_LList.SelectIRow(pTemplate->hLeftPanePosition);
	pDialog->Option_SetRowColor(hNewItemTitle,0,95);
	pDialog->Option_SetRowColor(hNewItemTitle,1,95);
	// Путь и описание
	BOOL bCategItem=FALSE;
	if(pTemplate->lMacros==CATEG_MACR){
		bCategItem=TRUE;
	}
	if(bCategItem && bDefaultValues){
		pDialog->Option_AddTitle(hNewItemTitle,_l("Click on 'Apply' button after changing this options"),86);
	}
	CString sAuthor;
	CString sHotkeyTitle=_l("Shortcut key");
	CString sKeybMacroFile,sContent;
	if(pTemplate->lMacros==2){
		// Пытаемся считать из файла
		sKeybMacroFile=pwk->GetMacrosFileName(pTemplate->sItemPath,FALSE);
		if(sContent==""){
			ReadFileCashed(sKeybMacroFile,sContent);
			sContent+="\n";
		}
		if(pTemplate->sDsc==""){
			pTemplate->sDsc=CDataXMLSaver::GetInstringPart("// Description:","\n",sContent);
			if(pTemplate->sDsc==""){
				pTemplate->sDsc=CDataXMLSaver::GetInstringPart("' Description:","\n",sContent);
			}
			if(pTemplate->sDsc==""){
				// Первая строка
				pTemplate->sDsc=CDataXMLSaver::GetInstringPart("//","\n",sContent);
			}
			if(pTemplate->sDsc==""){
				// Первая строка
				pTemplate->sDsc=CDataXMLSaver::GetInstringPart("'","\n",sContent);
			}
			pTemplate->sDsc.TrimLeft();
			pTemplate->sDsc.TrimRight();
			pTemplate->sDsc.Replace("\r","");
		}
		sAuthor=CDataXMLSaver::GetInstringPart("// Author:","\n",sContent);
		if(sAuthor==""){
			sAuthor=CDataXMLSaver::GetInstringPart("' Author:","\n",sContent);
		}
		sAuthor.TrimLeft();
		sAuthor.TrimRight();
		CString sEntryDsc,sDecl;
		if(pwk->GetEntryDesc(sContent,0,sEntryDsc,sDecl)){
			sHotkeyTitle=sEntryDsc;
		}
	}
	if(pTemplate->sDsc!=""){
		CString sDsc="";//Не переводим, одни проблемы с этим _l(CString("DSCMAC_")+pTemplate->sDsc.Left(15),pTemplate->sDsc);
		CString sDscLangKey=CString("DSCMAC[")+pTemplate->sUniqueID+"]";
		sDsc=_l(sDscLangKey,_L_NODEFAULT);
		if(sDsc=="" || sDsc==_L_NODEFAULT){
			sDsc=pTemplate->sDsc;
		}
		pDialog->Option_AddTitle(hNewItemTitle,sDsc,61);
	}
	if(sAuthor!=""){
		pDialog->Option_AddTitle(hNewItemTitle,_l("Author")+": "+sAuthor,61);
	}
	if(pTemplate->qrHotkeys.GetSize()<1){
#ifdef _DEBUG
		DebugBreak();
#endif
		pTemplate->qrHotkeys.Add(new CQRunHotkeyItem());
	}
	// Включаем что изменилась горклавиша...
	pDialog->bAnyHotkeyWasChanged=pwk->bHotkeyChangedMode;// Чтобы перенести это и в модуль по гор. клавишам....
	pTemplate->qrHotkeys[0]->oHotKey.SetActionId(pwk->GetQrunKeyDsc(pTemplate,0));
	pDialog->Option_AddHotKey(hNewItemTitle,_l(sHotkeyTitle),&pTemplate->qrHotkeys[0]->oHotKey, pTemplate->qrHotkeys[0]->oHotKey, pTemplate->lOptionClass);
	if(pTemplate->lMacros==2){
		int iEntryCount=2;
		CString sHotkeyTitleEx,sDecl;
		while(pwk->GetEntryDesc(sContent,iEntryCount,sHotkeyTitleEx,sDecl)){
			if(pTemplate->qrHotkeys.GetSize()<iEntryCount){
				pTemplate->qrHotkeys.Add(new CQRunHotkeyItem());
			}
			pTemplate->qrHotkeys[iEntryCount-1]->oHotKey.SetActionId(pwk->GetQrunKeyDsc(pTemplate,iEntryCount-1));
			pDialog->Option_AddHotKey(hNewItemTitle,_l(sHotkeyTitleEx), &pTemplate->qrHotkeys[iEntryCount-1]->oHotKey, pTemplate->qrHotkeys[iEntryCount-1]->oHotKey, pTemplate->lOptionClass);
			iEntryCount++;
		}
	}
	if(bCategItem){
		pDialog->Option_AddBOOL(hNewItemTitle,_l("Launch this item at "PROGNAME" startup"),&pTemplate->lStartAtStartUp,pTemplate->lStartAtStartUp,0,pTemplate->lOptionClass);
		if(pwk->isPluginLoaded("WP_OpenWithExt")){
			pDialog->Option_AddBOOL(hNewItemTitle,_l("Add to Explorer`s context menu as a submenu"),&pTemplate->lShowInUpMenu,pTemplate->lShowInUpMenu,0,pTemplate->lOptionClass);
		}
		pDialog->Option_AddBOOL(hNewItemTitle,_l("Add to tray special icon at "PROGNAME" startup")+" ("+_l("Clicking it, you will run this item")+")",&pTemplate->lUseDetachedPresentation,pTemplate->lUseDetachedPresentation,0,pTemplate->lOptionClass);
		pDialog->Option_AddNString(hNewItemTitle,_l("Run all on double-press: Add delay between runs (sec)"),&pTemplate->lStartupTime,pTemplate->lStartupTime,0,1000,pTemplate->lOptionClass);
	}
	if(!bCategItem){
		CString sPatPerm="";
		if(pTemplate->lMacros!=2){
			sPatPerm=" (";
			sPatPerm+=_l("patterns permitted");
			sPatPerm+=")";
		}
		CString sAppFileTitle;
		if(pTemplate->lMacros==0 || pTemplate->lMacros==1){
			if(pTemplate->lParamType==2){
				sAppFileTitle=_l("Folder path");
			}else{
				if(pTemplate->sItemPath.Find("http://")==-1){
					sAppFileTitle=_l("Application path");
				}else{
					sAppFileTitle=_l("Internet address");
				}
			}
			sAppFileTitle+="\t"+_l("To quickly set value for this item")+" "+_l("You can simply drag file from explorer")+" "+_l("to the right part of this row");
		}else if(pTemplate->lMacros==2){
			sAppFileTitle=_l("Macro file");
		}else if(pTemplate->lMacros==3){
			sAppFileTitle=_l("Text to insert")+sPatPerm+"\t"+_l("This text will be automatically inserted into active application when you call this macro");
		}
		HIROW hNewItem=pDialog->Option_AddString(hNewItemTitle,sAppFileTitle,&(pTemplate->sItemPath), pTemplate->sItemPath,pTemplate->lOptionClass);
		if(pTemplate->lMacros==0 || pTemplate->lMacros==1){
			if(pTemplate->lParamType==2){
				pDialog->Option_AddActionToEdit(hNewItem, ChooseDir, hNewItem, pTemplate->lOptionClass, 94);
				pDialog->Option_AddBOOL(hNewItemTitle,_l("Create folder when needed")+"\t"+_l("If this folder is not present, "PROGNAME" will try to create it","If this folder is not present, "PROGNAME" will try to create it. You can add patterns into folder name to create unique folders"), &pTemplate->lAutoCreate, pTemplate->lAutoCreate, 0, pTemplate->lOptionClass);
			}else{
				pDialog->Option_AddActionToEdit(hNewItem, ChooseAnyFile, hNewItem, pTemplate->lOptionClass, 94);
			}
		}
		if(pTemplate->lMacros!=2){
			if(pTemplate->lMacros==3){
				HIROW hNewItemParam=pDialog->Option_AddString(hNewItemTitle,_l("Or file to insert")+sPatPerm+"\t"+_l("If you type file name here, it`s content will be inserted too (right after text from the previous filed)"),&(pTemplate->sItemParams), pTemplate->sItemParams, pTemplate->lOptionClass);
				pDialog->Option_AddActionToEdit(hNewItemParam, ChooseAnyFile, hNewItemParam, pTemplate->lOptionClass, 94);
			}else if(pTemplate->lParamType!=2){
				HIROW hNewItemParam=pDialog->Option_AddString(hNewItemTitle,_l("Parameter")+sPatPerm,&(pTemplate->sItemParams), pTemplate->sItemParams, pTemplate->lOptionClass);
				pDialog->Option_AddActionToEdit(hNewItemParam, ChooseAnyFile, hNewItemParam, pTemplate->lOptionClass, 94);
			}
		}else{
			if(pTemplate->sParamTypeName=="" || pTemplate->lParamType==-1){
				// Считываем тип параметра макроса
				CString sContent;
				ReadFileCashed(sKeybMacroFile,sContent);
				CString sParTypeU=CDataXMLSaver::GetInstringPart("// ParamType:","\n",sContent);
				if(sParTypeU==""){
					sParTypeU=CDataXMLSaver::GetInstringPart("' ParamType:","\n",sContent);
				}
				sParTypeU.TrimLeft();
				sParTypeU.TrimRight();
				if(sParTypeU==""){
					pTemplate->sParamTypeName="Parameter";
				}
				CString sParType=sParTypeU;
				sParType.MakeLower();
				pTemplate->lParamType=0;
				if(sParType=="file"){
					pTemplate->lParamType=1;
					pTemplate->sParamTypeName="File name";
				}
				if(sParType=="folder"){
					pTemplate->lParamType=2;
					pTemplate->sParamTypeName="Folder";
				}
				if(sParType=="none"){
					pTemplate->lParamType=3;
				}
				if(sParType=="string"){
					pTemplate->lParamType=4;
					pTemplate->sParamTypeName="String";
				}
			}
			switch(pTemplate->lParamType){
			case 1://File
				{
					HIROW hNewItemParam=pDialog->Option_AddString(hNewItemTitle,_l("Parameter")+" ("+_l(pTemplate->sParamTypeName)+")"+sPatPerm,&(pTemplate->sItemParams), pTemplate->sItemParams, pTemplate->lOptionClass);
					pDialog->Option_AddActionToEdit(hNewItemParam, ChooseAnyFile, hNewItemParam, pTemplate->lOptionClass, 94);
					break;
				}
			case 2://Folder
				{
					HIROW hNewItemParam=pDialog->Option_AddString(hNewItemTitle,_l("Parameter")+" ("+_l(pTemplate->sParamTypeName)+")"+sPatPerm,&(pTemplate->sItemParams), pTemplate->sItemParams, pTemplate->lOptionClass);
					pDialog->Option_AddActionToEdit(hNewItemParam, ChooseDir, hNewItemParam, pTemplate->lOptionClass, 94);
					break;
				}
			case 3://None
				break;
			case 4://String
				{
					HIROW hNewItemParam=pDialog->Option_AddString(hNewItemTitle,_l("Parameter")+" ("+_l(pTemplate->sParamTypeName)+")"+sPatPerm,&(pTemplate->sItemParams), pTemplate->sItemParams, pTemplate->lOptionClass);
					break;
				}
			default://String
				{
					HIROW hNewItemParam=pDialog->Option_AddString(hNewItemTitle,_l(pTemplate->sParamTypeName)+sPatPerm,&(pTemplate->sItemParams), pTemplate->sItemParams, pTemplate->lOptionClass);
					break;
				}
			}
		}
		if(pTemplate->lMacros==2){
			// Дублируем для пущего эффекта
			pDialog->Option_AddActionToEdit(hNewItem, ChooseWkmFile, hNewItem, pTemplate->lOptionClass, 94);
			pDialog->Option_AddActionEx(hNewItemTitle, _l("Open macro script in text editor"), ChooseAnyFile_Macros, 0, hNewItem, 107, pTemplate->lOptionClass);
		}
		// Последние настройки...
		if(pTemplate->lMacros==3){
			pDialog->Option_AddBOOL(hNewItemTitle,_l("Insert random string, not file in whole")+"\t"+_l("If you set file name in the field above, you can check this option to use it as a set of lines. Every time this macro is called, "PROGNAME" will use random string from this set"),&pTemplate->lExpandFolderOrCheckInstance,pTemplate->lExpandFolderOrCheckInstance,0,pTemplate->lOptionClass);
		}
		if(ISIN2(pTemplate->lMacros,2,3)){
			HIROW hNewItemAdd=pDialog->Option_AddTitle(hNewItemTitle,_l("Advanced options"),5);
			if(pTemplate->lMacros==2){
				pDialog->Option_AddBOOL(hNewItemAdd,_l("Launch this item at "PROGNAME" startup"),&pTemplate->lStartAtStartUp,pTemplate->lStartAtStartUp,0,pTemplate->lOptionClass);
			}
			pDialog->Option_AddBOOL(hNewItemAdd,_l("Add to tray special icon at "PROGNAME" startup")+" ("+_l("Clicking it, you will run this item")+")",&pTemplate->lUseDetachedPresentation,pTemplate->lUseDetachedPresentation,0,pTemplate->lOptionClass);
			//pDialog->Option_AddBOOL(hNewItemAdd,_l("Show it in the root menu where applicable")+"\t"+_l("Convenient for quick one-click access to important items"),&pTemplate->lShowInUpMenu,pTemplate->lShowInUpMenu,0,pTemplate->lOptionClass);
		}
		// Десктопы
		if(pTemplate->lMacros==0 && psk->lEnableAddDesktops){
			CString sDesktopNames=_l("Active desktop");
			for(int i=0;i<psk->lDesktopSwitches;i++){
				sDesktopNames+="\t"+pwk->GetDesktopName(i);
			}
			pDialog->Option_AddCombo(hNewItemTitle,_l("Start in desktop")+"\t"+_l("Start application in different desktop"), &pTemplate->lStartInDesktop, pTemplate->lStartInDesktop, sDesktopNames, pTemplate->lOptionClass);
		}
		if(pTemplate->lMacros!=2 && pTemplate->lMacros!=3){
			//HIROW hNewItemAdd=pDialog->Option_AddBOOL(hNewItemTitle,_l("Advanced options")+"\t"+_l("Note: Some of them may not work in different cases"),&pTemplate->lUseAdditionalOptions,pTemplate->lUseAdditionalOptions,-5,pTemplate->lOptionClass);
			HIROW hNewItemAdd=pDialog->Option_AddTitle(hNewItemTitle,_l("Advanced options")+"\t"+_l("Note: Some of them may not work in different cases"),5);
			if(pTemplate->lParamType!=2){
				HIROW hStartDir=pDialog->Option_AddString(hNewItemAdd,_l("Startup directory"),&(pTemplate->sStartupDir), pTemplate->sStartupDir,pTemplate->lOptionClass);
				pDialog->Option_AddActionToEdit(hStartDir, ChooseDir, hStartDir, pTemplate->lOptionClass, 94);
				// Что делать с приложением после старта
				pDialog->Option_AddCombo(hNewItemAdd,_l("After start")+"\t"+_l("What to do after application start (hide to tray for example)"),&pTemplate->lAfterStart, pTemplate->lAfterStart, _l("Nothing specific")+"\t"+_l("Hide to tray")+sDesktopWarning+"\t"+_l("Hide completely")+"\t"+_l("Minimize")+"\t"+_l("Maximize"),pTemplate->lOptionClass);
				pDialog->Option_AddCombo(hNewItemAdd,_l("If you have already started this application"),&pTemplate->lSearchInstanceFirst, pTemplate->lSearchInstanceFirst, _l("Start new instance")+"\t"+_l("Switch to the opened instance")+"\t"+_l("Close previous instance")+" "+_l("and start new one")+"\t"+_l("Kill previous instance")+" "+_l("and start new one")+"\t"+_l("Close previous instance")+", "+_l("do not start new one")+"\t"+_l("Kill previous instance")+", "+_l("do not start new one"),pTemplate->lOptionClass);
				pDialog->Option_AddBOOL(hNewItemAdd,_l("Make application window 'Always on top'")+" ("+_l("After start")+")"+APP_DISCLAMER,&pTemplate->lExpandFolderOrCheckInstance,pTemplate->lExpandFolderOrCheckInstance,0,pTemplate->lOptionClass);
			}
			pDialog->Option_AddBOOL(hNewItemAdd,_l("Add to tray special icon at "PROGNAME" startup")+" ("+_l("Clicking it, you will run this item")+")",&pTemplate->lUseDetachedPresentation,pTemplate->lUseDetachedPresentation,0,pTemplate->lOptionClass);
			// Как показывать
			if(pTemplate->lParamType!=2){
				pDialog->Option_AddBOOL(hNewItemAdd,_l("Launch this item at "PROGNAME" startup"),&pTemplate->lStartAtStartUp,pTemplate->lStartAtStartUp,0,pTemplate->lOptionClass);
				//pDialog->Option_AddBOOL(hNewItemAdd,_l("Show it in the root menu where applicable")+"\t"+_l("Convenient for quick one-click access to important items"),&pTemplate->lShowInUpMenu,pTemplate->lShowInUpMenu,0,pTemplate->lOptionClass);
			}
			if(pTemplate->lParamType==2){
				pDialog->Option_AddBOOL(hNewItemAdd,_l("Open this folder at "PROGNAME" startup"),&pTemplate->lStartAtStartUp,pTemplate->lStartAtStartUp,0,pTemplate->lOptionClass);
				pDialog->Option_AddBOOL(hNewItemAdd,_l("Expand content of this folder into additional submenu"),&pTemplate->lExpandFolderOrCheckInstance,pTemplate->lExpandFolderOrCheckInstance,0,pTemplate->lOptionClass);
				if(pTemplate->lParamType==2 && pTemplate->lMacros!=2){
					pDialog->Option_AddString(hNewItemAdd,_l("Show matching files only")+"\t"+_l("Here you can set file filter mask","Here you can set file filter mask (to prevent showing unwanted files). Example: *.txt;*.doc;*.lnk"),&(pTemplate->sAutoExpandMask), pTemplate->sAutoExpandMask, pTemplate->lOptionClass);
				}
			}
			// Менять ли положение окна
			if(pTemplate->lParamType!=2){
				HIROW hNewItemFPos=pDialog->Option_AddBOOL(hNewItemAdd,_l("Set application`s window position")+APP_DISCLAMER,&pTemplate->lFixPostion,pTemplate->lFixPostion,5,pTemplate->lOptionClass);
				pDialog->Option_AddNString(hNewItemFPos,_l("Window X position")+" ("+_l("pixels")+")"+APP_DISCLAMER, &pTemplate->lFixedXPos, pTemplate->lFixedXPos, -10000, 10000,pTemplate->lOptionClass);
				pDialog->Option_AddNString(hNewItemFPos,_l("Window Y position")+" ("+_l("pixels")+")"+APP_DISCLAMER, &pTemplate->lFixedYPos, pTemplate->lFixedYPos, -10000, 10000,pTemplate->lOptionClass);
				HIROW hNewItemFSiz=pDialog->Option_AddBOOL(hNewItemAdd,_l("Set application`s window size")+APP_DISCLAMER,&pTemplate->lFixSize,pTemplate->lFixSize,5,pTemplate->lOptionClass);
				pDialog->Option_AddNString(hNewItemFSiz,_l("Window width")+" ("+_l("pixels")+")"+APP_DISCLAMER, &pTemplate->lFixedWidth, pTemplate->lFixedWidth, 0, 10000,pTemplate->lOptionClass);
				pDialog->Option_AddNString(hNewItemFSiz,_l("Window height")+" ("+_l("pixels")+")"+APP_DISCLAMER, &pTemplate->lFixedHeight, pTemplate->lFixedHeight, 0, 10000,pTemplate->lOptionClass);
				pDialog->Option_AddNSlider(hNewItemAdd,_l("Set application`s window opacity (percents)")+"\t"+APP_DISCLAMER0,&pTemplate->lOpacity,pTemplate->lOpacity,1,100,pTemplate->lOptionClass);
				pDialog->Option_AddNString(hNewItemAdd,_l("Time to wait after start")+" ("+_l("sec")+")\t"+_l("How long application initialize itself after start")+", "+_l("This time must be big enough to give application possibility to create application`s main window"), &pTemplate->lStartupTime, pTemplate->lStartupTime, 0, 999,pTemplate->lOptionClass);
			}
		}
	}
	pDialog->Option_AddTitle(hNewItemTitle,_l("Item position in popup menus"),61);
	if(pwk->g_aListOfCategs.GetSize()>1){
		// Категория
		if(pTemplate->lMacros!=CATEG_MACR){
			pDialog->Option_AddCombo(hNewItemTitle,_l("Launch Set")+"\t"+_l("Click on 'Apply' button after changing this options"),&pTemplate->lTempCategNumber,pTemplate->lTempCategNumber,g_ListOfCategs,pTemplate->lOptionClass);
		}
	}
	// Где показывать
	pDialog->Option_AddCombo(hNewItemTitle,_l("Item visibility"),&pTemplate->lHowToShowInTray,pTemplate->lHowToShowInTray,_l("Hide item from menus")+"\t"+_l("Show item in menus"),pTemplate->lOptionClass);
	HIROW hActions=pDialog->Option_AddTitle(hNewItemTitle,_l("Actions"),61);
	// Общие действия
	pDialog->Option_AddActionEx(hActions,_l("Make a copy"), DuplQRun, 0, HIROW(pTemplate->lUniqueCount), 88, pTemplate->lOptionClass);
	if(pTemplate->lMacros!=CATEG_MACR){
		pDialog->Option_AddActionEx(hActions,_l("Move up"), MoveQRunUp, 0, HIROW(pTemplate->lUniqueCount), 90, pTemplate->lOptionClass);
		pDialog->Option_AddActionEx(hActions,_l("Move down"), MoveQRunDown, 0, HIROW(pTemplate->lUniqueCount), 91, pTemplate->lOptionClass);
	}
	if(pTemplate->lMacros==0){
		pDialog->Option_AddActionEx(hActions,_l("Test this item"),TestQRun,0, HIROW(pTemplate->lUniqueCount), 103, pTemplate->lOptionClass);
	}
	if(pTemplate->lMacros==2){
		pDialog->Option_AddActionEx(hActions,_l("Play macro"),TestQRun,0, HIROW(pTemplate->lUniqueCount), 103, pTemplate->lOptionClass);
	}
	pDialog->Option_AddActionEx(hActions,_l("Delete"), RemoveQRun, 0, HIROW(pTemplate->lUniqueCount), 62, pTemplate->lOptionClass);
	if(bMoveFocus){
		pDialog->MoveGlobalFocus(pTemplate->hLeftPanePosition,TRUE);
	}
	psk->lCreateQRunFrom=0;
	return TRUE;
}

BOOL CALLBACK DuplQRun(HIROW hData, CDLG_Options* pDialog)
{
	CSmartLock ql(psk->aQRunLock,TRUE,-1);
	static long lDupItem=0;
	if(lDupItem || !pDialog){
		return FALSE;
	}
	SimpleTracker Track(lDupItem);
	long lID=long(hData);
	CQuickRunItem* pItem=NULL;

	for(int i=0;i<psk->aQuickRunApps.GetSize();i++){
		if(psk->aQuickRunApps[i] && psk->aQuickRunApps[i]->lUniqueCount==lID){
			pItem=psk->aQuickRunApps[i];
			break;
		}
	}

	if(pItem){
		CQuickRunItem* pTemplate=AddQRunTemplate(0,0,pItem);
		pwk->bHotkeyChangedMode=1;
		AddQRunEx(pItem->hThisItemParent, pDialog, pTemplate, pTemplate->lMacros, TRUE);
	}
	return TRUE;
}

BOOL CALLBACK AddQRunFromFile(HIROW hData, CDLG_Options* pDialog)
{
	char* szFile=(char*)hData;
	if(!szFile){
		return 0;
	}
	// чтобы не повторялось!!!
	pDialog->m_pStartupInfo->fpPostAction=0;
	CString sFilePathName=szFile;
	free(szFile);
	long lAttr=GetFileAttributes(sFilePathName);
	if(lAttr==-1){
		return 0;
	}
	//wkm?
	CQuickRunItem* pTemplate=NULL;
	if(sFilePathName.Find(".wkm")!=-1){
		pTemplate=AddQRunTemplate(2,sFilePathName);
		if(pTemplate==NULL){
			return FALSE;
		}
		pwk->bHotkeyChangedMode=1;
		AddQRunEx(pwk->hQRunType2Right, pDialog, pTemplate, 2);
		return TRUE;
	}
	psk->lCreateQRunFrom=1;
	if(isFolder(sFilePathName)){
		psk->lCreateQRunFrom=2;
	}
	pTemplate=AddQRunTemplate(0,sFilePathName);
	if(pTemplate==NULL){
		return FALSE;
	}
	pwk->bHotkeyChangedMode=1;
	AddQRunEx(pwk->hQRunType0Right, pDialog, pTemplate, 0);
	return TRUE;
}

BOOL CALLBACK AddQRunFromHWnd(HIROW hData, CDLG_Options* pDialog)
{
	pwk->bHotkeyChangedMode=1;
	CQuickRunItem* pTemplate=pwk->AddQRunFromHWnd(hData, pDialog);
	AddQRunEx(pwk->hQRunType0Right, pDialog, pTemplate, 0);
	return TRUE;
}

BOOL CALLBACK AddQRunApp(HIROW hData, CDLG_Options* pDialog)
{
	psk->lCreateQRunFrom=0;
	pDialog->PushSomeDataOnScreen(OC_QRUNFROM);
	pwk->bHotkeyChangedMode=1;
	return AddQRunEx(hData, pDialog, NULL, 0);
}

BOOL CALLBACK AddQRunLink(HIROW hData, CDLG_Options* pDialog)
{
	psk->lCreateQRunFrom=3;
	pDialog->PushSomeDataOnScreen(OC_QRUNFROM);
	pwk->bHotkeyChangedMode=1;
	return AddQRunEx(hData, pDialog, NULL, 0);
}

BOOL CALLBACK AddQRunEmpty(HIROW hData, CDLG_Options* pDialog)
{
	psk->lCreateQRunFrom=4;
	pDialog->PushSomeDataOnScreen(OC_QRUNFROM);
	pwk->bHotkeyChangedMode=1;
	return AddQRunEx(hData, pDialog, NULL, 0);
}

BOOL CALLBACK AddQRunFile(HIROW hData, CDLG_Options* pDialog)
{
	psk->lCreateQRunFrom=1;
	pDialog->PushSomeDataOnScreen(OC_QRUNFROM);
	pwk->bHotkeyChangedMode=1;
	return AddQRunEx(hData, pDialog, NULL, 0);
}

BOOL CALLBACK AddQRunFold(HIROW hData, CDLG_Options* pDialog)
{
	psk->lCreateQRunFrom=2;
	pDialog->PushSomeDataOnScreen(OC_QRUNFROM);
	pwk->bHotkeyChangedMode=1;
	return AddQRunEx(hData, pDialog, NULL, 0);
}

BOOL CALLBACK AddQRun(HIROW hData, CDLG_Options* pDialog)
{
	pwk->bHotkeyChangedMode=1;
	return AddQRunEx(hData, pDialog, NULL, 0);
}

BOOL CALLBACK AddQRun2(HIROW hData, CDLG_Options* pDialog)
{
	pwk->bHotkeyChangedMode=1;
	return AddQRunEx(hData, pDialog, NULL, 1);
}

BOOL CALLBACK AddQRun3(HIROW hData, CDLG_Options* pDialog)
{
	pwk->bHotkeyChangedMode=1;
	return AddQRunEx(hData, pDialog, NULL, 2);
}

BOOL CALLBACK AddQRun4(HIROW hData, CDLG_Options* pDialog)
{
	pwk->bHotkeyChangedMode=1;
	BOOL bRes=AddQRunEx(hData, pDialog, NULL, 3);
	return bRes;
}

BOOL CALLBACK AddQRunCateg(HIROW hData, CDLG_Options* pDialog)
{
	pwk->bHotkeyChangedMode=1;
	BOOL bRes=AddQRunEx(hData, pDialog, NULL, CATEG_MACR);
	if(pDialog && bRes){
		pwk->bCategsWereChanged=1;
		//pwk->vAlert(_l("Change name of the new Launch set item\nand click 'Apply' button to finish"),_l(PROGNAME": Quick-run"));
		pDialog->CallApply(IDOK,FALSE);
		//pDialog->MoveGlobalFocus(pwk->hQRunType3,TRUE);
	}
	return bRes;
}

BOOL CALLBACK ShowQRun3Folder(HIROW hData, CDLG_Options* pDialog)
{
	CString sFile=CString(MACROS_FOLDER)+"\\";//*.wkm
	CString sFullFile=getFileFullName(sFile);
	if(OpenFileDialog("Wkm files (*.wkm,*.js,*.vbs)|*.wkm;*.js;*.vbs||",&sFullFile,pDialog,FALSE, TRUE)){
		CQuickRunItem* pTemplate=AddQRunTemplate(2,sFullFile);
		if(pTemplate==NULL){
			return FALSE;
		}
		pwk->bHotkeyChangedMode=1;
		AddQRunEx(hData, pDialog, pTemplate, 2);
		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK ShowQRunHelp(HIROW hData, CDLG_Options* pDialog)
{
	ShowHelp("Macros");
	return TRUE;
}

void GetRegInfoFromText(CString &sText,CString& sLikUser,CString& sLikKey, const char* szSalt);
BOOL CALLBACK SubmitLikKeys(CString& sText, HIROW hData, CDLG_Options* pDialog)
{
	BOOL bRes=FALSE;
	if(!sText.IsEmpty()){
		CString sUser,sKey;
		GetRegInfoFromText(sText,sUser,sKey,0);
		if(sKey.IsEmpty() || sUser.IsEmpty()){
			bRes=FALSE;
		}else{
			bRes=TRUE;
			psk->sLikUser=sUser;
			psk->sLikKey=sKey;
			if(pDialog){
				pDialog->PushSomeDataOnScreen(OC_LICDATA);
			}
		}
	}
	if(pDialog){
		if(!bRes){
			pwk->vAlert(_l("No registration info found")+"!\n"+_l("Please, enter registration data\n(registration email address and serial key)\nmanually"),_l("Registration"));
		}else{
			pwk->vAlert(_l("Registration info pasted successfully")+"!",_l("Registration"));
		}
	}
	if(bRes){
		pwk->lLikKeysPasted=1;
	}
	return bRes;
}

BOOL CALLBACK PasteLikKeysFromFile(HIROW hData, CDLG_Options* pDialog)
{
	static long bLikPaste=0;
	if(bLikPaste>0){
		return FALSE;
	}
	SimpleTracker TrackLik(bLikPaste);
	if(!OpenFileDialog("Any files (*.*)|*.*||",&psk->sLikFile,pDialog,FALSE,TRUE)){
		return FALSE;
	}
	CString sText;
	if(psk->sLikFile!=""){
		ReadFile(psk->sLikFile,sText);
	}
	return SubmitLikKeys(sText,hData, pDialog);
}

BOOL CALLBACK PasteLikKeys(HIROW hData, CDLG_Options* pDialog)
{
	static long bLikPaste=0;
	if(bLikPaste>0){
		return FALSE;
	}
	SimpleTracker TrackLik(bLikPaste);
	WCHAR* wszText=GetClipboardText();
	if(!wszText){
		return FALSE;
	}
	USES_CONVERSION;
	CString sData=W2A(wszText);
	delete[] wszText;
	return SubmitLikKeys(sData,hData, pDialog);
}

BOOL CALLBACK OpenPlugins(HIROW hData, CDLG_Options* pDialog)
{
	ShellExecute(pwk->pMainWnd?pwk->pMainWnd->GetSafeHwnd():0,"open",(CString)"http://www.wiredplane.com/cgi-bin/wp_engine.php?target=wkplugins&who="PROGNAME+PROG_VERSION,NULL,NULL,SW_SHOWNORMAL);
	return TRUE;
}

BOOL CALLBACK OpenMacros(HIROW hData, CDLG_Options* pDialog)
{
	ShellExecute(pwk->pMainWnd?pwk->pMainWnd->GetSafeHwnd():0,"open",(CString)"http://www.wiredplane.com/cgi-bin/wp_engine.php?target=wkmacros&who="PROGNAME+PROG_VERSION,NULL,NULL,SW_SHOWNORMAL);
	return TRUE;
}

HIROW hSysResInd=0;
BOOL CALLBACK MoveToResIndic(HIROW hData, CDLG_Options* pDialog)
{
	if(hSysResInd && pDialog){
		pDialog->MoveGlobalFocus(hSysResInd,TRUE);
	}
	return FALSE;
}

BOOL CALLBACK ReattachClip(HIROW hData, CDLG_Options* pDialog)
{
	pwk->pMainWnd->PostMessage(WM_COMMAND,ID_REATTACHCLIP,0);
	return FALSE;
}

BOOL CALLBACK MovePaneFocus(HIROW hData, CDLG_Options* pDialog)
{
	if(hData && pDialog){
		hData=pDialog->GetAliasHIROW(hData);
		if(hData){
			pDialog->MoveGlobalFocus(hData);
		}
	}
	return FALSE;
}

BOOL CALLBACK MovePaneFocusP(HIROW hDataP, CDLG_Options* pDialog)
{
	HIROW hDataCell=*((HIROW*)hDataP);
	return MovePaneFocus(hDataCell, pDialog);
}

BOOL CALLBACK AddDefMacros_Warn(HIROW hData, CDLG_Options* pDialog)
{
	if(AfxMessageBox(_l("Do you really want to install\ndefault quick-run items again")+"?",MB_YESNO|MB_ICONQUESTION)==IDYES){
		psk->lDefaultMacros=0;
		AfxMessageBox(_l("Default Quick-run items will be\nadded next time you start "PROGNAME));
	}
	return TRUE;
}

// Для задания горячих клавиш на активацию флоатеров
BOOL CALLBACK AddDefMacros(HIROW hData, CDLG_Options* pDialog);
BOOL CALLBACK ProcessUpMenu(WPARAM wParam, LPARAM lParam, CDLG_Options* pDialog)
{
	if(wParam>=UPMENU_ID+100 && wParam<UPMENU_ID+100+100){
		int iNewLang=wParam-(UPMENU_ID+100);
		if(pwk->vGetApplicationLang()!=iNewLang){
			pDialog->AddChanged(OC_LANG);
			pDialog->CommitData();// Делаем принудительный коммит, так как AddDefMacros меняет по живому!!!
			// А теперь заперепишем переменные за язык отвечающие
			pwk->vGetApplicationLang()=iNewLang;
			GetApplicationLang()=iNewLang;
			psk->lTempLang=iNewLang;
			pDialog->bManualPreventCommit=1;
			AddDefMacros(0,0);
			pDialog->CallApply(IDOK,FALSE);
			return 1;
		}else{
			return 0;
		}
	}
	switch(wParam)
	{
	case UPMENU_ID+1:
		BackupToFile(0,pDialog);
		break;
	case UPMENU_ID+2:
		BackupFromFile(0,pDialog);
		break;
	case UPMENU_ID+4:
		AddQRunCateg(pwk->hQRunTypeCCRight,pDialog);
		break;
	case UPMENU_ID+5:
		AddQRunFile(pwk->hQRunType0Right,pDialog);
		break;
	case UPMENU_ID+6:
		AddQRunFold(pwk->hQRunType0Right,pDialog);
		break;
	case UPMENU_ID+7:
		AddQRunApp(pwk->hQRunType0Right,pDialog);
		break;
	case UPMENU_ID+8:
		AddQRun3(pwk->hQRunType2Right,pDialog);
		break;
	case UPMENU_ID+9:
		ShowQRun3Folder(pwk->hQRunType2Right,pDialog);
		break;
	case UPMENU_ID+10:
		AddQRun4(pwk->hQRunType2Right,pDialog);
		break;
	case UPMENU_ID+11:
		AddQRun2(pwk->hQRunType2Right,pDialog);
		break;
	case UPMENU_ID+13:
		ShowHelp("Overview");
		break;
	case UPMENU_ID+14:
		BuyWK(0,pDialog);
		break;
	case UPMENU_ID+12:
		OpenMacros(0,pDialog);
		break;
	case UPMENU_ID+15:
		OpenPlugins(0,pDialog);
		break;
	case UPMENU_ID+16:
		ListHotkeys(0,0);
		break;
	case UPMENU_ID+17:
		AddQRunLink(pwk->hQRunType0Right,pDialog);
		break;
	case UPMENU_ID+19:
		AddQRunEmpty(pwk->hQRunType0Right,pDialog);
		break;
	case UPMENU_ID+18:
		{
			if(pwk->pMainWnd){
				pDialog->PostMessage(WM_COMMAND,IDCANCEL,0);
				pwk->pMainWnd->PostMessage(WM_COMMAND,ID_SYSTRAY_RECMACRO,0);
			}
		}
		break;
	case UPMENU_ID+20:
		if(pwk->pMainWnd){
			SendMessage(pDialog->GetSafeHwnd(),WM_SYSCOMMAND,SC_MINIMIZE,0);
			pwk->pMainWnd->PostMessage(WM_COMMAND,ID_SYSTRAY_ABOUT,0);
		}
		break;
	case UPMENU_ID+21:
		AddDefMacros_Warn(0,0);
		break;
	case UPMENU_ID+22:
		ShowIntroduction(pDialog);
		break;
	default:
	case UPMENU_ID+3:
		pDialog->CallApply(IDOK,TRUE);
		pwk->pMainWnd->PostMessage(WM_COMMAND,ID_SYSTRAY_EXIT,0);
		break;
	};
	return TRUE;
}

HIROW hLSwitch=0;
HIROW hWHelperPlugin=0;
CString& getOptionsDefaultFind();// Для поиска
BOOL CALLBACK AddOptionsToDialog(CDLG_Options* pData)
{
	CDLG_Options* pDialog=pData;
	if(!pDialog){
		return FALSE;
	}
	pwk->bCategsWereChanged=0;
	/*
	pDialog->m_OptionsList.m_crIItemText=GetSysColor(COLOR_WINDOWTEXT);
	pDialog->m_OptionsList.m_crIRowBackground=GetSysColor(COLOR_WINDOW);
	pDialog->m_OptionsList.SetBkColor(GetSysColor(COLOR_WINDOW));
	pDialog->m_OptionsList.m_crSelectedIRowBackground=GetSysColor(COLOR_HIGHLIGHT);
	pDialog->m_OptionsList.m_crSelectedIItemBackground=GetSysColor(COLOR_HIGHLIGHT);
	pDialog->m_OptionsList.m_crSelectedIItemText=GetSysColor(COLOR_HIGHLIGHTTEXT);
	*/
	pDialog->m_OptionsList.m_crIItemText=GetSysColor(COLOR_WINDOWTEXT);
	pDialog->m_OptionsList.m_crIRowBackground=GetSysColor(COLOR_BTNFACE);
	pDialog->m_OptionsList.SetBkColor(GetSysColor(COLOR_BTNFACE));
	pDialog->m_OptionsList.m_crSelectedIRowBackground=GetSysColor(COLOR_HIGHLIGHT);
	pDialog->m_OptionsList.m_crSelectedIItemBackground=GetSysColor(COLOR_HIGHLIGHT);
	pDialog->m_OptionsList.m_crSelectedIItemText=GetSysColor(COLOR_HIGHLIGHTTEXT);
	pDialog->m_LList.m_crSelectedIRowBackground=GetSysColor(COLOR_BTNFACE);
	pDialog->m_LList.m_crSelectedIItemBackground=GetSysColor(COLOR_BTNFACE);
	pDialog->bManualPreventCommit=0;
	int i=0;
	iQAppCount=0;
	iQMacCount=0;
	HIROW hHotkeys=0,hSplash=0;
	HIROW hQRun_Topic=NULL;
	HIROW hQMac_Topic=NULL;
	HIROW hCommon_Topic=NULL;
	/*if(psk->lEnableAddDesktops){
		if(sDesktopWarning.GetLength()==0){
			sDesktopWarning=" (";
			sDesktopWarning+=_l("On default desktop only")+")";
		}
	}*/
	// Чтобя перечитать все с новым языком
	CStringArray aLangBufferNames;
	int iTempLang=pwk->vGetApplicationLang();
	pwk->GetBuffersLang(aLangBufferNames,iTempLang);
	// Авто-запуск...
	psk->bStartWithWindows=IsStartupWithWindows();
	// Категории пунктов быстрого запуска...
	pwk->g_aListOfCategs.RemoveAll();
	g_ListOfCategs=_l("Common");
	pwk->g_aListOfCategs.Add("");//No category
	CSmartLock ql(psk->aQRunLock,TRUE,-1);
	// 1. Заполняем массив с категориями
	for(i=0;i<psk->aQuickRunApps.GetSize();i++){
		if(psk->aQuickRunApps[i] && psk->aQuickRunApps[i]->lMacros==CATEG_MACR){
			g_ListOfCategs+="\t";
			g_ListOfCategs+=psk->aQuickRunApps[i]->sItemName;
			pwk->g_aListOfCategs.Add(psk->aQuickRunApps[i]->sUniqueID);
		}
	}
	if(pwk->g_aListOfCategs.GetSize()>0){
		// 2. Обновляем данные о номере категории в массиве
		for(int i=0;i<psk->aQuickRunApps.GetSize();i++){
			if(psk->aQuickRunApps[i] && psk->aQuickRunApps[i]->lMacros<CATEG_MACR){
				psk->aQuickRunApps[i]->lTempCategNumber=0;
				int iCategNumber=pwk->FindQRunItemByID(psk->aQuickRunApps[i]->sCategoryID,&pwk->g_aListOfCategs);
				if(iCategNumber>=0){
					psk->aQuickRunApps[i]->lTempCategNumber=iCategNumber;
				}
			}
		}
	}
	CString sDoubleClickingTrayActions=_l("Do nothing");
	{// 3. Список действий
		for(i=0;i<=LAST_ACTION_CODE;i++){
			sDoubleClickingTrayActions+="\t";
			sDoubleClickingTrayActions+=pwk->GetBasicHotkeyDsc(i);
		}
	}
#ifndef GPL
	// Пункты дополнительного меню
	if(psk->iLikStatus<2){
		pDialog->Option_AddMenuAction(_l("Order "PROGNAME" now")+"!", BuyWK, 0, 0);
	}
#endif
	pDialog->Option_AddMenuAction(_l("Download more plugins"), OpenPlugins, 0, 0);
	pDialog->Option_AddMenuAction(_l("Download more macros"), OpenMacros, 0, 0);
	////////////////////////
	// Задаем список опций
	////////////////////////
	if(pData->iTab==0){
		{
			HIROW hKeybOpts=NULL;
			CString sWhereToShow=_l("Tray+Hot menu")+"\t"+_l("Hot menu")+"\t"+_l("Tray")+"\t"+_l("Nowhere")+"\t"+_l("Disable even in preferences");
			HIROW hCommon=pDialog->Option_AddHeader(FL_ROOT, _l(PROGNAME" options")+"\t"+_l("Interface, menus and some common stuff"),82);
			hCommon_Topic=hCommon;
			{
				HIROW hBasicOpts=pDialog->Option_AddHeader(hCommon,	_l("Main parameters")+"\t"+_l("Interface, menus and some common stuff"),80);
				pDialog->Option_AddBOOL(hBasicOpts,_l("Start with windows"),&psk->bStartWithWindows, 1, 0, OC_STARTUP);
				psk->lTempLang=pwk->vGetApplicationLang();
				pDialog->Option_AddCombo(hBasicOpts,_l("User interface language"),&psk->lTempLang,pwk->vGetApplicationLang(),pwk->sLangsNames,OC_LANG);
				pDialog->Option_AddHotKey(hBasicOpts,_l("Show preferences dialog"),&psk->DefActionMapHK[SHOW_OPTIONS],pwk->getDefActionMapHK_Preinit()[SHOW_OPTIONS]);
				pDialog->Option_AddHotKey(hBasicOpts,_l("Popup hot menu")+sDesktopWarning+"\t"+_l("Hotkey access to active window transparency and a lot of other useful stuff"),&psk->DefActionMapHK[HOT_MENU],pwk->getDefActionMapHK_Preinit()[HOT_MENU]);
				{// Visibility
					HIROW hWhere=pDialog->Option_AddTitle(hBasicOpts, _l("Menus and preferences folders")+"\t"+_l("When and where show different submenus and subfolders","When and where show different submenus and subfolders. Here you can quickly remove rarely used folders from tray and preferences"),61);
					pDialog->Option_AddCombo(hBasicOpts,_l("'Miscellaneous' folder"),&psk->bMiscSaverMenu, DEFAULT_SCRSUBMENU, sWhereToShow);
					pDialog->Option_AddCombo(hBasicOpts,_l("'Quick run' folder"),&psk->bQRunMenu, DEFAULT_QRUNSUBMENU, sWhereToShow);
					pDialog->Option_AddCombo(hBasicOpts,_l("'Macros' folder"),&psk->bMacroMenu, DEFAULT_MACRSUBMENU, sWhereToShow);
					pDialog->Option_AddCombo(hBasicOpts,_l("'Volume' folder"),&psk->bVolumeMenu, DEFAULT_VOLUMEMENU, sWhereToShow);
					pDialog->Option_AddCombo(hBasicOpts,_l("'Processes' folder")+"\t"+_l("Note: construction of processes list can take some time"),&psk->bSysActionsInHotMenu, DEFAULT_SYSSUBMENU, sWhereToShow);
					pDialog->Option_AddCombo(hBasicOpts,_l("Menu to show on 'Hot menu' hotkey double-press"),&psk->bMenuOnDblHotMenu, 0, _l("Quick-run items")+"\t"+_l("Macros")+"\t"+_l("Desktop related")+"\t"+_l("Miscellaneous"));
					pDialog->Option_AddBOOL(hBasicOpts,_l("Include window`s system menu in the hot menu")+"\t"+_l("Caution: This feature may conflict with some applications"),&psk->bRetrieveSysMenu, 0);
					pDialog->Option_AddBOOL(hBasicOpts,_l("Show 'Application shortcut' item in the 'hot' menu"),&psk->bAppShortcInHotMenu,1);
				}
				{// Config backup
					HIROW hBackup=pDialog->Option_AddTitle(hBasicOpts,_l("Configuration protection and backup"),61);
					pDialog->Option_AddString(hBasicOpts,_l("Protect preferences with code word")+"\t"+_l("Fill this field if you want WireKeys to ask for right code before opening preferences"),&psk->sCodeWord);
					HIROW hBackupFile=pDialog->Option_AddString(hBasicOpts,_l("Backup file"),&psk->sBackupFile,psk->sBackupFile,OC_BACKUP);
					pDialog->Option_AddActionToEdit(hBackupFile,ChooseAnyFile, hBackupFile, 0, 94);
					pDialog->Option_AddAction(hBasicOpts,_l("Backup config to file"), BackupToFile, hBackupFile);
					pDialog->Option_AddAction(hBasicOpts,_l("Restore config from file"),BackupFromFile, hBackupFile);
					pDialog->Option_AddMenuAction(_l("Export to file"), BackupToFile, 0, 0);
					pDialog->Option_AddMenuAction(_l("Import from file"), BackupFromFile, 0, 0);

				}
			}
			{// Tray icon
				HIROW hTrayico=pDialog->Option_AddHeader(hCommon, _l("Tray icon"),102);
				// Назначение - Иконка из файла???
				pDialog->Option_AddBOOL(hTrayico,_l("Show icon in tray")+"\t"+_l("Clear checkbox to enable invisible mode"),&psk->bShowIconInTray, 1, 0, OC_SHOWICON);
				CString sLngPossible="";
				if(pwk->isPluginLoaded("WP_KeyMaster")){
					sLngPossible="\t"+_l("Language indicator");
				}
				pDialog->Option_AddCombo(hTrayico,_l("Text over tray icon"),&psk->lShowDateInTrayIcon, 0, _l("None")+"\t"+_l("Current date")+"\t"+_l("Current day of week")+sLngPossible, OC_SHOWICON);
				pDialog->Option_AddCombo(hTrayico,_l("Left mouse button: Action to perform on double clicking tray icon"),&psk->lLBTray,35,sDoubleClickingTrayActions,OC_ICONTYPE);
				pDialog->Option_AddCombo(hTrayico,_l("Middle mouse button: Action to perform on double clicking tray icon"),&psk->lMBTray,16,sDoubleClickingTrayActions,OC_ICONTYPE);
				pDialog->Option_AddCombo(hTrayico,_l("Right mouse button: Action to perform on double clicking tray icon"),&psk->lRBTray,45,sDoubleClickingTrayActions,OC_ICONTYPE);
				pDialog->Option_AddBOOL(hTrayico,_l("Show additional icon when playing macro"),&psk->lShowMacroIcon, 1, 0);
				HIROW hIconFile=pDialog->Option_AddString(hTrayico,_l("Use my icon for tray icon")+"\t"+_l("You can leave this field empty to use default icon"),&psk->sUDIcon,psk->sUDIcon,OC_SHOWICON);
				pDialog->Option_AddActionToEdit(hIconFile,ChooseIcoFile, hIconFile, 0, 94);
				pDialog->Option_AddAction(hTrayico,_l("Related")+": "+_l("Use tray icon as resource indicator"),MoveToResIndic, NULL);
			}
			BOOL bExtended=0;
			{// Hotkeys
				CPlugin* plg=0;
				int iPlgIndex=0;
				hHotkeys=pDialog->Option_AddHeader(hCommon, _l("Hotkeys")+"\t"+_l("Hotkeys-related options"), 55);
				if(psk->plugins.GetSize()>0){
					for(int i=0;i<psk->plugins.GetSize();i++){
						plg=psk->plugins[i];
						if(plg && plg->sFileName.CompareNoCase("WP_KeyMaster")==0){
							iPlgIndex=i;
							break;
						}
					}
				}
				if(plg){
					bExtended=1;
					if(AddPluginHotkeys(_l("Enable extended hotkey processing")+"\t"+_l("Allows one to use mouse buttons, multimedia keys, left- and right- key modifiers (ctrl/alt/shift/win) in hotkeys"),_l("Extended hotkeys processing"),iPlgIndex,hHotkeys,pDialog,plg,TRUE)){
						pDialog->Option_AddBOOL(hHotkeys,_l("Disable 'Double-press' extensions")+"\t"+_l("'Double-press' hotkey is not triggered","'Double-press' hotkey is not triggered just after press but after you release corresponding Ctrl/Alt/Shift/Win key. This option disables such behaviour along with additional functionality"), &psk->lDisableDblPressExtensions, 0);
						//pDialog->Option_AddBOOL(hHotkeys,_l("Target mouse-enabled hotkeys to pointed window")+"\t"+_l("Direct actions from hotkeys with mouse clicks to window under cursor, not foreground window"),&psk->lDirectMouseToUnderC, 0);
						pDialog->Option_AddAction(hHotkeys,_l("Keyboard related options"), MovePaneFocusP, (HIROW)&hLSwitch);
					}
				}else{
					pDialog->Option_AddTitle(hHotkeys,_l("'Extended hotkeys processing' can be used in registered version only")+"!",86);
				}
			}
			{//WP_Schedule
				CPlugin* plg=0;
				int iPlgIndex=0;
				HIROW hSchedule=pDialog->Option_AddHeader(hCommon, _l("Scheduler")+"\t"+_l("Scheduler options"), 34);
				if(psk->plugins.GetSize()>0){
					for(int i=0;i<psk->plugins.GetSize();i++){
						plg=psk->plugins[i];
						if(plg && plg->sFileName.CompareNoCase("WP_Schedule")==0){
							iPlgIndex=i;
							break;
						}
					}
				}
				if(plg){
					AddPluginHotkeys(_l("Enable scheduling")+"\t"+_l("Allows to activate actions on schedule"),_l("Scheduler"),iPlgIndex,hSchedule,pDialog,plg,TRUE);
				}else{
					pDialog->Option_AddTitle(hSchedule,_l("'Scheduler' can be used in registered version only")+"!",86);
				}
			}
			{
				hSplash=pDialog->Option_AddHeader(hCommon, _l("Splash string")+" ("+_l("OSD")+")\t"+_l("On-Screen-Display")+": "+_l("Splash string size, position and colors"),96);
				pDialog->Option_AddCombo(hSplash,_l("Splash string position")+"\t"+_l("Where to show splash string on quick run and volume control"),&psk->lStartupMsgType,DEF_STARTUPMSGT,_l("Do not show at all")+"\t"+_l("Left-Top")+"\t"+_l("Right-Top")+"\t"+_l("Right-Bottom")+"\t"+_l("Left-Bottom")+"\t"+_l("Center")+"\t"+_l("Top-Center")+"\t"+_l("Bottom-Center"));
				pDialog->Option_AddNString(hSplash,_l("Maximum splash string length (chars)"),&psk->lOSDChars, DEF_OSDCHARS, 1, 300);
				pDialog->Option_AddNString(hSplash,_l("Show splash for at least ... secs"),&psk->lOSDSecs, DEF_OSDSECS, 1, 300);
				pDialog->Option_AddBOOL(hSplash,_l("Show splash for WinAmp controls"),&psk->lWinAmpNeedOSD,1);
				pDialog->Option_AddBOOL(hSplash,_l("Show splash for Volume controls"),&psk->lVolumeNeedOSD,1);
				pDialog->Option_AddBOOL(hSplash,_l("Show splash for Quick run application"),&psk->lShowSplashForQuickRun, 1);
				pDialog->Option_AddBOOL(hSplash,_l("Show splash for Desktop switches"),&psk->lDeskNeedOSD,1);
				pDialog->Option_AddBOOL(hSplash,_l("Show splash for Memory optimization"),&psk->lDeskMemOptOSD,1);
				pDialog->Option_AddFont(hSplash,_l("Splash string text font"),&psk->pSplashFont,0,98);
				pDialog->Option_AddColor(hSplash,_l("Splash string text color"),&psk->lSplashTextColor,DEF_SPLASHTEXTCOL,OC_SPLASHC, 99);
				pDialog->Option_AddColor(hSplash,_l("Splash string background color"),&psk->lSplashBgColor,DEF_SPLASHBGCOL,OC_SPLASHC, 99);
				pDialog->Option_AddCombo(hSplash,_l("Border around OSD string"),&psk->lOSDBorder,1, _l("None")+"\t"+_l("Bold glow")+"\t"+_l("Light glow")+"\t"+_l("Bold shadow")+"\t"+_l("Light shadow"));
				//pDialog->Option_AddBOOL(hSplash,_l("Disable OSD"),&psk->lNoNeedOSD,0);
				if(!psk->bUnderWindows98){
					pDialog->Option_AddBOOL(hSplash,_l("Enable OSD fade"),&psk->lDeskfadeOSD,1);
				}
			}
			{
				HIROW hMisc=pDialog->Option_AddHeader(hCommon,	_l("Additional options"),95);
				HIROW hUpdates=pDialog->Option_AddBOOL(hMisc,_l("Check for updates every ... day(s)"),&psk->lCheckForUpdate, 1);
				pDialog->Option_AddNString(hUpdates,"",&psk->lDaysBetweenChecks, 20, 0, 300);
				pDialog->Option_AddNString(hMisc,_l("Distance to stick windows together (pix)"),&psk->iStickPix, 10, 0, 30);
				pDialog->Option_AddBOOL(hMisc,_l("Do not show help items in menus"), &psk->lDisableSCHelp, 0);
				pDialog->Option_AddBOOL(hMisc,_l("Do not show warning about hotkeys conflicts"), &psk->bSkipHKWarning, 0);
				pDialog->Option_AddBOOL(hMisc,_l("Do not move focus for popup menus")+"\t"+_l("Focus moving may conflict with","Focus moving may conflict with some other utilities, check this box if you have any troubles with additional third-party tools"), &psk->lDisableFocusMove, 0);
				pDialog->Option_AddBOOL(hMisc,_l("Disable notifications via cursor shape"), &psk->lNoNotifyByCursor, 1);
				//pDialog->Option_AddBOOL(hMisc,_l("Place alerts on taskbar instead of tray"), &psk->lAlertsOnTaskbar, 1);
				pDialog->Option_AddBOOL(hMisc,_l("Allow "PROGNAME" to work in high priority mode")+"\t"+_l("Effective on restart only"), &psk->bAllowHighPriority, 0);
				/*if(IsOtherProgrammAlreadyRunning(PARENT_PROG)){
					pDialog->Option_AddBOOL(hMisc,_l("WireNote integration")+"\t"+_l(PROGNAME" will be accessible from WireNote menus"),&psk->bIntegrateWithWN, 0);
				}*/
				pDialog->Option_AddString(hMisc,_l("File names: replace wrong characters with this one")+"\t"+_l("When program needs to save data under auto-generated file name, all wrong (for file name) characters will be replaced with this one"),&psk->sSafeChar, "_");
				pDialog->Option_AddBOOL(hMisc,_l("Use the most compatible way to start lnk files"),&psk->lUseOldStartStyle, 1);
				AddPluginsOption(hMisc,pDialog,OL_ADDOPTIONS);
			}
			AddPluginsOption(hCommon,pDialog,OL_COMMONOPT);
		}
	}
	//==================================================================
	//==================================================================
	//==================================================================
	pwk->bHotkeyChangedMode=0;
	g_aCategRowsMap.RemoveAll();
	if(pData->iTab==1){
		if(psk->bQRunMenu!=4 || psk->bMacroMenu!=4 || pwk->g_INFOWNDHK_TOPIC_InfoWnd!=NULL){
			g_aCategsL2R.RemoveAll();
			g_aCategsL2R_targs.RemoveAll();
			HIROW hQRunRoot=FL_ROOT;//pDialog->Option_AddHeader(FL_ROOT,DEFQRUN_TOPIC,65);
			{// Categories
				hQCateg=pDialog->Option_AddHeader(hQRunRoot,_l("Launch Sets"),97);
				pDialog->Option_AddTitle(hQCateg,_l("Launch sets can be used to"),86);
				pDialog->Option_AddTitle(hQCateg,_l("- Show popup menu with all applications from the set (press Launch set hotkey)"),86);
				pDialog->Option_AddTitle(hQCateg,_l("- Start all applications from set in one click (Double-press Launch set hotkey)"),86);
				pDialog->Option_AddTitle(hQCateg,_l("- Put applications, etc in categories for better navigation (Check preferences: every application or macro will be moved to its Launch set subfolder)"),86);
				pDialog->Option_AddBOOL(hQCateg,_l("Start all applications from the set if double-press is detected"),&psk->lAllowDblCategStart, 1);

				pwk->hQRunTypeCCRight=hQCateg;
				pwk->hQRunType3=pDialog->GetAliasHIROW(hQCateg);
				pDialog->Option_AddMenuAction(_l("Add new Launch Set"), AddQRunCateg, hQCateg, hQCateg);
				pDialog->Option_AddAction(hQCateg,_l("Add new Launch Set"), AddQRunCateg, hQCateg);
				CSmartLock ql(psk->aQRunLock,TRUE,-1);		
				for(i=0;i<psk->aQuickRunApps.GetSize();i++){
					if(psk->aQuickRunApps[i]){
						AddQRunEx(hQCateg,pDialog,psk->aQuickRunApps[i],CATEG_MACR,FALSE);
					}
				}
			}
			HIROW hActionsRow=pDialog->Option_AddHeader(hQRunRoot,DEFAPPLACTIONS_TOPIC,61);
			if(psk->bQRunMenu!=4){// Quick run
				hQRun_Topic=hQRunRoot;//pDialog->Option_AddHeader(hQRunRoot,_l(DEFQRUN_TOPIC)+"\t"+_l("Quick run list"),65);
				pwk->hQRunType0Right=hQRun_Topic;
				pwk->hQRunType0=pDialog->GetAliasHIROW(hQRun_Topic);
				pDialog->Option_AddTitle(hActionsRow,_l("Quick-run items"),61);
				pDialog->Option_AddAction(hActionsRow,_l("Add new File item"), AddQRunFile, hQRun_Topic,0,32);
				pDialog->Option_AddAction(hActionsRow,_l("Add new Application item"), AddQRunApp, hQRun_Topic,0,106);
				pDialog->Option_AddAction(hActionsRow,_l("Add new Internet address"), AddQRunLink, hQRun_Topic,0,22);
				pDialog->Option_AddAction(hActionsRow,_l("Add new Folder item"), AddQRunFold, hQRun_Topic);
				pDialog->Option_AddAction(hActionsRow,_l("Add new empty item")+" ("+_l("You will need to set file path manually")+")", AddQRunEmpty, hQRun_Topic);
				pDialog->Option_AddMenuAction(_l("Add new File item"), AddQRunFile, hQRun_Topic, hQRun_Topic);
				pDialog->Option_AddMenuAction(_l("Add new Application item"), AddQRunApp, hQRun_Topic, hQRun_Topic);
				pDialog->Option_AddMenuAction(_l("Add new Internet address"), AddQRunLink, hQRun_Topic, hQRun_Topic);
				pDialog->Option_AddMenuAction(_l("Add new Folder item"), AddQRunFold, hQRun_Topic, hQRun_Topic);
				pDialog->Option_AddMenuAction(_l("Add new empty item"), AddQRunEmpty, hQRun_Topic, hQRun_Topic);
				CSmartLock ql(psk->aQRunLock,TRUE,-1);		
				for(i=0;i<psk->aQuickRunApps.GetSize();i++){
					if(psk->aQuickRunApps[i]){
						AddQRunEx(hQRun_Topic,pDialog,psk->aQuickRunApps[i],0,FALSE);
					}
				}
				AddPluginsOption(hQRun_Topic,pDialog,OL_QUICKRUNS);
			}
			if(psk->bMacroMenu!=4){// Macros
				hQMac_Topic=hQRunRoot;//pDialog->Option_AddHeader(hQRunRoot,_l(DEFQMAC_TOPIC)+"\t"+_l("List of keyboard and application macros"),70);
				pwk->hQRunType2Right=hQMac_Topic;
				pwk->hQRunType2=pDialog->GetAliasHIROW(hQMac_Topic);
				pDialog->Option_AddTitle(hActionsRow,_l("Text macros"),61);
				pDialog->Option_AddAction(hActionsRow,_l("Add new text macro"), AddQRun3, hQMac_Topic,0,70);
				pDialog->Option_AddAction(hActionsRow,_l("Add new insertion macro"), AddQRun4, hQMac_Topic,0,19);
				pDialog->Option_AddAction(hActionsRow,_l("Add new application macro"), AddQRun2, hQMac_Topic);
				pDialog->Option_AddAction(hActionsRow,_l("Add macro from file"), ShowQRun3Folder, hQMac_Topic);
				pDialog->Option_AddMenuAction(_l("Add new text macro"), AddQRun3, hQMac_Topic, hQMac_Topic);
				pDialog->Option_AddMenuAction(_l("Add new insertion macro"), AddQRun4, hQMac_Topic, hQMac_Topic);
				pDialog->Option_AddMenuAction(_l("Add new application macro"), AddQRun2, hQMac_Topic, hQMac_Topic);
				pDialog->Option_AddMenuAction(_l("Add macro from file"), ShowQRun3Folder, hQMac_Topic, hQMac_Topic);
				CSmartLock ql(psk->aQRunLock,TRUE,-1);
				for(i=0;i<psk->aQuickRunApps.GetSize();i++){
					if(psk->aQuickRunApps[i]){
						AddQRunEx(hQMac_Topic,pDialog,psk->aQuickRunApps[i],1,FALSE);
					}
				}
				for(i=0;i<psk->aQuickRunApps.GetSize();i++){
					if(psk->aQuickRunApps[i]){
						AddQRunEx(hQMac_Topic,pDialog,psk->aQuickRunApps[i],2,FALSE);
					}
				}
				for(i=0;i<psk->aQuickRunApps.GetSize();i++){
					if(psk->aQuickRunApps[i]){
						AddQRunEx(hQMac_Topic,pDialog,psk->aQuickRunApps[i],3,FALSE);
					}
				}
				AddPluginsOption(hQMac_Topic,pDialog,OL_MACROS);
			}
			{//Categories aliases
				for(int i2=0;i2<g_aCategsL2R.GetSize();i2++){
					for(int i=0;i<psk->aQuickRunApps.GetSize();i++){
						if(psk->aQuickRunApps[i] && psk->aQuickRunApps[i]->lUniqueCount==g_aCategsL2R_targs[i2]){
							pDialog->SetLAliasTarget(g_aCategsL2R[i2], psk->aQuickRunApps[i]->hThisItemRoot);
							break;
						}
					}
				}
			}
			pDialog->Option_AddTitle(hActionsRow,_l("Additionally"),61);
			//pDialog->Option_AddAction(hActionsRow,_l("Download more plugins"), OpenPlugins, 0);
			pDialog->Option_AddAction(hActionsRow,_l("Download additional macros"), OpenMacros, 0);
			pDialog->Option_AddAction(hActionsRow,_l("Show Macro help page"), ShowQRunHelp, hQMac_Topic,0,21);
			//pDialog->Option_AddAction(hActionsRow,_l("Install default macros and quick-run items"), AddDefMacros_Warn, 0);
			if(pwk->g_INFOWNDHK_TOPIC_InfoWnd!=NULL){
				HIROW hFloaterHK=pDialog->Option_AddHeader(hQRunRoot,INFOWNDHK_TOPIC+"\t"+_l("Simple way to quickly activate commonly used floaters or hidden windows"),65);
				pDialog->Option_AddTitle(hFloaterHK,_l("This hotkey can quickly activate hidden window or floater"),86);
				pDialog->Option_AddHotKey(hFloaterHK,_l("Choose hotkey to activate")+" '"+pwk->g_INFOWNDHK_TOPIC_InfoWnd->Sets->sTitle+"'",&pwk->g_INFOWNDHK_TOPIC_Hk,pwk->g_INFOWNDHK_TOPIC_Hk);
			}
		}
	}
	//==================================================================
	//==================================================================
	//==================================================================
	if(pData->iTab==0){
		{// Additional text operations
			HIROW hTxCRoot=pDialog->Option_AddHeader(FL_ROOT, SELTXT_TOPIC+"\t"+_l("Additional operations over selected text")+", "+_l("Additional clipboards"),68);
			HIROW hTxC=pDialog->Option_AddHeader(hTxCRoot, _l("Main parameters")+"\t"+_l("Additional operations over selected text")+", "+_l("Additional clipboards"),80);
			if(psk->lDisableClipboard==0){
				pDialog->Option_AddTitle(hTxC,_l("Caution: These operations affect the clipboard state","Caution: These operations affect clipboard state. Everything except the last text snippet will be removed from clipboard"),61);
				pDialog->Option_AddHotKey(hTxC,_l("Change case of the selected text"),&psk->DefActionMapHK[CHANGE_CASE],pwk->getDefActionMapHK_Preinit()[CHANGE_CASE]);
				pDialog->Option_AddHotKey(hTxC,_l("Run as command string/Calculate")+"\t"+_l("Run/Calculate selected text. To be treated as expression, selected text must end with '=' character")+". "+_l("Double-press this key to popup history of runs"),&psk->DefActionMapHK[RUN_ASCOMMAND],pwk->getDefActionMapHK_Preinit()[RUN_ASCOMMAND],0,105);
				pDialog->Option_AddHotKey(hTxC,_l("Show history for standard clipboard"),&psk->DefClipMapHK_H[0],pwk->getDefClipMapHK_H_Preinit()[0]);
				pDialog->Option_AddHotKey(hTxC,_l("Show application-specific history for standard clipboard"),&psk->DefClipMapHK_HA[0],pwk->getDefClipMapHK_HA_Preinit()[0]);
				if(!psk->bAddClipboard){
					pDialog->Option_AddTitle(hTxC,_l("Warning: To work with clipboard histories, Additional clipboards must be enabled")+"!",86);
				}
				pDialog->Option_AddHotKey(hTxC,_l("Context menu for selected text"),&psk->DefActionMapHK[HOTSEL_MENU],pwk->getDefActionMapHK_Preinit()[HOTSEL_MENU]);
				pDialog->Option_AddAction(hTxC,_l("Edit smart keywords")+" ("+_l("Run as command string/Calculate")+")",EditSmartKeywords, NULL);

				if(psk->lHKLCount>1){
					pDialog->Option_AddTitle(hTxC,_l("Keyboard layout options"),86);
					HIROW hMoreHKL=pDialog->Option_AddHotKey(hTxC,_l("Convert selected text to another layout"),&psk->DefActionMapHK[CONVERT_SELECTED],pwk->getDefActionMapHK_Preinit()[CONVERT_SELECTED]);
					HIROW hMoreLW=pDialog->Option_AddHotKey(hTxC,_l("Convert last line to another layout"),&psk->DefActionMapHK[CONVERT_LASTWORD],pwk->getDefActionMapHK_Preinit()[CONVERT_LASTWORD]);
					pDialog->Option_AddBOOL(hMoreLW,_l("Convert only last word, not line in whole"),&psk->lLastWordNotLine, 1);
					if(psk->lHKLCount>2){
						pDialog->Option_AddCombo(hTxC,_l("Preffered layout")+" #1",&psk->lHKL1, 0, psk->sHKLList);
						pDialog->Option_AddCombo(hTxC,_l("Preffered layout")+" #2",&psk->lHKL2, 1, psk->sHKLList);
					}
					pDialog->Option_AddString(hTxC,_l("Replace following characters")+"\t"+_l("Each character from this list will be replaced with the character on the same position from list below"),&psk->sLayoutP1);
					pDialog->Option_AddString(hTxC,_l("With characters from the list")+"\t"+_l("Each character from list above will be replaced with the character on the same position from this list"),&psk->sLayoutP2);
				}
				pDialog->Option_AddTitle(hTxC,_l("Common options"),86);
				pDialog->Option_AddCombo(hTxC,_l("Selection method for text operations"),&psk->bGetTextSelectionStyle,psk->lDefaultSelMethod,_l("Ctrl-C/Ctrl-V")+"\t"+_l("Ctrl-Insert/Shift-Insert"));//+"\t"+_l("Compatible mode")-Deleted!
				//_l("Disable this option if you have troubles with some applications")
				//pDialog->Option_AddCombo(hTxC,_l("Level of integration with clipboard")+"\t"+_l("If clipboard-related actions are not working in some applications, try to change this option")+". "+_l("Fast inserts are really fast but may not work in some applications. Fast inserts affect all features that requre text inserting. Besides that, 'Fastest inserts' can destroy clipboard content"),&psk->lDefaultSelQuality,1,_l("Full level (slow but accurate)")+"\t"+_l("Use fast inserts")+"\t"+_l("Use fastest inserts"));//+"\t"+_l("Basic level")
				pDialog->Option_AddCombo(hTxC,_l("Histories menus: Tooltip with additional information")+"\t"+_l("Additional tooltip contains information about application where corresponding snippet was taken"),&psk->lShowHPMTooltip, 1, _l("Do not show tooltip")+"\t"+_l("Show as balloon tooltip"));
				pDialog->Option_AddBOOL(hTxC,_l("Leave clip in clipboard after 'Paste' action"),&psk->lLeaveInClipOnPaste, 0);
				HIROW hBmpPars=pDialog->Option_AddBOOL(hTxC,_l("Save bitmaps in history too"),&psk->lSaveBMPSinHist, 1);
				pDialog->Option_AddNString(hBmpPars,_l("Do not catch bitmaps larger than ... bytes"),&psk->lMaxBMPHistSizeCatch, MAX_BMPCLIP_SIZE,0,0x0FFFFFFF);
				pDialog->Option_AddNString(hBmpPars,_l("Do not save bitmaps larger than ... bytes"),&psk->lMaxBMPHistSize, MAX_BMPCLIP_SIZE_SAVE,0,0x0FFFFFFF);
				HIROW hSavePars=pDialog->Option_AddBOOL(hTxC,_l("Save Windows clipboard content between restarts"),&psk->lSaveClipContent, 1);
				pDialog->Option_AddBOOL(hSavePars,_l("Save clipboard content periodically")+"\t"+_l("This can prevent data loss in case of PC crash"),&psk->lSaveClipContentPer, 0);
				{//Additional text clipboards
					HIROW hClipsOpt=pDialog->Option_AddHeader(hTxCRoot,_l("Additional text clipboards")+"\t"+_l("Clipboards hotkeys, some other options"),88);
					pDialog->Option_AddBOOL(hClipsOpt,_l("Enable additional text clipboards"),&psk->bAddClipboard, 1);
					pDialog->Option_AddNString(hClipsOpt,_l("How many clips to handle (Effective on restart)"),&psk->lClipCountAfterNextStart, 3, 1, MAX_CLIPS);
					if(!psk->bAddClipboard){
						pDialog->Option_AddTitle(hClipsOpt,_l("Warning: To work with clipboard histories, Additional clipboards must be enabled")+"!",86);
					}

					//pDialog->Option_AddAlias(hClipsOpt,_l("Additional text clipboards"),5,hTxC);
					for(int i=1;i<=psk->lClipCount;i++){
						CString sClipTitle=_l("Clip")+Format(" #%i",i);
						pDialog->Option_AddTitle(hClipsOpt,sClipTitle,86);
						pDialog->Option_AddHotKey(hClipsOpt,_l("'Copy' hot key"),&psk->DefClipMapHK[i*2],pwk->getDefClipMapHK_Preinit()[i*2]);
						pDialog->Option_AddHotKey(hClipsOpt,_l("'Paste' hot key"),&psk->DefClipMapHK[i*2+1],pwk->getDefClipMapHK_Preinit()[i*2+1]);
						pDialog->Option_AddHotKey(hClipsOpt,_l("Show history")+"\t"+_l("Popup menu with recently used clipboard items"),&psk->DefClipMapHK_H[i],pwk->getDefClipMapHK_H_Preinit()[i]);
						pDialog->Option_AddHotKey(hClipsOpt,_l("Show application-specific history")+"\t"+_l("Popup menu with recently used clipboard items taken in the active application"),&psk->DefClipMapHK_HA[i],pwk->getDefClipMapHK_HA_Preinit()[i]);
					}
					pDialog->Option_AddTitle(hClipsOpt,_l("Other options"),86);
					pDialog->Option_AddHotKey(hClipsOpt,_l("Clear clipboard histories"),&psk->DefActionMapHK[CLEARHISTORIES],pwk->getDefActionMapHK_Preinit()[CLEARHISTORIES]);
					pDialog->Option_AddNString(hClipsOpt,_l("History")+": "+_l("Maximum number of items"),&psk->lClipHistoryDepth, DEF_CLIPHIST, 1, 999);
#ifndef WK_DEPRECATED
					HIROW hTransmit=pDialog->Option_AddBOOL(hClipsOpt,_l("Enable transmit copy mode")+"\t"+_l("Special mode, when any copy","Special mode, when any copy will be automatically pasted in the selected window"),&psk->lEnableTransmitMode, 1, 5);
					pDialog->Option_AddBOOL(hTransmit,_l("Append string to every copy"),&psk->lAppendTextToTransmits, 1);
					pDialog->Option_AddString(hTransmit,_l("Append string (patterns permitted)"),&psk->sTransmitAppendText, DEF_TRANSMITAPP);
					HIROW hRegexps=pDialog->Option_AddBOOL(hClipsOpt,_l("Use regexp to filter paste operations")+"\t"+_l("All pasted strings will be processed by regular expression first"),&psk->bUseRegexps, 0, 5);
					pDialog->Option_AddString(hRegexps,_l("Regexp for search")+"\t"+_l("Enter JScript regexp, use '\\' to escape special chars"),&psk->sRegexpWhat, "", OC_CLREGEXP);
					pDialog->Option_AddString(hRegexps,_l("String to replace matches")+"\t"+_l("All suitable chars will be replaced with this string"),&psk->sRegexpTo, "", OC_CLREGEXP);
#endif
					HIROW hUseMaxLimit=pDialog->Option_AddBOOL(hClipsOpt,_l("Max clip item size")+", Kb",&psk->lUseMaxClipSizeLimit, 1, 0);
					pDialog->Option_AddNString(hUseMaxLimit,"",&psk->lMaxClipSizeLimit,DEF_MAXCLIPSIZE,1,100);
					pDialog->Option_AddAction(hClipsOpt,_l("Clear clipboards"),ClearAllClips, NULL);
					AddPluginsOption(hClipsOpt,pDialog,OL_ADDCLIPS);
				}
				AddPluginsOption(hTxC,pDialog,OL_SELTEXTOPERATIONS);
			}
			pDialog->Option_AddAction(hTxC,_l("Reattach WireKeys to clipboard"),ReattachClip, 0);
			//pDialog->Option_AddBOOL(hTxC,_l("Change tray icon when working with clipboards"),&psk->lChTrayOnClips, 0);
			pDialog->Option_AddBOOL(hTxC,_l("Do not interfere with clipboard, disable all clipboard-related features")+"\t"+_l("If you change this option, you should restart "PROGNAME" in order to get effect from it"),&psk->lDisableClipboard, 0);
		}
		{// Desktops/icops and so
			HIROW hScreens=pDialog->Option_AddHeader(FL_ROOT,_l("Desktop, icons and windows")+"\t"+_l("Screenshots, additional desktops, desktop icons and more"),81);
			HIROW hWinControl=0;
			{// Window controls
				hWinControl=pDialog->Option_AddHeader(hScreens,WINCONT_TOPIC+"\t"+_l("Options about active window - hotkeys, boost and more"),78);
				pDialog->Option_AddHotKey(hWinControl,_l("Move active window into view"),&psk->DefActionMapHK[BRING_INTOVIEW],pwk->getDefActionMapHK_Preinit()[BRING_INTOVIEW]);
				pDialog->Option_AddHotKey(hWinControl,_l("Switch on-top state of active application"),&psk->DefActionMapHK[SWITCH_ONTOP],pwk->getDefActionMapHK_Preinit()[SWITCH_ONTOP]);
				pDialog->Option_AddHotKey(hWinControl,_l("Restore active window to previous size and position")+"/"+_l("Maximize active window"),&psk->DefActionMapHK[WINDOW_RESTORE],pwk->getDefActionMapHK_Preinit()[WINDOW_RESTORE]);
				pDialog->Option_AddHotKey(hWinControl,_l("Minimize active window")+(!psk->lDisableDblPressExtensions?(CString("/")+_l("Restore last minimized on double-press")):""),&psk->DefActionMapHK[WINDOW_MINIMIZE],pwk->getDefActionMapHK_Preinit()[WINDOW_MINIMIZE],0,105);
				pDialog->Option_AddHotKey(hWinControl,_l("Rollup/Unroll window to title"),&psk->DefActionMapHK[ROLLUP_TOTITLE],pwk->getDefActionMapHK_Preinit()[ROLLUP_TOTITLE]);
				pDialog->Option_AddBOOL(hWinControl,_l("Rollup")+": "+_l("Preserve window`s main menu")+"\t"+_l("Window can be rolled-up to title or main menu. Second is preffered because you will be able to work with application`s menu items"),&psk->bPreserveTitle, 1);
				pDialog->Option_AddHotKey(hWinControl,_l("Make totally transparent"),&psk->DefActionMapHK[WINDOW_TRANS],pwk->getDefActionMapHK_Preinit()[WINDOW_TRANS]);
				pDialog->Option_AddHotKey(hWinControl,_l("Close active application")+(!psk->lDisableDblPressExtensions?(CString(" / ")+_l("Double-press to")+" "+_l("Terminate active application")):""),&psk->DefActionMapHK[KILL_ACTWND],pwk->getDefActionMapHK_Preinit()[KILL_ACTWND],0,105);
				{// Scrolling
					HIROW hScroll=pDialog->Option_AddTitle(hWinControl,_l("Active window`s scrolling"));
					pDialog->Option_AddHotKey(hScroll,_l("Scroll up"),&psk->DefActionMapHK[SCROLL_UP],pwk->getDefActionMapHK_Preinit()[SCROLL_UP]);
					pDialog->Option_AddHotKey(hScroll,_l("Scroll down"),&psk->DefActionMapHK[SCROLL_DOWN],pwk->getDefActionMapHK_Preinit()[SCROLL_DOWN]);
					pDialog->Option_AddHotKey(hScroll,_l("Scroll left"),&psk->DefActionMapHK[SCROLL_LEFT],pwk->getDefActionMapHK_Preinit()[SCROLL_LEFT]);
					pDialog->Option_AddHotKey(hScroll,_l("Scroll right"),&psk->DefActionMapHK[SCROLL_RIGHT],pwk->getDefActionMapHK_Preinit()[SCROLL_RIGHT]);
					pDialog->Option_AddHotKey(hScroll,_l("Scroll one page up"),&psk->DefActionMapHK[SCROLL_PAGEUP],pwk->getDefActionMapHK_Preinit()[SCROLL_PAGEUP]);
					pDialog->Option_AddHotKey(hScroll,_l("Scroll one page down"),&psk->DefActionMapHK[SCROLL_PAGEDN],pwk->getDefActionMapHK_Preinit()[SCROLL_PAGEDN]);
					pDialog->Option_AddNString(hScroll,_l("Number of lines to scroll"),&psk->lNumberOfLinesToScroll,1,1,100);
				}
				//pDialog->Option_AddAction2(hWinControl,_l("More options: Keep all windows in-screen, Auto-minimize windows to tray, etc"),MovePaneFocusP, &hWHelperPlugin);
				AddPluginsOption(hWinControl,pDialog,OL_ACTIVEWND);
			}
			HIROW hHidApps=0;
			{// Hide to tray
				hHidApps=pDialog->Option_AddHeader(hScreens,_l("Hidden applications")+"\t"+_l("Hidden applications options"),74);
				pDialog->Option_AddTitle(hHidApps,_l("Hidden application will be reachable through "PROGNAME"`s tray menu"),86);
				pDialog->Option_AddHotKey(hHidApps,_l("Hide active application to tray")+(!psk->lDisableDblPressExtensions?(CString(" / ")+_l("Unhide on double-press")):""),&psk->DefActionMapHK[HIDE_TOTRAY],pwk->getDefActionMapHK_Preinit()[HIDE_TOTRAY],0,105);
				pDialog->Option_AddHotKey(hHidApps,_l("Hide active application completely")+(!psk->lDisableDblPressExtensions?(CString(" / ")+_l("Unhide on double-press")):""),&psk->DefActionMapHK[HIDE_COMPLIT],pwk->getDefActionMapHK_Preinit()[HIDE_COMPLIT],0,105);
				pDialog->Option_AddHotKey(hHidApps,_l("Hide active application to floater")+(!psk->lDisableDblPressExtensions?(CString(" / ")+_l("Unhide on double-press")):""),&psk->DefActionMapHK[HIDE_TOFLOAT],pwk->getDefActionMapHK_Preinit()[HIDE_TOFLOAT],0,105);
				pDialog->Option_AddHotKey(hHidApps,_l("Unhide all hidden applications"),&psk->DefActionMapHK[LAST_HIDDEN],pwk->getDefActionMapHK_Preinit()[LAST_HIDDEN]);
				pDialog->Option_AddNString(hHidApps,_l("Recently used menu items","Recently used menu items to show (0 to disable)")+"\t"+_l("When you choose item from hidden application tray submenu","When you choose item from hidden application tray submenu, it will be showed at root menu level next time"),&psk->lHTrayRecentItems,10,0,100);

				pDialog->Option_AddBOOL(hHidApps,_l("Animate window when hiding"),&psk->bAnimateFloaters, 1);
				pDialog->Option_AddBOOL(hHidApps,_l("Overlay tray icon with marker"),&psk->bOverlayTrayAppIcon, 0);
				pDialog->Option_AddBOOL(hHidApps,_l("Free unused memory when hiding windows"),&psk->bFreeMemOfHidApp, 0);
				pDialog->Option_AddBOOL(hHidApps,_l("Show ownerdraw menus too")+"\t"+_l("Additional application`s menu support, may crash "PROGNAME" on some non-standart menus"),&psk->bSupportOwnerDraw, SUPPORT_OWONDRDEF);
				HIROW hFloat=pDialog->Option_AddTitle(hHidApps,_l("Floater`s settings")+"\t"+_l("Options of the items, detached from tray or tray menu into separate icon"));
				pDialog->Option_AddNString(hFloat,_l("Size (pixels)"),&psk->sDetachedSize,30,0,300);
				pDialog->Option_AddColor(hFloat,_l("Background color"),&psk->lFloaterBG,BGCOLOR,0,99);
				if(!psk->bUnderWindows98){
					pDialog->Option_AddNString(hFloat,_l("Opacity (percents)"),&psk->lFloaterTransp,70,10,100);
				}
				pDialog->Option_AddBOOL(hFloat,_l("Use single click for opening floater"),&psk->lFloatUseSingleClick, 1);
				pDialog->Option_AddBOOL(hFloat,_l("Open directory content menu on left-clicking directory floater"),&psk->lFloatUseLBDir, 1);
				pDialog->Option_AddBOOL(hFloat,_l("Identify floaters via corresponding application`s command line"),&psk->bTakeComLineForFloater, 0);
				pDialog->Option_AddCombo(hHidApps,_l("Show application menu in tray")+"\t"+_l("You will be able to choose application`s menu items from tray icon menu, may not work with some applications"),&psk->bHidedAppShowMenu, DEFAULT_SHOWHMENU, _l("Do not show menu")+"\t"+_l("Show main menu")+"\t"+_l("Show system menu"));
				AddPluginsOption(hHidApps,pDialog,OL_HIDDENAPPS);
			}
			HIROW hShot=0;
			{// Screenshot
				hShot=pDialog->Option_AddHeader(hScreens,_l(SCRSHOT_TOPIC)+"\t"+_l("Screenshots")+": "+_l("Screenshot file, parameters, sign and more"),72);
				CString sScrshotActions=_l("Whole screen")+"\t"+_l("Active window")+((!psk->bUnderWindows98)?(CString("\t")+_l("User rectangle")):CString(""));
				pDialog->Option_AddHotKey(hShot,_l("Make screenshot"),&psk->DefActionMapHK[TAKE_SCREENSHOT],pwk->getDefActionMapHK_Preinit()[TAKE_SCREENSHOT],0,105);
				pDialog->Option_AddCombo(hShot,_l("Screenshot target")+" "+sDesktopWarning,&psk->bShotActiveWindow, 0, sScrshotActions);
				if(!psk->lDisableDblPressExtensions){
					pDialog->Option_AddCombo(hShot,_l("Screenshot target for double-press")+" "+sDesktopWarning+"\t"+_l("For example you are using 'Win-Q' as hotkey","For example you are using 'Win-Q' as hotkey. This option allows you to set another behaviour for 'Win-Q-Q' key"),&psk->bShotActiveWindow2, ((psk->bUnderWindowsNT)?2:1), sScrshotActions);
				}
				pDialog->Option_AddCombo(hShot,_l("Confirmation type"),&psk->bBlackAndWhiteConfirm, 1, _l("None")+"\t"+_l("With invert effect")+"\t"+_l("With alert window"));
				HIROW hUseJpg=pDialog->Option_AddBOOL(hShot,_l("Save screenshot in jpeg format")+", "+_l("quality"),&psk->bUseJpg, 0, 0);
				pDialog->Option_AddNString(hUseJpg,"",&psk->bUseJpgQuality,DEF_JPGQUALITY,1,100);
				pDialog->Option_AddBOOL(hShot,_l("Put screenshot in clipboard"),&psk->bPutScreshotInClipboard, 1, 0);
				{
					HIROW hShotSave=pDialog->Option_AddTitle(hShot,_l("Main parameters")+"\t"+_l("File name, directory and screenshot format"));
					pDialog->Option_AddString(hShotSave,_l("Screenshot file name (patterns permitted)")+" ("+_l("without extension")+")",&psk->sScrshotFile, DEFAULT_SCRFILE);
					HIROW hScrShotDir=pDialog->Option_AddString(hShotSave,_l("Screenshot directory")+" ("+_l("patterns permitted")+")",&psk->sScrshotDir, psk->sScrshotDir);
					pDialog->Option_AddActionToEdit(hScrShotDir, ChooseDir, hScrShotDir,0,94);
					HIROW hScrShotDir2=pDialog->Option_AddString(hShotSave,_l("Directory")+" "+_l("for double-press screenshots")+" ("+_l("patterns permitted")+")",&psk->sScrshotDir2, "%ASKFOLDER");
					pDialog->Option_AddActionToEdit(hScrShotDir2, ChooseDir, hScrShotDir2,0,94);
					pDialog->Option_AddCombo(hShotSave,_l("Auto open in editor")+"\t"+_l("Automatically open in the image editor"),&psk->bAutoEditOnShot, 0, _l("Do not open")+"\t"+_l("Open for single-press screenshots")+"\t"+_l("Open for double-press screenshots")+"\t"+_l("Open for any screenshot"));
					HIROW hIEditor=pDialog->Option_AddString(hShotSave,_l("Image editor")+" ("+_l("Leave blank to use default editor")+")",&psk->sJpgEditor,psk->sJpgEditorDef);
					pDialog->Option_AddActionToEdit(hIEditor, ChooseAnyFile, hIEditor,0,94);
				}
				{
					HIROW hShotAdd=pDialog->Option_AddTitle(hShot,_l("Additional parameters")+"\t"+_l("Screenshot sign, size and more"));
					HIROW hShotSign=pDialog->Option_AddBOOL(hShotAdd,_l("Sign screenshot (patterns permitted)"),&psk->bSignSShot, 0, 0);
					pDialog->Option_AddString(hShotSign,"",&psk->sSignString, DEFAULT_SCRSIGN);
					HIROW hShotXFix=pDialog->Option_AddBOOL(hShotAdd,_l("Fix X size"),&psk->bFixXSize, 0, 0);
					pDialog->Option_AddNString(hShotXFix,"",&psk->lFixXSizeTo, DEFAULT_XRES,1,10000);
					HIROW hShotYFix=pDialog->Option_AddBOOL(hShotAdd,_l("Fix Y size"),&psk->bFixYSize, 0, 0);
					pDialog->Option_AddNString(hShotYFix,"",&psk->lFixYSizeTo, DEFAULT_YRES,1,10000);
					pDialog->Option_AddBOOL(hShotAdd,_l("Show screenshot`s area info in tooltip"),&psk->lShowScrData, 0, 0);
					pDialog->Option_AddBOOL(hShotAdd,_l("Add cursor into screenshot"),&psk->lAddCursorToScreenshot, 0, 0);
				}
				{
					HIROW hShotSq=pDialog->Option_AddTitle(hShot,_l("Sequential screenshots"));
					pDialog->Option_AddTitle(hShotSq,SCRSEQCAUTION,61);
					HIROW hSSSecs=pDialog->Option_AddBOOL(hShotSq,_l("Make screenshot every ... seconds"),&psk->lScreenshotByTimer, 0, 0);
					pDialog->Option_AddNString(hSSSecs,"",&psk->lScreenshotByTimerSec, 5,5,10000);
				}
				//pDialog->Option_AddAction(hShot,_l("Switch video windows support"), SwitchOverlaySupport, 0);
				AddPluginsOption(hScreens,pDialog,OL_SCRSHOTS);
			}
			// Additional desktops
			HIROW hDesktopOpt=0;
			if(!psk->bUnderWindows98){
				hDesktopOpt=pDialog->Option_AddHeader(hScreens, ADDDESK_TOPIC+"\t"+_l("Additional desktops hotkeys and options"),75);
				pDialog->Option_AddBOOL(hDesktopOpt,_l("Enable additional desktops"),&psk->lEnableAddDesktops, 0);
				pDialog->Option_AddBOOL(hDesktopOpt,_l("Remember desktop layout for each desktop"),&psk->lRemeberIconsPosInDesktops, 1);
				pDialog->Option_AddHotKey(hDesktopOpt,_l("Switch to previous desktop"),&psk->DefActionMapHK[ADDDESK_PREV],pwk->getDefActionMapHK_Preinit()[ADDDESK_PREV]);
				pDialog->Option_AddHotKey(hDesktopOpt,_l("Switch to next desktop"),&psk->DefActionMapHK[ADDDESK_NEXT],pwk->getDefActionMapHK_Preinit()[ADDDESK_NEXT]);
				for(int i=0;i<psk->lDesktopSwitches;i++){
					pDialog->Option_AddHotKey(hDesktopOpt,pwk->GetDesktopName(i),&psk->DefDSwitchMapHK[i],pwk->getDefDsSwMapHK_Preinit()[i]);
				}
				if(psk->videoModesShort.GetSize()>0){
					HIROW hAddDeskResol=pDialog->Option_AddBOOL(hDesktopOpt,_l("Set different resolution for each desktop"),&psk->lRemeberResolutInDesktops, 0, 5);
					CString sDesktopResolutions=psk->videoModesShort[0].Name();
					for(int j=1;j<psk->videoModesShort.GetSize();j++){
						sDesktopResolutions+="\t";
						sDesktopResolutions+=psk->videoModesShort[j].Name();
					}
					for(int i=0;i<psk->lDesktopSwitches;i++){
						for(int k=0;k<psk->videoModesShort.GetSize();k++){
							if(psk->lResolutions[i].m_dwWidth==psk->videoModesShort[k].m_dwWidth &&
								psk->lResolutions[i].m_dwHeight==psk->videoModesShort[k].m_dwHeight &&
								psk->lResolutions[i].m_dwBitsPerPixel==psk->videoModesShort[k].m_dwBitsPerPixel &&
								psk->lResolutions[i].m_dwFrequency==psk->videoModesShort[k].m_dwFrequency){
									psk->lChoosenResolutions[i]=k;
								}
						}
						pDialog->Option_AddCombo(hAddDeskResol,pwk->GetDesktopName(i),&psk->lChoosenResolutions[i],0,sDesktopResolutions,OC_DESKRES);
					}
					pDialog->Option_AddBOOL(hAddDeskResol,_l("Allow to set ALL possible resolutions for additional desktops")+"\t"+_l("Changes in this option ")+" "+_l("Effective on restart only"),&psk->lUseAllVModes);
				}
				HIROW hAddDeskShell=pDialog->Option_AddBOOL(hDesktopOpt,_l("Use different Shell for each desktop")+"\t"+_l("You must restart computer to apply changes to this option"),&psk->lRemeberShellInDesktops, 0, 5);
				for(int i2=0;i2<psk->lDesktopSwitches;i2++){
					HIROW hDeskShell=pDialog->Option_AddString(hAddDeskShell,pwk->GetDesktopName(i2),&psk->lDeskShells[i2],"explorer.exe");
					pDialog->Option_AddActionToEdit(hDeskShell, ChooseAnyFile, hDeskShell,0,94);
				}
				pDialog->Option_AddNString(hDesktopOpt,_l("How many desktops to handle (including default desktop)")+"\t"+_l("Set 0 to disable this feature at all. Effective on restart only"),&psk->lDesktopSwitchesAfterNextStart,3,0,MAX_DESKTOPS);
				pDialog->Option_AddAction(hDesktopOpt,_l("OSD options"),MovePaneFocus, hSplash);
				AddPluginsOption(hDesktopOpt,pDialog,OL_ADDDESKTOPS);
			}
			// Desktops icons
			HIROW hDesktopIcons=0;
			{
				hDesktopIcons=pDialog->Option_AddHeader(hScreens, SAVEICONS_TOPIC+"\t"+_l("Save/restore/frozen desktop icons position"),47);
				pDialog->Option_AddBOOL(hDesktopIcons,_l("Restore desktop icons positions at startup"),&psk->lRestoreDeskIconsPos, 0);
				pDialog->Option_AddBOOL(hDesktopIcons,_l("Disable confirmation alert"),&psk->lDisableDeskicoAlert, 0);
				pDialog->Option_AddCombo(hDesktopIcons,_l("Adjust icons positions to display resolution"),&psk->lRestorePreservRes, 1, _l("Never")+"\t"+_l("After manual icons restore only")+"\t"+_l("Automatically with resolution change"));
				pDialog->Option_AddHotKey(hDesktopIcons,_l("Save icons"),&psk->DefActionMapHK[ICONS_SAVE],pwk->getDefActionMapHK_Preinit()[ICONS_SAVE]);
				pDialog->Option_AddHotKey(hDesktopIcons,_l("Restore icons"),&psk->DefActionMapHK[ICONS_RESTORE],pwk->getDefActionMapHK_Preinit()[ICONS_RESTORE]);
				pDialog->Option_AddAction(hDesktopIcons,_l("Save current positions"),SaveIconsPos, NULL);
				pDialog->Option_AddAction(hDesktopIcons,_l("Restore saved positions"),RestoreIconsPos, NULL);
				AddPluginsOption(hDesktopIcons,pDialog,OL_DESKICONS);
			}
			HIROW hBossk=0;
			{// Boss key
				hBossk=pDialog->Option_AddHeader(hScreens,BOSS_TOPIC+"\t"+_l("Key to hide all shady applications at once"),77);
				CString sBossActions=_l("Hide completely")+"\t"+_l("Hide to tray")+"\t"+_l("Terminate");
				pDialog->Option_AddHotKey(hBossk,_l("Boss key")+"\t"+_l("This key can hide all shady applications just in seconds"),&psk->DefActionMapHK[BOSS_KEY],pwk->getDefActionMapHK_Preinit()[BOSS_KEY],0,105);
				pDialog->Option_AddCombo(hBossk,_l("Boss key behaviour")+"\t"+_l("What to do when you click the boss key"),&psk->lBosskeyAction, 0, sBossActions);
				pDialog->Option_AddString(hBossk,_l("Process only matching applications")+" ("+_l("Matching by window title")+")\t"+_l("Use '*' as a wildcard for partial matches")+", "+_l("leave blank to process all application. Example: 'Explorer*'")+" "+_l("or")+" 'Explorer*;BSPlayer*'",&psk->sBossAppMask, "");
				if(!psk->lDisableDblPressExtensions){
					pDialog->Option_AddCombo(hBossk,_l("Double-press behaviour")+"\t"+_l("For example you are using 'Win-Q' as hotkey","For example you are using 'Win-Q' as hotkey. This option allows you to set another behaviour for 'Win-Q-Q' key"),&psk->lBosskeyAction2, 1, sBossActions);
					pDialog->Option_AddString(hBossk,_l("Double-press")+": "+_l("Process only matching applications")+" ("+_l("Matching by window title")+")\t"+_l("Use '*' as a wildcard for partial matches")+", "+_l("leave blank to process all application. Example: 'Explorer*'")+" "+_l("or")+" 'Explorer*;BSPlayer*'",&psk->sBossAppMask2, "");
				}
				pDialog->Option_AddBOOL(hBossk,_l("Fill display with the last screenshot"),&psk->lBossShotSwap,0);
				AddPluginsOption(hBossk,pDialog,OL_BOSSKEY);
			}
			pDialog->Option_AddAction(hScreens,_l("Hotkeys to control running applications"),MovePaneFocus, hWinControl);
			pDialog->Option_AddAction(hScreens,_l("Hotkeys to control hidden applications"),MovePaneFocus, hHidApps);
			pDialog->Option_AddAction(hScreens,_l("Screenshots options - files, modes, etc."),MovePaneFocus, hShot);
			if(hDesktopOpt){
				pDialog->Option_AddAction(hScreens,_l("Additional desktops"),MovePaneFocus, hDesktopOpt);
			}
			pDialog->Option_AddAction(hScreens,_l("Save/Restore desktop icons positions"),MovePaneFocus, hDesktopIcons);
			pDialog->Option_AddAction(hScreens,_l("Boss key and related options"),MovePaneFocus, hBossk);
		}
		//HIROW hModules=pDialog->Option_AddHeader(FL_ROOT, _l("Miscellaneous operations")+"\t"+_l("Boss key, winamp/volume controls and more"),67);
		if(psk->bMiscSaverMenu!=4){// Main modules
			HIROW hModules=pDialog->Option_AddHeader(FL_ROOT, _l("System related controls")+"\t"+_l("Shutdown, processes and more"),66);
			HIROW hTrayProc=0;
			if(psk->bSysActionsInHotMenu!=4){//List of processes
				hTrayProc=pDialog->Option_AddHeader(hModules,	PROCLIST_TOPIC+"\t"+_l("Processess list options"),33);
				pDialog->Option_AddHotKey(hTrayProc,_l("Popup list of started processes"),&psk->DefActionMapHK[SHOWPROCLIST],pwk->getDefActionMapHK_Preinit()[SHOWPROCLIST]);
				pDialog->Option_AddBOOL(hTrayProc,_l("Extract process icons")+" ("+_l("to show them in menu")+")\t"+_l("Note: for the first time icons extraction can be lengthy operation"),&psk->bExtractIconsForSysMenu, 1);
				pDialog->Option_AddBOOL(hTrayProc,_l("Sort processes alphabetically"),&psk->bSortForSysMenu, 1);
				pDialog->Option_AddNString(hTrayProc,_l("Show list in ... columns"),&psk->bPlistUse2Columns, 2);
				pDialog->Option_AddBOOL(hTrayProc,_l("Show full file paths")+"\t"+_l("Otherwise file names will be shown"),&psk->bPlistUseFullPaths, 0);
				pDialog->Option_AddBOOL(hTrayProc,_l("Show overall memory usage"),&psk->bShowMemUsage, 1);
				pDialog->Option_AddString(hTrayProc,_l("Application list to hide from tray list")+"\t"+_l("Use '*' as a wildcard for partial matches"),&psk->sListToHideSim);
				pDialog->Option_AddCombo(hTrayProc,_l("Kill confirmation")+"\t"+_l("Kill selected application immediately or ask about your opinion before"),&psk->bAskForKill,0,_l("Ask")+"\t"+_l("Kill immediatly"));
				pDialog->Option_AddTitle(hTrayProc,_l("Memory optimization"),86);
				pDialog->Option_AddHotKey(hTrayProc,_l("Free physical memory"),&psk->DefActionMapHK[FREE_MEMORY],pwk->getDefActionMapHK_Preinit()[FREE_MEMORY]);
				pDialog->Option_AddBOOL(hTrayProc,_l("Flush registry and disk cache during memory optimizations"),&psk->lOptimizeRegistryON, 1);
				pDialog->Option_AddTitle(hTrayProc,_l("Kill several applications simultaneously"),86);
				pDialog->Option_AddHotKey(hTrayProc,_l("Kill applications from list"),&psk->DefActionMapHK[KILL_PROCLIST],pwk->getDefActionMapHK_Preinit()[KILL_PROCLIST]);
				pDialog->Option_AddString(hTrayProc,_l("Application list to kill simultaneously")+"\t"+_l("Use '*' as a wildcard for partial matches"),&psk->sListToKillSim);
				AddPluginsOption(hTrayProc,pDialog,OL_PROCLIST);
			}
			HIROW hStanEx=0;
			if(GetModuleHandle("wp_wtraits.wkp")){
				hStanEx=pDialog->Option_AddHeader(hModules,_l("Standard Windows hotkeys")+"\t"+_l("Default hotkeys for opening 'Find', 'Run' and other common dialogs"),79);
				//pDialog->Option_AddHotKey(hStanEx,_l("Display or hide the Start menu"),&psk->DefActionMapHK[WS_SHOWSTART],pwk->getDefActionMapHK_Preinit()[WS_SHOWSTART]);
				pDialog->Option_AddHotKey(hStanEx,_l("Display the System Properties dialog box"),&psk->DefActionMapHK[WS_SYSPROPS],pwk->getDefActionMapHK_Preinit()[WS_SYSPROPS]);
				pDialog->Option_AddHotKey(hStanEx,_l("Display the desktop"),&psk->DefActionMapHK[WS_SHOWDESK],pwk->getDefActionMapHK_Preinit()[WS_SHOWDESK]);
				pDialog->Option_AddHotKey(hStanEx,_l("Minimize all of the windows"),&psk->DefActionMapHK[WS_MINIMALL],pwk->getDefActionMapHK_Preinit()[WS_MINIMALL]);
				pDialog->Option_AddHotKey(hStanEx,_l("Open My Computer"),&psk->DefActionMapHK[WS_SHOWMYCOMP],pwk->getDefActionMapHK_Preinit()[WS_SHOWMYCOMP]);
				pDialog->Option_AddHotKey(hStanEx,_l("Search for a file or a folder"),&psk->DefActionMapHK[WS_SEARCHFILE],pwk->getDefActionMapHK_Preinit()[WS_SEARCHFILE]);
				pDialog->Option_AddHotKey(hStanEx,_l("Search for computers"),&psk->DefActionMapHK[WS_SEARCHCOMP],pwk->getDefActionMapHK_Preinit()[WS_SEARCHCOMP]);
				pDialog->Option_AddHotKey(hStanEx,_l("Display Windows Help"),&psk->DefActionMapHK[WS_SHOWWHELP],pwk->getDefActionMapHK_Preinit()[WS_SHOWWHELP]);
				pDialog->Option_AddHotKey(hStanEx,_l("Lock the keyboard"),&psk->DefActionMapHK[WS_LOCKKEYB],pwk->getDefActionMapHK_Preinit()[WS_LOCKKEYB]);
				pDialog->Option_AddHotKey(hStanEx,_l("Open the Run dialog box"),&psk->DefActionMapHK[WS_SHOWRUND],pwk->getDefActionMapHK_Preinit()[WS_SHOWRUND]);
				//pDialog->Option_AddHotKey(hStanEx,_l("Open Utility Manager"),&psk->DefActionMapHK[WS_SHOWUTILM],pwk->getDefActionMapHK_Preinit()[WS_SHOWUTILM]);
				AddPluginsOption(hStanEx,pDialog,OL_EXPLORERHK);
			}
			{// Layout switch
				hLSwitch=pDialog->Option_AddHeader(hModules,_l("Keyboard")+", "+_l("Keyboard layout")+"\t"+_l("Keys to set preffered layout in one keypress"),104);
				pDialog->Option_AddHotKey(hLSwitch,_l("Switch to next layout"),&psk->DefActionMapHK[SWITCHLAYOUT],pwk->getDefActionMapHK_Preinit()[SWITCHLAYOUT]);
				for(int i=0;i<MAX_HKL;i++){
					if(psk->aKBLayouts[i]){
						pDialog->Option_AddHotKey(hLSwitch,_l("Set layout")+": "+pwk->GetHKLName(i),&psk->aKBLayoutKeys[i],psk->aKBLayoutKeys[i]);
					}else{
						break;
					}
				}
				pDialog->Option_AddCombo(hLSwitch,_l("Notification type"),&psk->lLayEffectType, 0, _l("None")+"\t"+_l("Sound"));//+"\t"+_l("Negative")
				if(hHotkeys){
					AddPluginsOption(hLSwitch,pDialog,OL_KEYBOARD);
					pDialog->Option_AddAction(hLSwitch,_l("Block keyboard hotkey, other keyboard and hotkeys options"),MovePaneFocus, hHotkeys);
				}
			}
			HIROW hQShut=0;
			{// Quick shutdown
				hQShut=pDialog->Option_AddHeader(hModules,SHUTDOWN_TOPIC+"\t"+_l("Rapid shutdown preferences"),76);
				if(psk->bUnderWindowsNT){
					pDialog->Option_AddHotKey(hQShut,_l("Lock computer"),&psk->DefActionMapHK[LOCK_STATION],pwk->getDefActionMapHK_Preinit()[LOCK_STATION]);
				}
				pDialog->Option_AddHotKey(hQShut,_l("Show shutdown/restart dialog"),&psk->DefActionMapHK[DIALOG_SHUTDOWN],pwk->getDefActionMapHK_Preinit()[DIALOG_SHUTDOWN],0,105);
				pDialog->Option_AddHotKey(hQShut,_l("Quick shutdown (double press to restart)"),&psk->DefActionMapHK[FAST_SHUTDOWN],pwk->getDefActionMapHK_Preinit()[FAST_SHUTDOWN],0,105);
				pDialog->Option_AddCombo(hQShut,_l("Quick shutdown type")+"\t"+_l("Note: Extreme shutdown is very fast, but all unsaved data will be lost")+"!",&psk->lShutdownStrength, 1, _l("Normal")+"\t"+_l("Auto")+"\t"+_l("Extreme")+"\t"+_l("Suspend")+"\t"+_l("Hibernate"));
				pDialog->Option_AddBOOL(hQShut,_l("Invert hotkey: Restart on single press, Shutdown on double press"),&psk->lExitWndOnDbl, 0);
				pDialog->Option_AddString(hQShut,_l("Kill denoted applications before shutdown")+"\t"+_l("You can use mask with '*' char and ';' as delimiter, example: 'explorer*;*pad*'"),&psk->sKillBeforeShutdown, "");
				pDialog->Option_AddHotKey(hQShut,_l("Extreme shutdown"),&psk->DefActionMapHK[EXTREM_SHUTD],pwk->getDefActionMapHK_Preinit()[EXTREM_SHUTD]);
				AddPluginsOption(hQShut,pDialog,OL_QUICKSHUTDOWN);
			}
			HIROW hTIco=0;
			{// SR indication
				hTIco=pDialog->Option_AddHeader(hModules, _l("System resource indication"),101);
				hSysResInd=pDialog->GetAliasHIROW(hTIco);
				pDialog->Option_AddBOOL(hTIco,_l("Show CPU and Memory usage in the tray tooltip"),&psk->bShowCPUinTT, 1);
				pDialog->Option_AddCombo(hTIco,_l("Use tray icon as indicator"),&psk->lShowCPUInTrayIcon, pwk->_ShowCPUUsageDef, _l("Do not use as indicator")+"\t"+_l("Of CPU Usage")+"\t"+_l("Of Memory Usage"), OC_SHOWICON);
				pDialog->Option_AddNString(hTIco,_l("Icon refresh rate")+" ("+_l("Every ... seconds")+")\t"+_l("Caution: Higher rates use more system resources")+" ("+_l("Slightly affects MS Windows perfomance")+")",&psk->lShowCPUInTraySecs,2,1,100,OC_SHOWICON);
				HIROW hCPURefreshC=pDialog->Option_AddBOOL(hTIco,_l("Show histogram, with number of columns"),&psk->lShowCPUHistInTray, pwk->_ShowCPUUsageDef, 0, OC_SHOWICON);
				pDialog->Option_AddNString(hCPURefreshC,CString(""),&psk->lShowCPUHistInTrayC,4,2,100,OC_SHOWICON);
				HIROW hDTF=pDialog->Option_AddHotKey(hTIco,_l("Show time and date in OSD")+"\t"+_l("Very useful in 3D games - when you just don`t have a time enough to look at the clocks")+"; "+_l("Expand this node to set up date and time formats"),&psk->DefActionMapHK[SHOWCLOCKS],pwk->getDefActionMapHK_Preinit()[SHOWCLOCKS]);
				pDialog->Option_AddString(hDTF,_l("Time format"),&psk->sODSTimeFormat);
				pDialog->Option_AddString(hDTF,_l("Date format"),&psk->sODSDateFormat);
				pDialog->Option_AddAction(hDTF,_l("Show help on format specifications"), TimeDateFormats, NULL, 0);
				HIROW cpuHungers=pDialog->Option_AddBOOL(hTIco,_l("Log CPU-hungry processes to file"),&psk->lLogCPUHungers, 0);
				HIROW hLogDir=pDialog->Option_AddString(cpuHungers,_l("Log file"),&psk->sLogCPUHungers, psk->sLogCPUHungers);
				pDialog->Option_AddActionToEdit(hLogDir, ChooseAnyFile, hLogDir, 0, 94);
				

			}
			HIROW hVolC=0;
			if(!IsVistaSoundControl()){
				if(psk->bVolumeMenu!=4){// Volume control
					hVolC=pDialog->Option_AddHeader(hModules,VOL_TOPIC+"\t"+_l("Volume up/down hotkeys"),53);
					pDialog->Option_AddHotKey(hVolC,_l("Mute sound"),&psk->DefActionMapHK[SOUND_MUTE],pwk->getDefActionMapHK_Preinit()[SOUND_MUTE]);
					pDialog->Option_AddHotKey(hVolC,_l("Volume up"),&psk->DefActionMapHK[VOLUME_UP],pwk->getDefActionMapHK_Preinit()[VOLUME_UP]);
					pDialog->Option_AddHotKey(hVolC,_l("Volume down"),&psk->DefActionMapHK[VOLUME_DOWN],pwk->getDefActionMapHK_Preinit()[VOLUME_DOWN]);
					pDialog->Option_AddNString(hVolC,_l("Step (percents)"),&psk->lVolumeCStep,10,0,100);
					HIROW hOnStartShut=pDialog->Option_AddBOOL(hVolC,_l("Set volume on Windows startup (percents)"),&psk->lVolumeAfterStart,0);
					pDialog->Option_AddNString(hOnStartShut,"",&psk->lVolumeAfterStartLevel,20,0,100);
					HIROW hBeforeShut=pDialog->Option_AddBOOL(hVolC,_l("Set volume on Windows shutdown (percents)"),&psk->lVolumeBeforeShutdown,0);
					pDialog->Option_AddNString(hBeforeShut,"",&psk->lVolumeBeforeShutdownLevel,20,0,100);
					pDialog->Option_AddAction(hVolC,_l("OSD options"),MovePaneFocus, hSplash);
					pDialog->Option_AddTitle(hVolC,_l("You can use 'Sound control' plugin to get control over specific mixers (wave, aux, etc)"),86);
					AddPluginsOption(hVolC,pDialog,OL_VOLUME);
				}
			}
			HIROW hCDEject=0;
			{// CD Ejecter
				BOOL bOneCD=1;
				hCDEject=pDialog->Option_AddHeader(hModules,CDEJECT_TOPIC+"\t"+_l("Quick CDROM access hotkeys"),71);
				pDialog->Option_AddHotKey(hCDEject,_l("Load/Eject CD drive"),&psk->DefActionMapHK[OPEN_CD],pwk->getDefActionMapHK_Preinit()[OPEN_CD],0,105);
				if(!psk->sCDDisk.IsEmpty() || !psk->sCDDiskDefault.IsEmpty()){
					HIROW hCD1=pDialog->Option_AddString(hCDEject,_l("CD Drive letter")+"\t"+_l("Type CD Drive letter to eject and load by hotkey"),&psk->sCDDisk, psk->sCDDiskDefault.IsEmpty()?psk->sCDDisk:psk->sCDDiskDefault);
					pDialog->Option_AddActionToEdit(hCD1, ChooseDir, hCD1, 0, 94);
					if(!psk->lDisableDblPressExtensions){
						bOneCD=0;
						HIROW hCD2=pDialog->Option_AddString(hCDEject,_l("CD Drive letter for double-press")+"\t"+_l("Type CD Drive letter to eject and load by hotkey double-press"),&psk->sCDDisk2, psk->sCDDisk2);
						pDialog->Option_AddActionToEdit(hCD2, ChooseDir, hCD2, 0, 94);
					}
				}
				HIROW hCDCloseTime=pDialog->Option_AddBOOL(hCDEject,_l("Auto close CD drive after some seconds"),&psk->bAutoCloseCD, 1, 0);
				pDialog->Option_AddNString(hCDCloseTime,"",&psk->lAutoCloseCDTime, DEF_CDCLOSETIME,0,100);
				pDialog->Option_AddBOOL(hCDEject,_l("Eject CD drive with compact disk on Windows shutdown")+"\t"+_l(PROGNAME" will automatically eject (for 2 seconds) forgotten compact disk on shutdown or restart")+"; "+_l("If you want to use this feature on the second CD Drive, fill CD letter for the double-press Eject hotkey"),&psk->bEjectCDOnShut, 1, 0);
				AddPluginsOption(hCDEject,pDialog,OL_CDEJECT);
			}
			HIROW hWinAmp=0;
			{// WinAmp Controls
				hWinAmp=pDialog->Option_AddHeader(hModules,_l("WinAmp controls")+"\t"+_l("Hotkeys to control WinAmp playback"),73);
				pDialog->Option_AddHotKey(hWinAmp,_l("Play/Pause"),&psk->DefActionMapHK[WINAMP_PLAY],pwk->getDefActionMapHK_Preinit()[WINAMP_PLAY]);
				pDialog->Option_AddHotKey(hWinAmp,_l("Stop/Play"),&psk->DefActionMapHK[WINAMP_STOP],pwk->getDefActionMapHK_Preinit()[WINAMP_STOP]);
				pDialog->Option_AddHotKey(hWinAmp,_l("Next song"),&psk->DefActionMapHK[WINAMP_NEXTS],pwk->getDefActionMapHK_Preinit()[WINAMP_NEXTS]);
				pDialog->Option_AddHotKey(hWinAmp,_l("Prev song"),&psk->DefActionMapHK[WINAMP_PREVS],pwk->getDefActionMapHK_Preinit()[WINAMP_PREVS]);
				pDialog->Option_AddHotKey(hWinAmp,_l("Back 5 secs"),&psk->DefActionMapHK[WINAMP_PREV5S],pwk->getDefActionMapHK_Preinit()[WINAMP_PREV5S]);
				pDialog->Option_AddHotKey(hWinAmp,_l("Forward 5 secs"),&psk->DefActionMapHK[WINAMP_NEXT5S],pwk->getDefActionMapHK_Preinit()[WINAMP_NEXT5S]);
				pDialog->Option_AddHotKey(hWinAmp,_l("Increase volume"),&psk->DefActionMapHK[WINAMP_VOLUP],pwk->getDefActionMapHK_Preinit()[WINAMP_VOLUP]);
				pDialog->Option_AddHotKey(hWinAmp,_l("Decrease volume"),&psk->DefActionMapHK[WINAMP_VOLDOWN],pwk->getDefActionMapHK_Preinit()[WINAMP_VOLDOWN]);
				pDialog->Option_AddNString(hWinAmp,_l("Step"),&psk->lVolumeWStep,10,0,100);
				pDialog->Option_AddHotKey(hWinAmp,_l("Switch shuffling"),&psk->DefActionMapHK[WINAMP_SHUFFLING],pwk->getDefActionMapHK_Preinit()[WINAMP_SHUFFLING]);
				pDialog->Option_AddHotKey(hWinAmp,_l("Show/Hide WinAmp window"),&psk->DefActionMapHK[WINAMP_SHOWHIDE],pwk->getDefActionMapHK_Preinit()[WINAMP_SHOWHIDE]);
				pDialog->Option_AddHotKey(hWinAmp,_l("Start/Stop visualization plugin"),&psk->DefActionMapHK[WINAMP_VISPLUGIN],pwk->getDefActionMapHK_Preinit()[WINAMP_VISPLUGIN]);
				HIROW hWAFile=pDialog->Option_AddString(hWinAmp,_l("Path to WinAmp's executable")+"\t"+_l(PROGNAME" will start WinAmp automatically when you press one of the above hotkey while WinAmp is not running. Leave this field blank to autodetect WinAmp's executable file"),&psk->sWinAmpStartPas);
				pDialog->Option_AddActionToEdit(hWAFile, ChooseAnyFile, hWAFile, 0, 94);
				pDialog->Option_AddBOOL(hWinAmp,_l("Always show current song in the splash string"),&psk->lShowWinampTitle,1);
				pDialog->Option_AddBOOL(hWinAmp,_l("Strip song number and WinAmp state from the splash string"),&psk->lStripNumberWinampTitle,1);
				pDialog->Option_AddBOOL(hWinAmp,_l("Stop WinAmp")+": "+_l("before locking computer")+", "+_l("when screen saver started"),&psk->lStopWinAmpOnLock, 0);
				pDialog->Option_AddBOOL(hWinAmp,_l("Start WinAmp")+": "+_l("after computer is unlocked")+", "+_l("when screen saver stopped"),&psk->lStartWinAmpAfterLock, 0);
				pDialog->Option_AddTitle(hWinAmp,_l("Currently playing song and OSD settings"),61);
				pDialog->Option_AddHotKey(hWinAmp,_l("Show current song in OSD"),&psk->DefActionMapHK[WINAMP_SHOWTITL],pwk->getDefActionMapHK_Preinit()[WINAMP_SHOWTITL]);
				pDialog->Option_AddNString(hWinAmp,_l("Show OSD for ... seconds"),&psk->lWinampOSDTime,4,1,100);
				pDialog->Option_AddBOOL(hWinAmp,_l("Add position of playback to the OSD"),&psk->lAddTLWinampTitle,1);
				pDialog->Option_AddBOOL(hWinAmp,_l("Add total track length to the OSD"),&psk->lAddTTWinampTitle,1);
				pDialog->Option_AddBOOL(hWinAmp,_l("Show song title only, without additional text"),&psk->lStripAllWinampTitle,0);
				HIROW hChanges=pDialog->Option_AddBOOL(hWinAmp,_l("Show song title every time next song starts to play"),&psk->lCatchWinampTitle,0,5);
				pDialog->Option_AddBOOL(hChanges,_l("Show new song in the tray tooltip instead of OSD"),&psk->lShowWTitInTooltip,0);
				pDialog->Option_AddAction(hWinAmp,_l("OSD options"),MovePaneFocus, hSplash);
				AddPluginsOption(hWinAmp,pDialog,OL_WINAMP);
			}
			HIROW hSSaver=0;
			{// Screen-saver
				hSSaver=pDialog->Option_AddHeader(hModules,SCRSAVE_TOPIC+"\t"+_l("Screen saver hotkey and additional features"),82);
				pDialog->Option_AddHotKey(hSSaver,_l("Launch screen saver"),&psk->DefActionMapHK[RUN_SCREENSAVER],pwk->getDefActionMapHK_Preinit()[RUN_SCREENSAVER]);
				pDialog->Option_AddHotKey(hSSaver,_l("Switch monitor into stand-by mode"),&psk->DefActionMapHK[MONITOR_STANDBY],pwk->getDefActionMapHK_Preinit()[MONITOR_STANDBY]);
				pDialog->Option_AddBOOL(hSSaver,_l("Stop WinAmp first")+" ("+_l("before launching screen saver")+")",&psk->lStopWinAmpOnScrsave, 0);
				HIROW hRWS=pDialog->Option_AddString(hSSaver,_l("Launch this application along with screensaver")+"\t"+_l("Leave blank to disable this feature"),&psk->sAppToRWS);
				pDialog->Option_AddActionToEdit(hRWS, ChooseAnyFile, hRWS, 0, 94);
				pDialog->Option_AddBOOL(hRWS,_l("And stop it along with screensaver"),&psk->lStopAppToRWS, 1);
				HIROW hValidScrs=pDialog->Option_AddBOOL(hSSaver,_l("Set random screen saver first")+" ("+_l("before launching screen saver")+")",&psk->lSetRandomScreensaver, 0, 5, OC_SCRSAVE);
				for(int i=0;i<psk->dirScreenSavers.GetSize();i++){
					pDialog->Option_AddBOOL(hValidScrs,psk->dirScreenSavers[i].sName,&psk->dirScreenSavers[i].bUse, 1, 0, OC_SCRSAVE);
				}
				pDialog->Option_AddAction(hValidScrs,_l("Refresh list of Screensavers"), InitSSs, NULL, 0);
				AddPluginsOption(hSSaver,pDialog,OL_SCRSAVER);
			}
			// Корневые настройки
			pDialog->Option_AddAction(hModules,_l("Define hotkeys for shutdown/log off/etc actions"),MovePaneFocus, hQShut);
			pDialog->Option_AddAction(hModules,_l("Tray icon, Start button and system resources indication"),MovePaneFocus, hTIco);
			if(hTrayProc){
				pDialog->Option_AddAction(hModules,_l("List of active processes and memory usage"),MovePaneFocus, hTrayProc);
			}
			pDialog->Option_AddAction(hModules,_l("Hotkeys for switching keyboard layout"),MovePaneFocus, hLSwitch);
			pDialog->Option_AddAction(hModules,_l("Screen saver related hotkeys"),MovePaneFocus, hSSaver);
			pDialog->Option_AddAction(hModules,_l("Define hotkeys for controlling WinAmp"),MovePaneFocus, hWinAmp);
			if(hCDEject){
				pDialog->Option_AddAction(hModules,_l("CD Ejecting (including auto eject before shutdown)"),MovePaneFocus, hCDEject);
			}
			if(hVolC){
				pDialog->Option_AddAction(hModules,_l("Common volume controls"),MovePaneFocus, hVolC);
			}
		}
	}
	if(pData->iTab==2){//Plugins
		HIROW hPlugins=pDialog->Option_AddHeader(FL_ROOT, _l(PLUGINS_TOPIC),63);
		HIROW hPluginsUnused=pDialog->Option_AddHeader(FL_ROOT, _l("Inactive plugins"),63);//hPlugins
		hWHelperPlugin=hPlugins;
		int iInac=0;
		if(psk->plugins.GetSize()>0){
			for(int i=0;i<psk->plugins.GetSize();i++){
				CPlugin* plg=psk->plugins[i];
				if(plg && plg->sFileName.CompareNoCase("WP_KeyMaster")!=0 /*&& plg->sFileName.CompareNoCase("WP_Schedule")!=0*/){
					CString sTitle=plg->_l(plg->sTitle);
					DWORD dwIcon=(plg->iPluginIconNum==-1)?63:plg->iPluginIconNum;
					HIROW hPlgFolder=pDialog->Option_AddTitle(FL_ROOT,sTitle,dwIcon);
					if(plg->sFileName.CompareNoCase("WP_WTraits")==0){
						hWHelperPlugin=hPlgFolder;
					}
					if(plg->bIsActive){
						pDialog->Option_AddAlias(hPlgFolder, sTitle, dwIcon, hPlugins);
					}else{
						iInac++;
						pDialog->Option_AddAlias(hPlgFolder, sTitle, dwIcon, hPluginsUnused);
					}
					AddPluginHotkeys("","",i,hPlgFolder,pDialog,plg,FALSE);
				}
			}
		}
		if(iInac){
			pDialog->Option_AddTitle(hPlugins,_l("Check inactive plugins, you may find extrimely useful things there"),86);
		}
		pDialog->Option_AddTitle(hPlugins,_l("It is better to disable unused plugins"),86);
		pDialog->Option_AddAction(hPlugins,_l("Download more plugins"), OpenPlugins, 0);
	}
	if(pData->iTab==0){
		{//Sounds
			HIROW hSounds=pDialog->Option_AddHeader(FL_ROOT,	_l("Sound options"),64);
			pDialog->Option_AddBOOL(hSounds,_l("Disable all sounds"),&psk->lDisableSounds, 0, 53);
			//
			HIROW hSoundsErr=pDialog->Option_AddBOOL(hSounds,_l("Error"),&psk->bErrorSound, 1, 53);
			HIROW hSoundsErrWav=pDialog->Option_AddString(hSoundsErr,_l("Wav file"),&psk->sErrorSound,DEFAULT_ONERRSOUND);
			pDialog->Option_AddActionToEdit(hSoundsErrWav,ChooseWavFile, hSoundsErrWav,0,94);
			pDialog->Option_AddAction(hSoundsErr,_l("Play sound"),TestSound, hSoundsErrWav);
			//
			HIROW hSoundsOnCCopy=pDialog->Option_AddBOOL(hSounds,_l("Additional clip - copy"),&psk->bClipCopySound, 1, 53);
			HIROW hSoundsOnCCopyWav=pDialog->Option_AddString(hSoundsOnCCopy,_l("Wav file"),&psk->sClipCopyWav, DEFAULT_ONCCOPYSND);
			pDialog->Option_AddActionToEdit(hSoundsOnCCopyWav,ChooseWavFile, hSoundsOnCCopyWav,0,94);
			pDialog->Option_AddAction(hSoundsOnCCopy,_l("Play sound"),TestSound, hSoundsOnCCopyWav);
			//
			HIROW hSoundsOnCPaste=pDialog->Option_AddBOOL(hSounds,_l("Additional clip - paste"),&psk->bClipPasteSound, 1, 53);
			HIROW hSoundsOnCPasteWav=pDialog->Option_AddString(hSoundsOnCPaste,_l("Wav file"),&psk->sClipPasteWav, DEFAULT_ONCPASTESND);
			pDialog->Option_AddActionToEdit(hSoundsOnCPasteWav,ChooseWavFile, hSoundsOnCPasteWav,0,94);
			pDialog->Option_AddAction(hSoundsOnCPaste,_l("Play sound"),TestSound, hSoundsOnCPasteWav);
			//
			HIROW hSoundsOnLConv=pDialog->Option_AddBOOL(hSounds,_l("Convert text layout"),&psk->bOnLConv, 1, 53);
			HIROW hSoundsOnLConvWav=pDialog->Option_AddString(hSoundsOnLConv,_l("Wav file"),&psk->sOnLConvWav, DEFAULT_ONLCONV);
			pDialog->Option_AddActionToEdit(hSoundsOnLConvWav,ChooseWavFile, hSoundsOnLConvWav,0,94);
			pDialog->Option_AddAction(hSoundsOnLConv,_l("Play sound"),TestSound, hSoundsOnLConvWav);
			//
			HIROW hSoundsSSHot=pDialog->Option_AddBOOL(hSounds,_l("Make screenshot"),&psk->bSShot, 1, 53);
			HIROW hSoundsSSHotWav=pDialog->Option_AddString(hSoundsSSHot,_l("Wav file"),&psk->sSShotWav, DEFAULT_SCRSHOT);
			pDialog->Option_AddActionToEdit(hSoundsSSHotWav,ChooseWavFile,hSoundsSSHotWav,0,94);
			pDialog->Option_AddAction(hSoundsSSHot,_l("Play sound"),TestSound, hSoundsSSHotWav);
			//
			HIROW hSoundsCalc=pDialog->Option_AddBOOL(hSounds,_l("Calculate")+"/"+_l("Text macro")+"/"+_l("Run as from command string"),&psk->bSCalc, 1, 53);
			HIROW hSoundsCalcWav=pDialog->Option_AddString(hSoundsCalc,_l("Wav file"),&psk->sSCalcWav, DEFAULT_CALCSOUND);
			pDialog->Option_AddActionToEdit(hSoundsCalcWav,ChooseWavFile,hSoundsCalcWav,0,94);
			pDialog->Option_AddAction(hSoundsCalc,_l("Play sound"),TestSound, hSoundsCalcWav);
			//
			HIROW hSoundsHK=pDialog->Option_AddBOOL(hSounds,_l("Triggered hotkey"),&psk->bSHk, 0, 53);
			HIROW hSoundsHKWav=pDialog->Option_AddString(hSoundsHK,_l("Wav file"),&psk->sSHkWav, DEFAULT_HKSOUND);
			pDialog->Option_AddActionToEdit(hSoundsHKWav,ChooseWavFile,hSoundsHKWav,0,94);
			pDialog->Option_AddAction(hSoundsHK,_l("Play sound"),TestSound, hSoundsHKWav);
		}
	}
	if(pData->iTab==0){
		{// Getting started
			HIROW hGStarted=pDialog->Option_AddHeader(hCommon_Topic?hCommon_Topic:FL_ROOT,	GSTARTED_TOPIC+"\t"+_l("Quick access to all common actions"),97,FALSE,hCommon_Topic?NULL:FL_ROOT);
#ifndef GPL
			if(psk->iLikStatus<2){
				pDialog->Option_AddAction(hGStarted,REG_DSC, BuyWK, 0);
			}
#endif
			pDialog->Option_AddAction(hGStarted,_l("List of all hotkeys"), ListHotkeys, 0);
			//HIROW hFindOpt=pDialog->Option_AddString(hGStarted,_l("Quick search: text to find"),&psk->sFindOptionMask, psk->sFindOptionMask, 0);
			//pDialog->Option_AddAction(hGStarted,_l("Find option containing text"), FindOption, hFindOpt);
			if(hQRun_Topic){
				pDialog->Option_AddTitle(hGStarted,_l("Quick-run items"),61);
				pDialog->Option_AddAction(hGStarted,_l("New hotkey for File"), AddQRunFile, hQRun_Topic);
				pDialog->Option_AddAction(hGStarted,_l("New hotkey for Application"), AddQRunApp, hQRun_Topic);
				pDialog->Option_AddAction(hGStarted,_l("New hotkey for Folder"), AddQRunFold, hQRun_Topic);
				pDialog->Option_AddAction(hGStarted,_l("New hotkey for URL"), AddQRunLink, hQRun_Topic);
				pDialog->Option_AddAction(hGStarted,_l("New initially unassigned hotkey"), AddQRunEmpty, hQRun_Topic);
			}
			if(hQMac_Topic){
				pDialog->Option_AddTitle(hGStarted,_l("Text macros"),61);
				pDialog->Option_AddAction(hGStarted,_l("Add new text macro"), AddQRun3, hQMac_Topic);
				pDialog->Option_AddAction(hGStarted,_l("Add macro from file"), ShowQRun3Folder, hQMac_Topic);
				pDialog->Option_AddAction(hGStarted,_l("Add insertion macro"), AddQRun4, hQMac_Topic);
				pDialog->Option_AddAction(hGStarted,_l("Add application macro"), AddQRun2, hQMac_Topic);
				pDialog->Option_AddAction(hGStarted,_l("Show Macro help page"), ShowQRunHelp, hQMac_Topic);
			}
			pDialog->Option_AddTitle(hGStarted,_l("Configuration backup"),61);
			pDialog->Option_AddAction(hGStarted,_l("Backup config to file"), BackupToFile, 0);
			pDialog->Option_AddAction(hGStarted,_l("Restore config from file"),BackupFromFile, 0);
			pDialog->Option_AddTitle(hGStarted,_l("Miscellaneous"),61);
			pDialog->Option_AddAction(hGStarted,_l("Download more plugins"), OpenPlugins, 0);
			pDialog->Option_AddAction(hGStarted,_l("Download additional macros"), OpenMacros, 0);
			pDialog->Option_AddAction(hGStarted,_l("Install default macros and quick-run items"), AddDefMacros_Warn, 0);
		}
		pDialog->Option_AddMenuAction(_l("List of all hotkeys"), ListHotkeys, 0, 0);
#ifndef GPL
#ifndef _DEBUG
		if(psk->iLikStatus<2)
#endif
		{
			HIROW hRegistration=pDialog->Option_AddHeader(FL_ROOT,_l(REGISTRATION_TOPIC)+"\t"+_l("Registration information"),87);
			{
				if(psk->iLikStatus<2){
					pDialog->Option_AddAction(hRegistration,REG_DSC, BuyWK, NULL, 0);
					pDialog->Option_AddAction(hRegistration,_l("How to get registration key"), HowGetLikKeys, NULL, 0);
				}
				pDialog->Option_AddAction(hRegistration,_l("Insert from clipboard")+"\t"+_l("Use this to quickly register application")+": "+_l("Copy email with your registration info into clipboard and press this button"), PasteLikKeys, NULL, OC_LICDATA);
				pDialog->Option_AddAction(hRegistration,_l("Insert from file")+"\t"+_l("Use this to quickly register application")+": "+_l("Save email with your registration info into file and press this button"),PasteLikKeysFromFile, NULL, OC_LICDATA);
				pDialog->Option_AddString(hRegistration,_l("Registration email"),&psk->sLikUser, psk->sLikUser, OC_LICDATA);
				pDialog->Option_AddString(hRegistration,_l("Serial number"),&psk->sLikKey, psk->sLikKey, OC_LICDATA);
			}
		}
#endif
	}
#ifndef GPL
	if(psk->iLikStatus==1){
		pDialog->m_pStartupInfo->szTooltipPostfix="\n---\n P.S.:"+_l("This is unregistered copy of")+" "PROGNAME;
		pDialog->m_pStartupInfo->szTooltipPostfix+="\n"+_l("Your changes will not be saved")+"!";
		pDialog->m_pStartupInfo->szTooltipPostfix+="\n"+_l("Please, don`t forget to register")+"!";
	}
#endif
	// Верхнее меню
	CMenu* pSysMenu=pData->GetMenu();
	if(pSysMenu){
		delete pSysMenu;
		pDialog->DrawMenuBar();
		pSysMenu=0;
	}
	if(!pSysMenu){// Создаем верхнее меню
		pSysMenu=new CMenu();
		pSysMenu->CreateMenu();
		{// File
			CMenu mFile;
			mFile.CreatePopupMenu();
			AddMenuString(&mFile,UPMENU_ID+16,_l("List of all hotkeys"),NULL);
			AddMenuString(&mFile,UPMENU_ID+1,_l("Save config"),NULL);
			AddMenuString(&mFile,UPMENU_ID+2,_l("Load config"),NULL);
			mFile.AppendMenu(MF_SEPARATOR, 0, "");
			AddMenuString(&mFile,UPMENU_ID+3,_l("Exit "PROGNAME),NULL);
			AddMenuSubmenu(pSysMenu,&mFile,_l("File"),NULL);
		}
		if(hQRun_Topic){// QRun
			CMenu mQRun;
			mQRun.CreatePopupMenu();
			AddMenuString(&mQRun,UPMENU_ID+5,_l("New file item"),NULL);
			AddMenuString(&mQRun,UPMENU_ID+6,_l("New folder item"),NULL);
			AddMenuString(&mQRun,UPMENU_ID+7,_l("From running program"),NULL);
			AddMenuString(&mQRun,UPMENU_ID+17,_l("New Internet address"),NULL);
			AddMenuString(&mQRun,UPMENU_ID+19,_l("New empty item"),NULL);
			AddMenuString(&mQRun,UPMENU_ID+4,_l("New launch set"),NULL);
			AddMenuString(&mQRun,UPMENU_ID+21,_l("Add Default items"),NULL);
			AddMenuSubmenu(pSysMenu,&mQRun,_l("Quick-runs"),NULL);
		}
		if(hQMac_Topic){// Macros
			CMenu mMacros;
			mMacros.CreatePopupMenu();
			AddMenuString(&mMacros,UPMENU_ID+8,_l("New text macro item"),NULL);
			AddMenuString(&mMacros,UPMENU_ID+9,_l("New macro from file"),NULL);
			AddMenuString(&mMacros,UPMENU_ID+10,_l("New insertion macro"),NULL);
			AddMenuString(&mMacros,UPMENU_ID+11,_l("New application macro"),NULL);
			AddMenuString(&mMacros,UPMENU_ID+18,_l("Record a macro"),NULL);
			AddMenuString(&mMacros,UPMENU_ID+12,_l("Download macros"),NULL);
			//AddMenuString(&mMacros,UPMENU_ID+21,_l("Default macros"),NULL);

			AddMenuSubmenu(pSysMenu,&mMacros,_l("Macros"),NULL);
		}
		CStringArray(sLangs);
		ConvertComboDataToArray(pwk->sLangsNames,sLangs);
		if(sLangs.GetSize()>1){// Language
			CMenu mLang;
			mLang.CreatePopupMenu();
			for(int i=0;i<sLangs.GetSize();i++){
				int iID=UPMENU_ID+100+i;
				AddMenuString(&mLang,iID,sLangs[i],0,pwk->vGetApplicationLang()==i);
			}
			AddMenuSubmenu(pSysMenu,&mLang,_l("Language"),NULL);
		}
		{// Help
			CMenu mHelp;
			mHelp.CreatePopupMenu();
			if(GetApplicationLang()==1){
				AddMenuString(&mHelp,UPMENU_ID+22,_l("Introduction"),NULL);
			}
			AddMenuString(&mHelp,UPMENU_ID+13,_l("Help topics"),NULL);
			AddMenuString(&mHelp,UPMENU_ID+20,_l("About program"),NULL);
#ifndef _DEBUG
			if(psk->iLikStatus<2)
#endif
			{
				AddMenuString(&mHelp,UPMENU_ID+14,_l("Order "PROGNAME),NULL);
			}
			AddMenuString(&mHelp,UPMENU_ID+15,_l("Download plugins"),NULL);
			AddMenuSubmenu(pSysMenu,&mHelp,_l("Help"),NULL);
		}
		pData->SetMenu(pSysMenu);
	}
	if(psk->bDskTutorial==0 && GetApplicationLang()==1)
	{
		psk->bDskTutorial=1;
		ShowIntroduction(pDialog);
	}
	return TRUE;
}

BOOL CALLBACK AddOptionsToDialog(CDLG_Options* pData);
BOOL CALLBACK ApplyOptionsFromDialog(BOOL bSaveAndClose,DWORD dwRes,CDLG_Options* dlgOpt)
{
	int iRes=pwk->ApplyOptionsFromDialog(bSaveAndClose,dwRes,dlgOpt);
	GetApplicationLang()=pwk->vGetApplicationLang();
	return iRes;
}

BOOL WINAPI ExternalEnterFind(const char* szTitle, CString& sText,CWnd* pDialog)
{
	return pwk->ExternalEnterFind(szTitle, sText, pDialog);
}

BOOL OpenOptionsDialog(int iTab,const char* szDefTopic, CWnd* pParentWnd, FP_OPTIONACTION fpPostActionType, LPVOID pPostActionParam)
{
	if(psk->lStartupDesktop!=0){
		pwk->vAlert(_l("Sorry, options can be edited on the main desktop only")+"!\n"+_l("And changes will not affect other desktops until reboot"),_l(PROGNAME": Warning"));
		return 1;
	}
	if(psk->lOptionsDialogOpened){
		return 0;
	}
	SimpleTracker Track(psk->lOptionsDialogOpened);
	SimpleTracker Track2(psk->bDoModalInProcess);
	// Подготавливаем ----------------------------------
	if(psk->sCodeWord!=""){
		CString sText;
		BOOL bRet=0;
		pwk->CreateMlineEdit(pParentWnd,1,sText,_l("Configuration protection"));
		if(sText!=psk->sCodeWord){
			pwk->vAlert(_l("Access denied"),_l(PROGNAME": Warning"));
			Sleep(3000);
			bRet=1;
		}
		if(bRet){
			return bRet;
		}
	}
	// Готовим структуру диалога...
	CDLG_Options_Startup pStartupInfo;
	pStartupInfo.fpInitOptionList=AddOptionsToDialog;
	pStartupInfo.fpApplyOptionList=ApplyOptionsFromDialog;
	pStartupInfo.fpUpMenuAction=ProcessUpMenu;
	pStartupInfo.iStickPix=psk->iStickPix;
	pStartupInfo.szRegSaveKey=PROG_REGKEY"\\OptionsWindowPosition";
	pStartupInfo.pImageList=&pwk->theApp->MainImageList;
	CString sOptionWndTitle=CString(PROGNAME)+": "+(iTab==2?PLUGINS_TOPIC:(iTab==1?DEFQRUN_TOPIC:_l("Main window")));
	pStartupInfo.szWindowTitle=sOptionWndTitle;
	pStartupInfo.sz1stTopic="";//iTab?DEFQRUN_TOPIC:_l("Options");
	pStartupInfo.iWindowIcon=0;
	pStartupInfo.hWindowIcon=pwk->vLoadIcon(IDR_MAINICO);
	pStartupInfo.szDefaultTopicTitle=szDefTopic;
	pStartupInfo.fpPostAction=fpPostActionType;
	pStartupInfo.pPostActionParam=pPostActionParam;
	pStartupInfo.szQHelpByDefault=_l("Choose topic from the left pane. Double-click option value in the right pane to change it");
	CFakeWindow tmpWin(sOptionWndTitle,WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT);//WS_EX_TOPMOST|WS_EX_APPWINDOW//|WS_EX_TOPMOST
	CDLG_Options dlgOpt(pStartupInfo,tmpWin.GetWnd(),1);
	dlgOpt.iTab=iTab;
	dlgOpt.bShowFind=2;
	dlgOpt.bAutoSkipEmptyPanes=1;
	dlgOpt.fpExternalSearch=ExternalEnterFind;
	dlgOpt.m_bSelInBold=0;
	dlgOpt.lSipleActionIconNum=54;
	if(psk->bStartWithOptions){
		dlgOpt.bBlockCancel=1;
	}
	psk->pOpenedOptions=&dlgOpt;
	// Вызываем
	DWORD dwRes=dlgOpt.DoModal();
	DWORD dwErr=GetLastError();
	if(dlgOpt.sCurrentOptionFromLPane!=""){
		psk->sLastOpenedOptionsTopic=dlgOpt.sCurrentOptionFromLPane;
	}
	psk->pOpenedOptions=0;
	SimpleTracker Track3(psk->lPreventTrayMenu);
	return 1;
}

#define EXECRYPTOR_SALT	"WK999"
void GetRegInfoFromText(CString &sText,CString& sLikUser,CString& sLikKey, const char* szSalt)
{
	if(szSalt==0){
		szSalt=EXECRYPTOR_SALT;
	}
	sText+="\n";
	if(sText.Find("WPK")!=-1){
		sLikUser=CDataXMLSaver::GetInstringPart("Registration name","\n",sText);
		sLikUser.TrimLeft(" \n\r><\\/,;:\t");
		sLikUser.TrimRight(" \n\r><\\/,;:\t");
		sLikKey=CDataXMLSaver::GetInstringPart("Registration key","\n",sText);
		if(sLikKey==""){
			sLikKey=CDataXMLSaver::GetInstringPart("Serial number","\n",sText);
		}
		sLikKey.TrimLeft(" \n\r><\\/,;:\t");
		sLikKey.TrimRight(" \n\r><\\/,;:\t");
	}else{
		sLikUser=CDataXMLSaver::GetInstringPart("Your registration name is:","\n",sText);
		if(sLikUser==""){
			sLikUser=CDataXMLSaver::GetInstringPart("Registration name:","\n",sText);
		}
		sLikUser.TrimLeft(" \n\r><\\/,;:\t");
		sLikUser.TrimRight(" \n\r><\\/,;:\t");
		sLikUser.Replace(szSalt,"");
		sLikKey=CDataXMLSaver::GetInstringPart("begin","end",sText);
		if(sLikKey==""){
			sLikKey=CDataXMLSaver::GetInstringPart("Serial number","\n",sText);
		}
		sLikKey.Replace("\n","");
		sLikKey.Replace("\r","");
		sLikKey.Replace("\t","");
		sLikKey.Replace("-","");
		sLikKey.Replace(" ","");
		sLikKey.Replace(">","");
		sLikKey.Replace("<","");
		sLikKey.TrimLeft(" \n\r><\\/,;:\t");
		sLikKey.TrimLeft(" \n\r><\\/,;:\t");
	}
}

CWnd* AfxWPGetMainWnd()
{
	if(pwk){
		return pwk->pMainWnd;
	}
	return 0;
}

BOOL OpenFileDialog(const char* szExtension, LPVOID lpData, CDLG_Options* pDialog, BOOL bSkipFileCheck, BOOL bWithCString, BOOL bOpenDialog)
{
	if(bFileDialogOpened){
		return FALSE;
	}
	if(lpData==NULL){
		return FALSE;
	}
	CString sFile;
	HIROW hData=NULL;
	CString* sOutFileName=NULL;
	if(bWithCString){
		sOutFileName=(CString*)lpData;
		sFile=*sOutFileName;
	}else{
		hData=(HIROW)lpData;
		sFile=pDialog->m_OptionsList.GetIItemText(hData,1);
	}
	BOOL bRes=FALSE;
	bFileDialogOpened=TRUE;
	OleInitialize(0);
	CFileDialog dlg(bOpenDialog, NULL, bSkipFileCheck?((const char*)NULL):(sFile), OFN_NODEREFERENCELINKS, szExtension, pDialog);//OFN_EXPLORER|OFN_ENABLESIZING|
	SimpleTracker Track(psk->bDoModalInProcess);
	if(dlg.DoModal()==IDOK){
		bRes=TRUE;
		CString sNewFile=dlg.GetPathName();
		if(pDialog && hData){
			pDialog->m_OptionsList.SetIItemText(hData,1,sNewFile);
			FLNM_ROWSTATE flnmRowState;
			flnmRowState.bCollapsed= FALSE;
			flnmRowState.dwData= 0;
			flnmRowState.strText=sNewFile;
			flnmRowState.hIRow= hData;
			flnmRowState.iIItem=1;
			LRESULT result;
			pDialog->OnEndEdit((NMHDR*)(&flnmRowState), &result);
		}else if(sOutFileName){
			*sOutFileName=sNewFile;
		}
	}
	bFileDialogOpened=FALSE;
	if(!bRes && !bSkipFileCheck){
		// Пробуем без стартового имени файла...
		DWORD dwErr=CommDlgExtendedError();
		if(dwErr==12290){
			return OpenFileDialog(szExtension, lpData, pDialog, TRUE, bWithCString);
		}
	}
	return bRes;
}

const char* GetAppVersion()
{
	return pwk->GetAppVers();
}

int CountPluginOption(CPlugin* plg,DWORD dwPosition)
{
	int iRes=0;
	if(!plg){
		return iRes;
	}
	CString sName;
	for(int i=0;i<(plg->options.GetSize());i++){
		CPluginOption* pValue=plg->options[i];
		if(pValue && pValue->m_pOption && pValue->m_dwOptionPosition==dwPosition){
			iRes++;
		}
	}
	return iRes;
}

BOOL IsVistaSoundControl()
{
	return IsVista();
}