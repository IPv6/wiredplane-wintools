// ItemModif.cpp: implementation of the ItemModif class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <sys/timeb.h>
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Функции
void CItem::SetModify(BOOL bForcedly)
{
	if(objSettings.bTreeNeedUpdate==0){
		objSettings.bTreeNeedUpdate=bForcedly?2:1;
	}
};

CItem::CItem(DWORD Type)
{
	m_iRevision=0;
	hIRow=NULL;
	CreationTime=COleDateTime::GetCurrentTime();
	SetType(Type);
	SetID(GenerateNewId());
	index=-1;
	iPrior=0;
	bState=-1;
	iListIndex=-1;
	iColorCode=-1;
	ReadOnly=0;
	lShared=0;
	SetModify();
};


void CItem::DeleteFromList()
{
	if(pMainDialogWindow && objSettings.bRunStatus!=-1 && hIRow){
		EnterCriticalSection(&objSettings.bTreeNeedUpdateLock);
		pMainDialogWindow->m_TaskList.DeleteIRow(hIRow);
		iListIndex=-1;
		hIRow=NULL;
		LeaveCriticalSection(&objSettings.bTreeNeedUpdateLock);
	}
	SetModify();
	ItemChanged();
}

void CItem::DeleteItem()
{
	DeleteFromList();
	if(index>=0){
		objSettings.Items[index]=NULL;
		index=-1;
	}
}

CItem::~CItem()
{
	DeleteItem();
};

void CItem::Attach(int AttachToListIndex)
{
	if(index==-1){
		EnterCriticalSection(&objSettings.lock);
		index=objSettings.Items.Add(this);
		LeaveCriticalSection(&objSettings.lock);
	}
	SetModify();
	ItemChanged(AttachToListIndex);
}

void CItem::Detach()
{
	EnterCriticalSection(&objSettings.lock);
	DeleteFromList();
	if(index>=0){
		objSettings.Items[index]=NULL;
		index=-1;
	}
	LeaveCriticalSection(&objSettings.lock);
}

CString CItem::GetTitle(int iCount)
{
	return TrimMessage(m_sTitle,iCount>3?iCount:strlen(m_sTitle)+1);
};

void CItem::SetType(DWORD dwT){
	ItemChanged();
	dwType=dwT;
	switch(dwType){
	case 0://Folder
		SetIconNum(5);
		break;
	case 1://ToDo
		SetPath(TODO_PATH);
		SetIconNum(TR_ICONS_OFFSET);
		break;
	case 2://Note
		SetPath(NOTE_PATH);
		SetIconNum(NOTE_ICON_NUM);
		break;
	case 3://Reminder
		SetPath(REMD_PATH);
		SetIconNum(REM_ICONS_OFFSET);
		break;
	case 4://Links
		SetPath(LINK_PATH);
		SetIconNum(22);
		break;
	case 5://Person
		SetIconNum(99);
		break;
	case 6://WPaper
		SetIconNum(99);
		break;
	}
};

long& CItem::GetColorCode()
{
	return iColorCode;
}

void CItem::SetColorCode(long iCode)
{
	iColorCode=iCode;
	if(iColorCode>=objSettings.objSchemes.GetSize()){
		iColorCode=0;
	}
	SetModify();
	ItemChanged();
}

void CItem::SetTitle(const char* szTitle)
{
	if(szTitle!=NULL && m_sTitle==szTitle){
		return;
	}
	m_sTitle=szTitle;
	m_sTitle.Replace("\n"," ");
	m_sTitle.Replace("\r","");
	m_sTitle.TrimLeft();
	m_sTitle.TrimRight();
	if(m_sTitle==""){
		static int iCount=0;
		m_sTitle=Format("%s #%i",_l("Item"),++iCount);
	}
	SetModify();
	ItemChanged();
};

void CItem::SetPath(const char* szPath)
{
	if(szPath[0]=='/'){
		m_sPath=szPath+1;
	}else{
		m_sPath=szPath;
	}
	if(m_sPath.Right(1)=="/"){
		m_sPath=m_sPath.Left(m_sPath.GetLength()-1);
	}
	SetModify();
	ItemChanged();
};

void CItem::SetCrTime(COleDateTime& Time)
{
	CreationTime=Time;
	SetModify();
	ItemChanged();
};

void CItem::SetPriority(int pr)
{
	iPrior=pr;
	SetModify();
	ItemChanged();
};

void CItem::SetIconNum(int iIcon)
{
	iIconNum=iIcon;
	SetModify();
	ItemChanged();
};

void CItem::SetState(BOOL b)
{
	bState=b;
	SetModify();
	ItemChanged();
};

void CItem::DeleteFolders()
{
	int size=objSettings.Items.GetSize();
	for(int i=0;i<size;i++){
		if(objSettings.Items[i]==NULL){
			continue;
		}
		int iType=objSettings.Items[i]->GetType();
		if(iType==0 || iType==4){
			DeleteItem(objSettings.Items[i]->GetID());
		}
	}
}

void CItem::DeleteItem(const char* szID)
{
	CItem* pItem=GetItemByID(szID);
	if(pItem){
		int i=pItem->index;
		delete objSettings.Items[i];
		objSettings.Items[i]=NULL;
		SetModify();
	}
}

CItem* CItem::GetItemByIndex(int index)
{
	if(index>=0 && index<objSettings.Items.GetSize()){
		return objSettings.Items[index];
	}
	return NULL;
}

CItem* CItem::GetItemByTitle(const char* szTitle)
{
	EnterCriticalSection(&objSettings.lock);
	CItem* pRes=NULL;
	for(int i=0;i<objSettings.Items.GetSize();i++){
		if(objSettings.Items[i] && strcmp(objSettings.Items[i]->GetTitle(),szTitle)==0){
			pRes=objSettings.Items[i];
			break;
		}
	}
	LeaveCriticalSection(&objSettings.lock);
	return pRes;
}

CItem* CItem::GetItemByID(const char* szID)
{
	EnterCriticalSection(&objSettings.lock);
	CItem* pRes=NULL;
	for(int i=0;i<objSettings.Items.GetSize();i++){
		if(objSettings.Items[i] && strcmp(objSettings.Items[i]->GetID(),szID)==0){
			pRes=objSettings.Items[i];
			break;
		}
	}
	LeaveCriticalSection(&objSettings.lock);
	return pRes;
}

CItem* CItem::GetItemByPath(const char* szPath)
{
	if(!szPath){
		return NULL;
	}
	EnterCriticalSection(&objSettings.lock);
	CItem* pRes=NULL;
	for(int i=0;i<objSettings.Items.GetSize();i++){
		const char* szTargetPath=objSettings.Items[i]->GetPath();
		if(objSettings.Items[i] && strcmp(szTargetPath,szPath)==0){
			pRes=objSettings.Items[i];
			break;
		}
	}
	LeaveCriticalSection(&objSettings.lock);
	return pRes;
}

CItem* CItem::GetItemParent(CString* sParentID, CString* sParentPath)
{
	CString sPath=GetPath();
	if(sPath.GetLength()>0 && sPath[0]=='/'){
		sPath=sPath.Mid(1);
	}
	CString sID=sPath.SpanExcluding("/");
	if(sID.GetLength()>0){
		sPath=sPath.Mid(sID.GetLength());
		if(sPath.GetLength()>0 && sPath[0]=='/'){
			sPath=sPath.Mid(1);
		}
		if(sParentID){
			*sParentID=sID;
		}
		if(sParentPath){
			*sParentPath=sPath;
		}
		return CItem::GetItemByID(sID);
	}
	if(sParentID){
		*sParentID=sID;
	}
	if(sParentPath){
		*sParentPath=sPath;
	}
	return NULL;
}

void CItem::SetShared(BOOL bNew,const char* sItemPath)
{
	AddBBExcl(GetPathPart(sItemPath,0,0,1,1));
	SetRevision(atol(CDataXMLSaver::GetInstringPart("[","]",sItemPath)));
	lShared=bNew;
	AsyncPlaySound(SOUND_MSGDELIV);
};

BOOL CItem::isItemCorrupted()
{
	return (m_sID==WRONG_ITEMID);
}

const char szTypePrefix[]="FTNRLPW________";
CString CItem::GenerateNewId()
{
	CString sRes;
	struct _timeb tstruct;
	_ftime(&tstruct);
	DWORD dwRand=rand();
	DWORD dwRand2=rand()<<16;
	dwRand+=dwRand2;
	sRes.Format("%c%04lu_%lx%x",szTypePrefix[dwType],objSettings.dwGlobalCount++,dwRand,tstruct.millitm);
	if(strlen(sRes)>IIDSIZE){
		sRes=sRes.Left(IIDSIZE);
	}
	return sRes;
};

CString CItem::ConvertToString()
{
	CString sRes;
	CDataXMLSaver itemData("");
	itemData.putValue("title",GetTitle());
	itemData.putValue("priority",GetPriority());
	itemData.putValue("shared",IsShared());
	itemData.putValue("state",GetState());
	itemData.putValue("read-only",GetReadOnly());
	itemData.putValue("type",GetType());
	itemData.putValue("icon",GetIconNum());
	itemData.putValue("color",GetColorCode());
	itemData.putValue("id",GetID());
	itemData.putValue("rev",m_iRevision);
	itemData.putValue("path",GetPath());
	itemData.putValue("created",GetCrTime());
	itemData.putValue("author",GetAuthor()==""?objSettings.sDefAuthor:GetAuthor());
	int iIndex=iListIndex;
	if(hIRow!=NULL && pMainDialogWindow){
		int iIndexCur=pMainDialogWindow->m_TaskList.GetListIndex(hIRow);
		if(iIndexCur>=0){
			iIndex=iIndexCur;
		}
	}
	if(iIndex>=0){
		itemData.putValue("list-index",iIndex);
	}
	return itemData.GetResult();
}

void CItem::ConvertFromString(const char* szData,const char* szFileName)
{
	int iTmp=0;
	CString sTmp;
	CDataXMLSaver itemData(szData);
	COleDateTime time;
	itemData.getValue("created",time);
	if(time.GetStatus()==COleDateTime::invalid){
		time=COleDateTime::GetCurrentTime();
	}
	CString st=time.Format("%d.%m.%Y %H:%M:%S");
	SetCrTime(time);
	itemData.getValue("title",sTmp);
	SetTitle(sTmp);
	itemData.getValue("icon",iTmp);
	SetIconNum(iTmp);
	itemData.getValue("color",iTmp);
	SetColorCode(iTmp);
	itemData.getValue("id",sTmp,WRONG_ITEMID);
	SetID(sTmp);
	SetRevision(atol(CDataXMLSaver::GetInstringPart("[","]",szFileName)));
	if(GetRevision()==0){
		itemData.getValue("rev",iTmp,0);
		SetRevision(iTmp);
	}
	itemData.getValue("type",dwType);
	BOOL bRO=0;
	itemData.getValue("read-only",bRO);
	SetReadOnly(bRO);
	itemData.getValue("path",sTmp);
	SetPath(sTmp);
	itemData.getValue("state",iTmp);
	SetState(iTmp);
	itemData.getValue("shared",lShared);
	itemData.getValue("priority",iTmp);
	SetPriority(iTmp);
	itemData.getValue("author",sTmp,objSettings.sDefAuthor);
	SetAuthor(sTmp);
	CString sListIndex;
	itemData.getValue("list-index",sListIndex,"",XMLAsAttribute);
	if(sListIndex.GetLength()>0){
		iListIndex=atol(sListIndex);
	}else{
		iListIndex=-1;
	}
}

void CItem::ActivateChanges(const char* szTitle, DWORD* pdwPrir, DWORD* pdwState)
{
	if(szTitle){
		SetTitle(szTitle);
	}
	if(pdwPrir){
		SetPriority(*pdwPrir);
	}
	if(GetState()!=-1 && pdwState){
		SetState(*pdwState);
	}
	ItemContentChanged();
}

void CItem::SetID(const char* szID){
	if(szID && strlen(szID)>0){
		// Сравниваем SID на уникальность???
		m_sID=szID;
		SetModify();
		ItemChanged();
	}else{
		SetID(GenerateNewId());
		SetTitle(Format("Item %s", GetID()));
	}
};

void CItem::SwapItems(int iIndex1, int iIndex2)
{
	// Меняем в списке в памяти...
	CItem* iTmp=objSettings.Items[iIndex1];
	objSettings.Items[iIndex1]=objSettings.Items[iIndex2];
	objSettings.Items[iIndex2]=iTmp;
	// Обновляем индексы....
	if(objSettings.Items[iIndex1]!=NULL){
		objSettings.Items[iIndex1]->index=iIndex1;
		objSettings.Items[iIndex1]->ItemChanged();
	}
	if(objSettings.Items[iIndex2]!=NULL){
		objSettings.Items[iIndex2]->index=iIndex2;
		objSettings.Items[iIndex2]->ItemChanged();
	}
	return;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CItemFolder::CItemFolder(const char* szTitle):CItem(0)
{
	if(szTitle){
		SetTitle(szTitle);
	}else{
		SetTitle(objSettings.bGenerateRandomNames?GetRandomName():_l("New folder"));
	}
}

void CItemFolder::ActivateChanges(const char* szTitle, DWORD* pdwPrir, DWORD* pdwState)
{
	if(szTitle){
		SetTitle(szTitle);
	}
	if(pdwPrir){
		SetPriority(*pdwPrir);
	}
	if(pMainDialogWindow!=NULL && hIRow!=NULL){
		pMainDialogWindow->m_TaskList.SetIItemControlType(hIRow,1,FLC_READONLY);
	}
	ItemContentChanged();
}

void CItemFolder::SetID(const char* szID)
{
	if(strcmp(szID,TODO_PATH)==0){
		CItem::SetIconNum(23);
	}
	if(strcmp(szID,NOTE_PATH)==0){
		CItem::SetIconNum(24);
	}
	if(strcmp(szID,REMD_PATH)==0){
		CItem::SetIconNum(25);
	}
	if(strcmp(szID,LINK_PATH)==0){
		CItem::SetIconNum(26);
	}
	if(strcmp(szID,MESS_PATH)==0){
		CItem::SetIconNum(27);
	}
	CItem::SetID(szID);
}

CString CItemFolder::GetTextContent()
{
	return "";
}

CString CItemFolder::GetItemDsc()
{
	CString sOut=GetTitle();
	if(pMainDialogWindow!=NULL && hIRow!=NULL){
		int iCount=pMainDialogWindow->m_TaskList.GetChildIRowCount(hIRow);
		if(iCount>0){
			sOut+=Format(" (%s: %i)",_l("Items"),iCount);
		}
	}
	return sOut;
};

CItemFileLink::CItemFileLink(const char* szFilePath):CItem(4)
{
	sFilePath=szFilePath;
	SetTitle(szFilePath);
}

void CItemFileLink::ActivateChanges(const char* szTitle, DWORD* pdwPrir, DWORD* pdwState)
{
	if(szTitle){
		SetTitle(szTitle);
	}
	if(pdwPrir){
		SetPriority(*pdwPrir);
	}
	ItemContentChanged();
}

CColorScheme::CColorScheme()
{
	// Неиспользуется 
	Add("System",GetSysColor(COLOR_WINDOW),GetSysColor(COLOR_WINDOWTEXT),100);
}

CColorScheme::~CColorScheme()
{
	for(int i=0;i<GetSize();i++){
		if(Font[i]){
			delete Font[i];
			Font[i]=NULL;
		}
	}
	Font.RemoveAll();
};

int CColorScheme::GetSize()
{
	return sNoteShemeName.GetSize();
}

void CColorScheme::Add(const char* szName, DWORD BGColor, DWORD TXColor, DWORD Alpha, DWORD dwFontSize, DWORD dwAutoTrans)
{
	CFont* font=new CFont();
	font->CreateFont(dwFontSize?dwFontSize:15,0,0,0,FW_NORMAL,0,0,0,GetUserDefaultLangID()==0x0419?RUSSIAN_CHARSET:DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE,"Arial");
	Add(szName, BGColor, TXColor, font, Alpha, dwAutoTrans);
};

void CColorScheme::Add(const char* szName, DWORD BGColor, DWORD TXColor, CFont* font, DWORD Alpha, DWORD dwAutoTrans)
{
	NoteColorsAlpha.Add(Alpha);
	sNoteShemeName.Add(szName);
	NoteColorsBG.Add(BGColor);
	NoteColorsTX.Add(TXColor);
	Font.Add(font);
};

void CColorScheme::Delete(const char* szName)
{
	for(int i=1;i<sNoteShemeName.GetSize();i++){
		if(sNoteShemeName[i]==szName){
			NoteColorsAlpha.RemoveAt(i);
			sNoteShemeName.RemoveAt(i);
			NoteColorsBG.RemoveAt(i);
			NoteColorsTX.RemoveAt(i);
			if(Font[i]){
				delete Font[i];
				Font[i]=NULL;
			}
			Font.RemoveAt(i);
		}
	}
};

DWORD& CColorScheme::getTxtColor(long& iIndex)
{
	if(iIndex>=0 && iIndex<NoteColorsTX.GetSize()){
		return NoteColorsTX[iIndex];
	}
	iIndex=0;
	return NoteColorsTX[0];
}

DWORD& CColorScheme::getBgColor(long& iIndex)
{
	if(iIndex>=0 && iIndex<NoteColorsTX.GetSize()){
		return NoteColorsBG[iIndex];
	}
	iIndex=0;
	return NoteColorsBG[0];
}

CString& CColorScheme::getName(long& iIndex)
{
	if(iIndex>=0 && iIndex<NoteColorsTX.GetSize()){
		return sNoteShemeName[iIndex];
	}
	iIndex=0;
	return sNoteShemeName[0];
}

CFont* CColorScheme::getFont(long& iIndex)
{
	if(iIndex>=0 && iIndex<Font.GetSize()){
		return Font[iIndex];
	}
	iIndex=0;
	return Font[0];
}

DWORD dwNullAlphaRes=1;
DWORD& CColorScheme::getAlpha(long& iIndex)
{
	if(iIndex<0 || iIndex>=Font.GetSize()){
		iIndex=0;
	}
	if(NoteColorsAlpha[iIndex]>0){
		return NoteColorsAlpha[iIndex];
	}
	return dwNullAlphaRes;
}

void CColorScheme::setFont(int iIndex, CFont* font)
{
	if(iIndex>=0 && iIndex<Font.GetSize()){
		if(Font[iIndex]){
			delete Font[iIndex];
			Font[iIndex]=NULL;
		}
		Font[iIndex]=font;
	}
}

/*
Пример:
[=WPOSS=] - точное совпадение
[?WPOSS?] - вхождение 'WPOSS' в заголовок
[!WPOSS!] - невхождение 'WPOSS' в заголовок
*/
BOOL CActiItem::TestTitleMatchCondition_Raw(CString& sTitle)
{
	CString sPinTitle="\n";
	sPinTitle+=sPinnedToApp;
	sPinTitle+="\n";
	int iFrom=0;
	while(iFrom!=-1 && iFrom!=sPinTitle.GetLength()){
		CString sTitlePart=CDataXMLSaver::GetInstringPart("\n","\n",sPinTitle,iFrom);
		if(iFrom!=sPinTitle.GetLength()){
			iFrom--;
		}
		sTitlePart.TrimLeft();
		sTitlePart.TrimRight();
		if(sTitlePart!=""){
			BOOL bRes=CheckMyMaskSubstr(sTitle,sTitlePart,FALSE);
			if(bRes){
				return bRes;
			}
		}
	}
	return FALSE;
}

void CActiItem::SetApplicationsList(CString sApps)
{
	sPinnedToApp=sApps;
	sPinnedToApp.Replace("\r\n","\n");
	sPinnedToApp.TrimLeft();
	sPinnedToApp.TrimRight();
}

CString CActiItem::GetApplicationsList()
{
	return sPinnedToApp;
};