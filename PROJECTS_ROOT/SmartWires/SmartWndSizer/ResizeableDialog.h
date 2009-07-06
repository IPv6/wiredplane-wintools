#ifndef _CRESIZE
#define _CRESIZE
/******************************************************************************

  $Author$
  
	$Modtime$
	$Revision$
	
	  Description: Interface of class "CResizeableDialog" (resizeable dialog box with
	  smart controls)
	  
		$Log$
		
******************************************************************************/

/*** Declaration of class "CResizeableDialog" ********************************/
class CResizeableDialog: public CDialog
{
public:
	CRect rLastGripRect;
	void ApplyLayout();
	int iGlobalOffsetX;
	int iGlobalOffsetY;
	BOOL bMiniScroll;
	static const UINT ATTACH_TO_LEFT_EDGE;
	static const UINT ATTACH_TO_RIGHT_EDGE;
	static const UINT ATTACH_TO_VERTICAL_EDGES;
	static const UINT ATTACH_TO_TOP_EDGE;
	static const UINT ATTACH_TO_BOTTOM_EDGE;
	static const UINT ATTACH_TO_HORIZONTAL_EDGES;
	static const UINT ATTACH_TO_ALL_EDGES;
	static const UINT CENTER_HORIZONTAL;
	static const UINT CENTER_VERTICAL;
	static const UINT CENTER;
	int                                    m_nMinWidth;
	int                                    m_nMinHeight;
	int                                    m_nMaxWidth;
	int                                    m_nMaxHeight;
	
	
	CResizeableDialog(UINT nIDTemplate, CWnd* pParentWnd = 0);
	~CResizeableDialog();
	
	UINT RegisterControl(UINT nID, UINT nSmartMode);
	
	void SetStandardSize()
	{
		MoveWindow(m_nStandardXPos, m_nStandardYPos, m_nStandardWidth,
			m_nStandardHeight);
	}
	
	void ThisSizeIsMaxSize();
	void ThisSizeIsMinSize();
	
	// Dialog Data
	//{{AFX_DATA(CResizeableDialog)
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResizeableDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CResizeableDialog)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
private:
	struct CtrlData
	{
		UINT  m_nSmartMode;
		CRect m_rectDistances;
	};
	
	static const int m_nMaxStandardWidth;
	static const int m_nMaxStandardHeight;
	
	CMap<UINT, UINT, CtrlData*, CtrlData*> m_mapCtrls;
	CRect                                  m_rectOrig;
	int                                    m_nScreenWidth;
	int                                    m_nScreenHeight;
	int                                    m_nStandardXPos;
	int                                    m_nStandardYPos;
	int                                    m_nStandardWidth;
	int                                    m_nStandardHeight;
	bool                                   m_bNotYetMaximized;
};
#endif