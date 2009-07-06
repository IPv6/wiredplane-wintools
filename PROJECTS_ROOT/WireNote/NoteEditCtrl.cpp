// NoteEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CNoteEditCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(CNoteEditCtrl)
	ON_NOTIFY_REFLECT(EN_LINK, OnLinkEvent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//ON_WM_PAINT()


BEGIN_MESSAGE_MAP(CNoteComboBoxEx, CComboBoxEx)
	//{{AFX_MSG_MAP(CNoteComboBoxEx)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNoteComboBoxEx
CNoteComboBoxEx::CNoteComboBoxEx():CComboBoxEx()
{
	hbr=0;
	bgColor=RGB(255,255,255);
	txColor=RGB(0,0,0);
}

CNoteComboBoxEx::~CNoteComboBoxEx()
{
}

HBRUSH CNoteComboBoxEx::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor(txColor);
	pDC->SetBkColor(bgColor);
	return NULL;
}

HBRUSH CNoteComboBoxEx::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr_def=CComboBoxEx::OnCtlColor(pDC, pWnd, nCtlColor);
	//pDC->SetTextColor(txColor);
	//pDC->SetBkColor(bgColor);
	//return hbr;
	return hbr_def;
}

/////////////////////////////////////////////////////////////////////////////
// CNoteEditCtrl
CNoteEditCtrl::CNoteEditCtrl():CRichEditCtrl()
{
	pLinkHandler=0;
	bWBProc=0;
	bSmilesPresent=0;
	dwBGColor=0;
	pFont=0;
	sInternalStringCopy="";
	bIgnoreFocus=0;
	bBorderSet=0;
	InFocus=0;
	bWordWrap=0;
	IsFlatInit=0;
	bDisableStandartContextMenu=TRUE;
	pFocusHandler=NULL;
	pFocusHandlerData=NULL;
}

CNoteEditCtrl::~CNoteEditCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CNoteEditCtrl message handlers
DWORD WINAPI OpenLink(LPVOID p)
{
	CString sFile=(const char*)p;
	free(p);
	if(sFile.Find("file://")==0){
		sFile=sFile.Mid(7);
	}
	ShellExecute(NULL,"open",sFile,NULL,NULL,SW_SHOWNORMAL);
	return 0;
}

void CNoteEditCtrl::OnLinkEvent(NMHDR* pLinkDat, LRESULT* pRes)
{
	ENLINK* pRealP=(ENLINK*)pLinkDat;
	if(pRealP->msg==WM_LBUTTONUP){
		SetSel(pRealP->chrg);
		CString sText=GetSelText();
		if(sText!=""){
			WNSubstMyVariables(sText,"web-lnk");
			if(!pLinkHandler || !pLinkHandler->HandleLink(sText)){
				FORK(OpenLink,strdup(sText));
			}
		}
	}
}

double GetTperP_X()
{
	static double dRes=0;
	if(dRes==0){
		HDC lngDC = GetDC(NULL);
		dRes = 1440 / GetDeviceCaps(lngDC, LOGPIXELSX);
		ReleaseDC(NULL, lngDC);
	}
	return dRes;
}

double GetTperP_Y()
{
	static double dRes=0;
	if(dRes==0){
		HDC lngDC = GetDC(NULL);
		dRes = 1440 / GetDeviceCaps(lngDC, LOGPIXELSX);
		ReleaseDC(NULL, lngDC);
	}
	return dRes;
}

typedef int (CALLBACK *_WordBreakPrc)(LPTSTR lpch,int ichCurrent,int cch,int code);
_WordBreakPrc oldProc=0;
#define BREAKS_CHARS	" \t\r\n-+*/\\=;,()[]{}"
/*
WBF_BREAKAFTER	Lines may be broken after the character.	
WBF_BREAKLINE	The character is a delimiter. Delimiters mark the ends of words. Lines may be broken after delimiters.	
WBF_ISWHITE	The character is a white-space character. Trailing white-space characters are not included in the length of a line when wrapping.
*/
int CALLBACK WordBreakPrc(LPTSTR lpch,int ichCurrent,int cch,int code)
{
	if(code==WB_CLASSIFY){
		return WBF_CLASS;
	}
	if(code==WB_ISDELIMITER){
		return strchr(BREAKS_CHARS,lpch[ichCurrent])!=0;
	}
	if(code==WB_RIGHTBREAK || code==WB_RIGHT || code==WB_MOVEWORDNEXT || code==WB_NEXTBREAK || code==WB_MOVEWORDRIGHT){
		int iCh=ichCurrent+1;
		while(iCh && iCh<cch){// Пропускаем делиметиеры если стоим на них
			if(strchr(BREAKS_CHARS,lpch[iCh])){
				iCh++;
			}else{
				break;
			}
		}
		while(iCh && iCh<cch){
			if(code==WB_RIGHTBREAK && lpch[iCh]==':'){
				return iCh;
			}
			if(strchr(BREAKS_CHARS,lpch[iCh])){
				return iCh;
			}
			iCh++;
		}
		return cch;
	}else{
		int iCh=ichCurrent-1;
		while(iCh && iCh<cch){// Пропускаем делиметиеры если стоим на них
			if(strchr(BREAKS_CHARS,lpch[iCh])){
				iCh--;
			}else{
				break;
			}
		}
		while(iCh && iCh<cch){
			if(code==WB_LEFTBREAK && lpch[iCh]==':'){
				return iCh;
			}
			if(strchr(BREAKS_CHARS,lpch[iCh])){
				return iCh+1;
			}
			iCh--;
		}
		return 1;
	}
	return 0;
};

LRESULT CNoteEditCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// Для попапного меню по правой кнопке не вызываем стандартный обработчик
	if(message==WM_RBUTTONUP){
		::PostMessage(GetOwner()->GetSafeHwnd(), message, wParam, lParam);
		return 0;
	}
	if(message==WM_MOUSEWHEEL){
		if(wParam&0x80000000){
			LineScroll(1);
		}else{
			LineScroll(-1);
		}
		return 0;
	}
	if(message==WM_KEYDOWN || message==WM_KEYUP){
		if(wParam==VK_RETURN && isPressed(VK_CONTROL)){
			return 0;
		}
	}
	/*if(objSettings.lNoteLeaveBorder){
		if(bBorderSet==0){
			ModifyStyle(0,WS_BORDER,SWP_FRAMECHANGED);
			bBorderSet++;
		}
	}*/
	if(!bIgnoreFocus){
		if(message==WM_KILLFOCUS){
			InFocus--;
			bBorderSet--;
			/*if(!objSettings.lNoteLeaveBorder){
				ModifyStyle(WS_BORDER,0,SWP_FRAMECHANGED);
			}*/
		}
		if(message==WM_SETFOCUS){
			bBorderSet++;
			InFocus++;
			if(pFocusHandler){
				pFocusHandler(pFocusHandlerData);
			}
			/*if(!objSettings.lNoteLeaveBorder){
				ModifyStyle(0,WS_BORDER,SWP_FRAMECHANGED);
			}*/
			CPoint pt;
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			/*int iW=0;
			int iYLast=0;
			int iFirstVisL=GetFirstVisibleLine();
			int iFirstVisC=LineIndex(iFirstVisL);
			for(int i=iFirstVisC;i<sInternalStringCopy.GetLength() && i<iFirstVisC+1000;i++){
				CPoint p=GetCharPos(i);
				if(p.x>=pt.x){
					if(iW!=0 && p.y>=pt.y){
						SetSel(iW,iW);
						break;
					}
					if(iYLast!=p.y){
						iW=i;
					}
					iYLast=p.y;
				}
			}*/
			POINTL ptL;
			ptL.x=pt.x;
			ptL.y=pt.y;
			int iW=SendMessage(EM_CHARFROMPOS,0,(LPARAM)&ptL);
			SetSel(iW,iW);
		}
	}
	if(message==WM_PRINT || message==WM_PRINTCLIENT){
		CRichEditCtrl::WindowProc(message, wParam, lParam);
		HDC dc=HDC(wParam);
		CDC dC;
		dC.Attach(dc);
		DrawContent(&dC,0);
		dC.Detach();
		return TRUE;
	}
	if(message==WM_KEYUP || message==WM_KEYDOWN || message==WM_CHAR || message==EM_PASTESPECIAL || message==WM_PASTE){
		int wndRes=CRichEditCtrl::WindowProc(message, wParam, lParam);
		int iLen1=sInternalStringCopy.GetLength();
		GetWindowText(sInternalStringCopy);// Тут берем напрямую. Иначе каретка сбрасывается
		int iLen2=sInternalStringCopy.GetLength();
		if(bSmilesPresent && iLen1!=iLen2){
			Invalidate();
		}
		return wndRes;
	}
	if(message==WM_PAINT){
		if(!bWBProc && IsWindow(GetSafeHwnd())){
			bWBProc=1;
			(_WordBreakPrc)::SendMessage(GetSafeHwnd(),EM_SETWORDBREAKPROC,0,(LPARAM)&WordBreakPrc);
		}
		bSmilesPresent=0;
		// Итерируем по смайлам
		CFileInfoArray& dir=GetSmilesEnum();
		if(dir.GetSize()>0){
			HideCaret();
			CDC* dc=GetDC();
			CRgn reg;
			CRect rcClient;
			GetClientRect(&rcClient);
			reg.CreateRectRgnIndirect(&rcClient);
			dc->SelectClipRgn(&reg,RGN_COPY);
			if(pFont){
				dc->SelectObject(pFont);
			}
			// Перебираем смайлы?
			int iFirstVisL=GetFirstVisibleLine();
			int iFirstVisC=LineIndex(iFirstVisL);
			long dwH=0;
			CString sLine=sInternalStringCopy.SpanExcluding("\n");
			CSize size=dc->GetTextExtent(sLine);
			dc->LPtoDP(&size);
			dwH=size.cy;
			for(int i=0;i<dir.GetSize();i++){
				DrawSmile(Format(":%s",dir[i].GetFileTitle()),dir[i].GetFilePath(),dc,iFirstVisC,dwH);
			}
			ReleaseDC(dc);
			ShowCaret();
		}
		int wndRes=CRichEditCtrl::WindowProc(message, wParam, lParam);
		return wndRes;
	}
	return CRichEditCtrl::WindowProc(message, wParam, lParam);
}

BOOL CNoteEditCtrl::DrawSmile(CString sSmile,CString sSmileBmp, CDC* dc,int iFirstVisC,DWORD iLIneH)
{
	if(sInternalStringCopy.Find(sSmile,iFirstVisC)==-1){
		return FALSE;
	}
	int i=iFirstVisC;
	DWORD dwWhite=RGB(255,255,255);
	CRect rcClient;
	GetClientRect(&rcClient);
	//rcClient.DeflateRect(0,0,GetSystemMetrics(SM_CYHSCROLL)+1,0);
	CSize zSmiles=dc->GetTextExtent(sSmile);
	while(i<sInternalStringCopy.GetLength() && i<iFirstVisC+1000){
		int iSPos=sInternalStringCopy.Find(sSmile,i);//+" "???
		if(iSPos!=-1){
			CPoint p=GetCharPos(iSPos);
			if(p.y<rcClient.bottom){
				int iSPosEnd=iSPos+sSmile.GetLength();
				CRect rt(p.x,p.y,p.x+zSmiles.cx,p.y+zSmiles.cy);
				DWORD dwBG=dwBGColor;
				long lSelBeg=0,lSelEnd=0;
				GetSel(lSelBeg,lSelEnd);
				if((iSPos>lSelBeg && iSPos<lSelEnd) || (iSPosEnd>lSelBeg && iSPosEnd<lSelEnd)){
					dwBG=GetSysColor(COLOR_HIGHLIGHT);
				}
				DrawBmp(dc,rt,_bmp().LoadBmpCached(sSmileBmp),&dwBG,0,0,0);//&dwWhite-не катит, ресайз будет!//DT_BOTTOM
				if(rt.right>rcClient.right){
					rt.right=rcClient.right;
				}
				ValidateRect(&rt);
				bSmilesPresent=1;
			}else{
				break;
			}
		}else{
			break;
		}
		i=iSPos+1;
	}
	return TRUE;
}

BOOL CNoteEditCtrl::SetText(const char* sz)
{
	BOOL bNoSet=0;
	CString sText=sz;
	if(sz==NULL){
		GetWindowText(sText);
		bNoSet=1;
	}else{
		sText.Replace("\r\n","\n");
	}
	if(::IsWindow(GetSafeHwnd())){
		SendMessage(WM_SETTEXT,0,(LPARAM)(const char*)sText);// Устанавливаем всегда иначе нет пользы на линках
		if(sText!=sInternalStringCopy){
			Invalidate();
		}
	}
	SetEventMask(GetEventMask()|ENM_LINK);
	SendMessage(EM_AUTOURLDETECT,1,0);
	sInternalStringCopy=sText;
	return 1;
}

BOOL CNoteEditCtrl::SetColor(COLORREF dwTX,COLORREF dwBG)
{
	CHARFORMAT cf;
	cf.cbSize=sizeof(cf);
	cf.dwMask = CFM_COLOR;
	cf.dwEffects = 0;
	cf.crTextColor = dwTX;
	SendMessage(EM_SETCHARFORMAT,(WPARAM)SCF_ALL,(LPARAM)&cf);
	SetBackgroundColor(FALSE,dwBG);
	dwBGColor=dwBG;
	SetText(NULL);
	return 1;
}

void CNoteEditCtrl::OnPaint()
{
	CPaintDC dc(this);
	//CDC* dc=GetDC();
	DrawContent(&dc,1);
	//ReleaseDC(dc);
}

void CNoteEditCtrl::DrawContent(CDC* dc, BOOL bDislpay)
{
	return;
	/*
	//HBRUSH br=GetParent()->OnCtlColor(CDC::FromHandle(HDC(wParam)),this,...);
	// Format the text and render it to the printer.
	RECT rcClient,rcWindow,rcInTwips,rcInTwipsW;
	GetParent()->GetWindowRect(&rcWindow);
	ScreenToClient(&rcWindow);
	GetClientRect(&rcClient);
	dc->FillSolidRect(&rcClient,dwBGColor);
	CPoint pt=GetCharPos(0);
	rcClient.left+=pt.x;//+GetSystemMetrics(SM_CXBORDER);
	rcClient.top+=pt.y;//+GetSystemMetrics(SM_CYBORDER);

	FORMATRANGE fr;
	fr.hdc = *dc;
	fr.hdcTarget = *dc;
	rcInTwips.left=(long)(rcClient.left*GetTperP_X()+GetTperP_X());
	rcInTwips.right=(long)(rcClient.right*GetTperP_X());
	rcInTwips.top=(long)(rcClient.top*GetTperP_Y());
	rcInTwips.bottom=(long)(rcClient.bottom*GetTperP_Y());
	rcInTwipsW.left=(long)(rcWindow.left*GetTperP_X());
	rcInTwipsW.right=(long)(rcWindow.right*GetTperP_X());
	rcInTwipsW.top=(long)(rcWindow.top*GetTperP_Y());
	rcInTwipsW.bottom=(long)(rcWindow.bottom*GetTperP_Y());
	if(!bWordWrap){
		rcInTwips.right=0xFFFF;
		rcInTwipsW.right=0xFFFF;
	}
	fr.rc = rcInTwips;
	fr.rcPage = rcInTwipsW;
	fr.chrg.cpMin = 0;
	fr.chrg.cpMax = -1;
	HRGN hOldReg=0;
	if(!bDislpay){
		CRgn reg;
		rcClient.bottom=rcClient.bottom-rcClient.top+(-rcWindow.top)+GetSystemMetrics(SM_CYBORDER);
		rcClient.top=(-rcWindow.top)+GetSystemMetrics(SM_CYBORDER);//-rcWindow.top+rcClient.top;
		rcClient.right=rcClient.right-rcClient.left+(-rcWindow.left)+GetSystemMetrics(SM_CXBORDER);
		rcClient.left=(-rcWindow.left)+GetSystemMetrics(SM_CXBORDER);//-rcWindow.top+rcClient.top;
		reg.CreateRectRgnIndirect(&rcClient);
		hOldReg=(HRGN)::SelectObject(*dc,HRGN(reg));
	}
	FormatRange(&fr, bDislpay);
	DisplayBand(&rcClient);
	if(hOldReg){
		::SelectObject(*dc,hOldReg);
	}
	*/
}


void CNoteEditCtrl::SetFont(CFont* font, BOOL bRedraw)
{
	CRichEditCtrl::SetFont(font, bRedraw);
	pFont=font;
	SetText(0);
}