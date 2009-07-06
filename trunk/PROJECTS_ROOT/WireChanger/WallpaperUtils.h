#ifndef WPUTILS
#define WPUTILS

#define HIMETRIC_PER_INCH   2540
typedef void (FAR WINAPI *OnImagePrepared)(const char* szFile);


class CSetWallPaper
{
public:
	// То, что нужно задавать
	BOOL bRegardMM;
	DWORD dwResamplingType;
	CString sWallpaperThemeName;
	CString sWallpaperFile;
	CString sSingleWPTMode;
	CString sCurrentWPT;
	CSize sizeOfRealImage;
	BOOL bJustNotes;
	BOOL bSkipWrapperStep;
	BOOL bSkipWrapperGeneration;
	BOOL bSuppressIntegration;// Если включена интеграция нужно ли ее учитывать (не имеет смысла при превью)
	BOOL bTimedOut;
	BOOL bApplyAntiTaskbar;
	BOOL bADContentInit;
	CString sADContent;
	CString sCurFilePath;
	//
	CSetWallPaper();
	~CSetWallPaper();
	//
	BOOL bWPType;//0-image, 1-html
	//
	// dwFilterType==0	- не переделывать ничего
	// dwFilterType==1	- exact fit
	// dwFilterType==2	- fit proportional
	// dwFilterType==3	- fit proportional, но только картинки, составляющие как минимум 70% экрана
	//
	HBITMAP hbmRealImage;
	HBITMAP hbmOutImage;
	// Это размер картинки который хочется получить до выполнения генерации и который реально получили после выполнения
	CSize sizeOfOutImage;
	// Это финальный размер которым мы точно хотим получить не смотря ни на что. Если пуст - размер экрана
	CRect requiredSizeOfOutImage;
	// Это информационный прямоугольник,  в который реально вписывается картинка фона
	CRect rcOutScreenOnRect;
	// Прямоугольник в который желательно бы вписать....
	CRect rcRecommendedWPPosition;
	// Скринсейвер ли?
	long lScrMode;
	// С каким пресетом
	CString sInPreset;
	// С какими шаблонами
	CStringArray TemplatesInWork;
	// инициализация для пресета
	void InitForPreset(CString sPres);
	tiscript::engine* tiscript;
};

class CNote
{
private:
	CString sNoteOriginalText;
public:
	CString sNoteOriginalTextFilePath;
	// Сохраняемые данные
	long lUniqueCounter;
	int iIcon;
	int left;
	int top;
	int right;
	int bottom;
	DWORD dwTColor;
	DWORD dwBColor;
	CString sTitle;
	CString sCustomIcon;
	CString sCustomFont;
	CString sCustomBg;
	CSize lTmpPreviousSize;
	long lTmpPreviousTileBG;
	long lShowMode;
	long lTileBG;
	long lOpacity;
	// Так, временные данные
	CString sNoteFile;
	CFont* font;
	BOOL bUnderEdit;
	// Функции
	CNote();
	~CNote();
	BOOL SaveToFile(const char* szFile=NULL);
	BOOL LoadFromFile(const char* szFile=NULL);
	CString GetReminderKey();
	CString GetRealTextFromNote();
	CString SetRealTextFromNote(CString sDef);
};

class CNoteSkin
{
public:
	CNoteSkin()
	{
		sTitle="Note skin";
		dwTXColor=RGB(255,255,255);
		dwBGColor=RGB(255,255,255);
		dXDelta=dYDelta=0;
		bXPos=bYPos=0;
		iMaxIcons=0;
		bIconsLoaded=0;
		bIsHTML=0;
	};
	BOOL bIconsLoaded;
	BOOL bIsHTML;
	int iMaxIcons;
	CString sTitle;
	CString sAuthor;
	CString sFolder;
	CString sHTML_Full;
	CString sHTML_Titleonly;
	CString sHTML_Iconic;
	CString sHTML_Text;
	CString sHTML_Css;
	DWORD dwTXColor;
	DWORD dwBGColor;
	BOOL bXPos;//1-слева, 0-справа
	double dXDelta;
	BOOL bYPos;//1-сверху, 0-снизу
	double dYDelta;
	CStringArray aIconsTitles;
	CNoteSkin& operator=(CNoteSkin& CopyObj)
	{
		sHTML_Css=CopyObj.sHTML_Css;
		sHTML_Text=CopyObj.sHTML_Text;
		sHTML_Full=CopyObj.sHTML_Full;
		sHTML_Titleonly=CopyObj.sHTML_Titleonly;
		sHTML_Iconic=CopyObj.sHTML_Iconic;
		bIsHTML=CopyObj.bIsHTML;
		bIconsLoaded=CopyObj.bIconsLoaded;
		iMaxIcons=CopyObj.iMaxIcons;
		sTitle=CopyObj.sTitle;
		dwTXColor=CopyObj.dwTXColor;
		dwBGColor=CopyObj.dwBGColor;
		bXPos=CopyObj.bXPos;
		dXDelta=CopyObj.dXDelta;
		bYPos=CopyObj.bYPos;
		dYDelta=CopyObj.dYDelta;
		sFolder=CopyObj.sFolder;
		sAuthor=CopyObj.sAuthor;
		aIconsTitles.RemoveAll();
		for(int i=0;i<CopyObj.aIconsTitles.GetSize();i++){
			aIconsTitles.SetAtGrow(i,CopyObj.aIconsTitles[i]);
		}
		return *this;
	}
};

CString GetTextIncludeFilePath_forTempl(CString sFile);
CString GetWPTemplateFilePath(const char* szWPT=NULL);
BOOL ReadFileWithConfig(CString szFile, CString& sContent, const char* szPreset, BOOL bPerformSimpleCalcs=0);
int PrepareHtmlWallpaper(CSetWallPaper* wpCommonData);
int PrepareImageWallpaper(CSetWallPaper* wpCommonData);
int PrepareWallpaperBg(CSetWallPaper* wpCommonData);
int FinishWallPaper(CSetWallPaper* wpCommonData);
int FinishWallPaperX(CSetWallPaper* wpCommonData, int iOnMonitor=-1);
int PrepareWallPaper(CSetWallPaper* wpCommonData);
int LoadAnyPicture(const char* szFile, HBITMAP* hbmScreen, CRect* rcBounds, DWORD dwResamplingType, CSize* picSize);
BOOL SetWallPaper(const char* szFile, long dwFilterType, const char* szThemeName=NULL, const char* szPreset=NULL);
int getRndBmpFormat();
BOOL IsWPTActive(const char* szWPT);
BOOL SwitchSingleTemplate(const char* szTemplate, BOOL b=1);
void SubstVariables(CString& sBodyText,CSetWallPaper* wpCommonData);
void SubstVariables_Vars(CString& sBodyText,CSetWallPaper* wpCommonData);
void SubstVariables_Simple(CString& sBodyText, BOOL bCalcOnly,CSetWallPaper* wpCommonData);
void CallFileForOpenOrEdit(const char* szFile,BOOL bJustTooltip=0);
void ShowWarningAboutMissedFile(const char* szFile);
CString GetHTMLConfigFile(const char* szFile, const char* szPreset);
BOOL PostFilterWallpaper(CSetWallPaper* wpCommonData, int iTile=1);
BOOL ReKolbassWallpaper(const char* sNewWPFile, int iTile=1);
BOOL RefreshWallpaperFileName();
CString GetCurrentWallpaper(BOOL* bActiveDesktopOn=NULL);
BOOL IsActiveDesctopEnable();
CString GetDesktopPrefix();
BOOL UpdateEnviverState();
void HideDeskIcons(BOOL bNewValue);
BOOL CallRenderTemplates(const char* szLayer,HBITMAP* hbmOutImage,CSetWallPaper* wpCommonData,CString* sADTextOut=0);
long SimpleCalcX(const char* szExpr);
void GenerateForm(CString sSrc,CString sAddParam, BOOL bStrick);
int GetNumberOfMons();
#endif