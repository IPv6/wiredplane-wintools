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
	// Для тем - каталог синхронизации
	// Для обоев - соотв. пресет
	CString sWallpaperParameters;
public:
	// Позиция в списке на экране
	HIROW hRow;
	// Заголовок
	CString sTitle;
	// Индекс темы для обоев и вес для тем
	DWORD dwTheme;
	// Тип: 0-folder,1-file
	DWORD dwType;
	// Для тем - входит/не входит в показ, для картинок - выключена (1)/ включена (0)
	DWORD dwStatus;
	// Для тем - синхронизация с последней папкой
	DWORD dwFolderSync;
	// Тип самплинга обоев
	DWORD dwSampling;
	// Количетсво обоев в теме
	DWORD dwImagesCount;
	// Количетсво активных обоев в теме
	DWORD dwActiveImagesCount;
	// Конструкторы/деструкторы
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
	long lLastValidPosition;// Номер последней картинки+1 из активной темы
	long GetLastActivePictureNum();
	void Save(const char* szFile=0);
	void Load(const char* szFile=0);
	void AddThemeToPictures(int iThemeStatus, const char* szStartPict);
	void AddThemeToPicturesByStatus(int iStatus, int iTheme, int& iThemeImagesCount, const char* szStartPict);
	void ReinitWPaperShuffle(CString szStartPic="",BOOL bInLowP=FALSE);
	long GetNextWPNumber(int iSelectionType);
	//------------------------------------
	// Индекс из aWPapersAndThemes!!!
	void GetWPTitleAndSampling(int iIndex, CString& sPath, CString& sThemeName, DWORD& dwSampling);
	BOOL SetWallPaperByIndex(int dwIndex);
	//------------------------------------
	CDWordArray aUsedThemes;// Номера из массива aWPapersAndThemes - темы, по которым идет просмотр
	CDWordArray aUsedThemes_Passive;// Номера тем без весов - из aUsedThemes
	CDWordArray aUsedThemesFDOff;// Смещение относительно начала для пределов в которых брать обою. взятая обоя уходит в конец
	CDWordArray aUsedThemesFD;// Первый номер обоя из aImagesInRotation. Соответсвие по aUsedThemes
	CDWordArray aUsedThemesTD;// Последний номер обоя из aImagesInRotation. Соответсвие по aUsedThemes
	CDWordArray aUsedThemesTD_Total;// Последний номер обоя из aImagesInRotation с учетом отключенных обоев. Соответсвие по aUsedThemes
	CDWordArray aImagesInRotation;// обои (название файла и пр.) - здесь все обои "лежат" один за другим, плоско, без папок. Элемент - номер из массива aWPapersAndThemes
	BOOL bUsedListsInvalidated;
	//------------------------------------
	CArray<WPaper*,WPaper*> aWPapersAndThemes;// Здесь подряд лежат картинки и темы со всеми внутренними ссылками друг на друга, базовый массив
	//------------------------------------
	WChanger();
	virtual ~WChanger();
};
WPaper* GetWPTheme(int& iIndex);
#endif // !defined(AFX_WPAPER_H__FEF8966B_304A_42A6_AD64_9E9DB2F54B4D__INCLUDED_)
