// ItemModif.h: interface for the ItemModif class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMMODIF_H__34DA10D0_313E_4397_9009_8BECA393B941__INCLUDED_)
#define AFX_ITEMMODIF_H__34DA10D0_313E_4397_9009_8BECA393B941__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "..\SmartWires\GUIClasses\IListCtrl.h"

class CItem
{
protected:
	BOOL bChanged;
	BOOL bState;
	int iPrior;
	int iIconNum;
	long iColorCode;
	CString m_sID;
	int m_iRevision;
	CString m_sTitle;
	CString m_sPath;
	DWORD dwType;
	COleDateTime CreationTime;
	BOOL ReadOnly;
	long lShared;
	CString sAuthor;
public:
	static void SetModify(BOOL bForcedly=FALSE);
	static void SwapItems(int iIndex1, int iIndex2);
	BOOL IsChanged(){return bChanged;};
	void ItemChanged(int AttachToListIndex=1){bChanged=AttachToListIndex;};
	void ClearChanged(){bChanged=FALSE;};
	void ItemContentChanged(){CreationTime=COleDateTime::GetCurrentTime();};
	virtual void ActivateChanges(const char* szTitle=NULL, DWORD* pdwPrir=NULL, DWORD* pdwState=NULL);
	virtual CString GetItemDsc(){return GetTitle();};
	virtual CString GetTextContent(){return GetTitle();};
	virtual CString GetFindText(){return GetTitle();};
	virtual BOOL SaveToFile(const char* szFile=0){return FALSE;};
	virtual CString GetItemFile(){return "";};
	virtual BOOL LoadFromFile(const char* szFileName){return FALSE;};
	virtual BOOL SetReadOnly(BOOL b){ReadOnly=b;return ReadOnly;};
	BOOL GetReadOnly(){return ReadOnly;};
	CString ConvertToString();
	void ConvertFromString(const char* szData,const char* szFileName);
	void DeleteFromList();
	void DeleteItem();
	HIROW hIRow;// Код линии в дереве основного окна
	int iListIndex;// Индекс в основном окне. нужен искл. для сохранения порядка элементов
	int index;
	CItem(DWORD dwType);
	virtual ~CItem();
	void Attach(int AttachToListIndex=1);
	void Detach();
	virtual void SetID(const char* szID);
	void SetType(DWORD dwT);
	long& GetColorCode();
	virtual void SetColorCode(long iCode);
	void SetTitle(const char* szTitle);
	void SetPath(const char* szPath);
	void SetCrTime(COleDateTime& Time);
	void SetPriority(int pr);
	void SetIconNum(int iIcon);
	void SetState(BOOL b);
	int GetIconNum(){return iIconNum;};
	int GetPriority(){return iPrior;};
	const char* GetID(){return m_sID;};
	CString GetTitle(int iCount=-1);
	DWORD GetType(){return dwType;};
	const char* GetPath(){return m_sPath;};
	CItem* GetItemParent(CString* sParentID=NULL, CString* sParentPath=NULL);
	COleDateTime GetCrTime(){return CreationTime;};
	BOOL GetState(){return bState;};
	static void DeleteItem(const char* szID);
	static void DeleteFolders();
	static CItem *GetItemByTitle(const char* szTitle);
	static CItem *GetItemByID(const char* szID);
	static CItem *GetItemByPath(const char* szPath);
	static CItem *GetItemByIndex(int index);
	CString GenerateNewId();
	BOOL isItemCorrupted();
	virtual BOOL ShareItem(BOOL bNewSharingData, int iSectionNum)=0;
	BOOL SetAuthor(const char* szA){sAuthor=szA;return 1;};
	CString GetAuthor(){return sAuthor;};
	int SetRevision(int i){m_iRevision=i;return 1;};
	int GetRevision(){return m_iRevision;};
	BOOL IsShared(){return lShared;};
	void SetShared(BOOL bNew,const char* szFName);
};

class CActiItem: public CItem
{
protected:
	CString sPinnedToApp;
public:
	CString GetApplicationsList();
	void SetApplicationsList(CString sApps);
	CActiItem(DWORD iType):CItem(iType){};
	BOOL TestTitleMatchCondition_Raw(CString& sTitle);
};

class CItemFolder: public CItem
{
public:
	CItemFolder(const char* szTitle=NULL);
	virtual ~CItemFolder(){};
	virtual void ActivateChanges(const char* szTitle=NULL, DWORD* pdwPrir=NULL, DWORD* pdwState=NULL);
	virtual CString GetItemDsc();
	CString GetTextContent();
	CString GetFindText(){return GetTitle();};
	virtual void SetID(const char* szID);
	BOOL ShareItem(BOOL bNewSharingData, int iSectionNum){return FALSE;};
};

class CItemFileLink: public CItem
{
public:
	CString sFilePath;
	CItemFileLink(const char* szFilePath=NULL);
	virtual ~CItemFileLink(){};
	virtual void ActivateChanges(const char* szTitle=NULL, DWORD* pdwPrir=NULL, DWORD* pdwState=NULL);
	virtual CString GetItemDsc(){return GetTitle();};
	CString GetTextContent(){return sFilePath;};
	CString GetFindText(){return GetTitle()+"|"+sFilePath;};
	BOOL ShareItem(BOOL bNewSharingData, int iSectionNum){return FALSE;};
};

class CColorScheme: public CObject
{
public:
	CColorScheme();
	~CColorScheme();
	int GetSize();
	void Add(const char* szName, DWORD BGColor, DWORD TXColor, DWORD Alpha, DWORD dwFontSize=0, DWORD dwAutoTrans=0);
	void Add(const char* szName, DWORD BGColor, DWORD TXColor, CFont* font, DWORD Alpha, DWORD dwAutoTrans);
	void Delete(const char* szName);
	DWORD& getTxtColor(long& iIndex);
	DWORD& getBgColor(long& iIndex);
	CString& getName(long& iIndex);
	CFont* getFont(long& iIndex);
	DWORD& getAlpha(long& iIndex);
	void setFont(int iIndex, CFont* font);
protected:
	CDWordArray NoteColorsAlpha;
	CDWordArray NoteColorsBG;
	CDWordArray NoteColorsTX;
	CStringArray sNoteShemeName;
	CArray<CFont*,CFont*> Font;
};

class CMessageFilter
{
public:
	CMessageFilter(CString _sSearchString,DWORD _dwSearchWhere,DWORD _dwDoWhat)
	{
		sSearchString=_sSearchString;
		dwSearchWhere=_dwSearchWhere;
		dwDoWhat=_dwDoWhat;
	};
	CString sSearchString;
	long dwSearchWhere;
	long dwDoWhat;
	long dwActive;
};

#endif // !defined(AFX_ITEMMODIF_H__34DA10D0_313E_4397_9009_8BECA393B941__INCLUDED_)
