//************************************************************************
// Generic Window Helper Class Implementation.
//*************************************************************************
// This is a generic window helper class. (Uses MSAA)
// Given a window handle , 
// -> This class can attach to the specified handle
// -> Extracts all information from the window
// -> Supports Win32/Internet Explorer_Server/Symhtml/...hopefully everything
// -> SendMessage/PostMessage to any window element/object using object name
// -> Perform clicks / doubleclicks.
// -> Perform Screen captures (in Bitmap) for the specific window.
//*************************************************************************
//Author: vinith raj
//email : slackerboy3@gmail.com
//Revision : 1 : Jan/17/2007
//My website: http://www.slacksoft.org
//*************************************************************************

#include "StdAfx.h"
#include "WindowHelper.h"

#if(_WIN32_WINNT < 0x0501)
BOOL PrintWindow(
  HWND hwnd,               // Window to copy
  HDC  hdcBlt,             // HDC to print into
  UINT nFlags              // Optional flags
);
#endif

#define     CHILDID_SELF        0
#define     INDEXID_OBJECT      0
#define     INDEXID_CONTAINER   0
#define     OBJID_CLIENT        ((LONG)0xFFFFFFFC)

//*************************************************************************
//*************************************************************************
CWindowHelper::CWindowHelper(void):
m_hwndSource(NULL),
m_hwndParent(NULL)
{
}
//*************************************************************************
//*************************************************************************
CWindowHelper::~CWindowHelper(void)
{
}
//*************************************************************************
//*************************************************************************
HRESULT CWindowHelper::Attach(HWND hwndWindow)
{
	//Release first
	Release();

	//check window
	if(!hwndWindow || !IsWindow(hwndWindow) || !IsWindowVisible(hwndWindow))
		return E_INVALIDARG;

	m_hwndSource=hwndWindow;
	m_hwndParent=::GetParent(m_hwndSource);//::GetAncestor(m_hwndSource, GA_ROOT);

	//Parse Obtained Window
	return ParseWindow();
}
//*************************************************************************
//*************************************************************************
HRESULT CWindowHelper::Release()
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if((*iter))
			delete (*iter);
	}
	m_AccObjects.clear();
	m_hwndSource=NULL;
	if(m_pAcc)
		m_pAcc.Release();
	return S_OK;
}
//*************************************************************************
//GetParent:
//*************************************************************************
HWND CWindowHelper::GetParent()
{
	return m_hwndParent;
}

CString CWindowHelper::GetCombinedTextInfo()
{
	CString sRes;
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		WindowHelperDataStructure* obj=(*iter);
		if(obj->szName!=""){
			sRes+=obj->szRole;
			sRes+=": ";
			sRes+=obj->szName;
			if(obj->szDsc.GetLength()){
				sRes+=" (";
				sRes+=obj->szDsc;
				sRes+=")";
			}
			sRes+="\r\n";
			if(obj->szValue.GetLength()){
				sRes+=obj->szValue;
				sRes+="\r\n";
			}
		}
	}
	return sRes;
}

//*************************************************************************
//GetHandle
//*************************************************************************
HWND CWindowHelper::GetHandle(CString szObjectName)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName))
		{
			return (*iter)->hwndObject;
		}
	}
	return NULL;
}

//*************************************************************************
//GetHandle
//*************************************************************************
HWND CWindowHelper::GetHandle(CString szObjectName, DWORD dwRole)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName) &&
			(*iter)->dwRole==dwRole)
		{
			return (*iter)->hwndObject;
		}
	}
	return NULL;
}

//*************************************************************************
//GetHandle
//*************************************************************************
HWND CWindowHelper::GetHandle(CString szObjectName, CString szRole)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName) &&
			!(*iter)->szRole.CompareNoCase(szRole))
		{
			return (*iter)->hwndObject;
		}
	}
	return NULL;
}
//*************************************************************************
//SendMessage : 
//*************************************************************************
HRESULT CWindowHelper::SendMessageEx(CString szObjectName, UINT Message, WPARAM wParam, LPARAM lParam)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName))
		{
			::SendMessage((*iter)->hwndObject, 
					Message, 
					wParam, 
					lParam);

			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//SendMessage:
//*************************************************************************
HRESULT CWindowHelper::SendMessageEx(CString szObjectName, DWORD dwRole, UINT Message, WPARAM wParam, LPARAM lParam)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName) &&
			(*iter)->dwRole==dwRole)
		{
			::SendMessage((*iter)->hwndObject, 
				Message, 
				wParam, 
				lParam);

			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//Send Message
//*************************************************************************
HRESULT CWindowHelper::SendMessageEx(CString szObjectName, CString szRole, UINT Message, WPARAM wParam, LPARAM lParam)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName) &&
			!(*iter)->szRole.CompareNoCase(szRole))
		{
			::SendMessage((*iter)->hwndObject, 
				Message, 
				wParam, 
				lParam);

			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//PostMessage
//*************************************************************************
HRESULT CWindowHelper::PostMessageEx(CString szObjectName, UINT Message, WPARAM wParam, LPARAM lParam)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName))
		{
			::PostMessage((*iter)->hwndObject, 
				Message, 
				wParam, 
				lParam);

			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//PostMessage
//*************************************************************************
HRESULT CWindowHelper::PostMessageEx(CString szObjectName, DWORD dwRole, UINT Message, WPARAM wParam, LPARAM lParam)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName) &&
			(*iter)->dwRole==dwRole)
		{
			::PostMessage((*iter)->hwndObject, 
				Message, 
				wParam, 
				lParam);

			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//PostMessage
//*************************************************************************
HRESULT CWindowHelper::PostMessageEx(CString szObjectName, CString szRole, UINT Message, WPARAM wParam, LPARAM lParam)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName) &&
			!(*iter)->szRole.CompareNoCase(szRole))
		{
			::PostMessage((*iter)->hwndObject, 
				Message, 
				wParam, 
				lParam);

			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//Click : [in] Object Name
//*************************************************************************
HRESULT CWindowHelper::Click(CString szObjectName)
{
	 WindowHelperDataVectorIter iter;
	 for(iter=m_AccObjects.begin();
		 iter!=m_AccObjects.end();
		 iter++)
	 {
		 if(!(*iter)->szName.CompareNoCase(szObjectName))
		 {
			SendMessage((*iter)->hwndObject, 
						WM_LBUTTONDOWN, 
						/*(WPARAM)MK_LBUTTON*/NULL, 
						MAKELONG((*iter)->pxy.x, 
						(*iter)->pxy.y));

			Sleep(50);
			
			SendMessage((*iter)->hwndObject, 
						WM_LBUTTONUP, 
						/*(WPARAM)MK_LBUTTON*/NULL, 
						MAKELONG((*iter)->pxy.x, 
						(*iter)->pxy.y));
			return S_OK;
		 }
	 }
	return E_FAIL;
}
//*************************************************************************
//Click : [in] Object Name , [in] DWORD dwRole (Overloaded)
//*************************************************************************
HRESULT CWindowHelper::Click(CString szObjectName, DWORD dwRole)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName) && 
			dwRole==(*iter)->dwRole)
		{
			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONDOWN, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			Sleep(50);

			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONUP, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));
			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//Click : [in] CString Object Name, [in] CString Object Role  (Overloaded)
//*************************************************************************
HRESULT CWindowHelper::Click(CString szObjectName, CString szRole)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName) && 
			!(*iter)->szRole.CompareNoCase(szRole))
		{
			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONDOWN, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			Sleep(50);

			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONUP, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));
			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//DoubleClick : [in] CString Object Name
//*************************************************************************
HRESULT CWindowHelper::DblClick(CString szObjectName)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName))
		{
			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONDOWN, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			Sleep(50);

			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONUP, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			Sleep(50);

			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONDOWN, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			Sleep(50);

			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONUP, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//DoubleClick : [in] CString Object Name , [in] DWORD Object Role
//*************************************************************************
HRESULT CWindowHelper::DblClick(CString szObjectName, DWORD dwRole)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName) &&
			(*iter)->dwRole==dwRole)
		{
			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONDOWN, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			Sleep(50);

			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONUP, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			Sleep(50);

			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONDOWN, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			Sleep(50);

			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONUP, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//DoubleClick : [in] CString Object Name , [in] CString Object Role
//*************************************************************************
HRESULT CWindowHelper::DblClick(CString szObjectName, CString szRole)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName) && 
			!(*iter)->szRole.CompareNoCase(szRole))
		{
			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONDOWN, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			Sleep(50);

			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONUP, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			Sleep(50);

			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONDOWN, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			Sleep(50);

			SendMessage((*iter)->hwndObject, 
				WM_LBUTTONUP, 
				(WPARAM)MK_LBUTTON, 
				MAKELONG((*iter)->pxy.x, 
				(*iter)->pxy.y));

			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//Capture
//*************************************************************************
HRESULT CWindowHelper::ConvertToBitmap(CString szObjectName, CString szFileName)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName))
		{
			GetCapture(
				(*iter)->hwndObject, 
				(*iter)->pxy_cl.x,
				(*iter)->pxy_cl.y,
				(*iter)->lHeight,
				(*iter)->lwidth,
				szFileName);
			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//*************************************************************************
HRESULT CWindowHelper::ConvertToBitmap(CString szObjectName, DWORD dwRole, CString szFileName)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName) && 
			(*iter)->dwRole==dwRole)
		{
			GetCapture(
				(*iter)->hwndObject, 
				(*iter)->pxy_cl.x,
				(*iter)->pxy_cl.y,
				(*iter)->lHeight,
				(*iter)->lwidth,
				szFileName);
			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//*************************************************************************
HRESULT CWindowHelper::ConvertToBitmap(CString szObjectName, CString szRole, CString szFileName)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		if(!(*iter)->szName.CompareNoCase(szObjectName) && 
			!(*iter)->szRole.CompareNoCase(szRole))
		{
			GetCapture(
				(*iter)->hwndObject, 
				(*iter)->pxy_cl.x,
				(*iter)->pxy_cl.y,
				(*iter)->lHeight,
				(*iter)->lwidth,
				szFileName);
			return S_OK;
		}
	}
	return E_FAIL;
}
//*************************************************************************
//Capture all objects as bitmap : [in] - capture folder name.
//*************************************************************************
HRESULT CWindowHelper::CaptureAllObjectsAsBitmaps(CString szFolder)
{
	WindowHelperDataVectorIter iter;
	for(iter=m_AccObjects.begin();
		iter!=m_AccObjects.end();
		iter++)
	{
		CString szCaptureFileName;
		szCaptureFileName.Format(_T("%s\\%s.bmp"), szFolder, (*iter)->szName);
		GetCapture(
			(*iter)->hwndObject, 
			(*iter)->pxy_cl.x,
			(*iter)->pxy_cl.y,
			(*iter)->lHeight,
			(*iter)->lwidth,
			szCaptureFileName);
	}
	return E_FAIL;
}
//*************************************************************************
//*************************************************************************
HRESULT CWindowHelper::GetObjectInfo(IAccessible *pAcc, VARIANT varCh,pWindowHelperDataStructure pData)
{
	if(!pAcc || !pData)
		return E_INVALIDARG;

	//name 
	BSTR _chName;
	pAcc->get_accName(varCh, &_chName);
	pData->szName="";
	USES_CONVERSION;
	_bstr_t _chName_t(_chName,false);
	if(_chName_t.length()){
		pData->szName.Format("%s", W2A(_chName));
	}

	BSTR _chVal;
	pAcc->get_accValue(varCh, &_chVal);
	pData->szValue="";
	_bstr_t _chVal_t(_chVal,false);
	if(_chVal_t.length()){
		pData->szValue.Format("%s", W2A(_chVal));
	}

	
	BSTR _chDsc;
	pAcc->get_accDescription(varCh, &_chDsc);
	pData->szDsc="";
	_bstr_t _chDsc_t(_chDsc,false);
	if(_chDsc_t.length()){
		pData->szDsc.Format("%s", W2A(_chDsc));
	}

	//role	
	CComVariant _vRole;
	pAcc->get_accRole(varCh, &_vRole);
	pData->dwRole=_vRole.lVal;
	char szRole[256]={NULL};
	GetRoleText(pData->dwRole, szRole,255);
	pData->szRole.Format(_T("%s"), szRole);
	_vRole.Clear();

	//Screen Cordinates
	long dummyX, dummyY, dummyH,dummyW;
	pAcc->accLocation(&dummyX, &dummyY, &dummyW, &dummyH, varCh);

	//Height and width
	pData->lHeight=dummyH;
	pData->lwidth=dummyW;

	//Median
	pData->pxy.x=dummyX+dummyW/2;
	pData->pxy.y=dummyY+dummyH/2;

	//Screen
	pData->pxy_screen.x=dummyX;
	pData->pxy_screen.y=dummyY;

	//Corner
	pData->pxy_cl.x=dummyX;
	pData->pxy_cl.y=dummyY;

	//Window From Point
	pData->hwndObject=WindowFromPoint(pData->pxy);
	
	//Screen to Client Cordinates
	::ScreenToClient(pData->hwndObject, &(pData->pxy));
	::ScreenToClient(pData->hwndObject, &(pData->pxy_cl));

	return S_OK;
}
//*************************************************************************
//*************************************************************************
//
//Simple rules!!!
//While parsing , 
//(1) - check parent first
//(2) - recursively check children
//		2.a VT_I4 = child object (query it directly)
//		2.b VT_DISPATCH = child frame (recurse it)
//
HRESULT CWindowHelper::ParseWindow()
{
	//check window
	if(!m_hwndSource || !IsWindow(m_hwndSource) || !IsWindowVisible(m_hwndSource))
		return E_INVALIDARG;

	//obtain iaccessible ptr from
	HRESULT hRes=AccessibleObjectFromWindow(m_hwndSource, 
											OBJID_CLIENT, 
											IID_IAccessible, 
											(void **)&m_pAcc);
	if((S_OK==hRes) && m_pAcc)
	{
		//////////////////////////////////////////////////////////////////////////
		//check parent first
		{
			CComVariant _self; _self.vt=VT_I4; _self.lVal=CHILDID_SELF;
			WindowHelperDataStructure *pData=new WindowHelperDataStructure;
			GetObjectInfo(m_pAcc, _self, pData);
			m_AccObjects.push_back(pData);
			_self.Clear();
		}	
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		//check child
		hRes=m_pAcc->get_accChildCount(&m_lChildCount);
		if((S_OK==hRes) && m_lChildCount)
		{	
			VARIANT *varCh=new VARIANT[m_lChildCount];
			long lObtained=0;
			hRes=AccessibleChildren(m_pAcc, 0, m_lChildCount, varCh, &lObtained);
			if(SUCCEEDED(hRes) && lObtained)
			{
				for(long i=0;i<lObtained;i++)
				{
					if(VT_I4==varCh[i].vt)
					{						
						WindowHelperDataStructure *pData=new WindowHelperDataStructure;
						GetObjectInfo(m_pAcc, varCh[i], pData);
						m_AccObjects.push_back(pData);
					} 
					else 
					{
						if(VT_DISPATCH==varCh[i].vt)
						{
							//Recurse for more children
							CComPtr<IAccessible> pAccCh;
							hRes=varCh[i].pdispVal->QueryInterface(IID_IAccessible, (LPVOID*)&pAccCh);
							if(SUCCEEDED(hRes) && pAccCh)
							{
								ParseWindow_Base(pAccCh);
							}
						} 
					}
				} //for
			} // if succeeded
			FREEVARIANTARRAY(varCh, m_lChildCount);
		}
		//////////////////////////////////////////////////////////////////////////
	}
	return hRes;
}
//*************************************************************************
//*************************************************************************
HRESULT CWindowHelper::ParseWindow_Base(IAccessible * pAcc)
{
	if(!pAcc)
		return E_INVALIDARG;


	HRESULT hRes=E_FAIL;
	//////////////////////////////////////////////////////////////////////////
	//check parent first
	{
		CComVariant _self; _self.vt=VT_I4; _self.lVal=CHILDID_SELF;
		WindowHelperDataStructure *pData=new WindowHelperDataStructure;
		GetObjectInfo(pAcc, _self, pData);
		m_AccObjects.push_back(pData);
		_self.Clear();
	}	
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	//check child
	long lChildCount;
	hRes=pAcc->get_accChildCount(&lChildCount);
	if((S_OK==hRes) && lChildCount)
	{	
		VARIANT *varCh=new VARIANT[lChildCount];
		long lObtained=0;
		hRes=AccessibleChildren(pAcc, 0, lChildCount, varCh, &lObtained);
		if((S_OK==hRes) && lObtained)
		{
			for(long i=0;i<lObtained;i++)
			{
				if(VT_I4==varCh[i].vt)
				{
					WindowHelperDataStructure *pData=new WindowHelperDataStructure;
					GetObjectInfo(m_pAcc, varCh[i], pData);
					m_AccObjects.push_back(pData);
				} 
				else 
				{
					if(VT_DISPATCH==varCh[i].vt)
					{
						//recurse for more children
						CComPtr<IAccessible> pAccCh;
						hRes=varCh[i].pdispVal->QueryInterface(IID_IAccessible, (LPVOID*)&pAccCh);
						if(SUCCEEDED(hRes) && pAccCh)
						{
							ParseWindow_Base(pAccCh);
						}
					} 
				}
			} //for
		} // if succeeded
		FREEVARIANTARRAY(varCh, lChildCount);
	}
	//////////////////////////////////////////////////////////////////////////
	return hRes;
}
//*************************************************************************
//Capture
//*************************************************************************
HRESULT CWindowHelper::GetCapture(HWND hwndSrc, int x,int y, int height, int width ,CString szFileName)
{
	if(!hwndSrc || !IsWindow(hwndSrc))
		return E_INVALIDARG;

	CRect TargetRect;
	if(GetWindowRect(hwndSrc, &TargetRect))
	{
		//Attach target dc to source window.
		CDC hTargetDC;
		hTargetDC.Attach(::GetDC(hwndSrc));
		if(hTargetDC.GetSafeHdc()==0)
			return E_UNEXPECTED;

		//create compatible dc to copy target image
		CDC hCaptureDC;
		if(!hCaptureDC.CreateCompatibleDC(&hTargetDC))
			return E_UNEXPECTED;

		CBitmap tempBitmap;
		if(!tempBitmap.CreateCompatibleBitmap(&hTargetDC, TargetRect.Width(), TargetRect.Height()) )
			return E_UNEXPECTED;

		//select the bitmap
		hCaptureDC.SelectObject(tempBitmap);

		//print window
		/* Error: in VC6 не линкуется
		if(!::PrintWindow(hwndSrc, hCaptureDC,NULL))
			return E_UNEXPECTED;
		*/

		//clipping
		CDC hTempDC;
		if(!hTempDC.CreateCompatibleDC(&hTargetDC))
			return E_UNEXPECTED;
 		
		//create compatible bitmap
		m_hBmpSource.DeleteObject();
		if(!m_hBmpSource.CreateCompatibleBitmap(&hTargetDC, width,height))
			return E_UNEXPECTED;
 		
		//select bitmap
 		hTempDC.SelectObject(m_hBmpSource);
 		hTempDC.BitBlt(0,0,width,height, &hCaptureDC, x,y,SRCCOPY);
	}	
	return SaveAsBitmap(szFileName);
}
//*************************************************************************
//Save As Bmp
//*************************************************************************
HRESULT CWindowHelper::SaveAsBitmap(CString szFileName)
{
	HRESULT hr=E_FAIL;
	CDC  hdc;
	FILE* fp=NULL;
	hdc.Attach(::GetDC(NULL));
	if(hdc)
	{
		//bitmap info
		BITMAPINFO bmpInfo;
		ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
		bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);

		//bitmap file header
		BITMAPFILEHEADER  bmpFileHeader; 
		ZeroMemory(&bmpFileHeader, sizeof(BITMAPFILEHEADER));
		bmpFileHeader.bfReserved1=0;
		bmpFileHeader.bfReserved2=0;
		bmpFileHeader.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmpInfo.bmiHeader.biSizeImage;
		bmpFileHeader.bfType='MB';
		bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER); 

		GetDIBits(hdc,m_hBmpSource,0,0,NULL,&bmpInfo,DIB_RGB_COLORS); 

		DWORD dwBmpSize=sizeof(BITMAPINFOHEADER)+sizeof(BITMAPFILEHEADER)+bmpInfo.bmiHeader.biSizeImage;
		BYTE *spBmpData=new BYTE[dwBmpSize];

		if(spBmpData)
		{
			bmpInfo.bmiHeader.biCompression=BI_RGB;
			GetDIBits(hdc, m_hBmpSource, 0, bmpInfo.bmiHeader.biHeight, spBmpData, &bmpInfo, DIB_RGB_COLORS);   
			fp=fopen(szFileName,_T("wb"));
			if(fp)
			{
				fwrite(&bmpFileHeader,sizeof(BITMAPFILEHEADER),1,fp);
				fwrite(&bmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
				fwrite(spBmpData,bmpInfo.bmiHeader.biSizeImage,1,fp); 
				fclose(fp);
				hr=S_OK;				
			}
			delete[] spBmpData;
		}
	}
	return hr;
}	
//*************************************************************************
//*************************************************************************

void __stdcall _com_issue_error(long)
{
	//???
}