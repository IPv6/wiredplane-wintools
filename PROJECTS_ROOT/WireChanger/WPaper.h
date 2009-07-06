// WPaper.h: interface for the WPaper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WPAPER_H__FEF8966B_304A_42A6_AD64_9E9DB2F54B4D__INCLUDED_)
#define AFX_WPAPER_H__FEF8966B_304A_42A6_AD64_9E9DB2F54B4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\SmartWires\SystemUtils\SupportClasses.h"
class WPaper
{
private:
	// ��� ��� - ������� �������������
	// ��� ����� - �����. ������
	CString sWallpaperParameters;
public:
	// ������� � ������ �� ������
	HIROW hRow;
	// ���������
	CString sTitle;
	// ������ ���� ��� ����� � ��� ��� ���
	DWORD dwTheme;
	// ���: 0-folder,1-file
	DWORD dwType;
	// ��� ��� - ������/�� ������ � �����, ��� �������� - ��������� (1)/ �������� (0)
	DWORD dwStatus;
	// ��� ��� - ������������� � ��������� ������
	DWORD dwFolderSync;
	// ��� ��������� �����
	DWORD dwSampling;
	// ���������� ����� � ����
	DWORD dwImagesCount;
	// ���������� �������� ����� � ����
	DWORD dwActiveImagesCount;
	// ������������/�����������
	WPaper();
	DWORD SetStatus(DWORD lNewVal);
	CString GetWPar(const char* szName="");
	void SetWPar(const char* szName, const char* szValue);
	virtual ~WPaper();
};

class WChanger
{
public:
	BOOL ClearWPList();
	CCriticalSection cShufflingLock;
	BOOL IsSuchWPExist(const char* sFile, DWORD dwTheme=DWORD(-1));
	BOOL AddFile(CString& sWPTitle, DWORD iTheme);
	long lLastValidPosition;// ����� ��������� ��������+1 �� �������� ����
	long GetLastActivePictureNum();
	void Save(const char* szFile=0);
	void Load(const char* szFile=0);
	void AddThemeToPictures(int iThemeStatus, const char* szStartPict);
	void AddThemeToPicturesByStatus(int iStatus, int iTheme, int& iThemeImagesCount, const char* szStartPict);
	void ReinitWPaperShuffle(CString szStartPic="",BOOL bInLowP=FALSE);
	long GetNextWPNumber(int iSelectionType);
	//------------------------------------
	// ������ �� aWPapersAndThemes!!!
	void GetWPTitleAndSampling(int iIndex, CString& sPath, CString& sThemeName, DWORD& dwSampling);
	BOOL SetWallPaperByIndex(int dwIndex);
	//------------------------------------
	CDWordArray aUsedThemes;// ������ �� ������� aWPapersAndThemes - ����, �� ������� ���� ��������
	CDWordArray aUsedThemes_Passive;// ������ ��� ��� ����� - �� aUsedThemes
	CDWordArray aUsedThemesFDOff;// �������� ������������ ������ ��� �������� � ������� ����� ����. ������ ���� ������ � �����
	CDWordArray aUsedThemesFD;// ������ ����� ���� �� aImagesInRotation. ����������� �� aUsedThemes
	CDWordArray aUsedThemesTD;// ��������� ����� ���� �� aImagesInRotation. ����������� �� aUsedThemes
	CDWordArray aUsedThemesTD_Total;// ��������� ����� ���� �� aImagesInRotation � ������ ����������� �����. ����������� �� aUsedThemes
	CDWordArray aImagesInRotation;// ���� (�������� ����� � ��.) - ����� ��� ���� "�����" ���� �� ������, ������, ��� �����. ������� - ����� �� ������� aWPapersAndThemes
	BOOL bUsedListsInvalidated;
	//------------------------------------
	CArray<WPaper*,WPaper*> aWPapersAndThemes;// ����� ������ ����� �������� � ���� �� ����� ����������� �������� ���� �� �����, ������� ������
	//------------------------------------
	WChanger();
	virtual ~WChanger();
};
WPaper* GetWPTheme(int& iIndex);
#endif // !defined(AFX_WPAPER_H__FEF8966B_304A_42A6_AD64_9E9DB2F54B4D__INCLUDED_)
