#if !defined(AFX_DLGENTERNUMBER_H__DDB6FEEF_734E_42B3_83F2_6FF9E530BC0D__INCLUDED_)
#define AFX_DLGENTERNUMBER_H__DDB6FEEF_734E_42B3_83F2_6FF9E530BC0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLGEnterNumber.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLGEnterNumber dialog

class CDLGEnterNumber : public CDialog
{
// Construction
public:
	DWORD* pdwNumber;
	CDLGEnterNumber(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLGEnterNumber)
	enum { IDD = IDD_DIALOG_ENTER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLGEnterNumber)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLGEnterNumber)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGENTERNUMBER_H__DDB6FEEF_734E_42B3_83F2_6FF9E530BC0D__INCLUDED_)
