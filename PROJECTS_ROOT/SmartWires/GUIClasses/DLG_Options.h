#if !defined(AFX_DLG_OPTIONS_H__051E3A1C_BE52_4AED_8293_78C8C6B37A65__INCLUDED_)
#define AFX_DLG_OPTIONS_H__051E3A1C_BE52_4AED_8293_78C8C6B37A65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/*
¬озможные дефайны:
_WIREKEYS
OPTIONS_TOOLBAR
*/

#include "systemtr.h"
#include "IListCtrl.h"
#include "..\SmartWndSizer\SmartWndSizer.h"
#include "..\SystemUtils\SupportClasses.h"
#include "..\BitmapUtils\BMPUtil.h"
#define DIALOG_MINXSIZE	300
#define DIALOG_MINYSIZE	200
#define OPTIONS_TP			"Preferences"
#define REMOVE_OPTIONS		WM_USER+0
#define UPMENU_ID			WM_USER+10
#define EDITITEM			WM_USER+11
// DLG_Options.h : header file
//
#define OPTTYPE_BOOL	1
#define OPTTYPE_STRING	2
#define OPTTYPE_NUMBER	3
#define OPTTYPE_COMBO	4
#define OPTTYPE_HOTKEY	5
#define OPTTYPE_ACTION	6
#define OPTTYPE_COLOR	7
#define OPTTYPE_FONT	8
#define OPTTYPE_DATE	9
//
class CDLG_Options;
typedef BOOL (CALLBACK *FP_APPOPTIONS) (BOOL bSaveAndClose, DWORD dwRes, CDLG_Options* pDialog);
typedef BOOL (CALLBACK *FP_ADDOPTIONS) (CDLG_Options* pDialog);
typedef BOOL (CALLBACK *FP_UPMENUACTION) (WPARAM wParam, LPARAM lParam, CDLG_Options* pDialog);
typedef BOOL (CALLBACK *FP_OPTIONACTION) (HIROW hData, CDLG_Options* pDialog);// ƒожно вернуть значение TRUE если что-то изменилось
typedef BOOL (CALLBACK *FP_OPTIONACTION2) (LPVOID hData, CDLG_Options* pDialog);// ƒожно вернуть значение TRUE если что-то изменилось
typedef BOOL (CALLBACK *FP_OPTIONENTERTEXT) (const char* szTitle, CString& sText,CWnd*);

CString getHotKeyStr(CIHotkey oHotKey, BOOL bWithFollowers=FALSE);
CString getHotKeyStrAlt(CIHotkey oHotKey,CString sPrefix);
//
class COption
{
public:
	long iOptionClass;
	long iOptionType;
	HIROW hIRow;
	CString sDsc;
	COption(long i, long iClass=0)
	{
		hIRow=NULL;
		iOptionType=i;
		iOptionClass=iClass;
	}
	virtual ~COption(){};
	virtual CString GetDsc()
	{
		return sDsc;
	};
};

class COptionDate:public COption
{
public:
	virtual ~COptionDate()
	{
	};
	COptionDate(COleDateTime Value_Def, COleDateTime* Value_Real, long iClass=0):COption(OPTTYPE_DATE, iClass)
	{
		m_Value_Def=Value_Def;
		m_Value_Real=Value_Real;
		m_Value=*Value_Real;
	}
	COleDateTime m_Value;
	COleDateTime m_Value_Def;
	COleDateTime* m_Value_Real;
	virtual CString GetDsc()
	{
		CString sReply="";//Format("%s\n%s: %s (%s: %s)",sDsc,_l("Currently"),m_Value?_l("Yes"):_l("No"),_l("Options"),_l("Yes/No"));
		if(m_Value!=m_Value_Def){
			sReply+=m_Value.Format("%x");
		}
		return sReply;
	};
};

class COptionBOOL:public COption
{
public:
	virtual ~COptionBOOL()
	{
	};
	COptionBOOL(long Value_Def, long* Value_Real, long iClass=0):COption(OPTTYPE_BOOL, iClass)
	{
		m_Value_Def=Value_Def;
		m_Value_Real=Value_Real;
		m_Value=*Value_Real;
	}
	long m_Value;
	long m_Value_Def;
	long* m_Value_Real;
	virtual CString GetDsc()
	{
		CString sReply="";//Format("%s\n%s: %s (%s: %s)",sDsc,_l("Currently"),m_Value?_l("Yes"):_l("No"),_l("Options"),_l("Yes/No"));
		if(m_Value!=m_Value_Def){
			sReply+=Format("%s: %s",_l("Default"),m_Value_Def?_l("Yes"):_l("No"));
		}
		return sReply;
	};
};

class COptionString:public COption
{
public:
	virtual ~COptionString()
	{
	};
	COptionString(CString Value_Def, CString* Value_Real, long iClass=0, BOOL bPsw=FALSE):COption(OPTTYPE_STRING, iClass)
	{
		m_Value_Def=Value_Def;
		m_Value_Real=Value_Real;
		m_Value=*Value_Real;
		bPassword=bPsw;
	}
	BOOL bPassword;
	CString m_Value;
	CString m_Value_Def;
	CString* m_Value_Real;
	virtual CString GetDsc()
	{
		if(bPassword){
			return CString('*',m_Value.GetLength());
		}
		CString sReply=(m_Value!=""?(_l("Value")+": "+m_Value):"");
		if(sDsc!=""){
			if(sReply!=""){
				sReply+="\n";
			}
			sReply+=sDsc;
		}
		if(m_Value!=m_Value_Def && m_Value_Def!=""){
			sReply+=Format("; %s: %s",_l("Default"),m_Value_Def);
		}
		return sReply;
	};
};

class COptionNumber:public COption
{
public:
	virtual ~COptionNumber()
	{
	};
	COptionNumber(long Value_Def, long* Value_Real, long dwMin, long dwMax, long iClass=0):COption(OPTTYPE_NUMBER, iClass)
	{
		m_Value_Def=Value_Def;
		m_Value_Real=Value_Real;
		m_Value=*Value_Real;
		m_dwMin=dwMin;
		m_dwMax=dwMax;
	}
	long m_Value;
	long m_Value_Def;
	long* m_Value_Real;
	long m_dwMin;
	long m_dwMax;
	virtual CString GetDsc()
	{
		CString sReply=_l("Value")+": "+Format("%i (%s: %i, %s: %i)",m_Value, _l("Min"),m_dwMin,_l("Max"),m_dwMax);
		if(sDsc!=""){
			sReply+="; ";
			sReply+=sDsc;
		}
		if(m_Value!=m_Value_Def){
			sReply+=Format("; %s: %i",_l("Default"),m_Value_Def);
		}
		return sReply;
	};
};

class COptionCombo:public COption
{
public:
	virtual ~COptionCombo()
	{
	};
	COptionCombo(long Value_Def, long* Value_Real, const char* szComboData, long iClass=0):COption(OPTTYPE_COMBO, iClass)
	{
		m_Value_Def=Value_Def;
		m_Value_Real=Value_Real;
		m_Value=*Value_Real;
		m_ComboData=szComboData;
	}
	long m_Value;
	long m_Value_Def;
	long* m_Value_Real;
	CString m_ComboData;
	virtual CString GetDsc()
	{
		CStringArray sItemsData;
		ConvertComboDataToArray(m_ComboData,sItemsData);
		CString sReply=_l("Value")+": "+sItemsData[m_Value]+Format(" (%s: %s)",_l("Options"),m_ComboData);
		sReply.Replace("\t",", ");
		if(sDsc!=""){
			sReply+="; ";
			sReply+=sDsc;
		}
		if(m_Value!=m_Value_Def){
			sReply+=Format("; %s: %s",_l("Default"),sItemsData[m_Value_Def]);
		}
		return sReply;
	};
};

class COptionHotKey:public COption
{
public:
	virtual ~COptionHotKey()
	{
	};
	COptionHotKey(CIHotkey Value_Def, CIHotkey* Value_Real, long iClass=0):COption(OPTTYPE_HOTKEY, iClass)
	{
		m_Value_Def=Value_Def;
		m_Value_Real=Value_Real;
		if(Value_Real){
			m_Value=*Value_Real;
		}
	}
	virtual CString GetDsc()
	{
		CString sReply=GetIHotkeyName(m_Value,TRUE);
		if(sReply!=""){
			sReply=sDsc+": "+sReply;
		}else{
			sReply=sDsc;
		}
		if(m_Value!=m_Value_Def && m_Value_Def.Present()){
			sReply+=Format("; %s: %s",_l("Default"),GetIHotkeyName(m_Value_Def));
		}
#ifdef _DEBUG
		sReply+=Format("\n<Debug>DWORD=%lu, str='%s'</Debug>",m_Value.GetAsDWORD(),m_Value.GetForSave());
#endif
		return sReply;
	};
	CIHotkey m_Value;
	CIHotkey m_Value_Def;
	CIHotkey* m_Value_Real;
};

class COptionAction:public COption
{
public:
	virtual ~COptionAction()
	{
	};
	COptionAction(FP_OPTIONACTION fp,FP_OPTIONACTION2 fp2, LPVOID pData, long iClass=0):COption(OPTTYPE_ACTION, iClass)
	{
		function=fp;
		function2=fp2;
		functionParam=pData;
		lIcon=0;
	}
	FP_OPTIONACTION function;
	FP_OPTIONACTION2 function2;
	LPVOID functionParam;
	long lIcon;
};

class COptionColor:public COption
{
public:
	virtual ~COptionColor()
	{
	};
	COptionColor(long Value_Def, long* Value_Real, long iClass=0):COption(OPTTYPE_COLOR, iClass)
	{
		m_Value_Def=Value_Def;
		m_Value_Real=Value_Real;
		m_Value=*Value_Real;
	}
	long m_Value;
	long m_Value_Def;
	long* m_Value_Real;
};

class COptionFont:public COption
{
public:
	virtual ~COptionFont()
	{
	};
	COptionFont(CFont** Value_Real, long iClass=0):COption(OPTTYPE_FONT, iClass)
	{
		m_Value_Real=Value_Real;
	}
	CFont** m_Value_Real;
};


/////////////////////////////////////////////////////////////////////////////
// CDLG_Options dialog
class CDLG_Options_Startup
{
public:
	CDLG_Options_Startup()
	{
		fpPostAction=0;
		pPostActionParam=0;
		bExpandByDef=FALSE;
		szRegSaveKey="";
		sz1stTopic=OPTIONS_TP;
		szDefaultTopicTitle="";
		szWindowTitle=OPTIONS_TP;
		szByDefButtonText="Additionally";
		szOkButtonText="OK";
		szCancelButtonText="Cancel";
		szHelpButtonText="Help";
		fpApplyOptionList=NULL;
		fpInitOptionList=NULL;
		fpUpMenuAction=NULL;
		pImageList=NULL;
		iStickPix=10;
		iWindowIcon=0;
		bTreeOnRight=0;
		hWindowIcon=0;
	}
	FP_ADDOPTIONS fpInitOptionList;
	FP_APPOPTIONS fpApplyOptionList;
	FP_UPMENUACTION fpUpMenuAction;
	CImageList* pImageList;
	int iWindowIcon;
	HICON hWindowIcon;
	DWORD iStickPix;
	BOOL bExpandByDef;
	CString sz1stTopic;
	CString szQHelpByDefault;
	CString szWindowTitle;
	CString szRegSaveKey;
	CString szByDefButtonText;// NULL - не нужна
	CString szOkButtonText;// NULL - не нужна
	CString szCancelButtonText;// NULL - не нужна
	CString szHelpButtonText;// NULL - не нужна
	CString szDefaultTopicTitle;
	CString szTooltipPostfix;
	FP_OPTIONACTION fpPostAction;
	LPVOID pPostActionParam;
	BOOL bTreeOnRight;
};

class CDLG_Options : public CDialog
{
// Construction
public:
	int iTab;
	BOOL IniIList();
	BOOL bNoGridOnRPanel;
	BOOL bAutoSkipEmptyPanes;
	BOOL bHHideAddit;
	BOOL bHHideHelp;
	DWORD dwFlg;
	CSize sSqBt;
	long lSipleRemindIconNum;
	long lSipleActionIconNum;
	BOOL m_bSelInBold;
	BOOL bManualPreventCommit;
	BOOL DrawNC();
	FP_OPTIONENTERTEXT fpExternalSearch;
	BOOL bBlockCancel;
	BOOL bRPaneOnly;
	BOOL bShowFind;
	CStringArray aCommonStrings;
	CDWordArray m_ChangedClasses;
	CString sCurrentOptionFromLPane;
	CArray<COption*,COption*> aOptions;
	CArray<COptionAction*,COptionAction*> aMenuOptions;
	CMap<HIROW,HIROW&,HIROW,HIROW&> aFromRtoL;
	//---------------
	CString sHelpPage;
	BOOL bAnyHotkeyWasChanged;
	HIROW hLastOpenedRPane;
	long isDialogInProcess;
	BOOL bDoNotSavePanePos;
	HIROW hLPaneRoot;
	HIROW hTopLPaneItem;
	int MainRulePos;
	HIROW hCurButtonsBarIRow;
	BOOL PushButtonsRow(HIROW hRow);
	COptionAction* GetMenuAction(HIROW hRightPane, int iCount, BOOL bNoIconOnly, int* iOutNumber=0);
	BOOL bShowColorForColorOptions;
	BOOL bParentOnTop;
	CWnd* pParentWnd;
	LRESULT SaveColumnWidths();
	LRESULT LoadColumnWidths();
	BOOL OnChanged(COption* pOpt);
	BOOL AddChanged(int iClass);
	HIROW hDefaultTopic;
	CImageList m_OptionsListImages;
	CImageList m_OptionsListImages2;
	HIROW SetLAliasTarget(HIROW hLPaneFrom,HIROW hRPaneTo);
	void GetOptionDsc(HIROW hIRow, int iCol, CString& sText, CString& sPopupText);
	HIROW GetOrAddAlias(HIROW hRPane, const char* szText=NULL, DWORD dwIcon=0, HIROW fRParent=NULL, HIROW hAfter=0);
	HIROW GetAliasHIROW(HIROW hRPane);
	BOOL CompareToDefTopic(const char* szText);
	long dwOptionCount;
	void OnStartEdit(NMHDR * pNotifyStruct, LRESULT * result);
	void OnEndEdit(NMHDR * pNotifyStruct, LRESULT * result);
	void OnCustomDialog(NMHDR * pNotifyStruct, LRESULT * result);
	void OnComboExpand(NMHDR * pNotifyStruct, LRESULT * result);
	virtual void CommitData(int iOptionClass=-1);
	void OnReset();
	void PushSomeDataOnScreen(long iOptionClass=-1);
	virtual void PopSomeDataFromScreen(long iOptionClass);
	virtual BOOL isOptionClassChanged(long iOptionsClass);
	BOOL PushOptionOnScreen(int iOptionIndex);
	void Option_SetRowColor(HIROW fRow, int iItem, int iColType=95);
	int Option_AddMenuActionEx(const char* szText, FP_OPTIONACTION fp, FP_OPTIONACTION2 fp2, LPVOID pActionData, HIROW hTargetItem, BOOL bPlaceAtTop, long lIcon);
	int  Option_AddMenuAction(const char* szText, FP_OPTIONACTION fp, HIROW hParam, HIROW hTargetItem=NULL, BOOL bPlaceAtTop=FALSE, long lIcon=0);
	virtual BOOL MoveGlobalFocus(HIROW hRPane, BOOL bExpand=TRUE,BOOL bPreventFocusStealing=FALSE);
	void AdjustRPaneW(BOOL bTwicked=0);
	virtual BOOL CallApply(DWORD dwRes, BOOL bSaveAndExit);
	BOOL MyEndDialog(DWORD dwRes);
	void InitExtraData();
	BOOL SetButtons();
	void OnAdditionalEx(BOOL,BOOL bType);
	//
	HIROW Option_AddCombo(HIROW hParent, const char* szText, long* pIntVariable, long iDefValue=0, const char* szComboItems="Nothing", long iOptionsClass=0);
	HIROW Option_AddAction(HIROW fParent, const char* szButtonText, FP_OPTIONACTION fp, HIROW hActionData, long iOptionsClass=0, int iNewIcon=-1);
	HIROW Option_AddAction2(HIROW fParent, const char* szButtonText, FP_OPTIONACTION2 fp, LPVOID pActionData,  long iOptionsClass=0, int iNewIcon=-1);
	HIROW Option_AddActionEx(HIROW fParent, const char* szButtonText, FP_OPTIONACTION fp, FP_OPTIONACTION2 fp2, LPVOID pActionData, int iIcon, long iOptionsClass=0);
	HIROW Option_AddActionToEdit(HIROW fParent, FP_OPTIONACTION fp, long iOptionsClass=0);
	HIROW Option_AddActionToEdit(HIROW fParent, FP_OPTIONACTION fp, HIROW hActionData, long iOptionsClass=0, int iNewIcon=-1);
	HIROW Option_AddString(HIROW fParent, const char* szText, CString* pStrVariable, const char* sDefValue="", long iOptionsClass=0);
	HIROW Option_AddPassword(HIROW fParent, const char* szText, CString* pStrVariable, const char* sDefValue="", long iOptionsClass=0);
	HIROW Option_AddNString(HIROW fParent, const char* szText, long* pNStrVariable, long dwDefValue=0, long dwMin=0, long dwMax=100, long iOptionsClass=0);
	HIROW Option_AddNSlider(HIROW fParent, const char* szText, long* pNStrVariable, long dwDefValue=0, long dwMin=0, long dwMax=100, long iOptionsClass=0);
	HIROW Option_AddHotKey(HIROW fParent, const char* szText, CIHotkey* pHotKeyVariable, CIHotkey dwDefValue=0, long iOptionsClass=0, int iIcon=-1);
	HIROW Option_AddBOOL(HIROW fParent, const char* szText, long* pBoolVariable, long dwDefValue=0, long dwIcon=0, long iOptionsClass=0);
	HIROW Option_AddSBOOL(HIROW fParent, const char* szText, CString* pBoolVariable, long dwDefValue=0, long dwIcon=0, long iOptionsClass=0);
	HIROW Option_AddBOOL_Plain(HIROW fParent, const char* szText, long* pBoolVariable, long dwDefValue=0, long dwIcon=0, long iOptionsClass=0);
	HIROW Option_AddColor(HIROW fParent, const char* szText, long* pNStrVariable, long dwDefValue=0, long iOptionsClass=0, int iNewIcon=-1);
	HIROW Option_AddFont(HIROW fParent, const char* szText, CFont** pFontVariable, long iOptionsClass=0, int iNewIcon=-1);
	HIROW AddRow(HIROW fParent, const char* szText, long dwIconNum, BOOL bAtStart=FALSE);
	HIROW Option_AddTitle(HIROW fParent, const char* szText, DWORD dwIcon=5);
	HIROW Option_AddHeader(HIROW fParent, const char* szText, DWORD dwIcon=5, BOOL bWithoutAlias=FALSE, HIROW hAfter=0);
	HIROW Option_AddTitleEx(HIROW fParent, const char* szText, DWORD dwIcon, BOOL bWithAlias, HIROW hAfter=0);
	HIROW Option_AddAlias(HIROW hThis, const char* szText, DWORD dwIcon, HIROW hParent, BOOL bToBeginning=FALSE);
	HIROW Option_AddDate(HIROW fParent, const char* szText, COleDateTime* pDateVariable, COleDateTime dtDefValue, long dwIcon=0, long iOptionsClass=0);
	//
	void DestroyOptions(BOOL bTemp);
	BOOL FinishFillOptionListWithData();
	void ResetOption(COption* pOpt);
	BOOL FillOptionListWithData();
	CDLG_Options_Startup* m_pStartupInfo;
	void InitAll(CDLG_Options_Startup& pStartupInfo);
	CDLG_Options(CDLG_Options_Startup& pStartupInfo, CWnd* pParent = NULL);   // standard constructor
	CDLG_Options(CDLG_Options_Startup& pStartupInfo, CWnd* pParent, DWORD dwFlg);   // standard constructor
	void ProcessEveryFolder(HIROW hRoot, BOOL bCollapse, BOOL bParLen=0);
	COption* GetOptionUnderCursor(int* iOptionIndex=NULL);
	void SetQHelpText(CString szText);
	~CDLG_Options();
	CSmartWndSizer Sizer;

	// Dialog Data
	//{{AFX_DATA(CDLG_Options)
	CIListCtrl	m_OptionsList;
	CIListCtrl	m_LList;
#ifdef OPTIONS_TOOLBAR
	CBitmapStatic m_Toolbar1;
	CBitmapStatic m_Toolbar2;
	CBitmapStatic m_Toolbar3;
	CBitmapStatic m_Toolbar4;
	CBitmapStatic m_Toolbar5;
	CBitmapStatic m_Toolbar6;
#endif
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_Options)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_tooltip;
	void ExecuteAction(COptionAction* act);
	// Generated message map functions
	//{{AFX_MSG(CDLG_Options)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void CancelThis();
	afx_msg void OnHelp();
	afx_msg void OnApply();
	afx_msg void OnCollapseAll();
	afx_msg void OnResetCurOption();
	afx_msg void OnExpandAll();
	afx_msg void OnAdditional();
	afx_msg void OnAction1();
	afx_msg void OnAction2();
	afx_msg void OnTOOLBAR1();
	afx_msg void OnTOOLBAR2();
	afx_msg void OnTOOLBAR3();
	afx_msg void OnTOOLBAR4();
	afx_msg void OnTOOLBAR5();
	afx_msg void OnTOOLBAR6();
	afx_msg void OnFind();
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
CString GetTooltipText(CToolTipCtrl* tl, CWnd* pWnd);
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_DLG_OPTIONS_H__051E3A1C_BE52_4AED_8293_78C8C6B37A65__INCLUDED_)
