/*
Module : ICONDLG.H
Purpose: Interface for an MFC class for an Icon Picker dialog similar to 
the version found in Internet Explorer 4 PP2
Created: PJN / ICONDLG/1 / 25-07-1997
History: None

  Copyright (c) 1997 by PJ Naughter.  
  All rights reserved.
  
*/


////////////////////////////////// Macros ///////////////////////////
#ifndef __ICONDLG_H__
#define __ICONDLG_H__


/////////////////////////////////  Includes  //////////////////////////////////
#include "resource.h"



/////////////////////////////////  Classes ////////////////////////////////////
class CIconDialog : public CDialog
{
public:
	CString sItemName;
	CIconDialog(CWnd* pParent = NULL);
	
	BOOL SetIcon(const CString& sFilename, int nIndex);
	void GetIcon(CString& sFilename, int& nIconIndex);
	
protected:
	//{{AFX_VIRTUAL(CIconDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	//{{AFX_DATA(CIconDialog)
	enum { IDD = IDD_CHOOSE_ICON };
	CListBox m_ctrlIconList;
	CString	m_sFilename;
	int		m_nIconIndex;
	//}}AFX_DATA
	
	//{{AFX_MSG(CIconDialog)
	afx_msg void OnBrowse();
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnDblclkIconlist();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
		void UpdateIconList();
};



#endif //__ICONDLG_H__

