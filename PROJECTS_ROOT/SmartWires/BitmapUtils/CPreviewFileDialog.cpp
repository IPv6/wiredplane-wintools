// PreviewFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CPreviewFileDialog.h"
#include <dlgs.h>

#define IDC_STATIC_RECT			6000
#define IDC_CHECK_PREVIEW		6001
#define IDC_IMAGEINFO			6002

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CPreviewFileDialog

IMPLEMENT_DYNAMIC(CPreviewFileDialog, CFileDialog)


CPreviewFileDialog::CPreviewFileDialog(CAbstractPreview *pPreview,BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	m_ofn.Flags |= OFN_ENABLETEMPLATE;
	m_ofn.hInstance = AfxGetInstanceHandle();
	m_ofn.lpTemplateName = "FILEOPEN_TEMPLATE";
	m_pPreview = pPreview;
	m_bChanged = FALSE;
	m_bPreview = TRUE;
}


BEGIN_MESSAGE_MAP(CPreviewFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CPreviewFileDialog)
	ON_BN_CLICKED(IDC_CHECK_PREVIEW,OnClickedPreview)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPreviewFileDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPreviewFileDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	ASSERT(pResult != NULL);

	// allow message map to override

	OFNOTIFY* pNotify = (OFNOTIFY*)lParam;
	switch(pNotify->hdr.code)
	{
	case CDN_SELCHANGE:
		OnFileNameChange();
		return TRUE;
	case CDN_FOLDERCHANGE:
		OnFolderChange();
		return TRUE;
	case CDN_INITDONE:
		OnInitDone();
		return TRUE;
	}
	return CFileDialog::OnNotify(wParam, lParam, pResult);
}

void CPreviewFileDialog::OnInitDone()
{
	CWnd *pWnd = GetDlgItem(IDC_STATIC_RECT);
	if(pWnd) 
	{
		CRect cr ;
		pWnd->GetWindowRect(&cr);
		ScreenToClient(&cr);
		cr.top = cr.bottom+15;
		cr.bottom = cr.top+17;
		cr.left += 40;
		checkBox.Create("&Preview",WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX,cr,this,IDC_CHECK_PREVIEW);
		checkBox.SetFont(GetFont());
		checkBox.SetCheck(m_bPreview);

		cr=PreviewRect;
		cr.top = PreviewRect.bottom+15;
		cr.bottom = cr.top+20;
		imageInfo.Create("Image info",WS_CHILD|WS_VISIBLE|SS_CENTER,cr,this,IDC_IMAGEINFO);
		imageInfo.SetFont(GetFont());

		AdjustSize();
	}
}

BOOL CPreviewFileDialog::AdjustSize()
{
	CWnd *pWnd = GetDlgItem(IDC_STATIC_RECT);
	if(pWnd) 
	{
		static CRect cLastRect(0,0,0,0);
		CRect cr;
		pWnd->GetWindowRect(&cr);
		ScreenToClient(&cr);
		if(cr!=cLastRect){
			cLastRect=cr;
			cr.top = cr.bottom + 35;
			cr.bottom = cr.top + 102;
			cr.left += 5;
			//cr.right = cr.left + 102;
			cr.right = cr.right - 10;
			PreviewRect = cr;
			m_Region.DeleteObject();
			m_Region.CreateRectRgn(PreviewRect.left+1,PreviewRect.top+1,PreviewRect.right-1,PreviewRect.bottom-1);

			cr.top = PreviewRect.bottom+15;
			cr.bottom = cr.top + 15;
			if(IsWindow(imageInfo.GetSafeHwnd())){
				imageInfo.MoveWindow(cr);
			}

			return TRUE;
		}
	}
	return FALSE;
}

void CPreviewFileDialog::OnFileNameChange()
{
	if(!m_bChanged && m_bPreview)
	{
		m_pPreview->SetPreviewFile(GetPathName());
		imageInfo.SetWindowText(m_pPreview->GetImageInfo());
		InvalidateRect(&PreviewRect);
	}
	m_bChanged = FALSE;
}


void CPreviewFileDialog::OnFolderChange()
{
	m_bChanged = TRUE;
	m_pPreview->SetPreviewFile(CString(""));
	imageInfo.SetWindowText(m_pPreview->GetImageInfo());
	InvalidateRect(&PreviewRect);
}

void CPreviewFileDialog::OnClickedPreview()
{
	m_bPreview = checkBox.GetCheck() == 1 ? TRUE : FALSE;
	if(!m_bPreview){
		imageInfo.SetWindowText("");
	}
	InvalidateRect(&PreviewRect);
}

void CPreviewFileDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if(m_bPreview)
	{
		dc.SelectClipRgn(&m_Region);
		m_pPreview->DrawPreview(&dc,PreviewRect.left+1,PreviewRect.top+1,PreviewRect.Width()-2,PreviewRect.Height()-2);
		dc.SelectClipRgn(NULL);
	}
	dc.Draw3dRect(&PreviewRect,::GetSysColor(COLOR_BTNSHADOW),::GetSysColor(COLOR_BTNHILIGHT));
}

LRESULT CPreviewFileDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==WM_WINDOWPOSCHANGING || message==WM_SIZE){
		if(AdjustSize()){
			Invalidate();
		}
	}
	LRESULT lRes=CFileDialog::WindowProc(message, wParam, lParam);
	return lRes;
}

