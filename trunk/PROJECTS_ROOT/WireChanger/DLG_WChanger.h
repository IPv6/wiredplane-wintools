#if !defined(AFX_DLG_WCHANGER_H__5FE48A37_7BCE_4B41_9D0D_9280B19C557A__INCLUDED_)
#define AFX_DLG_WCHANGER_H__5FE48A37_7BCE_4B41_9D0D_9280B19C557A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CWPPreview
class CDLG_WChanger;
class CWPPreview : public CButton
{
	DECLARE_DYNAMIC(CWPPreview)
	CDLG_WChanger* m_pWPDialog;
public:
	CString sOverwriteInfo;
	CString sImgOverlay;
	CWPPreview(CDLG_WChanger* pWPDialog);
	virtual ~CWPPreview();
	//{{AFX_VIRTUAL(InfoWnd)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL
protected:
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDLG_WChanger dialog
#include "..\SmartWires\SmartWndSizer\SmartWndSizer.h"
#include "..\SmartWires\GUIClasses\IListCtrl.h"
class CDLG_WChanger : public CDialog
{
// Construction
public:
	HIROW hLastSelected;
	BOOL SingleSwitchTemplate(CString sTempl,BOOL bNew);
	void On0Tab();
	void On1Tab();
	int iTemplatesInit;
	CString sCurrentWidget;
	long lWndMode;
	void SetMainMenu();
	int iNotesIndex;
	void RefreshNotesMenu();
	int dwFirstTheme;
	long lTemplDupIndes;
	long lTemplDowIndes;
	HIROW hDownButtonField;
	void OnExpandWList(NMHDR * pNotifyStruct, LRESULT * result);
	BOOL bForceSelWallpaper;
	char sImageToBeSetAfterClose[MAX_PATH];
	long lZipLock;
	void OnChangeAnim();
	void OnPackIntoArch();
	BOOL CloseThisWindow();
	BOOL bNeddRefreshAfterClose;
	CString sPopTip;
	CMenu* mainMenu;
	WPaper* wpNowShowingTheme;
	BOOL UpdateThemeWeightVal(WPaper* wpTheme,int iNewVal);//iNewVal<0 - From control
	int OnWchangerMoveToFolder(DWORD dwItem,DWORD dwToTheme);
	void RefreshTemplates(int iAtIndex=-1);
	void OnWchangerImportList();
	void OnWchangerBackupAll();
	void OnWchangerRestoreAll();
	long lSortLock;
	BOOL FillInitialData();
	void SetInfoBlockAlert(const char* szText);
	void SetInfoBlock(const char* szText, DWORD dwLevel=0);
	CString GetFirstFolderImagePath(DWORD dwTmemeIndex);
	void SetThemeNameInTitle(WPaper* wp);
	WPaper* GetItemUnderCursor(BOOL bType=0);
	BOOL OpenFileForView(CString sPath);
	void RefreshThemeTitle(int dwThemeIndex);
	void SetHours();
	int GetFirstThemeImage(int iThemeIndex, int iStart=0);
	void GetNextThemeImage(int iThemeIndex, int& iPos);
	void SwapThemeImage(int iPos1,int iPos2);
	BOOL bDialogInitOk;
	int OnWchangerDeleteallFromFolder(BOOL bInvalidOnly,WPaper** wpThemeOut=NULL);
	int OnWchangerDeleteallFromFolder(BOOL bInvalidOnly,int& dwTheme,WPaper** wpThemeOut=NULL);
	void SetLastWPSetted(int iNewValue, BOOL bSelectNew=TRUE);
	LOGFONT logfont;
	CFont* font;
	HBITMAP hPreviewBmp;
	CString sWallpaperFile;
	CString sPreparedWallpaperFile;
	CSize sizeOfRealImage;
	void DrawPreview(HDC hDC);
	void AddFolder(BOOL bNewOnly, BOOL bRecurse, const char* szFolderFrom=NULL, BOOL bAutoCreateThemes=0);
	int AddFolder(WPaper* wp, BOOL bRecurse, int dwTheme, BOOL bNewOnly, CString& szFolderFrom,BOOL bAutoCreateThemes=0);
	CCriticalSection cWDlgSection;
	long lLoadInProgress;
	BOOL bDataLoaded;
	~CDLG_WChanger();
	CRect rcPreviewBounds;
	BOOL bLastDrawPicRes;
	int iLastWPSetted;//Номер обоя из aWPapersAndThemes
	void OnSelChanged(NMHDR * pNotifyStruct, LRESULT * result);
	void OnWidgetSelChanged(NMHDR * pNotifyStruct, LRESULT * result);
	void OnWidgetCTriggerChanged(NMHDR * pNotifyStruct, LRESULT * result);
	void OnWidgetCustomDialog(NMHDR * pNotifyStruct, LRESULT * result);
	void UpdateThemeControls(int iThemeIndex);
	WPaper* Attach(int ind, BOOL bSkipParentUpdate=FALSE, BOOL bNeedPrevUpdate=FALSE);
	void SortThemeImages(int iSortType);
	void FillListWithData();
	void OnEndEditWList(NMHDR * pNotifyStruct, LRESULT * result);
	int iFirstIndex;
	CDWordArray aThemesCount;
	long lThemesWTotal;
	int Apply(BOOL bSilent=FALSE);
	BOOL AddFile(const char* szPath="", DWORD dwTheme=0, BOOL bSkipParentUpdate=FALSE, WPaper** wpOut=0, int* iIndexOut=0);
	int AddThemeFolder(const char* szTitle, DWORD dwStatus=1);
	void AddArchiveToArray(const char* sFile,CStringArray& aNames);
	BOOL OnRclickWlist(BOOL bType=0, CPoint posT=CPoint(0,0));
	BOOL OnRclickDlist(int iPos);
	BOOL OnLclickWlist();
	int volatile iCurrentItem;
	int iLastPreviewItem;
	void GetSelectedRow(BOOL bTo0=1);
	void GetDSelectedRow();
	void InitWList();
	CDLG_WChanger(CWnd* pParent = NULL);   // standard constructor
	CSmartWndSizer Sizer;
	CImageList m_imageList;
	void OnWchangerAdddirectory();
	void OnWchangerDeletefolderX(DWORD iNumber);
	void OnWchangerDeleteThemeX(DWORD dwImageID);
	void OnWchangerRemovewithimageX(int iNumber);
	BOOL OnWchangerDisableImgX(int iNumber);
// Dialog Data
	//{{AFX_DATA(CDLG_WChanger)
	enum { IDD = IDD_WCHANGER };
	CTabCtrl	m_MainTab;
	CButton	m_ThActive;
	CBitmapStatic m_BtRemImg;
	CBitmapStatic m_BtImgMisc;
	CBitmapStatic m_BtThSync;
	CBitmapStatic m_BtThNew;
	CBitmapStatic m_BtAddDir;
	CBitmapStatic m_BtAddFile;
	CBitmapStatic m_BtAddArch;
	CBitmapStatic m_BtRemove;
	CBitmapStatic m_BtSetNow;
	CBitmapStatic m_BtCal;
//	CBitmapStatic m_BtCalr;
	CSliderCtrl	m_WeightEdit;
	CWPPreview	m_Preview;
	CIListCtrl	m_WList;
	CIListCtrl	m_DList;
	CString	m_Hour;
	CString	m_Min;
	BOOL	m_UsePreview;
	BOOL	m_Activate;
	int		m_CycleType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLG_WChanger)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CDLG_WChanger)
	virtual BOOL OnInitDialog();
	afx_msg void OnWchangerAddfile();
	afx_msg void OnWchangerAddArch();
	afx_msg void OnWchangerDeleteall();
	afx_msg void OnWchangerDeletefolder();
	afx_msg void OnWchangerDeleteTheme();
	afx_msg void OnWchangerDisableImg();
	afx_msg void OnWchangerNewfolder();
	afx_msg void OnWchangerSortByName();
	afx_msg void OnWchangerSortShuffle();
	afx_msg void OnWchangerSortByType();
	afx_msg void OnWchangerSortByDims();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual void OnOK();
	afx_msg void OnThActive();
	afx_msg void OnPreviewCheck();
	afx_msg void OnSetnow();
	afx_msg void OnSetnowWallp();
	afx_msg void OnAddfullfolder();
	afx_msg void OnAddfullfolderNew();
	afx_msg void OnAddfullfolderAC();
	afx_msg void OnChangeEditWeight();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangeComboThemefitting();
	afx_msg void OnWchangerAddhyperlink();
	afx_msg void OnTemplParams();
	afx_msg void OnBtAddDir();
	afx_msg void OnBtSortTh();
	afx_msg void OnBtSyncTh();
	afx_msg void OnBtRemImg();
	afx_msg void OnBtAddFile();
	afx_msg void OnBtAddArch();
	afx_msg void OnBtRem();
	afx_msg void OnBtCal();
	afx_msg void OnBtCalR();
	afx_msg void OnPreviewClick();
	afx_msg void OnWchangerRemovebroken();
	afx_msg void OnWchangerRemoveDuplicates();
	afx_msg void OnWchangerRemoveDuplicatesAll();
	afx_msg void OnWchangerRemoveDuplicatesFld();
	afx_msg void OnWchangerSetScr();
	afx_msg void OnWchangerRenameTheme();
	afx_msg void OnWchangerRenameImage();
	afx_msg void OnWchangerFoldersync();
	afx_msg void OnWchangerFoldersyncNow();
	afx_msg void OnWchangerFoldersyncSet();
	afx_msg void OnWchangerRemovewithimage();
	afx_msg void OnBtEdit();
	afx_msg void OnBtProp();
	afx_msg void OnSelchangeComboSequence();
	afx_msg void OnCheckActive();
	afx_msg void OnEnlParams();
	afx_msg void OnSelchangeMaintab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtWopt();
	afx_msg void OnBtWoptDDown();
	afx_msg void OnWchangerNewPreset();
	afx_msg void OnWchangerPresetHlp();
	afx_msg void OnWchangerDelPreset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedOptions();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_WCHANGER_H__5FE48A37_7BCE_4B41_9D0D_9280B19C557A__INCLUDED_)



