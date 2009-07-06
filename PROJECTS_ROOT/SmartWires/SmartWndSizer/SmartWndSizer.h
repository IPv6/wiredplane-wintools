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

/** Классы предназначены для автоматического перемещения 
диалоговых элементов в диалоге по заданным правилам при изменении размеров окна.
Кроме того класс позволяет:
- "прилипать" к краям экрана и границам других окон написанных с исп. этого класса
- сохранять/восстанавливать положение диалогового окна между запусками программы (хранится в реестре)
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Для работы класса надо:
1. Cоздать в классе диалога переменную класса CSmartWndSizer (Sizer для примера)
2. в OnInitialize - инициализировать и прописать требуемые опции
		Sizer.InitSizer(this,10,"Software\\MyProg\\MyWinPos");
		Sizer.SetOptions(STICKABLE|SIZEABLE);
		CRect minRect(0,0,300,200);
		Sizer.SetMinRect(minRect);
		// Элементы
		Sizer.AddDialogElement(IDC_PERSONLIST);
		Sizer.AddDialogElement(IDC_NEWPERSON,widthSize|heightSize);
		Sizer.AddDialogElement(ID_DELETEPERSON,widthSize|heightSize);
		Sizer.AddDialogElement(IDC_PERSONMOVEUP,widthSize|heightSize,IDC_NEWPERSON);
		// Раскладка
		Sizer.AddLeftTopAlign(IDC_PERSONLIST,0,5);
		Sizer.AddAlignRule(IDC_PERSONLIST,0,bottomPos,bottomPos,-5);
		Sizer.AddRightTopAlign(IDC_NEWPERSON,0,5);
		Sizer.AddAlignRule(IDC_PERSONLIST,IDC_NEWPERSON,rightPos,leftPos,-5);
		Sizer.AddAlignRule(IDC_OK,IDC_NEWPERSON,topCenter,topCenter,0);

3. Перекрыть WindowProc, где написать
LRESULT MyDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT DispatchResultFromSizer;
	if(Sizer.HandleWndMesages(message, wParam, lParam, DispatchResultFromSizer)==TRUE){
		return DispatchResultFromSizer;
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

 Можно для отдельных сообщений возвращать не делать return а передавать управление 
стандартному обработчику
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 Замечания: 
 1. IDC_*** - это ID элементов управления. Перед ссыланием на какой-дибо элемент
надо его добавить путем вызова AddDialogElement (в этом же вызове можно установить какие
 параметры элемента не должны меняться - например ширина или высота)
 2. Если нужно отнестись к главному окну, в качестве ID надо передавать 0
 3. Если нужно делать несколько раскладок в зависимости от орп. условий,
 нужно модифицировать отдельные правила (есть соотв. функции) и пользоваться переходами (goto)
 4. в InitSizer(this,10,"Software\\MyProg\\MyWinPos") параметры означают следующее - 
 первый - окно к элементам которого будут применяться выравнивания
 второй - количество пикселов, с которого края окон будут приклеиваться (0 если это не нужно)
 последний - ключ реестра в разделе HKEY_CURRENT_USER в котором будут храниться и восстанавл.
 позиция и размер окна, NULL если это не нужно
*/

/** Перечисление возможных видов выравнивания
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

/** Перечисление результатов проверки положения окна
	- не сдвинулось или прилипло к чему нибудь таки
*/
enum{
	windowStickToWnd=1,
	windowStickToScreen=2,
	windowStickToMinSize=4
};

/** Перечисление опций сайзера
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

/** Критическая секция, самостоятельно заботящаяся о своей иниц/деинит
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

/** Описатель диалогового элемента
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
	// Возвращает копию этого объекта
	CSmartWndSizer_Element* Copy()
	{
		return (new CSmartWndSizer_Element(m_ID,m_dwFixed,m_iFixedFrom,&m_Rect));
	}
	// Идентификатор элемента
	int m_ID;
	// Какие параметры у данного элемента фиксированы и не могут меняться при выравниваниях
	DWORD m_dwFixed;
	// С каким элементом зафиксированные элементы должны совпадать (если 0 то берутся оригинальные значения параметров элемента)
	int m_iFixedFrom;
	// Кешированное положение элемента
	CRect m_Rect;
	// Разрешен ли показ элемента
	BOOL m_bCanBeVisible;
};

/** Описатель правила для выравнивания элемента
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
	// При каком уровне выполнять это правило. Уровень 0 выполняется всегда
	DWORD m_Level;
	// На какой элемент будет идти воздействие (ID)
	int m_TargetElement;
	// С какого элемента будут браться значения (ID)
	int m_TakeFrom;
	// Что будет браться
	DWORD m_TakeWhat;
	// К чему будет применяться
	DWORD m_ApplyToWhat;
	// Зазор между значениями
	int m_Spacing;
	// Номер метки к которой надо совершить переход вместо обработки правила
	int m_GotoLabel;
};

/** Класс, объекты которого применяют правила выравнивания к диалоговым элементам
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
	// Данные
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
	// Переменные для поддержки ресайза
	int iAffectType;
	BOOL bInResizingLock;
	CSmartWndSizer_Element* pResizeAffectedWnd;
	BOOL bResizingElementsArePresent;
	HCURSOR hResizeCur;
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// Основные функции
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
	/** Установить опции
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
	/** оператор приравнивания
	*	@param	CSmartWndSizer& pCopy
	*	@return	CSmartWndSizer& 
	*/
	CSmartWndSizer& operator=(CSmartWndSizer& pCopy);
	/** Установить уровень клейкости
	*	@param	int iStickness
	*	@return	void 
	*/
	void SetStickness(int iStickness);
	/** При самом первом вызове грузит положение окна из реестра
	*	@return	BOOL 
	*/
	BOOL SmartPRESetWindowPosition();
	/** Основная функция, которая обрабатывает все нужные сообщения
	*	@return	BOOL 
	*/
	BOOL HandleWndMesages(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& DispatchResultFromSizer);
	/** Применить правила выравнивания (двигает все элементы диалога)
	*	@param	BOOL bClearCash=FALSE
	*	@return	int 
	*/
	int ApplyShowRules(BOOL bFullCheck=TRUE);
	int ApplyLayout(BOOL bClearCash=FALSE, BOOL bCheckIfOnlyRules=TRUE);
	/** Проверить, пересекается ли один элемент с другими
	*	@return	int 
	*/
	int IsWindowIntersected(int iID, BOOL bFullCheck=TRUE, BOOL bWithMainWindow=FALSE);
	/** Применить клекость (двигает основное окно)
	*	@return	int 
	*/
	int ApplyStickness(BOOL bRedraw=TRUE, BOOL bUseCashedRect=FALSE);
	/** Проверить на то что заданный примоугольник сдвинется, реальных движений не происходит
	*	@param	CRect& - прямоугольник
	*	@param	BOOL - проверять только если окно видимо
	*	@return	int 
	*/
	int TestStickness(CRect& CurDialogRect, BOOL bTestVisibleOnly=TRUE);
	/** Проверить на то что заданный примоугольник сдвинется, но только в плане выхода за пределы экрана
	*	@param	CRect& - прямоугольник
	*	@return	int 
	*/
	static int TestOnScreenOut(CRect& CurDialogRect, int Stickness, BOOL bForceInScreen=0, BOOL bBestMonitorForMM=0);
	/** Сохранение в реестр
	*	@return	int 
	*/
	int SaveSizeAndPosToRegistry();
	/** Загрузка из реестра
	*	@return	int 
	*/
	int LoadSizeAndPosFromRegistry();
	// Добавление элементов и правил
	// Добавление элемента
	int AddDialogElement(int ID, DWORD dwFixed=0, int iFixedFrom=0);
	//int AddSetOfRules(DWORD dwLevel, DWORD)
	/** Добавить правило выравнивания
	*	@param		int TargetElement
	*	@param		int TakeFrom
	*	@param		DWORD TakeWhat
	*	@param		DWORD ApplyToWhat
	*	@param		long Spacing=0
	*	@return		int - номер правила в списке
	*/
	int AddAlignRule(int TargetElement,int TakeFrom,DWORD ApplyToWhat, DWORD TakeWhat,long Spacing=0)
	{
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(TargetElement,TakeFrom,ApplyToWhat,TakeWhat,Spacing));
	}
	/** Модификация правила выравнивания по номеру
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
	/** Добавление метки в текущей позиции с данным кодом
	*/
	int AddGotoLabel(DWORD dwLabelID)
	{
		// Метка указывает на еще пока не существующий индекс (который будет при следующем добавлении очередного правила)
		return m_Labels[dwLabelID]=m_AlignRules.GetSize();
	}
	/** Добавления перехода к метке
	*/
	int AddGotoRule(int dwLabelID){
		return m_AlignRules.Add(new CSmartWndSizer_AlignRule(dwLabelID));
	};
	/** Замена перехода на переход к другой метке
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
	/** Дополнительные функии для добавления правил выравнивания
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




