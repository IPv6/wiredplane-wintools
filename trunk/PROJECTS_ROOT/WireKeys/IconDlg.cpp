/*
Module : ICONDLG.CPP
Purpose: Implementation for an MFC class for an Icon Picker dialog similar to 
the version found in Internet Explorer 4 PP2
Created: PJN / ICONDLG/1 / 25-07-1997
History: None

  Copyright (c) 1997 by PJ Naughter.  
  All rights reserved.
  
*/

/////////////////////////////////  Includes  //////////////////////////////////
#include "stdafx.h"
#include "IconDlg.h"
#include "_common.h"
#include "_externs.h"

/////////////////////////////////  Defines  ///////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////  Implementation  /////////////////////////////
CIconDialog::CIconDialog(CWnd* pParent /*=NULL*/)
: CDialog(CIconDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIconDialog)
	m_sFilename = _T("");
	m_nIconIndex = -1;
	//}}AFX_DATA_INIT
	m_nIconIndex = 0;
	
	//Default will be to display icons from shell32 dll
	TCHAR pszSystemDir[_MAX_PATH];
	VERIFY(GetSystemDirectory(pszSystemDir, _MAX_PATH));
	m_sFilename = pszSystemDir;
	m_sFilename += _T("\\SHELL32.dll");
}

BOOL CIconDialog::SetIcon(const CString& sFilename, int nIndex)
{
	BOOL bSuccess = FALSE;
	int nNum = (int) ExtractIcon(AfxGetInstanceHandle(), sFilename, (UINT) -1);
	if (nIndex < nNum)
	{
		m_nIconIndex = nIndex;
		m_sFilename = sFilename;
		bSuccess = TRUE;
	}
	
	return bSuccess;
}

void CIconDialog::GetIcon(CString& sFilename, int& nIconIndex)
{
	sFilename = m_sFilename;
	nIconIndex = m_nIconIndex;
}

void CIconDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIconDialog)
	DDX_Control(pDX, IDC_ICONLIST, m_ctrlIconList);
	DDX_Text(pDX, IDC_FILENAME, m_sFilename);
	DDX_LBIndex(pDX, IDC_ICONLIST, m_nIconIndex);
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
	{
		pDX->PrepareCtrl(IDC_ICONLIST);
		if (m_nIconIndex == LB_ERR)
		{
			CString sCaption=_l("Choose icon");
			CString sMsg=_l("Please select new icon");
			MessageBox(sMsg, sCaption, MB_OK | MB_ICONEXCLAMATION);
			pDX->Fail();
		}
	}
}

BEGIN_MESSAGE_MAP(CIconDialog, CDialog)
ON_WM_CONTEXTMENU()
//{{AFX_MSG_MAP(CIconDialog)
ON_BN_CLICKED(IDBROWSE, OnBrowse)
ON_WM_MEASUREITEM()
ON_WM_DRAWITEM()
ON_WM_DESTROY()
ON_LBN_DBLCLK(IDC_ICONLIST, OnDblclkIconlist)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CIconDialog::OnBrowse() 
{
	//Bring up a standard File open dialog to select the file
	CString sFilter;
	sFilter="Any files (*.*)|*.*||";
	CString sCaption;
	sCaption=_l("Icon chooser");
	
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sFilter, this);
	dlg.m_ofn.lpstrTitle = sCaption;
	
	if (dlg.DoModal() == IDOK)
	{
		m_sFilename = dlg.GetPathName();
		m_nIconIndex = 0;
		UpdateData(FALSE);
		UpdateIconList();
	}
}

void CIconDialog::UpdateIconList() 
{
	//Free up any existing HICON's used for drawing
	int nCount = m_ctrlIconList.GetCount();
	int i;
	for (i=0; i<nCount; i++)
	{
		HICON hIcon = (HICON) m_ctrlIconList.GetItemData(i);
		DestroyIcon(hIcon);
	}
	m_ctrlIconList.ResetContent();
	
	//Do we have a valid filename
	OFSTRUCT ofs;
	if (OpenFile(m_sFilename, &ofs, OF_EXIST) == HFILE_ERROR)
	{    
		CString sMsg=_l("This is not a valid file");
		CString sCaption=_l("Icon chooser");
		MessageBox(sMsg, sCaption, MB_OK | MB_ICONEXCLAMATION);
	}
	else
	{
		//Display the new ones
		int nNum = (int) ExtractIcon(AfxGetInstanceHandle(), m_sFilename, (UINT) -1);
		m_ctrlIconList.ResetContent();
		if (nNum == 0)
		{
			CString sMsg=_l("No icons found");
			CString sCaption=_l("Icon chooser");
			MessageBox(sMsg, sCaption, MB_OK | MB_ICONEXCLAMATION);
		}
		for (i=0; i<nNum; i++)
		{
			HICON hIcon = ExtractIcon(AfxGetInstanceHandle(), m_sFilename, i);
			m_ctrlIconList.InsertString(i, _T(""));
			m_ctrlIconList.SetItemData(i, (LPARAM) hIcon);
		}
		m_ctrlIconList.SetCurSel(0);
	}
	
}

void CIconDialog::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	if (nIDCtl == IDC_ICONLIST)
		lpMeasureItemStruct->itemHeight = GetSystemMetrics(SM_CYICON) + 12;
	else
		CDialog::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CIconDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS) 
{
	if (nIDCtl == IDC_ICONLIST)
	{
		ASSERT(lpDIS->CtlType == ODT_LISTBOX);	
		
		CDC* pDC = CDC::FromHandle(lpDIS->hDC);
		COLORREF cr = (COLORREF)lpDIS->itemData; // RGB in item data
		
		if ((lpDIS->itemState & ODS_SELECTED) &&                         
			(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
		{
			// item has been selected - draw selection rectangle
			COLORREF crHilite = GetSysColor(COLOR_HIGHLIGHT);
			CBrush br(crHilite);
			pDC->FillRect(&lpDIS->rcItem, &br);
		}
		
		if (!(lpDIS->itemState & ODS_SELECTED) &&
			(lpDIS->itemAction & ODA_SELECT))
		{
			// Item has been de-selected -- remove selection rectangle
			CBrush br(RGB(255, 255, 255));
			pDC->FillRect(&lpDIS->rcItem, &br);
		}
		
		//Draw the icon
		pDC->DrawIcon(lpDIS->rcItem.left+2, lpDIS->rcItem.top+4, (HICON) lpDIS->itemData);
	}
	else
		CDialog::OnDrawItem(nIDCtl, lpDIS);
}

void CIconDialog::OnDestroy() 
{
	//Free up the HICON's used for drawing
	int nCount = m_ctrlIconList.GetCount();
	for (int i=0; i<nCount; i++)
	{
		HICON hIcon = (HICON) m_ctrlIconList.GetItemData(i);
		DestroyIcon(hIcon);
	}
	
	//Let the parent do its stuff
	CDialog::OnDestroy();
}

void CIconDialog::OnOK() 
{
	if (GetFocus() == GetDlgItem(IDC_FILENAME))
	{
		GetDlgItem(IDC_FILENAME)->GetWindowText(m_sFilename);
		if (UpdateData(FALSE))
			UpdateIconList();
	}
	else
		CDialog::OnOK();
}

void CIconDialog::OnDblclkIconlist() 
{
	PostMessage(WM_COMMAND, IDOK);	
}

BOOL CIconDialog::OnInitDialog() 
{
	//Let the parent do its stuff
	CDialog::OnInitDialog();
	
	//Update the icon list
	m_ctrlIconList.SetColumnWidth(GetSystemMetrics(SM_CXICON) + 6);  
	UpdateIconList();
	m_ctrlIconList.SetCurSel(m_nIconIndex);
	GetDlgItem(IDC_STATIC2)->SetWindowText(_l("Current icon"));
	GetDlgItem(IDC_STATIC1)->SetWindowText(_l("Choose icon for")+" "+sItemName);
	::SendMessage(::GetDesktopWindow(), WM_SYSCOMMAND, (WPARAM) SC_HOTKEY, (LPARAM)GetSafeHwnd());
	return TRUE;
}



