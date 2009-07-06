#ifndef _CLIP_H_123
#define _CLIP_H_123

// CDLGClip dialog
class CDLGClip : public CDialog
{
	DECLARE_DYNAMIC(CDLGClip)

public:
	CDLGClip(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDLGClip();

// Dialog Data
	enum { IDD = IDD_CLIPDIALOG };
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	long lCopyCount;
	HANDLE hClipChanged;
	HANDLE hClipChangedOut;
	HWND hNextClipboardChainWnd;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNcDestroy();
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
};
#endif