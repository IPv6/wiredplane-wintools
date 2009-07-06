// Uniquer.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Uniquer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUniquerApp

BEGIN_MESSAGE_MAP(CUniquerApp, CWinApp)
//{{AFX_MSG_MAP(CUniquerApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
BOOL bLibEnables=0;
BOOL bReplaceMode=0;
int GetStrMiniHash(const char* szName)
{
	int iHash=0;
	char const* szPos=szName;
	//-----------------------
	while(*szPos){
		iHash+=*szPos;
		szPos++;
	}
	iHash=iHash%20;
	return iHash;
}

DWORD dwDeleteOptions=1;
int WINAPI SpecOptions(DWORD dw)
{
	dwDeleteOptions=dw;
	return 0;
}

int WINAPI Handshake(const char* szIn, int iHash)
{
	if(GetStrMiniHash(szIn)!=iHash){
		return 0;
	}
	bLibEnables=1;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CUniquerApp construction
CUniquerApp::CUniquerApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUniquerApp object
CUniquerApp theApp;

CImgLoader*& WINAPI _bmpX()
{
	static CImgLoader* bmpThis=0;
	/*
	if(!bmpThis){
		bmpThis=&_bmp();
	}
	*/
	return bmpThis;
}

CBitmap* LoadBmpFromPathExt(const char* szPath)
{
	CString sPath=szPath;
	CBitmap* bmpExtern=_bmpX()->LoadBmpFromPath(sPath);
	if(!bmpExtern){
		sPath=Format("%s.jpg",szPath);
		bmpExtern=_bmpX()->LoadBmpFromPath(sPath);
	}
	if(!bmpExtern){
		sPath=Format("%s.gif",szPath);
		bmpExtern=_bmpX()->LoadBmpFromPath(sPath);
	}
	if(!bmpExtern){
		sPath=Format("%s.bmp",szPath);
		bmpExtern=_bmpX()->LoadBmpFromPath(sPath);
	}
	if(!bmpExtern){
		sPath=Format("%s.ico",szPath);
		bmpExtern=_bmpX()->LoadBmpFromPath(sPath);
	}
	CBitmap* bmp=0;
	if(bmpExtern){
		bmp=bmpExtern;//_bmp().Clone(bmpExtern);
		//_bmpX()->ReleaseBmp(bmpExtern);
	}
	return bmp;
}

HWND hDiag=0;
HANDLE hCheckThread=0;
HANDLE hStopEvent=0;
CStringArray sListOfImages;
CArray<long,long> aClosestImage;
CArray<long,long> aClosestImageTaken;
CStringArray sListOfImagesToDelete;
CArray<CDib*,CDib*> sListOfImageDibs;
BOOL iMode=0;
BOOL bStopCheck=0;
BOOL bStopLoad=0;
long lRenameOrder;//sListOfImagesToRename
long lDeletedFiles=0;
CCriticalSection csCheck;
DWORD WINAPI CheckImages(LPVOID pData)
{
	iMode=0;
	bStopCheck=0;
	bStopLoad=0;
	lDeletedFiles=0;
	// Первый этап - сбор дибов
	CSize aMins(100,100);
	CDesktopDC dcDesk;
	int iSize=sListOfImages.GetSize();
	for(int zi=0;zi<iSize;zi++){
		aClosestImage.SetAtGrow(zi,-1);
		aClosestImageTaken.SetAtGrow(zi,0);
	}
	/*CSplashParams* sp=0;
	CSplashWindow* sw=0;
	if(!hDiag){
		sp=new CSplashParams();
		sp->szText="Reading...";
		sp->bgColor=0;
		sp->txColor=RGB(255,255,255);
		sp->dwTextParams|=TXSHADOW_VCENT|TXSHADOW_FILLBG|TXSHADOW_WIDE|TXSHADOW_GLOW|TXSHADOW_MIXBG;////TXSHADOW_SHCOL
		sp->dwSplashAlign=SPLASH_OPACITY;
		sp->dwSplashAlign|=SPLASH_HLEFT;
		sp->dwSplashAlign|=SPLASH_VTOP;
		sw=new CSplashWindow(sp);
	}*/
	{
		CDesktopDC dcDesk;
		CDC dc;
		dc.CreateCompatibleDC(&dcDesk);
		CDC dc2;
		dc2.CreateCompatibleDC(&dcDesk);
		dc.SetStretchBltMode(HALFTONE);
		dc2.SetStretchBltMode(HALFTONE);
		CSmartLock cs(csCheck,TRUE);
		for(int i=0;i<iSize;i++){
			if(bStopCheck){
				break;
			}
			if(bStopLoad){
				iMode=1;
				break;
			}
			if(hDiag){
				::SetWindowText(hDiag,Format("%i%%, %s",long(100*i/double(iSize)),_l("Preparations")));
				::SetWindowText(GetDlgItem(hDiag,IDC_STATUS),Format(" %s #%i/%i...",_l("Preparing image"),i,iSize));
			}
			sListOfImageDibs.SetAtGrow(i,0);
			CBitmap* bmp=_bmpX()->LoadBmpFromPath(sListOfImages[i]);
			CSize size2=GetBitmapSize(bmp);
			if(bmp){
				{// Занести обратно
					//HBITMAP hSmall=0;
					// CopyBitmapToBitmap(*bmp,hSmall,CSize(0,0),&aMins,-1);
					CBitmap bmpSmall;
					bmpSmall.CreateCompatibleBitmap(&dc,aMins.cx,aMins.cy);
					CBitmap* bmpTmp=dc.SelectObject(bmp);
					CBitmap* bmpTmp2=dc2.SelectObject(&bmpSmall);
					dc2.StretchBlt(0,0,aMins.cx,aMins.cy,&dc,0,0,size2.cx,size2.cy,SRCCOPY);
					dc2.SelectObject(&bmpTmp2);
					dc.SelectObject(&bmpTmp);
					CDib* pDib=new CDib();
					if(pDib){
						pDib->Create(aMins.cx,aMins.cy);
						pDib->SetBitmap(&dc2,bmpSmall);
						sListOfImageDibs.SetAtGrow(i,pDib);
					}
					//DeleteObject(hSmall);
				}
				_bmpX()->ReleaseBmp(bmp);
			}
		}
		if(!bStopCheck){
			iMode=1;
			if(hDiag){
				::SetWindowText(GetDlgItem(hDiag,ID_NEXT),_l("Next image pair"));
				::EnableWindow(GetDlgItem(hDiag,ID_NEXT),TRUE);
				::SetWindowText(GetDlgItem(hDiag,IDC_STATUS),CString(" ")+_l("Looking for duplicates")+"...");
				::EnableWindow(GetDlgItem(hDiag,IDC_IMGR_DEL_ALL),TRUE);
				::PostMessage(hDiag,WM_COMMAND,ID_NEXT,0);
			}
		}
	}
	/*
	if(sp){
		delete sp;
	}
	if(sw){
		delete sw;
	}
	*/
	return 0;
};

int iBegin=0;
int iBegin2=0;
CBitmap* btLeft=0;
CBitmap* btRight=0;
DWORD dwPathInLeftRes;
DWORD dwPathInRightRes;
CString sPathInLeft;
CString sPathInRight;
CSize szLeftSize=CSize(0,0);
CSize szRightSize=CSize(0,0);
BOOL bAutoDelRight=0;
#define DMAXAFF	-1.0f
double dMaxAffinity=DMAXAFF;
DWORD WINAPI ContinueCheck(LPVOID p)
{
	static long l=0;
	if(l>0){
		return 0;
	}
	SimpleTracker cl(l);
	HWND hwndDlg=(HWND)p;
	CString sTmp;
	{
		CSmartLock cs(csCheck,TRUE);
		while(iBegin<sListOfImageDibs.GetSize()){
			if(bStopCheck==1){
				return TRUE;
			}
			static DWORD dwPTime=0;
			if(GetTickCount()-dwPTime>500){
				int iSize=sListOfImageDibs.GetSize();
				::SetWindowText(hDiag,Format("%i%%, %s. %s: %i",long(100*iBegin/double(iSize)),_l("Searching"),_l("Found"),sListOfImagesToDelete.GetSize()));
				::SetWindowText(GetDlgItem(hwndDlg,IDC_STATUS),Format(" %s %i-%i/%i",_l("Checking images"),iBegin,iBegin2,iSize));
				dwPTime=GetTickCount();
			}
			if(iBegin2>=sListOfImageDibs.GetSize()){
				iBegin++;
				iBegin2=iBegin;
				if(bReplaceMode){// иначе циклится одинаковости
					iBegin2=0;
				}
				dMaxAffinity=DMAXAFF;
			}
			if(iBegin>=sListOfImageDibs.GetSize()){
				break;
			}
			if(iBegin!=iBegin2){
				BOOL bAlreadyDeleted=0;
				CString sPathInLeftTmp=sListOfImages[iBegin];
				sPathInLeftTmp.TrimRight();
				sPathInLeftTmp.TrimLeft();
				CString sPathInRightTmp=sListOfImages[iBegin2];
				sPathInRightTmp.TrimRight();
				sPathInRightTmp.TrimLeft();
				BOOL bForceAff=0;
				if(sListOfImageDibs[iBegin]==0 || sListOfImageDibs[iBegin2]==0){
					bAlreadyDeleted=1;
				}else{
					for(int k=0;k<sListOfImagesToDelete.GetSize();k++){
						if(sPathInLeftTmp.CompareNoCase(sListOfImagesToDelete[k])==0){
							bAlreadyDeleted=1;
							break;
						}
						if(sPathInRightTmp.CompareNoCase(sListOfImagesToDelete[k])==0){
							bAlreadyDeleted=1;
							break;
						}
					}
				}
				if(!bAlreadyDeleted){
					CDib* dbL=sListOfImageDibs[iBegin];
					CDib* dbR=sListOfImageDibs[iBegin2];
					if(bReplaceMode){
						if(aClosestImageTaken[iBegin2]==0)//0-первый,особенный
						{
							double lAffinity=dbL->CalcDistance(dbR);
							//long lRenameOrder;//sListOfImagesToRename
							if(lAffinity<dMaxAffinity || fabs(dMaxAffinity-DMAXAFF)<1.0f){
								dMaxAffinity=lAffinity;
								if(aClosestImage[iBegin]!=-1){
									aClosestImageTaken[aClosestImage[iBegin]]--;
								}
								aClosestImage[iBegin]=iBegin2;
								aClosestImageTaken[iBegin2]++;
							}
						}
					}else {
						double lAffinity=dbL->CalcAffinity(dbR);
						if(lAffinity>.89 || bForceAff){
							sPathInLeft=sPathInLeftTmp;
							sPathInRight=sPathInRightTmp;
							if(sPathInLeft.CompareNoCase(sPathInRight)!=0){
								::SetWindowText(GetDlgItem(hwndDlg,IDC_LI_ST),sPathInLeft);
								sTmp=sPathInLeft;
								sTmp.MakeLower();
								if(sTmp.Find(".zip")==-1 && !IsFileHTML(sTmp)){
									::EnableWindow(GetDlgItem(hwndDlg,IDC_IMGL_DEL),TRUE);
								}
								::SetWindowText(GetDlgItem(hwndDlg,IDC_RI_ST),sPathInRight);
								sTmp=sPathInRight;
								sTmp.MakeLower();
								if(sTmp.Find(".zip")==-1 && !IsFileHTML(sTmp)){
									::EnableWindow(GetDlgItem(hwndDlg,IDC_IMGR_DEL),TRUE);
								}
								if(btLeft){
									_bmpX()->ReleaseBmp(btLeft);
									btLeft=0;
								}
								if(btLeft==0){
									btLeft=_bmpX()->LoadBmpFromPath(sPathInLeft);
									szLeftSize=GetBitmapSize(btLeft);
									::SetWindowText(GetDlgItem(hwndDlg,IDC_LI_ST),Format("%ix%i; %s; %s; %s:%i%%",szLeftSize.cx,szLeftSize.cy,GetSizeStrFrom(GetFileSize(sPathInLeft),0),sPathInLeft,_l("Similarity"),int(100*lAffinity)));
									dwPathInLeftRes=szLeftSize.cx*szLeftSize.cy;
								}
								if(btRight){
									_bmpX()->ReleaseBmp(btRight);
									btRight=0;
								}
								if(btRight==0){
									btRight=_bmpX()->LoadBmpFromPath(sPathInRight);
									szRightSize=GetBitmapSize(btRight);
									::SetWindowText(GetDlgItem(hwndDlg,IDC_RI_ST),Format("%ix%i; %s; %s; %s:%i%%",szRightSize.cx,szRightSize.cy,GetSizeStrFrom(GetFileSize(sPathInRight),0),sPathInRight,_l("Similarity"),int(100*lAffinity)));
									dwPathInRightRes=szRightSize.cx*szRightSize.cy;
								}
								if(dwPathInRightRes>dwPathInLeftRes){
									SendMessage(hwndDlg,DM_SETDEFID,IDC_IMGL_DEL,0);
									::SetFocus(GetDlgItem(hwndDlg,IDC_IMGL_DEL));
									::SetWindowText(GetDlgItem(hwndDlg,IDC_IMGR_DEL),_l("Delete this image"));
									::SetWindowText(GetDlgItem(hwndDlg,IDC_IMGL_DEL),_l("Delete this image")+"*");
								}else{
									SendMessage(hwndDlg,DM_SETDEFID,IDC_IMGR_DEL,0);
									::SetFocus(GetDlgItem(hwndDlg,IDC_IMGR_DEL));
									::SetWindowText(GetDlgItem(hwndDlg,IDC_IMGR_DEL),_l("Delete this image")+"*");
									::SetWindowText(GetDlgItem(hwndDlg,IDC_IMGL_DEL),_l("Delete this image"));
								}
								::InvalidateRect(hwndDlg,0,TRUE);
								break;
							}
						}
					}
				}
			}
			iBegin2++;
		}
	}
	::EnableWindow(GetDlgItem(hwndDlg,ID_NEXT),TRUE);
	if(iBegin>=sListOfImageDibs.GetSize()){
		::EnableWindow(GetDlgItem(hwndDlg,IDC_IMGL_DEL),FALSE);
		::EnableWindow(GetDlgItem(hwndDlg,IDC_IMGR_DEL),FALSE);
		::EnableWindow(GetDlgItem(hwndDlg,ID_NEXT),FALSE);
		PostMessage(hwndDlg,WM_COMMAND,IDOK,0);
	}else{
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATUS),Format(" %s: %i-%i/%i",_l("Choose one"),iBegin,iBegin2,sListOfImageDibs.GetSize()));
	}
	if(bAutoDelRight){
		if(bStopCheck==0){
			PostMessage(hwndDlg,WM_COMMAND,WM_USER,0);
		}
	}
	return 0;
}

int CALLBACK OptionsDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG){
		iBegin=0;
		iBegin2=0;
		hDiag=hwndDlg;
		bAutoDelRight=0;
		if(GetKeyState(VK_CONTROL)<0){
			bReplaceMode=1;
		}
		::SetWindowText(GetDlgItem(hwndDlg,IDC_STATUS),CString(" ")+_l("Preparing for search"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_IMGL_DEL),_l("Delete this image"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_IMGR_DEL),_l("Delete this image"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_IMGR_DEL_ALL),_l("Auto"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_LI_ST),_l("No image, please wait"));
		::SetWindowText(GetDlgItem(hwndDlg,IDC_RI_ST),_l("No image, please wait"));
		if(bReplaceMode){
			::SetWindowText(GetDlgItem(hwndDlg,ID_NEXT),"Stop renamer");
		}else{
			::SetWindowText(GetDlgItem(hwndDlg,ID_NEXT),_l("Stop processing"));
		}
		::SetWindowText(hwndDlg,_l("Search for duplicated images"));
		::EnableWindow(GetDlgItem(hwndDlg,IDC_IMGR_DEL_ALL),FALSE);
		::EnableWindow(GetDlgItem(hwndDlg,IDC_IMGL_DEL),FALSE);
		::EnableWindow(GetDlgItem(hwndDlg,IDC_IMGR_DEL),FALSE);
		SetTaskbarButton(hwndDlg);
		//::EnableWindow(GetDlgItem(hwndDlg,ID_NEXT),FALSE);
	}
	if(uMsg==WM_KEYDOWN){
		if(GetKeyState(VK_CONTROL)<0){
			if(wParam==VK_RETURN){
				uMsg=WM_COMMAND;
				wParam=IDOK;
			}
		}
	}
	if(bStopCheck==0){
		if(uMsg==WM_COMMAND && wParam==WM_USER){
			// Выбираем у кого разрешение ниже...
			CString sPath;
			if(dwPathInRightRes<dwPathInLeftRes){
				uMsg=WM_COMMAND;
				wParam=IDC_IMGR_DEL;
			}else{
				uMsg=WM_COMMAND;
				wParam=IDC_IMGL_DEL;
			}
		}
		if(uMsg==WM_COMMAND && wParam==IDC_IMGL_DEL){
			if(IsWindowEnabled(GetDlgItem(hwndDlg,IDC_IMGL_DEL))){
				if(sPathInLeft!=""){
					sListOfImagesToDelete.Add(sPathInLeft);
				}
			}
			sPathInLeft="";
			sPathInRight="";
			uMsg=WM_COMMAND;
			wParam=ID_NEXT;
		}
		if(uMsg==WM_COMMAND && wParam==IDC_IMGR_DEL){
			if(IsWindowEnabled(GetDlgItem(hwndDlg,IDC_IMGR_DEL))){
				if(sPathInRight!=""){
					sListOfImagesToDelete.Add(sPathInRight);
				}
			}
			sPathInLeft="";
			sPathInRight="";
			uMsg=WM_COMMAND;
			wParam=ID_NEXT;
		}
		if(uMsg==WM_COMMAND && wParam==IDC_IMGR_DEL_ALL){
			if(bAutoDelRight || AfxMessageBox(_l("Delete duplicated images automatically")+"?",MB_YESNO|MB_ICONQUESTION)==IDYES){
				bAutoDelRight=1-bAutoDelRight;
				if(bAutoDelRight){
					::SetWindowText(GetDlgItem(hwndDlg,IDC_IMGR_DEL_ALL),_l("Stop"));
					PostMessage(hwndDlg,WM_COMMAND,WM_USER,0);
				}else{
					::SetWindowText(GetDlgItem(hwndDlg,IDC_IMGR_DEL_ALL),_l("Auto"));
				}
			}
		}
		if(uMsg==WM_COMMAND && wParam==ID_NEXT){
			if(iMode==0){
				bStopLoad=1;
			}else if(bStopCheck==0){
				iBegin2++;
				::EnableWindow(GetDlgItem(hwndDlg,IDC_IMGL_DEL),FALSE);
				::EnableWindow(GetDlgItem(hwndDlg,IDC_IMGR_DEL),FALSE);
				::EnableWindow(GetDlgItem(hwndDlg,ID_NEXT),FALSE);
				FORK(ContinueCheck,hwndDlg);
			}
		}
	}
	if(uMsg==WM_PAINT){
		PAINTSTRUCT lpPaint;
		memset(&lpPaint,0,sizeof(lpPaint));
		HDC dcH=BeginPaint(hwndDlg,&lpPaint);
		CDC dc;
		dc.Attach(lpPaint.hdc);
		dc.SetStretchBltMode(HALFTONE);
		//DefWindowProc(
		if(sPathInLeft!=""){
			if(btLeft){
				CRect rt,rtw;
				::GetClientRect(GetDlgItem(hwndDlg,IDC_IMGL),&rt);
				::GetWindowRect(GetDlgItem(hwndDlg,IDC_IMGL),&rtw);
				CPoint ptLT(rtw.left,rtw.top);
				ScreenToClient(hwndDlg,&ptLT);
				CDC dc2;
				CDesktopDC dcDesk;
				dc2.CreateCompatibleDC(&dcDesk);
				CBitmap* bmpTmp=dc2.SelectObject(btLeft);
				FitImageToRect(rt,szLeftSize);
				dc.StretchBlt(ptLT.x+rt.left,ptLT.y+rt.top,rt.Width(),rt.Height(),&dc2,0,0,szLeftSize.cx,szLeftSize.cy,SRCCOPY);
				dc2.SelectObject(bmpTmp);
			}
		}
		if(sPathInRight!=""){
			if(btRight){
				CRect rt,rtw;
				::GetClientRect(GetDlgItem(hwndDlg,IDC_IMGR),&rt);
				::GetWindowRect(GetDlgItem(hwndDlg,IDC_IMGR),&rtw);
				CPoint ptLT(rtw.left,rtw.top);
				ScreenToClient(hwndDlg,&ptLT);
				CDC dc2;
				CDesktopDC dcDesk;
				dc2.CreateCompatibleDC(&dcDesk);
				CBitmap* bmpTmp=dc2.SelectObject(btRight);
				FitImageToRect(rt,szLeftSize);
				dc.StretchBlt(ptLT.x+rt.left,ptLT.y+rt.top,rt.Width(),rt.Height(),&dc2,0,0,szRightSize.cx,szRightSize.cy,SRCCOPY);
				dc2.SelectObject(bmpTmp);
			}
		}
		dc.Detach();
		EndPaint(hwndDlg,&lpPaint);
		return TRUE;
	}
	if(uMsg==WM_SYSCOMMAND && wParam==SC_CLOSE){
		uMsg=WM_COMMAND;
		wParam=IDOK;
	}
	if(uMsg==WM_COMMAND && wParam==IDOK){
		bStopCheck=1;
		csCheck.Lock();
		csCheck.Unlock();
		if(sListOfImagesToDelete.GetSize()>0){
			if(dwDeleteOptions){
				if(AfxMessageBox(Format("%s. %s: %i\n%s?",_l("Search for duplicated images: finished"),_l("Image(s) found"),sListOfImagesToDelete.GetSize(),_l("Do you really want to delete image(s)")),MB_YESNO|MB_ICONQUESTION)==IDYES){
					for(int i=0;i<sListOfImagesToDelete.GetSize();i++){
						char szPath[MAX_PATH+2]="";
						memset(&szPath,0,sizeof(szPath));
						strcpy(szPath,sListOfImagesToDelete[i]);
						if(isFileExist(szPath)){
							SHFILEOPSTRUCT str;
							memset(&str,0,sizeof(str));
							str.hwnd=hwndDlg;
							str.wFunc=FO_DELETE;
							str.pFrom=szPath;
							str.pTo=NULL;
							str.fFlags=FOF_ALLOWUNDO|FOF_NOCONFIRMATION|FOF_SILENT;
							SHFileOperation(&str);
							lDeletedFiles++;
						}
					}
				}
			}else{
				lDeletedFiles+=sListOfImagesToDelete.GetSize();
			}
		}
		if(bReplaceMode){
			CString sDump;
			int ik=0;
			for(ik=0;ik<aClosestImage.GetSize();ik++){
				sDump+=Format("%i -> %i, %i %s\n",ik,aClosestImage[ik],aClosestImageTaken[ik],sListOfImages[ik]);
				aClosestImageTaken[ik]=0;
			}
			int iCnt=0;
			int iJumper=-1;
			CStringArray aFrom;
			CStringArray aFromTo;
			while(1){
				if(iJumper==-1){
					// Ищем первую картинку
					for(ik=0;ik<aClosestImage.GetSize();ik++){
						if(aClosestImageTaken[ik]==0){
							iJumper=ik;
							break;
						}
					}
				}
				if(iJumper==-1){
					// Не нашли!!
					break;
				}
				aClosestImageTaken[iJumper]=1;// Чтобы не циклится
				int iNext=aClosestImage[iJumper];
				if(iNext==-1 || aClosestImageTaken[iNext]!=0){
					iJumper=-1;
					continue;
				}
				CString sFrom=sListOfImages[iJumper];
				CString sTo=Format("%s%05i_%s",GetPathPart(sFrom,1,1,0,0),iCnt+1,GetPathPart(sFrom,0,0,1,1));
				aFrom.SetAtGrow(iCnt,sFrom);
				aFromTo.SetAtGrow(iCnt,sTo);
				sDump+=Format("Jumping %i -> %i\n",iJumper,iNext);
				iJumper=iNext;
				iCnt++;
			}
			SaveFile("f:\\rename.txt",sDump);
			for(int i=0;i<aFrom.GetSize();i++)
			{
				if(isFileExist(aFrom[i])){
					SHFILEOPSTRUCT str;
					memset(&str,0,sizeof(str));
					str.hwnd=hwndDlg;
					str.wFunc=FO_RENAME;
					
					char szPath[MAX_PATH+2]="";
					memset(&szPath,0,sizeof(szPath));
					strcpy(szPath,aFrom[i]);
					str.pFrom=szPath;
					
					char szPath2[MAX_PATH+2]="";
					memset(&szPath2,0,sizeof(szPath2));
					strcpy(szPath2,aFromTo[i]);
					str.pTo=szPath2;
					str.fFlags=FOF_NOCONFIRMATION|FOF_SILENT;
					SHFileOperation(&str);
				}
			}

		}
		EndDialog(hwndDlg,0);
		return TRUE;
	}
	if(uMsg==WM_NCDESTROY){
		SetEvent(hStopEvent);
	}
	return FALSE;
}

int FindMini(CString sL,CString sR)
{
	sL.MakeLower();
	sR.MakeLower();
	return sL.Find(sR);
}

int WINAPI FindDupes2(const char* szList, HWND hParent)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	sListOfImages.RemoveAll();
	sListOfImagesToDelete.RemoveAll();
	sListOfImageDibs.RemoveAll();
	CString sFiles=szList;
	sFiles.Replace("\r","");
	sFiles+="\n";
	while(sFiles.GetLength()>0){
		CString sWPTitle=sFiles.SpanExcluding("\n");
		if(strlen(sWPTitle)!=strlen(sFiles)){
			sFiles=sFiles.Mid(strlen(sWPTitle)+1);
		}else{
			sFiles="";
		}
		if(sWPTitle!="" && FindMini(sFiles,sWPTitle+"\n")==-1){
			sListOfImages.Add(sWPTitle);
		}
	}
	CheckImages(0);
	{
		CSplashParams sp;
		sp.szText="Reading...";
		sp.bgColor=0;
		sp.txColor=RGB(255,255,255);
		sp.dwTextParams|=TXSHADOW_VCENT|TXSHADOW_FILLBG|TXSHADOW_WIDE|TXSHADOW_GLOW|TXSHADOW_MIXBG;////TXSHADOW_SHCOL
		sp.dwSplashAlign=SPLASH_OPACITY;
		sp.dwSplashAlign|=SPLASH_HLEFT;
		sp.dwSplashAlign|=SPLASH_VTOP;
		CSplashWindow sw(&sp);
		{//!!!
			for(int i=0;i<sListOfImageDibs.GetSize()-1;i++){
				int iClosest=i+1;
				if(sListOfImageDibs[i] && sListOfImageDibs[iClosest]){
					double daffmin=sListOfImageDibs[i]->CalcDistance(sListOfImageDibs[iClosest]);
					for(int j=i+2;j<sListOfImageDibs.GetSize();j++){
						if(sListOfImageDibs[j]){
							double lAffinity=sListOfImageDibs[i]->CalcDistance(sListOfImageDibs[j]);
							if(daffmin>lAffinity){
								daffmin=lAffinity;
								iClosest=j;
							}
						}
					}
				}
				// iClosest<>i+1
				CString sTmp=sListOfImages[iClosest];
				sListOfImages[iClosest]=sListOfImages[i+1];
				sListOfImages[i+1]=sTmp;
				CDib* sTmp2=sListOfImageDibs[iClosest];
				sListOfImageDibs[iClosest]=sListOfImageDibs[i+1];
				sListOfImageDibs[i+1]=sTmp2;
				sw.SetText(Format("Checking %i",i));
			}
		}
		sw.SetText("Renaming...");
		for(int i=0;i<sListOfImageDibs.GetSize();i++){
			MoveFile(sListOfImages[i],GetDirectoryFromPath(sListOfImages[i])+Format("%04i_%s",i,GetFileFromPath(sListOfImages[i])));
			if(sListOfImageDibs[i]){
				delete sListOfImageDibs[i];
			}
		}
	}
	return 0;
}

int WINAPI FindDupes(const char* szList, HWND hParent)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	hStopEvent=CreateEvent(0,0,0,0);
	sListOfImages.RemoveAll();
	sListOfImagesToDelete.RemoveAll();
	sListOfImageDibs.RemoveAll();
	CString sFiles=szList;
	sFiles.Replace("\r","");
	sFiles+="\n";
	while(sFiles.GetLength()>0){
		CString sWPTitle=sFiles.SpanExcluding("\n");
		if(strlen(sWPTitle)!=strlen(sFiles)){
			sFiles=sFiles.Mid(strlen(sWPTitle)+1);
		}else{
			sFiles="";
		}
		if(sWPTitle!="" && FindMini(sFiles,sWPTitle+"\n")==-1){
			sListOfImages.Add(sWPTitle);
		}
	}
	if(hCheckThread){
		TerminateThread(hCheckThread,0);
		hCheckThread=0;
	}
	DWORD dwID=0;
	hCheckThread=CreateThread(0,0,CheckImages,0,0,&dwID);
	DialogBox(theApp.m_hInstance,MAKEINTRESOURCE(IDD_DIALOG),hParent,OptionsDialogProc);
	bStopCheck=1;
	csCheck.Lock();
	csCheck.Unlock();
	WaitForSingleObject(hStopEvent,1000);
	CloseHandle(hStopEvent);
	if(hCheckThread){
		TerminateThread(hCheckThread,0);
	}
	CloseHandle(hCheckThread);
	hCheckThread=0;
	hStopEvent=0;
	for(int i=0;i<sListOfImageDibs.GetSize();i++){
		if(sListOfImageDibs[i]){
			delete sListOfImageDibs[i];
		}
	}
	sPathInLeft="";
	sPathInRight="";
	if(btLeft){
		_bmpX()->ReleaseBmp(btLeft);
		btLeft=0;
	}
	if(btRight){
		_bmpX()->ReleaseBmp(btRight);
		btRight=0;
	}
	return lDeletedFiles;
}

CWnd* AfxWPGetMainWnd()
{
	return 0;
}

bool fakemulti=false;
#ifndef NOSTUB_VC6
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>
#endif
