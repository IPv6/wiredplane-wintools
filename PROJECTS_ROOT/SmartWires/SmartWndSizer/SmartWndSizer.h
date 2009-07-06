// SmartWndSizer.h: interface for the CSmartWndSizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMARTWNDSIZER_H__B6217B55_E1F9_48ED_8348_A185ED4A7B5A__INCLUDED_)
#define AFX_SMARTWNDSIZER_H__B6217B55_E1F9_48ED_8348_A185ED4A7B5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x0400
#endif
#include <stdlib.h>
#include <math.h>
//#include <windows.h>
#include <afxwin.h>
#include <afxtempl.h>
#include <atlbase.h>
#include <afxmt.h>

/** ������ ������������� ��� ��������������� ����������� 
���������� ��������� � ������� �� �������� �������� ��� ��������� �������� ����.
����� ���� ����� ���������:
- "���������" � ����� ������ � �������� ������ ���� ���������� � ���. ����� ������
- ���������/��������������� ��������� ����������� ���� ����� ��������� ��������� (�������� � �������)
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
��� ������ ������ ����:
1. C������ � ������ ������� ���������� ������ CSmartWndSizer (Sizer ��� �������)
2. � OnInitialize - ���������������� � ��������� ��������� �����
		Sizer.InitSizer(this,10,"Software\\MyProg\\MyWinPos");
		Sizer.SetOptions(STICKABLE|SIZEABLE);
		CRect minRect(0,0,300,200);
		Sizer.SetMinRect(minRect);
		// ��������
		Sizer.AddDialogElement(IDC_PERSONLIST);
		Sizer.AddDialogElement(IDC_NEWPERSON,widthSize|heightSize);
		Sizer.AddDialogElement(ID_DELETEPERSON,widthSize|heightSize);
		Sizer.AddDialogElement(IDC_PERSONMOVEUP,widthSize|heightSize,IDC_NEWPERSON);
		// ���������
		Sizer.AddLeftTopAlign(IDC_PERSONLIST,0,5);
		Sizer.AddAlignRule(IDC_PERSONLIST,0,bottomPos,bottomPos,-5);
		Sizer.AddRightTopAlign(IDC_NEWPERSON,0,5);
		Sizer.AddAlignRule(IDC_PERSONLIST,IDC_NEWPERSON,rightPos,leftPos,-5);
		Sizer.AddAlignRule(IDC_OK,IDC_NEWPERSON,topCenter,topCenter,0);

3. ��������� WindowProc, ��� ��������
LRESULT MyDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

 ����� ��� ��������� ��������� ���������� �� ������ return � ���������� ���������� 
������������ �����������
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 ���������: 
 1. IDC_*** - ��� ID ��������� ����������. ����� ��������� �� �����-���� �������
���� ��� �������� ����� ������ AddDialogElement (� ���� �� ������ ����� ���������� �����
 ��������� �������� �� ������ �������� - �������� ������ ��� ������)
 2. ���� ����� ��������� � �������� ����, � �������� ID ���� ���������� 0
 3. ���� ����� ������ ��������� ��������� � ����������� �� ���. �������,
 ����� �������������� ��������� ������� (���� �����. �������) � ������������ ���������� (goto)
 4. � InitSizer(this,10,"Software\\MyProg\\MyWinPos") ��������� �������� ��������� - 
 ������ - ���� � ��������� �������� ����� ����������� ������������
 ������ - ���������� ��������, � �������� ���� ���� ����� ������������� (0 ���� ��� �� �����)
 ��������� - ���� ������� � ������� HKEY_CURRENT_USER � ������� ����� ��������� � ����������.
 ������� � ������ ����, NULL ���� ��� �� �����
*/

/** ������������ ��������� ����� ������������
*/
enum{
	fixedDimensions	=0x00000001,
	visibility		=0x00000002,
	hidable			=0x00000004,
	widthSize		=0x00000008,
	heightSize		=0x00000010,
	topCenter		=0x00000020,
	sideCenter		=0x00000040,
	leftPos			=0x00000080,
	rightPos		=0x00000100,
	topPos			=0x00000200,
	bottomPos		=0x00000400,
	leftResize		=0x00000800,
	rightResize		=0x00001000,
	topResize		=0x00002000,
	bottomResize	=0x00004000,
	ifBigger		=0x00008000,
	ifLess			=0x00010000,
	transparent		=0x00020000,
	alwaysVisible	=0x00040000,
	alwaysInvisible	=0x00080000
};

/** ������������ ����������� �������� ��������� ����
	- �� ���������� ��� �������� � ���� ������ ����
*/
enum{
	windowStickToWnd=1,
	windowStickToScreen=2,
	windowStickToMinSize=4
};

/** ������������ ����� �������
*/
enum{
	STICKABLE		=0x00000001,
	SIZEABLE		=0x00000002,
	FIXEDSIZE		=0x00000004,
	FIXEDXSIZE		=0x00000008,
	FIXEDYSIZE		=0x00000010,
	CENTERUNDERMOUSE=0x00000020,
	NODOUBLEBUFFER	=0x00000040,
	FIXEDPOS		=0x00000080,
	SETCLIPSIBL		=0x00000100
};

/** ����������� ������, �������������� ����������� � ����� ����/������
*/
class CSmartCritSection
{
	CRITICAL_SECTION lock;
public:
	CSmartCritSection()
	{
		InitializeCriticalSection(&lock);
	}
	~CSmartCritSection()
	{
		DeleteCriticalSection(&lock);
	}
	void Enter()
	{
		EnterCriticalSection(&lock);
	}
	void Leave()
	{
		LeaveCriticalSection(&lock);
	}
};

/** ��������� ����������� ��������
*/
class CSmartWndSizer_Element
{
public:
	CSmartWndSizer_Element(int ID, DWORD dwFixed=0, int iFixedFrom=0, CRect* pRect=NULL)
	{
		m_ID=ID;
		m_dwFixed=dwFixed;
		m_iFixedFrom=iFixedFrom;
		m_bCanBeVisible=TRUE;
		if(pRect==NULL){
			m_Rect.SetRect(-1,-1,-1,-1);
		}else{
			m_Rect.CopyRect(*pRect);
		}
	};
	~CSmartWndSizer_Element(){};
	// ���������� ����� ����� �������
	CSmartWndSizer_Element* Copy()
	{
		return (new CSmartWndSizer_Element(m_ID,m_dwFixed,m_iFixedFrom,&m_Rect));
	}
	// ������������� ��������
	int m_ID;
	// ����� ��������� � ������� �������� ����������� � �� ����� �������� ��� �������������
	DWORD m_dwFixed;
	// � ����� ��������� ��������������� �������� ������ ��������� (���� 0 �� ������� ������������ �������� ���������� ��������)
	int m_iFixedFrom;
	// ������������ ��������� ��������
	CRect m_Rect;
	// �������� �� ����� ��������
	BOOL m_bCanBeVisible;
};

/** ��������� ������� ��� ������������ ��������
*/
class CSmartWndSizer_AlignRule
{
public:
	~CSmartWndSizer_AlignRule(){};
	CSmartWndSizer_AlignRule(int TargetElement,int TakeFrom,DWORD ApplyToWhat, DWORD TakeWhat,long Spacing=0, DWORD dwLevel=0)
	{
		m_Level=dwLevel;
		m_TargetElement=TargetElement;
		m_TakeFrom=TakeFrom;
		m_TakeWhat=TakeWhat;
		m_ApplyToWhat=ApplyToWhat;
		m_Spacing=Spacing;
		m_GotoLabel=-1;
	};
	CSmartWndSizer_AlignRule(int dwLabel)
	{
		m_TargetElement=m_TakeFrom=m_TakeWhat=m_ApplyToWhat=m_Spacing=0;
		m_GotoLabel=dwLabel;
	};
	// ��� ����� ������ ��������� ��� �������. ������� 0 ����������� ������
	DWORD m_Level;
	// �� ����� ������� ����� ���� ����������� (ID)
	int m_TargetElement;
	// � ������ �������� ����� ������� �������� (ID)
	int m_TakeFrom;
	// ��� ����� �������
	DWORD m_TakeWhat;
	// � ���� ����� �����������
	DWORD m_ApplyToWhat;
	// ����� ����� ����������
	int m_Spacing;
	// ����� ����� � ������� ���� ��������� ������� ������ ��������� �������
	int m_GotoLabel;
};

/** �����, ������� �������� ��������� ������� ������������ � ���������� ���������
*/
class SizerWnd
{
public:
	SizerWnd()
	{
		bStickedForFollow=FALSE;
	};
	CWnd* pWnd;
	BOOL bStickedForFollow;
};

class CSmartWndSizer
{
public:
	// ������
	CWnd hInternalWnd;
	int iMenuH;
	static BOOL bSlowMachine;
	static CSmartCritSection* rectLock;
	static CArray<SizerWnd,SizerWnd&>* m_AllWndSizerRectangles;
	static int iGlobalCounter;
	static BOOL bFollowModeEnabled;
	static CRect DesktopDialogRect;
	BOOL bMaximized;
	BOOL bSkipResize;
	DWORD dwSizerLevel;
	CWnd* m_pMain;
	CRect m_rMain;
	CRect m_rMainMin;
	CRect m_rMainMax;
	long lClientOffsetL;
	long lClientOffsetR;
	long lClientOffsetT;
	long lClientOffsetB;
	int iInit;
	int m_iStickness;
	int iInPresetPosition;
	long bMessageRecursingPreventer;
	CString m_szRegKey;
	DWORD m_dwOptions;
	HKEY m_hKeyRoot;
	CMap<int,int,CSmartWndSizer_Element*,CSmartWndSizer_Element*> m_WndElements;
	CArray<CSmartWndSizer_AlignRule*,CSmartWndSizer_AlignRule*> m_AlignRules;
	CMap<DWORD,DWORD,DWORD,DWORD> m_Labels;
	// ���������� ��� ��������� �������
	int iAffectType;
	BOOL bInResizingLock;
	CSmartWndSizer_Element* pResizeAffectedWnd;
	BOOL bResizingElementsArePresent;
	HCURSOR hResizeCur;
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// �������� �������
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
	void UpdateClientOffsets();
	int GetWindowMenuHeightSW(BOOL bForceReCalc=FALSE);
	int RedrawWindowFlickerFree(CWnd* pWnd);
	BOOL ApllyAndRefresh(BOOL bNotUsingSys=FALSE);
	BOOL isNeedNeedFirstApplying(){return iInit==2;};
	CSmartWndSizer();
	virtual ~CSmartWndSizer();
	void InitSizer(CWnd* pMainWnd, int iStickness=0, HKEY hKeyRoot=HKEY_CURRENT_USER, const char* szKey=NULL);
	void InitSizer(HWND pMainWnd, int iStickness=0, HKEY hKeyRoot=HKEY_CURRENT_USER,const char* szKey=NULL);
	/** ���������� �����
	*	@param	DWORD dwOptions
	*	@return	void 
	*/
	void SetOptions(DWORD dwOptions){
		m_dwOptions=dwOptions;
		if(m_dwOptions & SETCLIPSIBL){
			SetWindowLong(m_pMain->GetSafeHwnd(),GWL_STYLE,GetWindowLong(m_pMain->GetSafeHwnd(),GWL_STYLE)|WS_CLIPCHILDREN|WS_CLIPSIBLINGS);
		}
	};
	void SetStartupPos(CRect& rect){
		m_rMain.CopyRect(rect);
	}
	void SetMinRect(CRect& rect){
		m_rMainMin.CopyRect(rect);
	}
	void SetMaxRect(CRect& rect){
		m_rMainMax.CopyRect(rect);
	}
	int m_iSaWi;
	void SetControlSafeZone(int iSaWi){
		m_iSaWi=iSaWi;
	}
	DWORD SetLevel(DWORD dwNewLev){
		DWORD dwRes=dwSizerLevel;
		dwSizerLevel=dwNewLev;
		return dwRes;
	}
	DWORD GetLevel(){
		return dwSizerLevel;
	}
	/** �������� �������������
	*	@param	CSmartWndSizer& pCopy
	*	@return	CSmartWndSizer& 
	*/
	CSmartWndSizer& operator=(CSmartWndSizer& pCopy);
	/** ���������� ������� ���������
	*	@param	int iStickness
	*	@return	void 
	*/
	void SetStickness(int iStickness);
	/** ��� ����� ������ ������ ������ ��������� ���� �� �������
	*	@return	BOOL 
	*/
	BOOL SmartPRESetWindowPosition();
	/** �������� �������, ������� ������������ ��� ������ ���������
	*	@return	BOOL 
	*/
	BOOL HandleWndMesages(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& DispatchResultFromSizer);
	/** ��������� ������� ������������ (������� ��� �������� �������)
	*	@param	BOOL bClearCash=FALSE
	*	@return	int 
	*/
	int ApplyShowRules(BOOL bFullCheck=TRUE);
	int ApplyLayout(BOOL bClearCash=FALSE, BOOL bCheckIfOnlyRules=TRUE);
	/** ���������, ������������ �� ���� ������� � �������
	*	@return	int 
	*/
	int IsWindowIntersected(int iID, BOOL bFullCheck=TRUE, BOOL bWithMainWindow=FALSE);
	/** ��������� �������� (������� �������� ����)
	*	@return	int 
	*/
	int ApplyStickness(BOOL bRedraw=TRUE, BOOL bUseCashedRect=FALSE);
	/** ��������� �� �� ��� �������� ������������� ���������, �������� �������� �� ����������
	*	@param	CRect& - �������������
	*	@param	BOOL - ��������� ������ ���� ���� ������
	*	@return	int 
	*/
	int TestStickness(CRect& CurDialogRect, BOOL bTestVisibleOnly=TRUE);
	/** ��������� �� �� ��� �������� ������������� ���������, �� ������ � ����� ������ �� ������� ������
	*	@param	CRect& - �������������
	*	@return	int 
	*/
	static int TestOnScreenOut(CRect& CurDialogRect, int Stickness, BOOL bForceInScreen=0, BOOL bBestMonitorForMM=0);
	/** ���������� � ������
	*	@return	int 
	*/
	int SaveSizeAndPosToRegistry();
	/** �������� �� �������
	*	@return	int 
	*/
	int LoadSizeAndPosFromRegistry();
	// ���������� ��������� � ������
	// ���������� ��������
	int AddDialogElement(int ID, DWORD dwFixed=0, int iFixedFrom=0);
	//int AddSetOfRules(DWORD dwLevel, DWORD)
	/** �������� ������� ������������
	*	@param		int TargetElement
	*	@param		int TakeFrom
	*	@param		DWORD TakeWhat
	*	@param		DWORD ApplyToWhat
	*	@param		long Spacing=0
	*	@return		int - ����� ������� � ������
	*/
	int AddAlignRule(int TargetElement,int TakeFrom,DWORD ApplyToWhat, DWORD TakeWhat,long Spacing=0)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,ApplyToWhat,TakeWhat,Spacing));
	}
	/** ����������� ������� ������������ �� ������
	*/
	int ModifyAlignRule(int iIndex, int TargetElement,int TakeFrom,DWORD ApplyToWhat, DWORD TakeWhat,long Spacing=0)
	{
		if(iIndex<0 || iIndex>=m_AlignRules.GetSize()){
			return FALSE;
		}
		delete m_AlignRules[iIndex];
		m_AlignRules[iIndex]=new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,ApplyToWhat,TakeWhat,Spacing);
		ApplyLayout();
		return TRUE;
	}
	/** ���������� ����� � ������� ������� � ������ �����
	*/
	int AddGotoLabel(DWORD dwLabelID)
	{
		// ����� ��������� �� ��� ���� �� ������������ ������ (������� ����� ��� ��������� ���������� ���������� �������)
		return m_Labels[dwLabelID]=m_AlignRules.GetSize();
	}
	/** ���������� �������� � �����
	*/
	int AddGotoRule(int dwLabelID){
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(dwLabelID));
	};
	/** ������ �������� �� ������� � ������ �����
	*/
	int SetGotoRule(int iIndex,int dwLabelID)
	{
		if(iIndex<0 || iIndex>=m_AlignRules.GetSize()){
			return FALSE;
		}
		delete m_AlignRules[iIndex];
		m_AlignRules[iIndex]=new CSmartWndSizer_AlignRule(dwLabelID);
		ApplyLayout();
		return TRUE;
	}
	/** �������������� ������ ��� ���������� ������ ������������
	*/
	int AddLeftAlign(int TargetElement,int TakeFrom,DWORD TakeWhat,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,leftPos,TakeWhat,Spacing));
	}
	int AddLeftAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,leftPos,leftPos,Spacing));
	}
	int AddRightAlign(int TargetElement,int TakeFrom,DWORD TakeWhat,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,rightPos,TakeWhat,Spacing));
	}
	int AddRightAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,rightPos,rightPos,Spacing));
	}
	int AddTopAlign(int TargetElement,int TakeFrom,DWORD TakeWhat,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,topPos,TakeWhat,Spacing));
	}
	int AddTopAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,topPos,topPos,Spacing));
	}
	int AddBottomAlign(int TargetElement,int TakeFrom,DWORD TakeWhat,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,bottomPos,TakeWhat,Spacing));
	}
	int AddBottomAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,bottomPos,bottomPos,Spacing));
	}
	int AddTopCenterAlign(int TargetElement,int TakeFrom,DWORD TakeWhat,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,topCenter,TakeWhat,Spacing));
	}
	int AddTopCenterAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,topCenter,topCenter,Spacing));
	}
	int AddSideCenterAlign(int TargetElement,int TakeFrom,DWORD TakeWhat,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,sideCenter,TakeWhat,Spacing));
	}
	int AddSideCenterAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,sideCenter,sideCenter,Spacing));
	}
	int AddWidthAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,widthSize,widthSize,Spacing));
	}
	int AddHeightAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,heightSize,heightSize,Spacing));
	}
	int AddLeftTopAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		int iRes=m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,leftPos,leftPos,Spacing));
		m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,topPos,topPos,Spacing));
		return iRes;
	}
	int AddLeftBottomAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		int iRes=m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,leftPos,leftPos,Spacing));
		m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,bottomPos,bottomPos,-Spacing));
		return iRes;
	}
	int AddRightTopAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		int iRes=m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,rightPos,rightPos,-Spacing));
		m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,topPos,topPos,Spacing));
		return iRes;
	}
	int AddRightBottomAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		int iRes=m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,rightPos,rightPos,Spacing));
		m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,bottomPos,bottomPos,Spacing));
		return iRes;
	}
	int AddWidthAndHeightAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		int iRes=m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,widthSize,widthSize,Spacing));
		m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,heightSize,heightSize,Spacing));
		return iRes;
	}
	int AddHeight2WidthAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		int iRes=m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,widthSize,heightSize,Spacing));
		m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,widthSize,heightSize,Spacing));
		return iRes;
	}
	int AddWidth2HeightAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		int iRes=m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,heightSize,widthSize,Spacing));
		m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,heightSize,widthSize,Spacing));
		return iRes;
	}
	int AddCenterInsideAlign(int TargetElement,int TakeFrom,long Spacing)
	{
		int iRes=m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,topCenter,topCenter,Spacing));
		m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,sideCenter,sideCenter,Spacing));
		return iRes;
	}
	int AddHideRule(int TargetElement)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,0,visibility,visibility,0));
	}
	int AddShowRule(int TargetElement)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,0,visibility,visibility,1));
	}
	static BOOL TestPointInWindows(CPoint pt);
	static CRect& GetScreenRect(CRect* rtBase=NULL, BOOL* bMultiMonsOut=NULL, BOOL bForBestMonitor=FALSE);
	static BOOL TestRectInWindows(CRect rt, BOOL bExcludeSelf=FALSE, BOOL bExcludeInvises=TRUE, const char* szTestOnProp=0);
	void FollowWndRect(CRect rMaster, CPoint pOffset);
	int IsRectStickedTogether(CRect rect1, CRect rect2, int iStickness=0);
};

class CSmartLocker
{
	CSmartCritSection* m_lock;
public:
	CSmartLocker(CSmartCritSection* lock)
	{
		m_lock=lock;
		if(m_lock){
			m_lock->Enter();
		}
	}
	~CSmartLocker()
	{
		if(m_lock){
			m_lock->Leave();
		}
	}
};

CRect GetScreenRect(CRect* rtBase=NULL, BOOL* bMultiMons=NULL);
BOOL& IsScreenBordersAreFAT();
CRect GetAllMonitorsRECT();
#endif // !defined(AFX_SMARTWNDSIZER_H__B6217B55_E1F9_48ED_8348_A185ED4A7B5A__INCLUDED_)




