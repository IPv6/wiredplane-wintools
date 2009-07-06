//
//	Элемент управления дерево-список
//	
//	Производство	IPv6 (Based on CIListCtrl by 2Los (2Los@land.RU))
//
//
//=================================================================================
//
//	Класс CIListCtrl наследует CListCtrlWithCustomDraw,
//		взятый с http://www.codeguru.com/listview/CustomDrawListViewControls2.html
//		и созданный by Roger Onslow
//
//	Допустим, требуется поместить элемент управления в диалоговое окно CSampleDialog,
//		тогда нужно выполнить следующие действия
//
//		1. Добавить к проекту файлы IListCtrl.h, IListCtrl.cpp, а также
//			ListCtrlWithCustomDraw.h, ListCtrlWithCustomDraw.cpp
//
//		2. В редакторе ресурсов
//				Поместить на шаблон диалога обычный List Control 
//				Добавить к ресурсам Bitmap размером n*16 на 16
//					первые две картинки - это изображения + и -
//		
//		3. В Class Wizard добавить переменную к классу CSampleDialog типа CListCtrl 
//
//		4. Изменить в "IListCtrl.h" тип переменной на CIListCtrl. А также
//			добавить новую переменную 
//			CImageList m_imageList
//
//		5. Добавить в CSampleDialog.h 
//			#include "IListCtrl.h
//
//		Теперь можно использовать этот контрол
//		Например:
//
//		BOOL CSampleDialog::OnInitDialog()
//		{
//			//..
//
//			m_imageList.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, IL_IMAGE_COUNT);
//			CBitmap bitmap;
//			bitmap.LoadBitmap(IDB_IMAGELIST);
//			m_imageList.Add(&bitmap, RGB(255, 0, 255));
//			
//			m_treeList.Create(WS_CHILD|WS_VISIBLE|LVS_SHAREIMAGELISTS, CRect(0, 0, 100, 100), this, IDC_TREELIST);
//			m_treeList.SetExtendedStyle(m_treeList.GetExtendedStyle()|LVS_EX_FLATSB);
//			m_treeList.SetImageList(&m_imageList, LVSIL_SMALL);
//		
//			m_treeList.InsertFColumn(0, "Column 1", FALSE, LVCFMT_LEFT, 100);
//			m_treeList.InsertFColumn(1, "Column 2", TRUE, LVCFMT_LEFT, 200);
//			m_treeList.InsertFColumn(2, "Column 3", FALSE, LVCFMT_LEFT, 100);
//			m_treeList.InsertFColumn(3, "Column 4", FALSE, LVCFMT_LEFT, 100);
//
//			HIROW hIRow = m_treeList.InsertIRow(FL_ROOT, FL_LAST, "Root Row", 2);
//			m_treeList.SetIItemText(hIRow, 2, "Root Item");
//			hIRow = m_treeList.InsertIRow(hIRow, FL_FIRST, "Sub Row", 3);
//			m_treeList.SetIItemText(hIRow, 2, "Sub Item");
//
//			//..
//
//			return TRUE;
//		}
//


#if !defined(AFX_FLISTCTRL_H__8A28A42B_B2A1_4C84_AD5B_E6AD6048EBE2__INCLUDED_)
#define AFX_FLISTCTRL_H__8A28A42B_B2A1_4C84_AD5B_E6AD6048EBE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IListCtrl.h : header file
//

#include <afxtempl.h>
#ifndef _NO_XMLHOTKEYSUPPORT
#include "..\SystemUtils\hotkeys.h"
#endif
/////////////////////////////////////////////////////////////////////////////
// CListCtrlWithCustomDraw
class CListCtrlWithCustomDraw : public CListCtrl
{
	// Construction
public:
	CListCtrlWithCustomDraw();
	
	// Attributes
public:
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlWithCustomDraw)
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	virtual ~CListCtrlWithCustomDraw();

protected:
	CFont* m_pOldItemFont;
	CFont* m_pOldSubItemFont;

	//
	// Callbacks for whole control
	//
	
	// do we want to do the drawing ourselves?
	virtual bool IsDraw() { return false; }
	// if we are doing the drawing ourselves
	// override and put the code in here
	// and return TRUE if we did indeed do
	// all the drawing ourselves
	virtual bool OnDraw(CDC* /*pDC*/) { return false; }
	// do we want to handle custom draw for
	// individual items
	virtual bool IsNotifyItemDraw() { return false; }
	// do we want to be notified when the
	// painting has finished
	virtual bool IsNotifyPostPaint() { return false; }
	// do we want to do any drawing after
	// the list control is finished
	virtual bool IsPostDraw() { return false; }
	// if we are doing the drawing afterwards ourselves
	// override and put the code in here
	// the return value is not used here
	virtual bool OnPostDraw(CDC* /*pDC*/) { return false; }
	
	//
	// Callbacks for each item
	//
	
	// return a pointer to the font to use for this item.
	// return NULL to use default
	virtual CFont* FontForItem(int /*nItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return NULL; }
	// return the text color to use for this item
	// return CLR_DEFAULT to use default
	virtual COLORREF TextColorForItem(int /*nItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return CLR_DEFAULT; }
	// return the background color to use for this item
	// return CLR_DEFAULT to use default
	virtual COLORREF BkColorForItem(int /*nItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return CLR_DEFAULT; }
	// do we want to do the drawing for this item ourselves?
	virtual bool IsItemDraw(int /*nItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return false; }
	// if we are doing the drawing ourselves
	// override and put the code in here
	// and return TRUE if we did indeed do
	// all the drawing ourselves
	virtual bool OnItemDraw(CDC* /*pDC*/, int /*nItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return false; }
	// do we want to handle custom draw for
	// individual sub items
	virtual bool IsNotifySubItemDraw(int /*nItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return false; }
	// do we want to be notified when the
	// painting has finished
	virtual bool IsNotifyItemPostPaint(int /*nItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return false; }
	// do we want to do any drawing after
	// the list control is finished
	virtual bool IsItemPostDraw() { return false; }
	// if we are doing the drawing afterwards ourselves
	// override and put the code in here
	// the return value is not used here
	virtual bool OnItemPostDraw(CDC* /*pDC*/, int /*nItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return false; }

	//
	// Callbacks for each sub item
	//
	
	// return a pointer to the font to use for this sub item.
	// return NULL to use default
	virtual CFont* FontForSubItem(int /*nItem*/, int /*nSubItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return NULL; }
	// return the text color to use for this sub item
	// return CLR_DEFAULT to use default
	virtual COLORREF TextColorForSubItem(int /*nItem*/, int /*nSubItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return CLR_DEFAULT; }
	// return the background color to use for this sub item
	// return CLR_DEFAULT to use default
	virtual COLORREF BkColorForSubItem(int /*nItem*/, int /*nSubItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return CLR_DEFAULT; }
	// do we want to do the drawing for this sub item ourselves?
	virtual bool IsSubItemDraw(int /*nItem*/, int /*nSubItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return false; }
	// if we are doing the drawing ourselves
	// override and put the code in here
	// and return TRUE if we did indeed do
	// all the drawing ourselves
	virtual bool OnSubItemDraw(CDC* /*pDC*/, int /*nItem*/, int /*nSubItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return false; }
	// do we want to be notified when the
	// painting has finished
	virtual bool IsNotifySubItemPostPaint(int /*nItem*/, int /*nSubItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return false; }
	// do we want to do any drawing after
	// the list control is finished
	virtual bool IsSubItemPostDraw() { return false; }
	// if we are doing the drawing afterwards ourselves
	// override and put the code in here
	// the return value is not used here
	virtual bool OnSubItemPostDraw(CDC* /*pDC*/, int /*nItem*/, int /*nSubItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return false; }
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlWithCustomDraw)
	afx_msg void OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CIListCtrl window


typedef POSITION HIROW;
#define FL_ROOT		((HIROW) 0x00000001L)
#define FL_LAST		((HIROW) 0x00000002L)
#define FL_FIRST	((HIROW) 0x00000003L)

#define FL_NORMAL	0x0000
#define FL_BOLD		0x0001
#define FL_STRIKE	0x0002
#define FL_UNLINE	0x0004
#define FL_ITALIC	0x0008
#define FL_PALE		0x0010
#define FL_SKIPTRI	0x0020
#define FL_SELBORD	0x0040
#define FL_PALEBG	0x0080
#define VIEW_NOTREE	0x0001
#define VIEW_NOGRID	0x0002
#define VIEW_TREE0	0x0004
#define VIEW_MSEL	0x0008
#define VIEW_NOGRD2	0x0010

//служебные сообщения
#define FLNM_EXPAND		WM_USER+90
#define FLNM_COLLAPSE	WM_USER+91
#define FLNM_SELCHANGED	WM_USER+92
#define FLNM_STARTEDIT	WM_USER+93
#define FLNM_ENDEDIT	WM_USER+94
#define FLNM_END_INPLACE_EDIT	WM_USER+95
#define	FLNM_END_INPLACE_COMBO	WM_USER+96
#define FLNM_COMBOEXPAND		WM_USER+97
#define FLNM_CUSTOMDIALOG		WM_USER+98
#define FLNM_END_INPLACE_BUTTON	WM_USER+99
#define FLMN_CHECKBOX_TRIGGER	WM_USER+100
#define FLNM_END_INPLACE_SLIDER	WM_USER+101
#define FLNM_END_INPLACE_DATE	WM_USER+102

//служебные константы
#define FL_BASE_OFFSET	5
#define FL_IMAGE_WIDTH	20
#define FL_BRANCH_DECOFFSET	0
#define IDC_INPLACE_EDIT	301
#define IDC_INPLACE_COMBO	302
#define IDC_INPLACE_BUTTON	303
#define IDC_INPLACE_HOTKEY	304
#define IDC_INPLACE_SLIDER	305
#define IDC_INPLACE_DATE	306

struct FLNM_ROWSTATE{
	NMHDR	hdr;
	HIROW	hIRow;
	int		iIItem;
	BOOL	bCollapsed;
	CString strText;
	DWORD	dwData;
	DWORD	dwNotifyData;
};

//
struct structInPlaceComboItem{
	CString strText;
	DWORD	dwData;
};
typedef CArray<structInPlaceComboItem, structInPlaceComboItem&> ARR_INPLACE_COMBOITEM;

struct FLNM_COMBOSTATE : FLNM_ROWSTATE{
	int	iSelectedItem;
	ARR_INPLACE_COMBOITEM	*paComboItems;
};

typedef CArray<HIROW,HIROW> CHIROWArray;
//эти контстанты используются в следующих методах
//	SetIItemControlType
//	GetIItemControlType
#define FLC_READONLY		0		//значение не редактируется
#define FLC_CHECKBOX		1		//
#define FLC_EDIT			2		//для редактирования используется edit
#define FLC_COMBOBOX		4		//для редактирования используется combobox
#define FLC_CUSTOMDIALOG	8		//для открытия собственного диалога
#define FLC_CUSTOMDIALOG2	16		//для открытия собственного диалога, кнопка отображается сразу
#define FLC_HOTKEY			32		//для редактирования hot-key
#define FLC_PASSWORD		64
#define FLC_SLIDER			128
#define FLC_DATE			256
//для редактирования edit, но могет и кастом лиалог вызваться
#define FLC_EDITWITHCUSTOM	(FLC_EDIT|FLC_CUSTOMDIALOG)
class CIListCtrl : public CListCtrlWithCustomDraw
{

public:	
	BOOL bInDestroy;
	char szName[50];
	BOOL bIgnoreTabKey;
	BOOL bExcludeLabelFromCheckboxes;
	COLORREF m_crIItemText;
	COLORREF m_crIRowBackground;
	COLORREF m_crSelectedIRowBackground;//цвет фона выделенной строки таблицы
	//значения этих свойств учитываются если GetReadOnly()==FALSE
	COLORREF m_crSelectedIItemBackground;//цвет фона выделенной ячейки таблицы
	COLORREF m_crSelectedIItemText;		//цвет текста в выделенной ячейке таблицы
	BOOL m_crSelectedBold;
	BOOL bSkipTreeLines;
	//
	//
	//
	//		PUBLIC OPERATIONS
	//
	//
	//
	//

public:
	CIListCtrl();
	//Добавление элементов без показа в дереве
	BOOL bSilentAdd;
	BOOL bLockUpdateForCollapse;
	void SetSilentAddMode(BOOL bSilent){bSilentAdd=bSilent;};
	//Получение кода ряда по индексу
	HIROW GetIRowFromIndex(int index);
	int GetIRowIndex(HIROW hIRow);
	//Создает дерево-список
	int Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);	

	//Вставляет колонку, если nFormat<0, то это формат как для DrawText
	int InsertFColumn(int iIndex, LPCTSTR lpszText, BOOL fTree, int nFormat=LVCFMT_LEFT, int nWidth=-1);
	//см. также: DeleteFColumn	

	//Есть ли отступ текста для картинки
	void SetFColumnImageOffset(int nCol, BOOL fImageOffset);

	//Есть ли отступ текста для картинки
	BOOL IsFColumnImageOffset(int nCol);

	//Устанавливает ширину колонки
	int SetFColumnWidth(int nCol, int cx);

	//Возвращает ширину колонки
	BOOL GetFColumnWidth(int nCol, int cx);		

	//Возвращает число колонок
	int GetFColumnCount();

	//Удаляет колонку
	void DeleteFColumn(int iIndex);	
	//см. также: InsertFColumn

	//Вставляет строку
	HIROW InsertIRow(HIROW hParentRow, HIROW hPosition, LPCTSTR lpszText, int lImageIndex=-1);
	//см. также: DeleteIRow, DeleteAllIRows

	//Удаляет строку
	void DeleteIRow(HIROW hIRow, BOOL bSaveSelection=TRUE);
	//см. также: InsertIRow, DeleteAllIRows

	//Устанавливает текст указанной ячейки
	BOOL SetIItemText(HIROW hIRow, int iSubItem, LPCTSTR lpszText);

	//Устанавливает цвет ячейки
	BOOL SetIItemColors(HIROW hIRow, int iSubItem, DWORD dwTextColor=-1, DWORD dwBgColor=-1);

	//Возвращает цвет ячейки
	BOOL GetIItemColors(HIROW hIRow, int iSubItem, DWORD& dwTextColor, DWORD& dwBgColor);

	//Возвращает общие настройки цвета
	BOOL GetIRowColors(DWORD& dwTextColor, DWORD& dwBgColor);

	//Возвращает текст указанной ячейки
	CString GetIItemText(HIROW hIRow, int iSubItem);

	//Устанавливает картинку для указанной ячейки
	void SetIItemImage(HIROW hIRow, int iSubItem, int iImageIndex);
	//см. также: SetIItemCheckboxImages

	//Возвращает картинку для указанной ячейки
	int GetIItemImage(HIROW hIRow, int iSubItem);
	//см. также: GetIItemCheckboxImages

	//Узел раскрыт
	BOOL IsCollapsed(HIROW hIRow);

	//Раскрыть узел (bWithRoot - c корнем)
	void Expand(HIROW hIRow, BOOL bWithRoot=FALSE, BOOL bNotify=TRUE);

	//Закрыть узел (bWithRoot - c корнем)
	void Collapse(HIROW hIRow, BOOL bWithRoot=FALSE);

	//Сделать узел незакрывающимся
	void SetUnCollapsible(HIROW hIRow, BOOL bFlag=FALSE);

	//Возвращает выделенную строку
	virtual HIROW GetSelectedIRow();
	//см. также: GetSelectedIItem

	//Возвращает число строк, включая скрытые
	int GetIRowCount();
	//см. также: GetChildIRowCount

	//Возвращает данные для строки
	DWORD GetIRowData(HIROW hIRow);
	//см. также: SetIRowData, GetIItemData

	// Наличие строки
	DWORD CheckIRow(HIROW hIRow);

	//Устанавливает данные для строки
	void SetIRowData(HIROW hIRow, DWORD dwData);
	//см. также: GetIRowData, SetIItemData

	//Возращает родительский узел
	HIROW GetParentIRow(HIROW hIRow);
	//см. также: GetFirstChildIRow, GetChildIRowAt

	//Возвращает число дочерних строк
	int GetChildIRowCount(HIROW hIRow);

	//Возвращает дескриптор дочерней строки по индексу
	HIROW GetChildIRowAt(HIROW hParentIRow, int index);
	
	// Глубина вложенности элемента
	DWORD GetChildIRowDeep(HIROW hIRow);
	//примечание: индексный доступ работает медлее, чем с использованием дескриптора HIROW

	//Возвращает первый дочерний узел
	HIROW GetFirstChildIRow(HIROW hIRow);
	//см. также: GetChildIRowAt

	//Возвращает последный дочерний узел
	HIROW GetLastChildIRow(HIROW hIRow);

	//Возвращает следующий узел
	HIROW GetNextIRow(HIROW hIRow);	

	//Возвращает предыдущий узел
	HIROW GetPrevIRow(HIROW hIRow);
	

	//возвращает TRUE, если колонка содержит содержит дерево
	BOOL IsFColumnTree(int iColumn)
	{
		return m_aFColumns.GetAt(iColumn)->m_fTree;
	}
	//выделяет текст в указанной строке указанными эффектами
	void SetIRowTextStyle(HIROW hIRow, BOOL fBoldText)
	{
		CIListRow* pIRow=FindRow(hIRow);
		ASSERT(pIRow!=NULL);
		pIRow->m_fBoldText=fBoldText;
		if(pIRow->m_iListIndex>=0)
			UpdateIRow(hIRow);
		//Update(pIRow->m_iListIndex);
	}

	BOOL IsValidHIRow(HIROW hIRow)
	{
		CIListRow* pIRow=FindRow(hIRow);
		return pIRow!=0;
	}

	//Возвращает TRUE, если для строки задан стиль Bold, либо он задан для любой дочерней строки при bExpectChilds=TRUE
	BOOL GetIRowTextStyle(HIROW hIRow, BOOL bExpectChilds=FALSE)
	{
		CIListRow* pIRow=FindRow(hIRow);
		ASSERT(pIRow!=NULL);
		BOOL bBold=pIRow->m_fBoldText;
		if(bExpectChilds && !bBold)
		{
			HIROW hChildIRow=pIRow->GetHeadChildPos();
			BOOL bStyle=0;
			while(hChildIRow!=NULL)
			{
				bStyle=GetIRowTextStyle(hChildIRow, TRUE);
				if(bStyle>0)
				{
					bBold=bStyle;
					break;
				}
				pIRow->GetNextChild(hChildIRow);				
			}
		}
		return bBold;
	}

	//перерисовывает указанную строку
	void UpdateIRow(HIROW hIRow)
	{
		CIListRow* pIRow=FindRow(hIRow);
		ASSERT(pIRow!=NULL);
		if(pIRow->m_iListIndex>=0)
		{
			RedrawItems(pIRow->m_iListIndex, pIRow->m_iListIndex);
			UpdateWindow();
		}
	}

	//возвращает корневую родительскую строку
	HIROW GetRootParent(HIROW hIRow)
	{
		CIListRow* pIRow=FindRow(hIRow);
		ASSERT(pIRow!=NULL);
		if(pIRow->m_pParentRow->m_pParentRow!=NULL)
			return GetRootParent(pIRow->m_pParentRow->m_hIRow);
		return pIRow->m_hIRow;
	}

	//выделяет указанную строку
	void SelectMIRow(HIROW hIRow,BOOL bAddSelected)
	{
		if(IsMSelChangeLocked()){
			return;
		}
		BOOL bCtrl=FALSE;
		BOOL bDrop=!(hIRow==m_hSelectedIRow);
		HIROW hOldIRow=m_hSelectedIRow;
		if(hIRow!=NULL)
		{
			if(::GetKeyState(VK_SHIFT)<0){
				int iPos1=GetIRowIndex(hOldIRow);
				int iPos2=GetIRowIndex(hIRow);
				if(iPos1>iPos2){
					int i=iPos1;
					iPos1=iPos2;
					iPos2=i;
				}
				if(iPos2>=0 && iPos1>=0){
					for(int i=iPos1;i<=iPos2;i++){
						AddMultySelection(GetIRowFromIndex(i),TRUE);
					}
					bDrop=FALSE;
				}
			}else if(::GetKeyState(VK_CONTROL)<0){
				AddMultySelection(hIRow);
				bCtrl=TRUE;
				bDrop=FALSE;
			}else if(FindRowINMultiSelect(hIRow)!=-1){
				bDrop=FALSE;
			}
		}
		if(bDrop){
			DropMultySelection();
		}else if(bAddSelected && !bCtrl){
			AddMultySelection(hIRow,TRUE);
		}
	}

	void SelectIRow(HIROW hIRow, BOOL bRedraw=FALSE, BOOL bNotifyParent=TRUE)
	{
		SelectMIRow(hIRow,TRUE);
		HIROW hOldIRow=m_hSelectedIRow;
		m_hSelectedIRow=hIRow;

		CIListRow* pIRow=NULL;
		if(m_hSelectedIRow!=NULL)
		{
			pIRow=FindRow(m_hSelectedIRow);
			ASSERT(pIRow!=NULL);				
			if(bRedraw){
				UpdateIRow(m_hSelectedIRow);
			}
		}
		if(FindRow(hOldIRow)!=NULL && m_hSelectedIRow!=NULL){
			UpdateIRow(hOldIRow);
		}
		if(bNotifyParent){
			NotifyOwner(FLNM_SELCHANGED, pIRow);
		}
	}
	//см. также: SelectIItem

	//выделяет указанную ячейку в строке и тутже ее редактировать дает
	void SelectAndEditIItem(HIROW hIRow, int iIItem)
	{
		SelectIItem(hIRow, iIItem);
		EditBegin(-1);
	}
	//выделяет указанную ячейку в строке
	void SelectIItem(HIROW hIRow, int iIItem, BOOL bSelectFromMouse=FALSE, BOOL bNotifyOwner=1)
	{
		SelectMIRow(hIRow,TRUE);
		AddMultySelection(hIRow,TRUE);
		HIROW hOldIRow=m_hSelectedIRow;
		int iOldIItem=m_iSelectedIItem;
		/* Нельзя - иначе не будут работать кнопки!!!
		if(hIRow == hOldIRow && iIItem == iOldIItem){
			return;
		}*/
		//
		m_hSelectedIRow=hIRow;
		m_iSelectedIItem= GetReadOnly() ? -1 : iIItem;
		//
		CIListRow* pIRow=NULL;
		if(m_hSelectedIRow!=NULL)
		{
			pIRow=FindRow(m_hSelectedIRow);
			ASSERT(pIRow!=NULL);				
			//			
			UpdateIRow(m_hSelectedIRow);
		}
		//
		if(FindRow(hOldIRow)!=NULL && m_hSelectedIRow!=NULL)
			UpdateIRow(hOldIRow);
		if(bNotifyOwner){
			NotifyOwner(FLNM_SELCHANGED, pIRow);
		}

		EditPrepare(bSelectFromMouse?VK_LBUTTON:0);
	}
	//см. также: GetSelectedIItem

	// возвращает TRUE, если все родительские узлы являются открытыми (expanded)
	BOOL IsIRowInList(HIROW hIRow)
	{
		CIListRow* pIRow=FindRow(hIRow);
		ASSERT(pIRow!=NULL);
		return (pIRow->m_iListIndex>=0);
	}

	//возвращает первую родительскую строку, которая не скрыта, т.е. её родительская строка имеет свойство IsCollapsed()=FALSE
	HIROW GetFirstParentInList(HIROW hIRow)
	{
		CIListRow* pIRow=FindRow(hIRow);
		ASSERT(pIRow!=NULL);
		if(pIRow->m_iListIndex>=0)
			return hIRow;
		HIROW hParentIRow=GetParentIRow(hIRow);
		ASSERT(hParentIRow!=FL_ROOT);
		return GetFirstParentInList(hParentIRow);
	}

	//	возвращает уровень вложенности для элементов, начиная с нуля для корневых элементов
	int GetDeep(HIROW hIRow)
	{
		CIListRow* pIRow=FindRow(hIRow);
		ASSERT(pIRow!=NULL);
		return pIRow->m_iDeep-1;
	}
	//вместе с GetNextAssoc может быть использована для последовательного перебора всех строк
	POSITION GetStartPosition()
	{
		return m_mapAllRows.GetStartPosition();
	}
	//см. также: GetNextAssoc

	//
	HIROW GetNextAssoc(POSITION& pos)
	{
		void* hIRow=NULL;
		void* pIRow=NULL;
		m_mapAllRows.GetNextAssoc(pos, hIRow, pIRow);
		return (HIROW)hIRow;
	}

	//
	BOOL EnsureVisible(HIROW hIRow, int iIItem, BOOL bPartialOK)
	{
		CIListRow* pIRow=FindRow(hIRow);
		ASSERT(pIRow!=NULL);
		BOOL bResult=FALSE;
		if(pIRow->m_iListIndex>=0)
		{
			bResult=CListCtrl::EnsureVisible(pIRow->m_iListIndex, bPartialOK);
			if(bResult && iIItem!=-1)
			{
				CRect rcSubItem, rcClient;
				GetSubItemRect(pIRow->m_iListIndex, iIItem, LVIR_BOUNDS, rcSubItem);				
				GetClientRect(&rcClient);
				//
				if(!bPartialOK && (rcSubItem.right>rcClient.right || rcSubItem.left<rcClient.left))
				{
					CSize scroll(0,0);					
					if(rcSubItem.left<rcClient.left)
						scroll.cx = rcSubItem.left-rcClient.left+(iIItem ? -10 : 0);
					else if(iIItem && rcSubItem.right>rcClient.right)
						scroll.cx = rcSubItem.right-rcClient.right+10;
					scroll.cy=rcSubItem.top-rcSubItem.Height()-10;
					bResult=Scroll(scroll);
				}
			}
		}else{
			CIListRow* pIRowPar=pIRow->m_pParentRow;
			while(pIRowPar){
				Expand(pIRowPar);
				pIRowPar=pIRowPar->m_pParentRow;
			}
			bResult=EnsureVisible(hIRow, iIItem, bPartialOK);
		}
		return bResult;
	}

	BOOL m_bStopUpdateScreen;
	BOOL bShowButtonsAsLinks;
	BOOL bAllowMultySelect;
	BOOL bDoNotShowTree;
	BOOL bDoNotShowGrid;
	BOOL bDrawTree0;
	CHIROWArray aMultySelects;
	BOOL IsSelectionMulty()
	{
		if(!bAllowMultySelect){
			return FALSE;
		}
		return (aMultySelects.GetSize()>1);
	}
	BOOL DropMultySelection()
	{
		if(!bAllowMultySelect){
			return FALSE;
		}
		for(int i=0;i<aMultySelects.GetSize();i++){
			HIROW hLine=aMultySelects[i];
			if(hLine && FindRow(hLine)){
				SetIRowTextStyle(hLine,GetIRowTextStyle(hLine)&(~FL_PALEBG));
			}
		}
		aMultySelects.RemoveAll();
		return TRUE;
	}

	int FindRowINMultiSelect(HIROW hNewLine)
	{
		BOOL bPrevIndex=-1;
		for(int i=0;i<aMultySelects.GetSize();i++){
			if(aMultySelects[i]==hNewLine){
				bPrevIndex=i;
				break;
			}
		}
		return bPrevIndex;
	}
	
	BOOL AddMultySelection(HIROW hNewLine, BOOL bNoInvert=FALSE)
	{
		if(IsMSelChangeLocked()){
			return FALSE;
		}
		if(!bAllowMultySelect || hNewLine==NULL){
			return FALSE;
		}
		BOOL bPrevIndex=FindRowINMultiSelect(hNewLine);
		if(bPrevIndex==-1){
			SetIRowTextStyle(hNewLine,GetIRowTextStyle(hNewLine)|FL_PALEBG);
			aMultySelects.Add(hNewLine);
		}else if(!bNoInvert){
			HIROW hLine=aMultySelects[bPrevIndex];
			SetIRowTextStyle(hLine,GetIRowTextStyle(hLine)&(~FL_PALEBG));
			aMultySelects.RemoveAt(bPrevIndex);
		}
		return TRUE;
	}
	CHIROWArray& GetMultySelection()
	{
		return aMultySelects;
	};
	void SetViewOptions(DWORD dwOptions){
		bAllowMultySelect=FALSE;
		bDoNotShowGrid=FALSE;
		bDoNotShowTree=FALSE;
		bDrawTree0=FALSE;
		if((dwOptions & VIEW_MSEL)!=0){
			bAllowMultySelect=TRUE;
		}
		if((dwOptions & VIEW_NOTREE)!=0){
			bDoNotShowTree=TRUE;
		}
		if((dwOptions & VIEW_NOGRID)!=0){
			bDoNotShowGrid=1;
		}
		if((dwOptions & VIEW_NOGRD2)!=0){
			bDoNotShowGrid=2;
		}
		if((dwOptions & VIEW_TREE0)!=0){
			bDrawTree0=TRUE;
		}

	}
	//Если значение аргумента FALSE, то появляется возможность выделять отедльные ячейки списка
	// кроме того, можно назначить каждой ячейке свой элемент управления, с помощью которого она будет редактироваться
	void SetReadOnly(BOOL bReadOnly=TRUE)
	{
		m_bReadOnly=bReadOnly;
	}

	//
	BOOL GetReadOnly()
	{
		return m_bReadOnly;
	}

	//
	int GetSelectedIItem()
	{
		return m_iSelectedIItem;
	}
	//см. также: GetSelectedIRow

	//	устанавливает тип элемента управления реадактирования (FLC_READONLY, FLC_EDIT, FLC_COMBO, FLC_BUTTON)
	//	совместно со любой из этих констант можно установить свойство FLC_CHECKBOX
	// если hIRow==NULL, то данный элемент управления будет установлен для всех последующих добавляемых строк в столбце iIItem
	void SetIItemControlType(HIROW hIRow, int iIItem, int iControlType=FLC_READONLY, int nMask=0)
	{
		if(hIRow!=NULL)
		{
			CIListRow *pIRow=FindRow(hIRow);
			ASSERT(pIRow!=NULL);

			pIRow->GetAt(iIItem)->m_iControlType &= nMask ? ~nMask : 0;
			pIRow->GetAt(iIItem)->m_iControlType |= nMask ? iControlType&nMask : iControlType;
		}
		else{
			m_aFColumns.GetAt(iIItem)->m_iControlType &= nMask ? ~nMask : 0;
			m_aFColumns.GetAt(iIItem)->m_iControlType |= nMask ? iControlType&nMask : iControlType;			
		}
	}

	//	возвращает тип элемента управления реадактирования (FLC_READONLY, FLC_EDIT, FLC_COMBO, FLC_BUTTON)
	int GetIItemControlType(HIROW hIRow, int iIItem, int nMask=0)
	{
		if(hIRow!=NULL)
		{
			CIListRow *pIRow=FindRow(hIRow);
			ASSERT(pIRow!=NULL);
			return nMask ? pIRow->GetAt(iIItem)->m_iControlType & nMask : pIRow->GetAt(iIItem)->m_iControlType;
		}
		return nMask ? m_aFColumns.GetAt(iIItem)->m_iControlType & nMask : m_aFColumns.GetAt(iIItem)->m_iControlType;		
	}
	//	удаляет все строки
	void DeleteAllIRows()
	{
		HIROW hIRow;
		while(hIRow=GetFirstChildIRow(FL_ROOT), hIRow!=NULL){
			DeleteIRow(hIRow,FALSE);
		}
		if(IsWindow(GetSafeHwnd())){
			DeleteAllItems();
		}
		SetSelectionToZero();
	}

	//	устанавливает название колонки
	void SetFColumnCaption(int iColumnIndex, CString strCaption)
	{		
		LVCOLUMN lvColumn;
		lvColumn.mask=LVCF_TEXT;
		lvColumn.pszText=strCaption.GetBuffer(strCaption.GetLength());
		SetColumn(iColumnIndex, &lvColumn);
	}

	//	возвращает название колонки
	CString GetFColumnCaption(int iColumnIndex)
	{
		CString strCaption;
		LVCOLUMN lvColumn;
		lvColumn.mask=LVCF_TEXT;
		lvColumn.pszText=strCaption.GetBufferSetLength(100);
		lvColumn.cchTextMax=100;
		GetColumn(iColumnIndex, &lvColumn);
		strCaption.ReleaseBuffer();
		return strCaption;
	}
	//	устанавливает пользовательские данные - значение типа DWORD ассоциированное с конкретным элементом
	void SetIItemData(HIROW hIRow, int iIItem, DWORD dwData)
	{
		CIListRow *pIRow=FindRow(hIRow);
		ASSERT(pIRow!=NULL);
		pIRow->GetAt(iIItem)->m_dwData=dwData;
	}
	//см. также: SetIRowData
	// Размер элемента в ширину (клеток)
	void SetIItemColSpan(HIROW hIRow, int iIItem, int iSpan)
		{
		CIListRow *pIRow=FindRow(hIRow);
		ASSERT(pIRow!=NULL);
		pIRow->GetAt(iIItem)->m_iColSpan=iSpan;
	}
	// Размер элемента в ширину (клеток)
	int GetIItemColSpan(HIROW hIRow, int iIItem)
		{
		CIListRow *pIRow=FindRow(hIRow);
		ASSERT(pIRow!=NULL);
		return pIRow->GetAt(iIItem)->m_iColSpan;
	}
	//	возвращает пользовательские данные - значение типа DWORD ассоциированное с конкретным элементом
	DWORD GetIItemData(HIROW hIRow, int iIItem)
	{
		CIListRow *pIRow=FindRow(hIRow);
		ASSERT(pIRow!=NULL);
		return pIRow->GetAt(iIItem)->m_dwData;
	}

	//Блокирует уведомляющие сообщения FLNM_SELCHANGED
	void LockSelChange(BOOL bLock=TRUE)
	{
		ASSERT(m_iLockSelChangeCounter>=0);
		if(bLock)
			m_iLockSelChangeCounter++;
		else
			m_iLockSelChangeCounter--;
	}

	void LockMSelChange(BOOL bLock=TRUE)
	{
		ASSERT(m_iLockMSelChangeCounter>=0);
		if(bLock)
			m_iLockMSelChangeCounter++;
		else
			m_iLockMSelChangeCounter--;
	}

	//
	BOOL IsSelChangeLocked()
	{
		ASSERT(m_iLockSelChangeCounter>=0);
		return m_iLockSelChangeCounter;
	}

	BOOL IsMSelChangeLocked()
	{
		ASSERT(m_iLockMSelChangeCounter>=0);
		return m_iLockMSelChangeCounter;
	}

	// устанавливает значки для checkbox'а
	// если hIRow==NULL то данные значки будут установлены для всех последующих добавлямых строк в колонке iIItem
	void SetIItemCheckboxImages(HIROW hIRow, int iIItem, int iChecked=-1, int iUnchecked=-1)
	{
		ASSERT(iIItem>=0 && iIItem<GetFColumnCount());
		if(hIRow!=NULL)
		{
			CIListRow *pIRow=FindRow(hIRow);
			ASSERT(hIRow!=NULL);			
			CIListItem* pIItem=pIRow->GetAt(iIItem);
			pIItem->m_iCheckedImage=iChecked;
			pIItem->m_iUncheckedImage=iUnchecked;
		}
		else{
			m_aFColumns.GetAt(iIItem)->m_iCheckedImage=iChecked;
			m_aFColumns.GetAt(iIItem)->m_iUncheckedImage=iUnchecked;
		}
	}

	void GetIItemCheckboxImages(HIROW hIRow, int iIItem, int &iChecked, int &iUnchecked)
	{
		ASSERT(iIItem>=0 && iIItem<GetFColumnCount());
		if(hIRow!=NULL)
		{
			CIListRow *pIRow=FindRow(hIRow);
			ASSERT(hIRow!=NULL);		
			CIListItem* pIItem=pIRow->GetAt(iIItem);
			iChecked=pIItem->m_iCheckedImage;
			iUnchecked=pIItem->m_iUncheckedImage;
		}
		else{
			iChecked=m_aFColumns.GetAt(iIItem)->m_iCheckedImage;
			iUnchecked=m_aFColumns.GetAt(iIItem)->m_iUncheckedImage;
		}
	}

	//
	void SetIItemCheck(HIROW hIRow, int iIItem, BOOL bCheck=TRUE, BOOL bRedraw=TRUE, BOOL bNotify=TRUE)
	{
		CIListRow *pIRow=FindRow(hIRow);
		ASSERT(hIRow!=NULL);
		ASSERT(iIItem>=0 && iIItem<GetFColumnCount());
		CIListItem* pIItem=pIRow->GetAt(iIItem);
		BOOL bOldState=pIItem->m_bChecked;
		pIItem->m_bChecked=bCheck;
		if(bOldState!=bCheck && bRedraw)
			UpdateIRow(hIRow);
		if(bNotify){
			NotifyOwner(FLMN_CHECKBOX_TRIGGER, pIRow, NULL, pIItem->m_bChecked);
		}
	}

	//
	BOOL IsIItemChecked(HIROW hIRow, int iIItem)
	{
		CIListRow *pIRow=FindRow(hIRow);
		ASSERT(hIRow!=NULL);
		ASSERT(iIItem>=0 && iIItem<GetFColumnCount());
		CIListItem* pIItem=pIRow->GetAt(iIItem);
		return pIItem->m_bChecked;
	}
	int GetListIndex(HIROW hIRow)
	{
		CIListRow *pIRow=FindRow(hIRow);
		if(pIRow){
			return pIRow->m_iListIndex;
		}
		return -1;
	}
	void SetListIndex(HIROW hIRow, int iIndex)
	{
		CIListRow *pIRow=FindRow(hIRow);
		if(pIRow && iIndex>=0){
			pIRow->m_iListIndex=iIndex;
		}
		return;
	}

	void ShowIRow(HIROW hIRow, int iListIndex, BOOL bRefreshIndexes=TRUE)
	{
		CIListRow* pIRow=FindRow(hIRow);
		if(!pIRow){
			return;
		}
		if(iListIndex<0){
			iListIndex=pIRow->m_iListIndex;
		}
		ShowIRow(pIRow, iListIndex, bRefreshIndexes);
	}
	
	void SetSelectionToZero()
	{
		m_hSelectedIRow=0;
		m_iSelectedIItem=0;
	}
	//
	//
	//
	//			IMPLEMENTATION
	//
	//
	//

protected:
	HIROW	m_hSelectedIRow;
	int		m_iSelectedIItem;
	int		m_iLockSelChangeCounter;
	int		m_iLockMSelChangeCounter;
	BOOL	m_bReadOnly;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIListCtrl)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	// Implementation
public:
	BOOL HitTestMouseClick(CPoint point);
	BOOL HitTestLabel(HIROW phIRow, CPoint point)
	{
		return HitTestLabel(FindRow(phIRow), point);
	}
	BOOL HitTestCollapse(HIROW phIRow, CPoint point)
	{
		return HitTestCollapse(FindRow(phIRow), point);
	}

	int HitTestEx(CPoint &point, int *col=NULL) const
	{
		CIListRow* pIRow=NULL;
		return HitTestEx(point, col, pIRow);
	}

	virtual ~CIListCtrl();

	// Generated message map functions
public:
	//{{AFX_MSG(CIListCtrl)	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndEdit( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnEndCombo( NMHDR * pNotifyStruct, LRESULT * result);
	afx_msg void OnEndInplaceButton( NMHDR * pNotifyStruct, LRESULT * result);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	//	IListRow
public:
	class CIListItem;
	class CIListRow{
	public:
		CMapPtrToPtr* m_pmapAllRows;

		CIListRow* m_pPrevRow;
		CIListRow* m_pNextRow;
		CIListRow* m_pParentRow;
		CTypedPtrList<CPtrList, CIListRow*> m_lstChildRows;
		BOOL m_bCollapsed;
		int m_iDeep;
		int m_iListIndex;
		HIROW m_hIRow;
		DWORD m_dwData;
		BOOL m_fBoldText;
		BOOL m_bUncollapsible;
		//
		CTypedPtrArray<CPtrArray, CIListItem*> m_aItems;
		//
		CIListRow(CMapPtrToPtr* pmapAllRows)
		{
			m_pmapAllRows=pmapAllRows;
			//
			m_pPrevRow=NULL;
			m_pNextRow=NULL;
			m_pParentRow=NULL;
			m_bCollapsed=TRUE;
			m_bUncollapsible=FALSE;
			m_iDeep=0;
			m_iListIndex=-1;
			m_hIRow=NULL;
			m_dwData=0;
			m_fBoldText=0;
		}
		void Finalize()
		{
			for(int i=m_aItems.GetSize()-1; i>=0; i--){
				delete m_aItems[i];
			}
			Free();

		}
		~CIListRow()
		{
			Finalize();
			TRACE1("***Deleted pointer %x\n",this);
		}

		//	
		//		ITEM
		//

		//
		int InsertItem(int iIndex, LPCTSTR lpszText, int lImageIndex=-1)
		{
			CIListItem* pItem=new CIListItem(lpszText, lImageIndex);
			m_aItems.InsertAt(iIndex, pItem);
			return iIndex;
		}
		//
		void RemoveItem(int iIndex)
		{
			delete m_aItems[iIndex];
			m_aItems.RemoveAt(iIndex);
		}

		CIListItem* GetAt(int iIndex)
		{
			return m_aItems[iIndex];
		}

		int Count()
		{
			return m_aItems.GetSize();
		}

		//
		//		ROW	
		//

		HIROW Insert(CIListRow* pIRow, HIROW hPosition)
		{
			if(!this)
				return NULL;
			HIROW hIRow;
			if(hPosition==FL_LAST){
				pIRow->m_pParentRow=this;
				pIRow->m_pPrevRow= m_lstChildRows.IsEmpty() ? NULL :m_lstChildRows.GetTail();
				if(pIRow->m_pPrevRow!=NULL)
					pIRow->m_pPrevRow->m_pNextRow=pIRow;
				hIRow=(HIROW)m_lstChildRows.AddTail(pIRow);				
			}
			else if(hPosition==FL_FIRST){
				pIRow->m_pParentRow=this;
				pIRow->m_pNextRow= m_lstChildRows.IsEmpty() ? NULL : m_lstChildRows.GetHead();
				if(pIRow->m_pNextRow!=NULL)
					pIRow->m_pNextRow->m_pPrevRow=pIRow;
				hIRow=(HIROW)m_lstChildRows.AddHead(pIRow);				
			}
			else{
				pIRow->m_pParentRow=this;
				pIRow->m_pPrevRow=m_lstChildRows.GetAt(hPosition);//GetChildIRow(hPosition);
				pIRow->m_pNextRow= pIRow->m_pPrevRow==NULL ? NULL : pIRow->m_pPrevRow->m_pNextRow;
				if(pIRow->m_pPrevRow!=NULL)
					pIRow->m_pPrevRow->m_pNextRow=pIRow;
				if(pIRow->m_pNextRow!=NULL)
					pIRow->m_pNextRow->m_pPrevRow=pIRow;
				hIRow=(HIROW)m_lstChildRows.InsertAfter((POSITION) hPosition, pIRow);
			}
			pIRow->m_iDeep=m_iDeep+1;
			return hIRow;
		}
		//
		void RemoveChildIRow(HIROW hIRow,CIListCtrl* fromWho)
		{
			CIListRow* pIRow=GetChildIRow(hIRow);
			//
			HIROW hChildIRow;			
			hChildIRow=pIRow->GetHeadChildPos();
			while(hChildIRow!=NULL)
			{
				pIRow->RemoveChildIRow(hChildIRow,fromWho);
				hChildIRow=pIRow->GetHeadChildPos();				
			}
			//
			if(pIRow->m_iListIndex!=-1){
				TRACE2("***Deleted item for irow %x on index=%i\n",pIRow,pIRow->m_iListIndex);
				fromWho->HideIRow(pIRow);// Скрываем чтобы одновить низлежащие индексы
				//fromWho->DeleteItem(pIRow->m_iListIndex);
				//pIRow->m_iListIndex=-1;
			}
			if(pIRow->m_pPrevRow!=NULL){
				pIRow->m_pPrevRow->m_pNextRow=pIRow->m_pNextRow;
			}
			if(pIRow->m_pNextRow!=NULL){
				pIRow->m_pNextRow->m_pPrevRow=pIRow->m_pPrevRow;
			}
			delete pIRow;
			m_lstChildRows.RemoveAt((POSITION)hIRow);

			m_pmapAllRows->RemoveKey((void*)hIRow);
		}

		//
		void Free()
		{
			POSITION hPos=m_lstChildRows.GetHeadPosition();
			while(hPos!=NULL)
				delete m_lstChildRows.GetNext(hPos);
			m_lstChildRows.RemoveAll();
		}
		//
		CIListRow* GetChildIRow(HIROW hIRow)
		{
			if(m_lstChildRows.IsEmpty())
				return NULL;
			if(hIRow==FL_LAST)
				return m_lstChildRows.GetTail();
			else if(hIRow==FL_FIRST)
				return m_lstChildRows.GetHead();			
			return m_lstChildRows.GetAt((POSITION) hIRow);
		}
		//
		int GetChildIRowCount()
		{
			return m_lstChildRows.GetCount();
		}
		//
		HIROW GetHeadChildPos()
		{
			if(!GetChildIRowCount())
				return NULL;
			return (HIROW)m_lstChildRows.GetHeadPosition();
		}
		//
		CIListRow* GetNextChild(HIROW &hIRow)
		{
			POSITION pos=(POSITION)hIRow;
			CIListRow* pIRow=m_lstChildRows.GetNext(pos);
			hIRow = (HIROW) pos;
			return pIRow;
		}
		//
		int GetNextListIndex()
		{
			if(GetChildIRowCount()  && !m_bCollapsed)
			{
				CIListRow* pIRow=m_lstChildRows.GetTail();
				return pIRow->GetNextListIndex();
			}			
			return m_iListIndex+1;
		}
		//*/

		//
	}m_rowTopRoot;

	//Устанавливает индексы рядов в соответствие с теми что в таблице
	void RefreshIndexes(int iStartIndex=0)
	{
		int iCount=GetItemCount();
		for(int i=iStartIndex; i<iCount; i++)
		{
			CIListRow* pIRow=(CIListRow*)GetItemData(i);
#ifdef _DEBUG
			CString sName=GetItemText(i,0);
			if(pIRow->m_pmapAllRows==(CMapPtrToPtr*)0xfeeefeee){
				DebugBreak();
			}
#endif
			pIRow->m_iListIndex=i;
		}
	}

	void RemoveAllRows()
	{
		m_mapAllRows.RemoveAll();
	}

	void FreeAllRows()
	{
		void* rKey;
		void* rValue;
		CIListRow* pIRow;
		POSITION pos=m_mapAllRows.GetStartPosition();
		while(pos!=NULL)
		{
			m_mapAllRows.GetNextAssoc(pos, rKey, rValue);
			pIRow=(CIListRow*) rValue;
			delete pIRow;
		}		
		RemoveAllRows();
		m_rowTopRoot.Finalize();
	}

	//
	CMapPtrToPtr m_mapAllRows;
	void AddRow(HIROW hIRow, CIListRow* pIRow)
	{
		m_mapAllRows.SetAt((void*)hIRow, (void*) pIRow);
	}

	CIListRow* FindRow(HIROW hIRow)
	{
		void* pIRow;
		if(m_mapAllRows.Lookup((void*) hIRow, pIRow) )
			return (CIListRow*)pIRow;
		return NULL;
	}

	void RemoveRow(HIROW hIRow)
	{
		m_mapAllRows.RemoveKey((void*) hIRow);
	}

	//
	//
	//

	class CIListItem{
	public:
		DWORD	m_dwFlickerFlag;
		CString	m_strText;
		int		m_iColSpan;
		int		m_iImageIndex;		
		int		m_iControlType;
		DWORD	m_dwData;
		BOOL	m_fOffsetForControlButton;
		int		m_iCheckedImage;
		int		m_iUncheckedImage;
		BOOL	m_bChecked;
		DWORD	m_TextColor;//-1 - по умолчанию
		DWORD	m_BgColor;//-1 - по умолчанию

		CIListItem()
		{
			m_dwFlickerFlag=0;
			m_iControlType=FLC_READONLY;
			m_iImageIndex=-1;			
			m_dwData=0;
			m_fOffsetForControlButton=FALSE;			
			m_iCheckedImage=-1;
			m_iUncheckedImage=-1;
			m_bChecked=FALSE;
			m_TextColor=m_BgColor=DWORD(-1);
			m_iColSpan=-1;
		}
		CIListItem(LPCTSTR lpszText, int iImageIndex=-1)
		{
			m_iControlType=FLC_READONLY;
			m_strText=lpszText;
			m_iImageIndex=iImageIndex;
			m_dwData=0;

			m_iCheckedImage=-1;
			m_iUncheckedImage=-1;
			m_fOffsetForControlButton=FALSE;
			m_bChecked=FALSE;
			m_TextColor=m_BgColor=DWORD(-1);
			m_iColSpan=-1;
		}

	};
	//
	class CIListColumn{
	public:
		BOOL m_fTree;
		BOOL m_fImageOffset;
		UINT m_nFormat;
		int m_iControlType;
		int m_iCheckedImage, m_iUncheckedImage;

		CIListColumn(BOOL fTree=FALSE)
		{
			m_fTree=fTree;
			m_fImageOffset=FALSE;
			m_nFormat=DT_LEFT;
			m_iControlType=FLC_READONLY;
			m_iCheckedImage=-1;
			m_iUncheckedImage=-1;
		}
	};


	class CIListColumns{
	public:
		CTypedPtrArray<CPtrArray, CIListColumn*> m_aColumns;
		int m_iTreeColumn;


		CIListColumns()
		{
			m_iTreeColumn=-1;
		}
		~CIListColumns()
		{
			for(int i=m_aColumns.GetSize()-1; i>=0; i--)
				delete m_aColumns[i];
		}
		int Insert(int iIndex, BOOL fTree, UINT nFormat)
		{
			ASSERT(iIndex>=0);
			CIListColumn* pColumn=new CIListColumn(fTree);
			pColumn->m_fTree=fTree;
			pColumn->m_nFormat=nFormat;
			if(fTree)
			{
				//
				for(int i=0; i<m_aColumns.GetSize(); i++)
					ASSERT(!m_aColumns[i]->m_fTree);
				//
				ASSERT(m_iTreeColumn<0);
				m_iTreeColumn=iIndex;
			}
			else if(iIndex<=m_iTreeColumn)
				m_iTreeColumn++;
			m_aColumns.InsertAt(iIndex, pColumn);
			return iIndex;
		}
		int Count()
		{return m_aColumns.GetSize();}

		CIListColumn* GetAt(int iIndex)
		{
			return m_aColumns[iIndex];
		}

		void Delete(int iIndex)
		{
			ASSERT(iIndex>=0);
			if(iIndex==m_iTreeColumn)
				m_iTreeColumn=-1;
			else if(iIndex<m_iTreeColumn)
				m_iTreeColumn--;
			delete m_aColumns[iIndex];
		}

	}m_aFColumns;
	//		SERVICE
	int HitTestEx(CPoint &point, int *col, CIListRow* &pIRow) const;

	BOOL HitTestLabel(CIListRow* pIRow, CPoint point, int iIItem=0)
	{
		if(pIRow==NULL)
			return FALSE;

		CRect rect(0,0,0,0);
		GetSubItemRect(pIRow->m_iListIndex,  iIItem, LVIR_LABEL, rect);
		if(m_aFColumns.m_iTreeColumn==iIItem)
		{
			if(!bDoNotShowTree){
				rect.left+=(pIRow->m_iDeep-1)*FL_IMAGE_WIDTH-(pIRow->m_iDeep-1)*FL_BRANCH_DECOFFSET;
			}
			rect.left+=FL_IMAGE_WIDTH;		
		}
		//rect.right=rect.left+FL_IMAGE_WIDTH;
		if(rect.PtInRect(point))
			return TRUE;
		return FALSE;
	}

	BOOL HitTestCollapse(CIListRow* pIRow, CPoint point)
	{
		CRect rect(0,0,0,0);
		if(pIRow<0)
			return FALSE;
		GetSubItemRect(pIRow->m_iListIndex,  m_aFColumns.m_iTreeColumn, LVIR_BOUNDS, rect);
		if(!bDoNotShowTree){
			rect.left+=(pIRow->m_iDeep-1)*FL_IMAGE_WIDTH-(pIRow->m_iDeep-1)*FL_BRANCH_DECOFFSET;
		}
		rect.right=rect.left+FL_IMAGE_WIDTH;
		if(rect.PtInRect(point))
			return TRUE;
		if(point.x<rect.left){//very bad
			return -1;
		}
		return FALSE;
	}

	BOOL HitTestCheckbox(CPoint point, CIListRow* pIRow, int iIItem)
	{
		if(pIRow==NULL || iIItem<0 || !(pIRow->m_aItems[iIItem]->m_iControlType&FLC_CHECKBOX)
			 || !GetReadOnly() && iIItem!=GetSelectedIItem())
			return FALSE;

		CRect rect(0,0,0,0);
		GetSubItemRect(pIRow->m_iListIndex,  iIItem, LVIR_BOUNDS, rect);
		if(m_aFColumns.m_iTreeColumn==iIItem)
		{
			if(!bDoNotShowTree){
				rect.left+=(pIRow->m_iDeep-1)*FL_IMAGE_WIDTH-(pIRow->m_iDeep-1)*FL_BRANCH_DECOFFSET;
			}
			rect.left+=FL_IMAGE_WIDTH;		
		}
		rect.right=rect.left+FL_IMAGE_WIDTH;
		if(rect.PtInRect(point))
			return TRUE;
		return FALSE;
	}

	void ShowIRow(CIListRow *pIRow, int iListIndex, BOOL bRefreshIndexes=TRUE)
	{
		pIRow->m_iListIndex=iListIndex;		
		InsertItem(LVIF_STATE|LVIF_TEXT, iListIndex,pIRow->GetAt(0)->m_strText, 0, 0, 0, 0);
		int iCount=pIRow->Count();
		for(int i=1; i<iCount; i++){
			SetItemText(iListIndex, i, pIRow->GetAt(i)->m_strText);
		}
		SetItemData(iListIndex, (DWORD)pIRow);
#ifdef _DEBUG
		TRACE2("***Setting data %x %i\n",pIRow,iListIndex);
#endif
		if(bRefreshIndexes){
			RefreshIndexes(iListIndex+1);
		}
	}
	//
	void HideIRow(HIROW hIRow, BOOL bRefreshIndexes=TRUE)
	{
		CIListRow* pIRow=FindRow(hIRow);
		HideIRow(pIRow, bRefreshIndexes);
	}
	//
	void HideIRow(CIListRow* pIRow, BOOL bRefreshIndexes=TRUE)
	{
		if(pIRow->m_iListIndex<0)
			return;
		BOOL bOk=DeleteItem(pIRow->m_iListIndex);
#ifdef _DEBUG
		if(!bOk && !bInDestroy){
			DebugBreak();
		}
		TRACE1("*Hiding row %x\n",pIRow);
#endif
		int iListIndex=pIRow->m_iListIndex;
		pIRow->m_iListIndex=-1;
		if(bRefreshIndexes && !bInDestroy)
		{
			int iCount=GetItemCount();
			for(iListIndex; iListIndex<iCount; iListIndex++){			
				((CIListRow*)GetItemData(iListIndex))->m_iListIndex=iListIndex;
			}
		}
	}

	//
	void Expand(CIListRow* pIRow, BOOL bNotify=TRUE)
	{
		if(!pIRow->m_bCollapsed || pIRow->m_iListIndex<0 || pIRow->GetHeadChildPos()==NULL)
			return;
		DWORD dwStartTime=GetTickCount();
		HCURSOR hCur=NULL;
		LockSelChange(TRUE);
		LockMSelChange(TRUE);
		CIListRow* pChildIRow=NULL;
		HIROW hChildIRow = pIRow->GetHeadChildPos();
		pIRow->m_bCollapsed=FALSE;

		int iListIndex=pIRow->m_iListIndex;
		while(hChildIRow!=NULL){
			if(dwStartTime>0 && GetTickCount()-dwStartTime>2000){
				hCur=SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_WAIT)));
				dwStartTime=0;
			}
			pChildIRow=pIRow->GetNextChild(hChildIRow);
			iListIndex++;
			pChildIRow->m_iListIndex=iListIndex;
			ShowIRow(pChildIRow, iListIndex, hChildIRow==NULL ? TRUE : FALSE);
		}
		LockSelChange(FALSE);

		HIROW hSelIRow=GetSelectedIRow();
		SelectIRow(hSelIRow, TRUE);
		CListCtrl::EnsureVisible(iListIndex, TRUE);
		CListCtrl::EnsureVisible(pIRow->m_iListIndex, TRUE);
		if(bNotify){
			NotifyOwner(FLNM_EXPAND, pIRow);
		}
		UpdateIRow(pIRow->m_hIRow);
		if(hCur!=NULL){
			SetCursor(hCur);
		}
		LockMSelChange(FALSE);
	}

	//Возвращает количетсво заколлапсенных элементов
	int Collapse(CIListRow* pIRow, BOOL bMoveSelection=TRUE)
	{
		if(pIRow->m_bUncollapsible){
			return 0;
		}
		int iCollapsedCount=0;
		if(pIRow->m_bCollapsed || pIRow->GetHeadChildPos()==NULL){
			return iCollapsedCount;
		}
		DWORD dwStartTime=GetTickCount();
		static HCURSOR hCur=NULL;
		BOOL bChangeSelect=IsAnyChildSelected(pIRow);
		int iItemListIndex=pIRow->m_iListIndex;
		LockSelChange(TRUE);
		LockMSelChange(TRUE);
		if(bLockUpdateForCollapse){
			LockWindowUpdate();
		}
		CIListRow* pChildIRow=NULL;
		HIROW hChildIRow = pIRow->GetHeadChildPos();
		pIRow->m_bCollapsed=TRUE;
		while(hChildIRow!=NULL){
			if(hCur==NULL && ((dwStartTime>0 && GetTickCount()-dwStartTime>2000) || bLockUpdateForCollapse)){
				hCur=SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_WAIT)));
				dwStartTime=0;
			}
			pChildIRow=pIRow->GetNextChild(hChildIRow);
			int iCollapsedChilds=Collapse(pChildIRow, FALSE);
			HideIRow(pChildIRow,TRUE);
			iCollapsedCount++;
		}
		LockSelChange(FALSE);
		if(iCollapsedCount>0){
			NotifyOwner(FLNM_COLLAPSE, pIRow);
		}
		if(bMoveSelection){
			if(bChangeSelect){
				SelectIRow(pIRow->m_hIRow, TRUE);
			}else{
				UpdateIRow(pIRow->m_hIRow);
			}
		}
		if(hCur!=NULL){
			SetCursor(hCur);
			hCur=NULL;
		}
		if(bLockUpdateForCollapse){
			UnlockWindowUpdate();
		}
		LockMSelChange(FALSE);
		return iCollapsedCount;
	}

	//
	BOOL IsAnyChildSelected(CIListRow* pIRow)
	{
		int iDeep=pIRow->m_iDeep;
		int iCount= GetItemCount();
		if(pIRow->m_iListIndex<0)
			return FALSE;
		CIListRow* pCurIRow;
		for(int i=pIRow->m_iListIndex+1; i<iCount; i++)
		{	
			pCurIRow=(CIListRow*)GetItemData(i);
			if(pCurIRow->m_iDeep<=iDeep)
				break;
			if(pCurIRow->m_hIRow==GetSelectedIRow())
				return TRUE;
		}
		return FALSE;
	}
	//
	BOOL IsCollapsed(CIListRow* pIRow)
	{
		return pIRow->m_bCollapsed;
	}
	//

	//		CUSTOM DRAW
protected:
	bool IsDraw(){return true;}
	bool IsNotifyItemDraw(){return true;}
	bool IsItemDraw(int /*nItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return true; }
	bool IsNotifySubItemDraw(int /*nItem*/, UINT /*nState*/, LPARAM /*lParam*/) { return true; }
	bool IsSubItemDraw(int nItem, int nSubItem, UINT nState, LPARAM lParam) 
	{
		//if(m_aFColumns.GetAt(nSubItem)->m_fTree || 
		//	((CIListRow*)GetItemData(nItem))->GetAt(nSubItem)->m_iImageIndex>=0
		//	|| !nSubItem || GetSelectionMark()==nItem || ((CIListRow*)GetItemData(nItem))->m_fBoldText)
		return true;
		//return false;	
	}

	//
	bool OnSubItemDraw(CDC* pDC, int nItem, int nSubItem, UINT nState, LPARAM lParam);
	BOOL SmartGetSubItemRect(CIListRow* pIRow, int& iSubItem, int nArea, CRect& ref);
	// 
	void GetSubItemRectEx(CIListRow* pIRow, int iSubItem, int nArea, CRect& rect)
	{
		rect.SetRectEmpty();
		int iItem=pIRow->m_iListIndex;
		SmartGetSubItemRect(pIRow, iSubItem, nArea, rect);

		if(TRUE)//nArea==LVIR_LABEL)
		{
			//CIListRow* pIRow=(CIListRow*)GetItemData(iItem);
			CIListItem* pIItem=pIRow->GetAt(iSubItem);
			CImageList* il=GetImageList(LVSIL_SMALL);

			CPoint point;
			point= rect.TopLeft();			
			if(IsFColumnTree(iSubItem))
				point.x+=(pIRow->m_iDeep)*FL_IMAGE_WIDTH-(pIRow->m_iDeep-1)*FL_BRANCH_DECOFFSET;

			if(pIItem->m_iControlType&FLC_CHECKBOX)
				point.x+=FL_IMAGE_WIDTH;
			//				

			if(m_aFColumns.GetAt(iSubItem)->m_fTree
				|| (pIItem->m_iImageIndex>=0 && il!=NULL)
				|| IsFColumnImageOffset(iSubItem))			
				point.x+=FL_IMAGE_WIDTH;

			if(pIItem->m_iControlType&FLC_CHECKBOX || m_aFColumns.GetAt(iSubItem)->m_fTree)
				point.x+=FL_BASE_OFFSET;

			rect.left=point.x;			
		}
	}

	//
	//
protected:

	LRESULT NotifyOwner(UINT code, CIListRow* pIRow)
	{
		CString sText="";
		return NotifyOwner(code, pIRow, sText, 0);
	}

	LRESULT NotifyOwner(UINT code, CIListRow* pIRow, const char* szText, DWORD dwNotifyData=0)
	{
		CString sText=szText;
		return NotifyOwner(code, pIRow, sText, dwNotifyData);
	}

	LRESULT NotifyOwner(UINT code, CIListRow* pIRow, CString& sText, DWORD dwNotifyData)
	{
		if(pIRow==NULL){
			return -1;
		}
		static HIROW s_hIRow=NULL;
		static int	s_iIItem=-1;

		if(IsSelChangeLocked())
			return -1;

		//
		if(code==FLNM_SELCHANGED)
		{			
			if((pIRow!=NULL && s_hIRow!=pIRow->m_hIRow) || 
				(pIRow==NULL && s_hIRow!=NULL) ||
				(!GetReadOnly() && m_iSelectedIItem!=s_iIItem))
			{
				s_hIRow= pIRow==NULL ? NULL : pIRow->m_hIRow;
				s_iIItem= m_iSelectedIItem;			
			}
			else
				return -1;			
		}

		//
		FLNM_ROWSTATE flnmRowState;
		flnmRowState.hdr.code=code;
		flnmRowState.hdr.hwndFrom=m_hWnd;
		flnmRowState.hdr.idFrom=GetDlgCtrlID();
		//
		flnmRowState.bCollapsed= pIRow==NULL ? NULL : pIRow->m_bCollapsed;
		flnmRowState.dwData= pIRow==NULL ? NULL : pIRow->m_dwData;
		flnmRowState.dwNotifyData= dwNotifyData;
		flnmRowState.strText= sText;
		flnmRowState.hIRow= pIRow==NULL ? NULL : pIRow->m_hIRow;
		flnmRowState.iIItem=GetSelectedIItem();
		BOOL bRes=GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(),  (LPARAM) &flnmRowState);
		sText=flnmRowState.strText;
		return bRes;
	}

	//
	void EditPrepare(UINT nChar);

	//
	void EditBegin(UINT nChar=0);

	//
	void EditFinish(FLNM_ROWSTATE *pRowState);	
	//
	BOOL Navigation(UINT nChar, BOOL bControlLostFocus);
};

//
//	CInPlaceButton
//
class CInPlaceButton : public CButton
{
	BOOL m_bCapture;
// Construction
public:
	CInPlaceButton(HIROW hIRow, int iIItem, CString strInitText);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceButton)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);	
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceButton();
	UINT	m_uChar;
	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceButton)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	HIROW m_hIRow;
	int m_iIItem;
	CString m_strInitText;
};


//
//	CInPlaceEdit
//
class CInPlaceEdit : public CEdit
{
// Construction
public:
	CInPlaceEdit(HIROW hIRow, int iIItem, CString strInitText, BOOL bSelectText=TRUE);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);	
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	HIROW m_hIRow;
	int m_iIItem;
	CString m_strInitText;
	BOOL	m_bSelectText;
	UINT	m_uChar;		
};


//
//	CInPlaceSlider
//
class CInPlaceSlider: public CSliderCtrl
{
// Construction
public:
	void UpdatePos();
	CEdit ed;
	CRect rtPos;
	CRect rtePos;
	CInPlaceSlider(HIROW hIRow, int iIItem, DWORD dwMinMax, WORD dwPos);
	void MySetPos(long lNewPos);
	DWORD dwClickPreventer;
// Attributes
public:
	WORD wPos;
	WORD wMin;
	WORD wMax;
// Operations
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceHotKey)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
// Implementation
public:
	virtual ~CInPlaceSlider();
protected:
	//{{AFX_MSG(CInPlaceSlider)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	HIROW m_hIRow;
	int m_iIItem;
};

//
//	CInPlaceSlider
//
class CInPlaceDate: public CMonthCalCtrl
{
// Construction
public:
	CWnd* m_pParent;
	BOOL bSended;
	void SendUp();
	BOOL OnButtonUp();
	COleDateTime m_dt;
	CInPlaceDate(HIROW hIRow, int iIItem, COleDateTime dt, CWnd*);
// Operations
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceHotKey)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CInPlaceSlider)
	afx_msg void OnNcDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	HIROW m_hIRow;
	int m_iIItem;
};


//
//	CInPlaceHotKey
//
CString& GetPluginsDirectory();
#ifndef _NO_XMLHOTKEYSUPPORT
class CInPlaceHotKey : public CStatic
{
// Construction
public:
	CWnd* pRParent;
	BOOL bHKNBt;
	CRect rcHKNBt;
	CRect rcHKNBt1;
	CRect rcHKNBt2;
	CRect rcHKNBt3;
	BOOL bCloseBt;
	CRect rcCloseBt;
	BOOL bMenuBt;
	CRect rcMenuBt;
	COLORREF m_crBackground;
	COLORREF m_crText;
	CFont	m_font;
	BOOL m_bPromiscON;
	CIHotkey* m_dwHotKey;
	CIHotkey* m_dwHotKeyStartup;
	CString sStartupValue;
	CInPlaceHotKey(HIROW hIRow, int iIItem, CIHotkey* oHotKey);
	void ActualizeHotkey(BOOL bFromScratch=TRUE, BOOL bStopKeysOnly=FALSE, UINT iAdded=0);
	virtual void RevertStartupValue(CIHotkey dwNewKey=CIHotkey(0));
	BOOL isStopHotkey(CIHotkey* oHotkey=NULL);
	BOOL AddEventToHotkey(char const* szEvent, CIHotkey*& hk);
	BOOL AbortProcess();
	void PostProcessHotkey(CIHotkey* m_dwHotKey);
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceHotKey)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceHotKey();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceHotKey)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_iIItem;
	HIROW m_hIRow;
};
#endif
//
//		CInPlaceList
//
class CInPlaceList : public CComboBox
{
// Construction
public:
	CInPlaceList(HIROW hIRow, int iIItem, ARR_INPLACE_COMBOITEM* pComboItems , int iSelItem);

// Attributes
public:
	COLORREF m_crBackground;
	COLORREF m_crText;
	CFont	m_font;
	UINT m_nFormat;	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceList)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcDestroy();
	afx_msg void OnCloseup();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	HIROW	m_hIRow;
	int		m_iIItem;	
	//CStringList m_lstItems;
	//CString	m_strSelItem;
	ARR_INPLACE_COMBOITEM	m_aComboItems;
	int		m_iSelItem;
	UINT	m_uChar;
};

BOOL& HotkeysSkipDD();

//
//
//

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLISTCTRL_H__8A28A42B_B2A1_4C84_AD5B_E6AD6048EBE2__INCLUDED_)
