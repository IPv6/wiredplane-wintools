#include "..\SmartWires\SystemUtils\math\parser.h"
#include "stdafx.h"
#include "_common.h"
#include "_externs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define STRX_PREF	CString("$=<STRXTR>")
CString ProcessWithTiScript(const char* sBodyText,BOOL asScript, CSetWallPaper* wpCommonData);
CSetWallPaper::CSetWallPaper()
{
	bTimedOut=0;
	bRegardMM=1;
	bJustNotes=FALSE;
	bSuppressIntegration=FALSE;
	bSkipWrapperStep=FALSE;
	hbmRealImage=hbmOutImage=NULL;
	bWPType=sizeOfRealImage.cx=sizeOfRealImage.cy=0;
	sizeOfOutImage=GetMainMonitorRECTX().Size();
	bSkipWrapperGeneration=FALSE;
	bApplyAntiTaskbar=0;
	sADContent="";
	bADContentInit=0;
	lScrMode=0;
	requiredSizeOfOutImage.SetRectEmpty();
	sInPreset=objSettings.sActivePreset;
	rcRecommendedWPPosition.SetRect(0,0,0,0);
	TemplatesInWork.Copy(objSettings.TemplatesInWork);
	tiscript=0;
};

CSetWallPaper::~CSetWallPaper()
{
	ClearBitmap(hbmOutImage);
	ClearBitmap(hbmRealImage);
	if(tiscript){
		delete tiscript;
		tiscript=0;
	}
};


void CSetWallPaper::InitForPreset(CString sPres)
{
	sInPreset=sPres;
	TemplatesInWork.RemoveAll();
	objSettings.LoadActiveWPT(sInPreset,TemplatesInWork,0);
	// Перезадаем вероятности
	for(int i=0;i<TemplatesInWork.GetSize();i++){
		CWPT* wpt=GetWPT(TemplatesInWork[i]);
		if(!wpt){
			continue;
		}
		wpt->dwWeTmp=rnd(0,99);
	}
}

void ReparseStrXtr(CString& sText, const char* szChangeWith)
{
	if(sText.GetLength()==0){
		return;
	}
	int iFrom=sText.Find('>');
	if(iFrom==-1){
		return;
	}
	sText=sText.Mid(iFrom+1);
	while(sText.Replace(STRX_PREF+STRX_PREF,"")){
		//
	}
	if(sText.Find(STRX_PREF)==0){
		sText=sText.Mid(STRX_PREF.GetLength());
	}
	if(ReverseFind(sText,STRX_PREF)==sText.GetLength()-STRX_PREF.GetLength()){
		sText=sText.Left(sText.GetLength()-STRX_PREF.GetLength());
	}
	sText.Replace(STRX_PREF,szChangeWith);
}

void RemoveTemplateAliases(CString& sBodyText, BOOL bPreperingPhase=0)
{
	// Удаляем все лишнее...
	BOOL bIncludeSkipped=FALSE;
	extTag(CUTLINE_TAG,sBodyText,&sBodyText);
	int iFrom=0;
	while(iFrom>=0){
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<STRXTR-BEGIN","$=<STRXTR-END>",sBodyText,iFrom);
		CString sTextToReplace=Format("$=<STRXTR-BEGIN%s$=<STRXTR-END>",sTextLineParams);
		CString sChangeWith=CDataXMLSaver::GetInstringPart(":\"","\">",sTextLineParams);
		ReparseStrXtr(sTextLineParams,sChangeWith);
		sBodyText.Replace(sTextToReplace,sTextLineParams);
		if(iFrom>=0){
			iFrom+=strlen(sTextLineParams)-strlen(sTextToReplace);
		}
	}
	iFrom=0;
	while(iFrom>=0){
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<",">",sBodyText,iFrom);
		if(!bPreperingPhase || (bPreperingPhase && (sTextLineParams.Find("_")!=0 && sTextLineParams.Find("!")!=0 && sTextLineParams.Find("/_")!=0 && sTextLineParams.Find("/!")!=0))){
			if(sTextLineParams=="DEBUG:TRUE"){
				bIncludeSkipped=TRUE;
			}
			CString sTextToReplace=Format("$=<%s>",sTextLineParams);
			CString sTextLine="";
			if(bIncludeSkipped){
				sTextLine=Format("<!-- Skipped '%s' -->",sTextLineParams);
			}
			sBodyText.Replace(sTextToReplace,sTextLine);
			if(iFrom>=0){
				iFrom+=strlen(sTextLine)-strlen(sTextToReplace);
			}
		}
	}
	if(!bPreperingPhase){
		CString sTagValue,sAttr;
		while((iFrom=extTag("wp:ShowAlert",sBodyText,&sTagValue,&sAttr))!=-1){
			if(sTagValue==""){
				sTagValue=sAttr;
			}
			sTagValue.TrimLeft("() \r\n\t");
			sTagValue.TrimRight("() \r\n\t");
			if(sTagValue!=""){
				AlertBox(_l("Warning"),sTagValue,0);
			}
			sTagValue="";
		}
		while((iFrom=extTag("wp:ShowOptions",sBodyText,&sTagValue,&sAttr))!=-1){
			if(sTagValue==""){
				sTagValue=sAttr;
			}
			CString sAlert=_arr(sTagValue,0);
			pMainDialogWindow->PostMessage(OPEN_TEMPL_PARS,WPARAM(strdup(sAlert)),0);
			sTagValue="";
		}
	};
}

void SubstVariables_Expr(CString& sBodyText)
{
	int iFrom=0;
	CString sTagValue,sAttr;
	while((iFrom=extTagSimple("wp:expr",sBodyText,&sTagValue,&sAttr))!=-1){
		if(sTagValue==""){
			sTagValue=sAttr;
		}
		// Для нашего парсера скобки непочем?
		sTagValue=_arr(sTagValue,0);
		if(sTagValue.Find('{')!=-1){
			CString sTest=sTagValue;
			sTest.Replace("{wp:expr","");
			if(sTest.Find('{')!=-1){
				// Пока ничего не трогаем...
				sBodyText.Insert(iFrom,CString("{wp:expr(")+sTagValue+")}");
				return;
			}
		}
		if(sTagValue.Find("wp:expr")!=-1){
			SubstVariables_Expr(sTagValue);
		}
		sBodyText.Insert(iFrom,Format("%i",SimpleCalcX(sTagValue)));
	}
}

CString GetApplicationLangAbbr()
{
	const char* szLangs[6]={"en","ru","es","it","ar","ukr"};
	if(GetApplicationLang()>5){
		return szLangs[0];
	}
	return szLangs[GetApplicationLang()];
}

// Подставляем параметры и стандартные переменные
void SubstVariables_Simple(CString& sBodyText, BOOL bOnlyCalcs, CSetWallPaper* wpCommonData)
{
	int iFrom=0;
	if(!bOnlyCalcs){
		CRect rtDeskRect=theApp.GetDesktopRECT();
		if(wpCommonData && !wpCommonData->requiredSizeOfOutImage.IsRectEmpty()){
			rtDeskRect.CopyRect(wpCommonData->requiredSizeOfOutImage);
		}
		CString sBasePath=GetWPTemplateFilePath("");
		sBasePath.TrimRight("\\");
		sBasePath.Replace("\\","/");
		COleDateTime dt=GETCURTIME;//COleDateTime::GetCurrentTime();
		int iDay=dt.GetDay();		
		CString sAppDat=GetPathPart(objSettings.sHTMLWrapperFile,1,1,0,0);

		sBodyText.Replace("{basePath}",sBasePath);
		sBodyText.Replace("{baseAppPath}",GetApplicationDir());
		sBodyText.Replace("{appDatPath}",sAppDat);
		sBodyText.Replace("{profile}",GetUserFolder());
		sBodyText.Replace("{day}",Format("%i",dt.GetDay()));
		sBodyText.Replace("{hour}",Format("%i",dt.GetHour()));
		sBodyText.Replace("{hour12}",Format("%i",1+dt.GetHour()%12));
		//sBodyText.Replace("{hour12}",Format("%i",1+dt.GetHour()%12));Добавить AM/PM
		sBodyText.Replace("{min}",Format("%i",dt.GetMinute()));
		sBodyText.Replace("{sec}",Format("%i",dt.GetSecond()));
		sBodyText.Replace("{hour02}",Format("%02i",dt.GetHour()));
		sBodyText.Replace("{min02}",Format("%02i",dt.GetMinute()));
		sBodyText.Replace("{sec02}",Format("%02i",dt.GetSecond()));
		sBodyText.Replace("{year}",Format("%i",dt.GetYear()));
		sBodyText.Replace("{yearNext}",Format("%i",dt.GetYear()+1));
		sBodyText.Replace("{lang}",Format("%i",GetApplicationLang()));
		sBodyText.Replace("{lang_abbr}",GetApplicationLangAbbr());
		sBodyText.Replace("{win9x}",Format("%i",isWin9x()));
		sBodyText.Replace("{bg}",Format("#%06X",objSettings.lWPBGColor));
		sBodyText.Replace("{bgi}",Format("#%06X",objSettings.lWPIBGColor));
		sBodyText.Replace("{fg}",Format("#%06X",objSettings.lWPTXColor));
		DWORD dwBgCol=GetSysColor(COLOR_BTNFACE);
		sBodyText.Replace("{window_bitmap_color}",Format("#%02X%02X%02X",GetRValue(dwBgCol),GetGValue(dwBgCol),GetBValue(dwBgCol)));
		sBodyText.Replace("{screenX}",Format("%i",rtDeskRect.Width()));
		sBodyText.Replace("{screenY}",Format("%i",rtDeskRect.Height()));
		CRect rtbNoTaskbar;
		::SystemParametersInfo(SPI_GETWORKAREA,0,&rtbNoTaskbar,0);
		sBodyText.Replace("{screen-ntb-left}",Format("%i",rtbNoTaskbar.left));
		sBodyText.Replace("{screen-ntb-top}",Format("%i",rtbNoTaskbar.top));
		sBodyText.Replace("{screen-ntb-right}",Format("%i",rtbNoTaskbar.right));
		sBodyText.Replace("{screen-ntb-bottom}",Format("%i",rtbNoTaskbar.bottom));
		sBodyText.Replace("{wc_version}",Format("%i",BUILD_NUM));
		sBodyText.Replace("{prog_id}",AppName()+""PROG_VERSION);
		sBodyText.Replace("{nocache}",GenerateNewId("",""));

		if(wpCommonData){
			CString sFile=wpCommonData->sCurFilePath;
			if(sFile.GetLength()==0){
				sFile=sBasePath;
			}
			sFile=GetPathPart(sFile,1,1,0,0);
			sBodyText.Replace("$=<THISFOLDER>",sFile);
			sBodyText.Replace("$=<THISFOLDERJS>",EscapeSlashes(sFile));
		}
		sBodyText.Replace("$=<BASEPATH>",sBasePath);
		sBodyText.Replace("$=<BASEAPPPATH>",GetApplicationDir());
		sBodyText.Replace("$=<APPDATPATH>",sAppDat);
		sBodyText.Replace("$=<PROFILE>",GetUserFolder());
		sBodyText.Replace("$=<DAY>",Format("%i",dt.GetDay()));
		sBodyText.Replace("$=<HOUR>",Format("%i",dt.GetHour()));
		sBodyText.Replace("$=<MINUTE>",Format("%i",dt.GetMinute()));
		sBodyText.Replace("$=<SECOND>",Format("%i",dt.GetSecond()));
		sBodyText.Replace("$=<YEAR>",Format("%i",dt.GetYear()));
		sBodyText.Replace("$=<YEAR+1>",Format("%i",dt.GetYear()+1));
		sBodyText.Replace("$=<LANG>",Format("%i",GetApplicationLang()));
		sBodyText.Replace("$=<WIN9X>",Format("%i",isWin9x()));
		sBodyText.Replace("$=<BGCOLOR>",Format("#%06X",objSettings.lWPBGColor));
		sBodyText.Replace("$=<BGICOLOR>",Format("#%06X",objSettings.lWPIBGColor));
		sBodyText.Replace("$=<FGCOLOR>",Format("#%06X",objSettings.lWPTXColor));
		sBodyText.Replace("$=<SCREEN_X>",Format("%i",rtDeskRect.Width()));
		sBodyText.Replace("$=<SCREEN_Y>",Format("%i",rtDeskRect.Height()));
		sBodyText.Replace("$=<WALLPAPER>",getFileFullName(DEFAULT_HTMLBG));
		sBodyText.Replace("$=<BGCOLOR>",Format("#%06X",objSettings.lWPBGColor));
		sBodyText.Replace("$=<BGICOLOR>",Format("#%06X",objSettings.lWPIBGColor));
		sBodyText.Replace("$=<FGCOLOR>",Format("#%06X",objSettings.lWPTXColor));
		if(sBodyText.Find("$=<WEEK")!=-1){
			sBodyText.Replace("$=<WEEKDAY>",Format("%i",dt.GetDayOfWeek()-1));//0-6, как в JavaScript
			sBodyText.Replace("$=<WEEKDAY_NAME_FULL>",dt.Format("%A"));
			sBodyText.Replace("$=<WEEKDAY_NAME_ABR>",dt.Format("%a"));
			int iWeekNum=int(iDay/7)+1;
			sBodyText.Replace("$=<WEEKNUM>",Format("%i",iWeekNum));
		}
		if(sBodyText.Find("{week")!=-1){
			sBodyText.Replace("{weekday}",Format("%i",dt.GetDayOfWeek()-1));//0-6, как в JavaScript
			sBodyText.Replace("{weekDayNameFull}",dt.Format("%A"));
			sBodyText.Replace("{weekDayNameAbr}",dt.Format("%a"));
			int iWeekNum=int(iDay/7)+1;
			sBodyText.Replace("{weekNum}",Format("%i",iWeekNum));
		}
		if(sBodyText.Find("$=<MONTH")!=-1){
			sBodyText.Replace("$=<MONTH>",Format("%i",dt.GetMonth()));
			int iNextMonth=dt.GetMonth()+1;
			if(iNextMonth>12){
				iNextMonth-=12;
			}
			sBodyText.Replace("$=<MONTH+1>",Format("%i",iNextMonth));
			sBodyText.Replace("$=<MONTH_NAME_FULL>",dt.Format("%B"));
			sBodyText.Replace("$=<MONTH_NAME_ABR>",dt.Format("%b"));
		}
		if(sBodyText.Find("{month")!=-1){
			sBodyText.Replace("{month}",Format("%i",dt.GetMonth()));
			int iNextMonth=dt.GetMonth()+1;
			if(iNextMonth>12){
				iNextMonth-=12;
			}
			sBodyText.Replace("{monthNext}",Format("%i",iNextMonth));
			sBodyText.Replace("{monthNameFull}",dt.Format("%B"));
			sBodyText.Replace("{monthNameAbr}",dt.Format("%b"));
		}
		if(sBodyText.Find("$=<LOCAL_")!=-1){
			char szTmp[54]={0};
			GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,0,0,szTmp,sizeof(szTmp));
			sBodyText.Replace("$=<LOCAL_DATE>",szTmp);
			GetTimeFormat(LOCALE_USER_DEFAULT,TIME_FORCE24HOURFORMAT,0,0,szTmp,sizeof(szTmp));
			sBodyText.Replace("$=<LOCAL_TIME>",szTmp);
			GetTimeFormat(LOCALE_USER_DEFAULT,TIME_FORCE24HOURFORMAT|TIME_NOSECONDS,0,0,szTmp,sizeof(szTmp));
			sBodyText.Replace("$=<LOCAL_TIME_NOSECS>",szTmp);
		}
		if(sBodyText.Find("{local")!=-1){
			char szTmp[54]={0};
			GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,0,0,szTmp,sizeof(szTmp));
			sBodyText.Replace("{localDate}",szTmp);
			GetTimeFormat(LOCALE_USER_DEFAULT,TIME_FORCE24HOURFORMAT,0,0,szTmp,sizeof(szTmp));
			sBodyText.Replace("{localTime}",szTmp);
			GetTimeFormat(LOCALE_USER_DEFAULT,TIME_FORCE24HOURFORMAT|TIME_NOSECONDS,0,0,szTmp,sizeof(szTmp));
			sBodyText.Replace("{localTimeNoSec}",szTmp);
		}
	}
	iFrom=0;
	while(iFrom>=0){// Date formated
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<DATE_FORMAT(",")>",sBodyText,iFrom);
		if(sTextLineParams!="" && sTextLineParams.Find("$=<PAR:")==-1){
			CString sTextToReplace=Format("$=<DATE_FORMAT(%s)>",sTextLineParams);
			CString sTextToReplaceWith=sTextLineParams;
			char szTmp[256]={0};
			GetDateFormat(LOCALE_USER_DEFAULT,0,0,sTextLineParams,szTmp,sizeof(szTmp));
			sTextToReplaceWith=szTmp;
			sBodyText.Replace(sTextToReplace,sTextToReplaceWith);
			iFrom+=strlen(sTextToReplaceWith)-strlen(sTextToReplace);
		}
	}
	iFrom=0;
	while(iFrom>=0){// Leave host only
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<GETHOST(",")>",sBodyText,iFrom);
		if(sTextLineParams!="" && sTextLineParams.Find("$=<PAR:")==-1){
			CString sTextToReplace=Format("$=<GETHOST(%s)>",sTextLineParams);
			CString sTextToReplaceWith=sTextLineParams;
			int iPrPos=sTextLineParams.Find("://");
			if(iPrPos!=-1){
				int iPrPos2=sTextLineParams.Find("/",iPrPos+3);
				if(iPrPos2!=-1){
					sTextToReplaceWith=sTextLineParams.Left(iPrPos2);
				}
			}
			sBodyText.Replace(sTextToReplace,sTextToReplaceWith);
			iFrom+=strlen(sTextToReplaceWith)-strlen(sTextToReplace);
		}
	}
	iFrom=0;
	SubstVariables_Expr(sBodyText);
	while(iFrom>=0){// Simple expressions
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<INTEXPR(",")>",sBodyText,iFrom);
		if(sTextLineParams!="" && sTextLineParams.Find("$=<PAR:")==-1){
			CString sTextToReplace=Format("$=<INTEXPR(%s)>",sTextLineParams);
			CString sTextToReplaceWith=Format("%i",SimpleCalcX(sTextLineParams));
			sBodyText.Replace(sTextToReplace,sTextToReplaceWith);
			iFrom+=strlen(sTextToReplaceWith)-strlen(sTextToReplace);
		}
	}
	iFrom=0;
	while(iFrom>=0){// list selector
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<LISTVV(",")>",sBodyText,iFrom);
		if(sTextLineParams!=""){
			CString sTextToReplace=Format("$=<LISTVV(%s)>",sTextLineParams);
			CString sTextToReplaceWith="";
			CStringArray aList;
			ConvertComboDataToArray(sTextLineParams,aList,',');
			if(aList.GetSize()>2){
				int iIndex=atol(aList[0]);
				int iIndexDef=atol(aList[1]);
				if(iIndex<0 || iIndex>aList.GetSize()-2){
					iIndex=iIndexDef;
				}
				sTextToReplaceWith=aList[iIndex+2];
			}
			sBodyText.Replace(sTextToReplace,sTextToReplaceWith);
			iFrom+=strlen(sTextToReplaceWith)-strlen(sTextToReplace);
		}
	}
	CString sTagValue;
	while((iFrom=extTag("wp:getHost",sBodyText,&sTagValue))!=-1){
		sTagValue=getURLHost(sTagValue);
		sBodyText.Insert(iFrom,sTagValue);
		sTagValue="";
	}
}

BOOL CheckIFRaw(CString& sWPPar)
{
	sWPPar.TrimLeft();
	sWPPar.TrimRight();
	int iEqSign=0;
	int iOffset=2;
	int iCheckOperation=-1;
	if(-1!=(iEqSign=sWPPar.Find("=="))){
		iCheckOperation=1;
	}else if(-1!=(iEqSign=sWPPar.Find("!="))){
		iCheckOperation=0;
	}else if(-1!=(iEqSign=sWPPar.Find(">="))){
		iCheckOperation=2;
	}else if(-1!=(iEqSign=sWPPar.Find("<="))){
		iCheckOperation=3;
	}else if(-1!=(iEqSign=sWPPar.Find(">"))){
		iCheckOperation=4;
		iOffset=1;
	}else if(-1!=(iEqSign=sWPPar.Find("<"))){
		iCheckOperation=5;
		iOffset=1;
	}else if(-1!=(iEqSign=sWPPar.Find(".gt."))){
		iCheckOperation=4;
		iOffset=4;
	}else if(-1!=(iEqSign=sWPPar.Find(".lt."))){
		iCheckOperation=5;
		iOffset=4;
	}else if(-1!=(iEqSign=sWPPar.Find("="))){
		iCheckOperation=1;
		iOffset=1;
	}
	if(iEqSign==-1){
		return atol(sWPPar)>0;
	}
	CString sWPParLeft=sWPPar.Left(iEqSign);
	CString sWPParRight=sWPPar.Mid(sWPParLeft.GetLength()+iOffset);
	sWPParLeft.TrimLeft();
	sWPParLeft.TrimRight();
	sWPParRight.TrimLeft();
	sWPParRight.TrimRight();
	if(sWPParRight=="\"\""){
		if(iCheckOperation==1 && strlen(sWPParLeft)>0){
			return 0;
		}
		if(iCheckOperation==0 && strlen(sWPParLeft)==0){
			return 0;
		}
	}else if(	(iCheckOperation==0 && !(sWPParRight!=sWPParLeft) || (iCheckOperation==1 && !(sWPParRight==sWPParLeft)))
		|| (iCheckOperation==2 && !(atol(sWPParLeft)>=atol(sWPParRight))) || (iCheckOperation==3 && !(atol(sWPParLeft)<=atol(sWPParRight)))
		|| (iCheckOperation==4 && !(atol(sWPParLeft)>atol(sWPParRight))) || (iCheckOperation==5 && !(atol(sWPParLeft)<atol(sWPParRight)))){
		return 0;
	}
	return 1;
}

BOOL CheckIF(CString& sWPPar)
{
	if(sWPPar==""){
		return -1;
	}
	int iPos=-1;
	CString sWPParLow=sWPPar;
	sWPParLow.MakeLower();
	if(-1!=(iPos=sWPParLow.Find(".contain."))){
		CString sRight=sWPPar.Mid(iPos+9);
		if(sRight.Left(1)=="\"" && sRight.Right(1)=="\""){
			sRight=sRight.Mid(1,sRight.GetLength()-2);
		}
		return sWPPar.Left(iPos).Find(sRight)==-1?0:1;
	}
	if(-1!=(iPos=sWPParLow.Find("not."))){
		return CheckIF(sWPPar.Left(iPos)+Format("%i",CheckIF(sWPPar.Mid(iPos+4))?0:1));
	}
	if(-1!=(iPos=sWPParLow.Find(".isempty"))){
		return (sWPPar.Left(iPos).GetLength()==0?1:0)||(sWPPar.Left(iPos)=="''")||(sWPPar.Left(iPos)=="\"\"");
	}
	if(-1!=(iPos=sWPParLow.Find(".or."))){
		return CheckIF(sWPPar.Left(iPos))||CheckIF(sWPPar.Mid(iPos+4));
	}
	if(-1!=(iPos=sWPParLow.Find(".and."))){
		return CheckIF(sWPPar.Left(iPos))&&CheckIF(sWPPar.Mid(iPos+4));
	}
	if(-1!=(iPos=sWPParLow.Find(".xor."))){
		return CheckIF(sWPPar.Left(iPos))^CheckIF(sWPPar.Mid(iPos+4));
	}
	return CheckIFRaw(sWPPar);
}

int CheckIFs(const char* szPref, CString& sBodyText)
{
	int iFrom=0,iIfCount=0;
	CString sTagValue,sAttr;
	while((iFrom=extTag("wp:if",sBodyText,&sTagValue,&sAttr))!=-1){
		iIfCount++;
		CString sElseLine=_atr("else",sAttr),sThenLine=_atr("then",sAttr),sEquation=_atr("condition",sAttr);
		if(sEquation.GetLength()==0){
			_tag("wp:condition",sTagValue,&sEquation);
		}
		if(sThenLine.GetLength()==0){
			_tag("wp:then",sTagValue,&sThenLine);
		}
		if(sThenLine.GetLength()==0){
			sThenLine=sTagValue;
		}
		if(sElseLine.GetLength()==0){
			_tag("wp:else",sTagValue,&sElseLine);
		}
		CString sResult=sThenLine;
		if(!CheckIF(sEquation)){
			sResult=sElseLine;
		}
		sBodyText.Insert(iFrom,sResult);
		if(iIfCount>1000){
#ifdef _DEBUG
			AfxMessageBox("WP:IF recurrency check error!");
#endif
			break;
		}
		sTagValue="";
	}
	if(sBodyText.Find("$=<")==-1){
		return 0;
	}
	int iRes=0;
	iFrom=0;
	while(iFrom>=0){
		CString sWPPar=CDataXMLSaver::GetInstringPart(Format("$=<%s:BEGIN|",szPref),Format("|%s:END>",szPref),sBodyText,iFrom);
		if(sWPPar.IsEmpty()){
			break;
		}
		CString sWPParOrig=sWPPar;
		CString sThenLine=Format("|%s:THEN|",szPref);
		CString sElseLine=Format("|%s:ELSE|",szPref);
		int iMaxSign=sWPPar.Find(sThenLine);
		if(iMaxSign==-1){
			continue;
		}
		CString sEquation=sWPPar.Left(iMaxSign);
		CString sWPParVal=sWPPar.Mid(iMaxSign+sThenLine.GetLength());
		int iElsePos=-1;
		CString sElse="";
		if((iElsePos=sWPPar.Find(sElseLine))!=-1){
			sElse=sWPPar.Mid(iElsePos+sElseLine.GetLength());
			sWPParVal=sWPPar.Mid(iMaxSign+sThenLine.GetLength(),iElsePos-(iMaxSign+sThenLine.GetLength()));
		}
		if(!CheckIF(sEquation)){
			sWPParVal=sElse;
		}
		iRes+=sBodyText.Replace(Format("$=<%s:BEGIN|%s|%s:END>",szPref,sWPParOrig,szPref),sWPParVal);
		iFrom=0;
	}
	iFrom=0;
	while(iFrom>=0){
		CString sWPPar=CDataXMLSaver::GetInstringPart(Format("$=<%s:",szPref),Format("/%s>",szPref),sBodyText,iFrom);
		if(sWPPar.IsEmpty()){
			break;
		}
		int iMaxSign=sWPPar.Find("?");
		if(iMaxSign==-1){
			continue;
		}
		CString sEquation=sWPPar.Left(iMaxSign);
		CString sWPParVal=sWPPar.Mid(iMaxSign+1);
		if(!CheckIF(sEquation)){
			sWPParVal="";
		}
		iRes+=sBodyText.Replace(Format("$=<%s:%s/%s>",szPref,sWPPar,szPref),sWPParVal);
		iFrom=0;
	}
	return iRes;
}

void SubstVariables_Vars(CString& sBodyText,CSetWallPaper* wpCommonData)
{
	int iFrom=0;
	CString sTagValue,sName;
	while((iFrom=extTag("wp:define",sBodyText,&sTagValue))!=-1){
		sName=CDataXMLSaver::GetInstringPart("[","]",sTagValue);
		if(sName==""){
			continue;
		}
		CString sDef=_atr(Format("[%s]",sName),sTagValue);
		// По старинке...
		int iPos=0;
		CString sCurValue=CDataXMLSaver::GetInstringPart(Format("$=<PARVAL:%s=",sName),">",sBodyText,iPos);
		if(iPos==-1){
			sCurValue=sDef;
		}
		sBodyText.Replace(Format("{%s}",sName),sCurValue);
		CDataXMLSaver::Str2Xml(sCurValue,XMLJScriptConv);
		sBodyText.Replace(Format("{escaped{%s}}",sName),sCurValue);
		sTagValue="";
	}
	if(sBodyText.Find("$=<PAR")==-1){
		return;
	}
	iFrom=0;
	while(iFrom>=0){
		// Подразумевается что используется переменная после объявления
		// иначе после Replace iFrom может перескочить непросмотренную область или проверить что-то дважды
		CString sWPPar=CDataXMLSaver::GetInstringPart("$=<PARDEF:",">",sBodyText,iFrom);
		if(sWPPar.IsEmpty()){
			break;
		}
#ifdef _DEBUG
		if(sWPPar.Find("Skin_Default")!=-1){
			int a=1;
		}
#endif

		sWPPar=sWPPar.SpanExcluding("=");
		DWORD iFromPar=0;
		CString sValName=Format("$=<PARVAL:%s=",sWPPar);
		CString sWPParVal=CDataXMLSaver::GetInstringPart(sValName,">",sBodyText,iFromPar);
		//if(sWPParVal.IsEmpty())
		if(sBodyText.Find(sValName)==-1){
			iFromPar=iFrom;
			sWPParVal=CDataXMLSaver::GetInstringPart(Format("$=<PARDEF:%s=",sWPPar),">",sBodyText,iFromPar);
			if(sWPParVal=="\"\""){
				sWPParVal="";
			}
		}
		sBodyText.Replace(Format("$=<PAR:%s>",sWPPar),sWPParVal);
		CString sExc2=EscapeString(sWPParVal);
		sBodyText.Replace(Format("$=<PAR:%s,ENC>",sWPPar),sExc2);
		CString sEsc1=sWPParVal;
		CDataXMLSaver::Str2Xml(sEsc1,XMLJScriptConv);
		sBodyText.Replace(Format("$=<PAR:%s,ESC>",sWPPar),sEsc1);
		sBodyText.Replace(Format("{%s}",sWPPar),sWPParVal);
		{// Замена случайных значений...
			CString sNameRnd=Format("$=<PAR:%s,RND>",sWPPar);
			if(sBodyText.Find(sNameRnd)!=-1){
				CString sRndVal=sWPParVal;
				sWPParVal.Replace(";","\t");
				CStringArray aList;
				ConvertComboDataToArray(sWPParVal,aList);
				if(aList.GetSize()>0){
					sRndVal=aList[rnd(0,aList.GetSize()-1)];
				}
				sRndVal.TrimLeft();
				sRndVal.TrimRight();
				sBodyText.Replace(sNameRnd,sRndVal);
			}
		}
		{// замена экстракта по маске
			CString sNameMask=Format("$=<PAR:%s,MASK[[",sWPPar);
			if(sBodyText.Find(sNameMask)!=-1){
				CString sMask=CDataXMLSaver::GetInstringPart(sNameMask,"]]>",sBodyText,iFrom);
				CString sTextToReplace=Format("%s%s]]>",sNameMask,sMask);
				CString sMaskedVal=sWPParVal;
				// Выципляем значение
				RxNode* pNode=RxCompile(sMask);
				if(pNode){
					RxMatchRes pRes;
					RxExec(pNode,sMaskedVal,strlen(sMaskedVal),sMaskedVal,&pRes,0);
					RxFree(pNode);
					int iIndex=NSEXPS-1;
					while(iIndex>0 && pRes.Open[iIndex]<0){
						iIndex--;
					}
					if(iIndex>=0 && pRes.Open[iIndex]>=0 && pRes.Close[iIndex]>=pRes.Open[iIndex]){
						sMaskedVal=sMaskedVal.Mid(pRes.Open[iIndex],pRes.Close[iIndex]-pRes.Open[iIndex]);
					}
				}
				sMaskedVal.TrimLeft();
				sMaskedVal.TrimRight();
				sBodyText.Replace(sTextToReplace,sMaskedVal);
			}
		}
	}
}

void SubstVariables(CString& sBodyText,CSetWallPaper* wpCommonData)
{
	int iFrom=0;
	SubstVariables_Vars(sBodyText,wpCommonData);
	SubstVariables_Simple(sBodyText,TRUE,wpCommonData);
	CString sTagValue,sAttr;
	while((iFrom=extTagSimple("rnd",sBodyText,&sTagValue,&sAttr))!=-1){
		CString sResult=Format("%i",rnd(atol(_arr(sTagValue,0)),atol(_arr(sTagValue,1,"9999"))));
		sBodyText.Insert(iFrom,sResult);
	}
	iFrom=0;
	while(iFrom>=0){
		CString sWPRnd=CDataXMLSaver::GetInstringPart("$=<RND(",")>",sBodyText,iFrom);
		if(sWPRnd.IsEmpty()){
			break;
		}
		int iMinRnd=atol(sWPRnd);
		int iMaxRndPos=sWPRnd.Find(",");
		int iMaxRnd=9999;
		if(iMaxRndPos!=-1){
			iMaxRnd=atol(sWPRnd.Mid(iMaxRndPos+1));
		}
		CString sWhatToReplace=Format("$=<RND(%s)>",sWPRnd);
		CString sRndNum=Format("%i",rnd(iMinRnd,iMaxRnd));
		CString sLeft=sBodyText.Left(iFrom-strlen(sWhatToReplace));
		CString sRight=sBodyText.Mid(iFrom);
		sBodyText=sLeft+sRndNum+sRight;
		iFrom=0;
	}
	//
	iFrom=0;
	while(iFrom>=0){
		CString sWPPar=CDataXMLSaver::GetInstringPart("$=<SWITCH-BEGIN:",">",sBodyText,iFrom);
		if(sWPPar.IsEmpty()){
			break;
		}
		CString sWPParBody=CDataXMLSaver::GetInstringPart(Format("$=<SWITCH-BEGIN:%s>",sWPPar),Format("$=<SWITCH-END:%s>",sWPPar),sBodyText);
		CString sWPParVal=CDataXMLSaver::GetInstringPart(Format("$=<CASE-BEGIN:%s>",sWPPar),Format("$=<CASE-END:%s>",sWPPar),sBodyText);
		if(sWPParVal==""){
			sWPParVal=CDataXMLSaver::GetInstringPart("$=<CASE-BEGIN:DEFAULT>","$=<CASE-END:DEFAULT>",sBodyText);
		}
		sBodyText.Replace(Format("$=<SWITCH-BEGIN:%s>%s$=<SWITCH-END:%s>",sWPPar,sWPParBody,sWPPar),sWPParVal);
		iFrom=0;
	}
	//
	iFrom=0;
	CheckIFs("IF",sBodyText);
	while((iFrom=sBodyText.Find("$=<IF",iFrom))!=-1){
		if(sBodyText[iFrom+5+1]!=':'){
			CString sPostFix="";
			sPostFix=sBodyText.Mid(iFrom+5).SpanExcluding(":");
			if(CheckIFs(Format("IF%s",sPostFix),sBodyText)==0){
				// Ошибка где-то!!!
#ifdef _DEFUG
				Alert("IF-ERROR!!!");
#endif
				break;
			}
			iFrom=0;
		}else{
			iFrom+=5;
		}
	}
	// НеСлучайно из списка
	iFrom=0;
	while(iFrom>=0){
		CString sWPRnd=CDataXMLSaver::GetInstringPart("$=<IN_LIST(",")>",sBodyText,iFrom);
		if(sWPRnd.IsEmpty()){
			break;
		}
		CString sWhatToReplace=Format("$=<IN_LIST(%s)>",sWPRnd);
		int iComboPos=atol(sWPRnd);
		sWPRnd.TrimLeft("0123456789,");
		sWPRnd.Replace(",","\t");
		CStringArray aComboItems;
		ConvertComboDataToArray(sWPRnd,aComboItems);
		CString sRndNum="";
		if(iComboPos>=0 && iComboPos<aComboItems.GetSize()){
			sRndNum=aComboItems[iComboPos];
		}
		CString sLeft=sBodyText.Left(iFrom-strlen(sWhatToReplace));
		CString sRight=sBodyText.Mid(iFrom);
		sBodyText=sLeft+sRndNum+sRight;
		iFrom=0;
	}
	// Случайно из списка
	while((iFrom=extTag("wp:list.rnd",sBodyText,&sTagValue,&sAttr))!=-1){
		if(sAttr==""){
			sAttr=sTagValue;
		}
		sAttr.Replace(",","\t");
		CStringArray aComboItems;
		ConvertComboDataToArray(sAttr,aComboItems);
		int i=0;
		while(i<aComboItems.GetSize() && aComboItems.GetSize()>0){
			if(aComboItems[i]==""){
				aComboItems.RemoveAt(i);
			}else{
				i++;
			}
		}
		CString sRndNum="";
		if(aComboItems.GetSize()>0){
			sRndNum=aComboItems[rnd(0,aComboItems.GetSize()-1)];
			sRndNum.TrimLeft();
			sRndNum.TrimRight();
		}
		sBodyText.Insert(iFrom,sRndNum);
		sTagValue="";
	}
	iFrom=0;
	while(iFrom>=0){
		CString sWPRnd=CDataXMLSaver::GetInstringPart("$=<RND_LIST(",")>",sBodyText,iFrom);
		if(sWPRnd.IsEmpty()){
			break;
		}
		CString sWhatToReplace=Format("$=<RND_LIST(%s)>",sWPRnd);
		sWPRnd.Replace(",","\t");
		CStringArray aComboItems;
		ConvertComboDataToArray(sWPRnd,aComboItems);
		int i=0;
		while(i<aComboItems.GetSize() && aComboItems.GetSize()>0){
			if(aComboItems[i]==""){
				aComboItems.RemoveAt(i);
			}else{
				i++;
			}
		}
		CString sRndNum="";
		if(aComboItems.GetSize()>0){
			sRndNum=aComboItems[rnd(0,aComboItems.GetSize()-1)];
		}
		CString sLeft=sBodyText.Left(iFrom-strlen(sWhatToReplace));
		CString sRight=sBodyText.Mid(iFrom);
		sBodyText=sLeft+sRndNum+sRight;
		iFrom=0;
	}
}

#include <intshcut.h>
#pragma comment(lib,"url.lib")
BOOL MyCheckConnection(const char* szUrl)
{
	CWebWorld url;
	url.bOnlyCheckConnection=1;
	CString sPageContent=url.GetWebPage(szUrl);
	if(sPageContent!=""){
		return 1;
	}
	return 0;
}

void TranslateTemplateAliases(CString& sBodyText, CSetWallPaper* wpCommonData)
{
	// Замены параметров
	if(wpCommonData){
		CRect rtDeskRect=theApp.GetDesktopRECT();
		if(wpCommonData && !wpCommonData->requiredSizeOfOutImage.IsRectEmpty()){
			rtDeskRect.CopyRect(wpCommonData->requiredSizeOfOutImage);
		}
		sBodyText.Replace("$=<WALLPAPER_X>",Format("%i",wpCommonData->sizeOfOutImage.cx));
		sBodyText.Replace("$=<WALLPAPER_Y>",Format("%i",wpCommonData->sizeOfOutImage.cy));
		sBodyText.Replace("$=<WALLPAPER_ORIG>",wpCommonData->sWallpaperFile);
		sBodyText.Replace("$=<WALLPAPER_ORIG_NAME>",GetPathPart(wpCommonData->sWallpaperFile,0,0,1,0));
		sBodyText.Replace("$=<THEME>",wpCommonData->sWallpaperThemeName);
		sBodyText.Replace("{wallpapersrc_file}",wpCommonData->sWallpaperFile);
		CString sFNamePath=GetPathPart(wpCommonData->sWallpaperFile,1,1,0,0);
		CString sFName=GetPathPart(wpCommonData->sWallpaperFile,0,0,1,0);
		
		sBodyText.Replace("{current_widget}",wpCommonData?wpCommonData->sCurrentWPT:"");
		sBodyText.Replace("{wallpapersrc_file_name}",sFName);
		sBodyText.Replace("{wallpaperlastsrc_file_name}",objSettings.sLastWallpaperFromWC!=""?objSettings.sLastWallpaperFromWC:objSettings.sLastWallpaperFromUser);
		sBodyText.Replace("{wallpapersrc_file_path}",sFNamePath);
		sBodyText.Replace("{wallpapersrc_info_file}",sFNamePath+sFName+".xml");
		sBodyText.Replace("{wallpapersrc_info_file_txt}",sFNamePath+sFName+".txt");
		sBodyText.Replace("{wallpaper_onscreenX}",Format("%i",wpCommonData->rcOutScreenOnRect.left));
		sBodyText.Replace("{wallpaper_onscreenY}",Format("%i",wpCommonData->rcOutScreenOnRect.top));
		sBodyText.Replace("{wallpaper_onscreenW}",Format("%i",wpCommonData->rcOutScreenOnRect.Width()));
		sBodyText.Replace("{wallpaper_onscreenH}",Format("%i",wpCommonData->rcOutScreenOnRect.Height()));
		CRect rtRecommended(wpCommonData->rcRecommendedWPPosition);
		if(rtRecommended.left>1){
			rtRecommended.left--;
		}
		if(rtRecommended.top>1){
			rtRecommended.top--;
		}
		if(rtRecommended.IsRectEmpty()){
			rtRecommended.CopyRect(&rtDeskRect);
		}
		sBodyText.Replace("{recommended-wallpaper-x}",Format("%i",rtRecommended.left));
		sBodyText.Replace("{recommended-wallpaper-y}",Format("%i",rtRecommended.top));
		sBodyText.Replace("{recommended-wallpaper-w}",Format("%i",rtRecommended.Width()));
		sBodyText.Replace("{recommended-wallpaper-h}",Format("%i",rtRecommended.Height()));
		sBodyText.Replace("{prog_id}",AppName()+""PROG_VERSION);
	}
	SubstVariables_Simple(sBodyText,FALSE,wpCommonData);
	// Все базовые штуки заенили - меняем переменные!
	SubstVariables_Vars(sBodyText,wpCommonData);
	int iFrom=0;
	CString sTagValue,sAttr;
	while((iFrom=extTagSimple("wp:isFileExist",sBodyText,&sTagValue,&sAttr))!=-1){
		CString sFile=_arr(sTagValue,0);
		CString sResult=Format("%i",isFileExist(sFile)?1:0);
		sBodyText.Insert(iFrom,sResult);
	}
	//
	iFrom=0;
	while(iFrom>=0){
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<DAYSTO(",")>",sBodyText,iFrom);
		if(sTextLineParams!=""){
			CString sParam=sTextLineParams;
			sParam.Replace("/","\t");
			sParam.Replace(":","\t");
			sParam.Replace(".","\t");
			sParam.Replace(",","\t");
			sParam.Replace(" ","\t");
			CStringArray aNumbers;
			CString sResult;
			ConvertComboDataToArray(sParam,aNumbers);
			if(aNumbers.GetSize()>=3){
				if(atol(aNumbers[2])<100){
					aNumbers[2]=Format("%i",atol(aNumbers[2])+2000);
				}
				COleDateTime dtCur=COleDateTime::GetCurrentTime();
				COleDateTime dtDate(atol(aNumbers[2]),atol(aNumbers[1]),atol(aNumbers[0]),dtCur.GetHour(),dtCur.GetMinute(),dtCur.GetSecond());
				COleDateTimeSpan diff=dtDate-dtCur;
				long lDaysBetween=diff.GetDays();
				/*
				if(lDaysBetween>=0 && dtDate>dtCur){
					lDaysBetween++;
				}
				*/
				if(sTextLineParams.Find("abs")!=-1){
					lDaysBetween=abs(lDaysBetween);
				}
				sResult=Format("%i",lDaysBetween);
			}
			CString sTextToReplace=Format("$=<DAYSTO(%s)>",sTextLineParams);
			sBodyText.Replace(sTextToReplace,sResult);
			iFrom+=strlen(sResult)-strlen(sTextToReplace);
		}
	}
	SubstVariables(sBodyText,wpCommonData);
	// Вставка путей файлов
	iFrom=0;
	while(iFrom>=0){
		CString sIncFileOri=CDataXMLSaver::GetInstringPart("$=<INCLUDEPATH(",")>",sBodyText,iFrom);
		if(sIncFileOri.IsEmpty()){
			break;
		}
		CString sIncFile=sIncFileOri;
		BOOL bEnc=sIncFile.Replace(",ENC","");
		CString sContent=GetWPTemplateFilePath(sIncFile);
		if(wpCommonData){
			wpCommonData->sCurFilePath=GetWPTemplateFilePath(sIncFile);
			SubstVariables_Simple(sContent,0,wpCommonData);
		}
		if(bEnc)
		{
			sContent=EscapeString(sContent);
		}
		CString sTextToReplace=Format("$=<INCLUDEPATH(%s)>",sIncFileOri);
		sBodyText.Replace(sTextToReplace,sContent);
		SubstVariables(sBodyText,wpCommonData);
		iFrom=0;
	}
	// Вставка файлов
	iFrom=0;
	while(iFrom>=0){
		CString sIncFile=CDataXMLSaver::GetInstringPart("$=<INCLUDE(",")>",sBodyText,iFrom);
		if(sIncFile.IsEmpty()){
			break;
		}
		CString sContent;
		ReadFile(GetWPTemplateFilePath(sIncFile),sContent);
		if(wpCommonData){
			wpCommonData->sCurFilePath=GetWPTemplateFilePath(sIncFile);
			SubstVariables_Simple(sContent,0,wpCommonData);
		}

		CString sTextToReplace=Format("$=<INCLUDE(%s)>",sIncFile);
		sBodyText.Replace(sTextToReplace,sContent);
		SubstVariables(sBodyText,wpCommonData);
		iFrom=0;
	}
	// Вставка файлов, содержимое преобразуется в одну строку
	iFrom=0;
	while(iFrom>=0){
		CString sIncFile=CDataXMLSaver::GetInstringPart("$=<INCLUDEESCAPED(",")>",sBodyText,iFrom);
		if(sIncFile.IsEmpty()){
			break;
		}
		CString sContent;
		ReadFile(GetWPTemplateFilePath(sIncFile),sContent);
		if(wpCommonData){
			wpCommonData->sCurFilePath=GetWPTemplateFilePath(sIncFile);
			SubstVariables_Simple(sContent,0,wpCommonData);
		}

		CString sTextToReplace=Format("$=<INCLUDEESCAPED(%s)>",sIncFile);
		CDataXMLSaver::Str2Xml(sContent,XMLJScriptConv);
		sBodyText.Replace(sTextToReplace,sContent);
		SubstVariables(sBodyText,wpCommonData);
		iFrom=0;
	}
	// После инклюдов - снова пробегаемся по заменам!!!
	SubstVariables_Simple(sBodyText,FALSE,wpCommonData);
	// Реестр
	iFrom=0;
	while(iFrom>=0){
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<REGKEY(",")>",sBodyText,iFrom);
		if(sTextLineParams!=""){
			CString sResult="";
			CString sParam=sTextLineParams;
			sParam=sParam.Mid(strlen("HKLM:"));
			CString sKey=sParam.SpanExcluding(",");
			CString sKVal=sParam.Mid(sKey.GetLength()+1);
			if(sTextLineParams.Find("HKLM:")!=-1){
				CRegKey key;
				key.Open(HKEY_LOCAL_MACHINE, sKey);
				if(key!=NULL){
					char szTmp[MAX_PATH]={0};
					DWORD lSize = sizeof(szTmp),dwType=0;
					if(RegQueryValueEx(key.m_hKey,sKVal,NULL, &dwType,(LPBYTE)(szTmp), &lSize)==ERROR_SUCCESS){
						sResult=szTmp;
					}
				}
			}
			//key.Open(HKEY_CURRENT_USER, "Control Panel\\Desktop");
			CString sTextToReplace=Format("$=<REGKEY(%s)>",sTextLineParams);
			sBodyText.Replace(sTextToReplace,sResult);
			iFrom+=strlen(sResult)-strlen(sTextToReplace);
		}
	}
	//
	if(wpCommonData){
		while((iFrom=extTagSimple("wp:set-wallpaper-rect",sBodyText,&sTagValue,&sAttr))!=-1){
			CString sX=_arr(sTagValue,0);
			CString sY=_arr(sTagValue,1);
			CString sW=_arr(sTagValue,2);
			CString sH=_arr(sTagValue,3);
			wpCommonData->rcRecommendedWPPosition.SetRect(atol(sX),atol(sY),atol(sX)+atol(sW),atol(sY)+atol(sH));
		}
	}
	while((iFrom=extTagSimple("wp:iniParam",sBodyText,&sTagValue,&sAttr))!=-1){
		CString sFile=_arr(sTagValue,0);
		CString sParams=_arr(sTagValue,1);
		if(sParams!=""){
			if(sParams.GetAt(0)=='"'){
				sParams.TrimLeft("\"");
				sParams.TrimRight("\"");
			}
			if(sParams.GetAt(0)=='\''){
				sParams.TrimLeft("'");
				sParams.TrimRight("'");
			}
		}
		CString sResult=objFileParser().getValueRandom(GetTextIncludeFilePath_forTempl(sFile),(sParams==""?"":(sParams+"=")),TRUE);
		if(sResult==""){
			sResult=sParams;
		}
		CDataXMLSaver::Str2Xml(sResult);
		sBodyText.Insert(iFrom,sResult);
	}
	while((iFrom=extTagSimple("wp:filePath",sBodyText,&sTagValue,&sAttr))!=-1){
		CString sFile=_arr(sTagValue,0);
		CString sResult=GetTextIncludeFilePath_forTempl(sFile);
		sBodyText.Insert(iFrom,sResult);
	}
	while((iFrom=extTagSimple("wp:format",sBodyText,&sTagValue,&sAttr))!=-1){
		CString sTemplate=_arr(sTagValue,0);
		sTemplate.Replace("???","%s");
		CString sResult=sTemplate;
		CString sParams1=_arr(sTagValue,1);
		CString sParams2=_arr(sTagValue,2);
		CString sParams3=_arr(sTagValue,3);
		CString sParams4=_arr(sTagValue,4);
		if(sParams1!=""){
			sResult=Format(sTemplate,sParams1,sParams2,sParams3,sParams4);
		}
		sResult.Replace("<brr>","\r\n");
		//CDataXMLSaver::Str2Xml(sResult);
		sBodyText.Insert(iFrom,sResult);
	}	
	while((iFrom=extTagSimple("wp:translate",sBodyText,&sTagValue,&sAttr))!=-1){
		CString sFile=_arr(sTagValue,0);
		CString sParams=_arr(sTagValue,1);
		if(sParams.GetAt(0)=='"'){
			sParams.TrimLeft("\"");
			sParams.TrimRight("\"");
		}
		if(sParams.GetAt(0)=='\''){
			sParams.TrimLeft("'");
			sParams.TrimRight("'");
		}
		CString sFileTest=sFile+Format("_l%i.txt",GetApplicationLang());
		CString sResult=objFileParser().getValueRandom(GetTextIncludeFilePath_forTempl(sFileTest),sParams+"=",TRUE);
		if(sResult==""){
			sFileTest=sFile+Format("_l%i.txt",0);
			sResult=objFileParser().getValueRandom(GetTextIncludeFilePath_forTempl(sFileTest),sParams+"=",TRUE);
		}
		if(sResult==""){
			sResult=_l(sParams);
		}
		CString sParams2=_arr(sTagValue,2);
		CString sParams3=_arr(sTagValue,3);
		CString sParams4=_arr(sTagValue,4);
		if(sParams2!=""){
			sResult=Format(sResult,sParams2,sParams3,sParams4);
		}
		sResult.Replace("<brr>","\r\n");
		//CDataXMLSaver::Str2Xml(sResult);
		sBodyText.Insert(iFrom,sResult);
	}
	iFrom=0;
	while(iFrom>=0){
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<TEXTLINE(",")>",sBodyText,iFrom);
		if(sTextLineParams!=""){
			CString sFile=sTextLineParams.SpanExcluding(",");
			CString sParams="";
			if(strlen(sFile)<strlen(sTextLineParams)){
				sParams=sTextLineParams.Mid(strlen(sFile)+1);
			}
			if(!sParams.IsEmpty()){
				if(sParams.GetAt(0)=='"'){
					sParams.TrimLeft("\"");
					sParams.TrimRight("\"");
				}
				if(sParams.GetAt(0)=='\''){
					sParams.TrimLeft("'");
					sParams.TrimRight("'");
				}
			}
			CString sRandomTextLine=objFileParser().getValueRandom(GetTextIncludeFilePath_forTempl(sFile),sParams,TRUE);
			CDataXMLSaver::Str2Xml(sRandomTextLine);
			CString sTextToReplace=Format("$=<TEXTLINE(%s)>",sTextLineParams);
			sBodyText.Replace(sTextToReplace,sRandomTextLine);
			iFrom+=strlen(sRandomTextLine)-strlen(sTextToReplace);
		}
	}
	//
	while((iFrom=extTagSimple("wp:getXMLcontent",sBodyText,&sTagValue,&sAttr))!=-1){
		CString sFile=_arr(sTagValue,0);
		CString sTag=_arr(sTagValue,1);
		CString sContent;
		ReadFile(GetTextIncludeFilePath_forTempl(sFile),sContent,TRUE);
		CString sResult=CDataXMLSaver::GetInstringPart(Format("<%s>",sTag),Format("</%s>",sTag),sContent);
		CDataXMLSaver::Xml2Str(sResult);
		sResult.TrimLeft();
		sResult.TrimRight();
		sBodyText.Insert(iFrom,sResult);
	}
	//
	iFrom=0;
	while(iFrom>=0){
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<RND_WALLPAPER(",")>",sBodyText,iFrom);
		if(iFrom==-1){
			break;
		}
		CString sTextLineParamsOrig=sTextLineParams;
		CString sPath;
		DWORD dwSampling=0;
		sTextLineParams.MakeLower();
		if(sTextLineParams.Find("main")!=-1){
			sPath=wpCommonData->sWallpaperFile;
			if(sPath==""){
				sPath=getFileFullName(DEFAULT_HTMLBG);
			}
		}else{
			int iPos=-1;
			int iSelType=0;
			if((iPos=sTextLineParams.Find(",fff,"))!=-1){
				CString sFolder=sTextLineParamsOrig.Left(iPos);
				CString sExt=sTextLineParamsOrig.Mid(iPos+5);
				CFileInfoArray dir;
				dir.AddDir(AddPathExtension(sFolder,"\\"),sExt==""?VALID_EXTEN:sExt,TRUE,CFileInfoArray::AP_SORTBYNAME | CFileInfoArray::AP_SORTASCENDING,FALSE);
				if(dir.GetSize()>0){
					sPath=dir[rnd(0,dir.GetSize()-1)].GetFilePath();
				}
			}else{
				if(sTextLineParams.Find("next")!=-1){
					iSelType=1;
				}else if(sTextLineParams.Find("prev")!=-1){
					iSelType=-1;
				}else if(sTextLineParams.Find("def")!=-1){
					iSelType=objSettings.bChangeWPaperCycleType;
				}
				int iNextWpNum=objSettings.m_WChanger.GetNextWPNumber(iSelType);
				if(iNextWpNum>=0 && iNextWpNum<objSettings.m_WChanger.aImagesInRotation.GetSize()){
					if(sTextLineParams.Find("num")!=-1){
						sPath=Format("wc-id://%lu",	objSettings.m_WChanger.aImagesInRotation[iNextWpNum]);
					}else{
						CString sThemeName;
						objSettings.m_WChanger.GetWPTitleAndSampling(objSettings.m_WChanger.aImagesInRotation[iNextWpNum], sPath, sThemeName, dwSampling);
					}
				}else{
					sPath="";
				}
			}
		}
		if(sTextLineParams.Find("jscript")!=-1){
			CDataXMLSaver::Str2Xml(sPath,XMLJScriptConv);
		}
		CString sTextToReplace=Format("$=<RND_WALLPAPER(%s)>",sTextLineParamsOrig);
		// Меняем по одному
		int iFirstReplacePos=sBodyText.Find(sTextToReplace);
		sBodyText=sBodyText.Left(iFirstReplacePos)+sPath+sBodyText.Mid(iFirstReplacePos+sTextToReplace.GetLength());
		iFrom=0;
	}
	iFrom=0;
	while(iFrom>=0){
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<HTML_MAX_SIZE(",")>",sBodyText,iFrom);
		if(iFrom==-1){
			break;
		}
		CString sTextToReplace=Format("$=<HTML_MAX_SIZE(%s)>",sTextLineParams);
		int iZP1=sTextLineParams.Find(",")+1;
		int iZP2=sTextLineParams.Find(",",iZP1)+1;
		double iOw=atol(sTextLineParams);
		double iOh=atol(sTextLineParams.Mid(iZP1));
		sTextLineParams=sTextLineParams.Mid(iZP2);
		CString sRandomTextLine="none";
		int w=0;
		int h=0;
		GetImageSizeFromFile(sTextLineParams,w,h);
		if(w!=0 && h!=0 && iOh>1 && iOw>1){
			if(w/iOw>h/iOh){
				sRandomTextLine="width";
			}else{
				sRandomTextLine="height";
			}
		}
		// Меняем по одному
		sBodyText.Replace(sTextToReplace,sRandomTextLine);
		iFrom+=strlen(sRandomTextLine)-strlen(sTextToReplace);
	}

	// Файлы
	iFrom=0;
	while(iFrom>=0){
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<DECLARE_TEXTFILE(",")>",sBodyText,iFrom);
		if(sTextLineParams!=""){
			CString sFile=sTextLineParams.SpanExcluding(",");
			CString sParams="";
			if(strlen(sFile)<strlen(sTextLineParams)){
				sParams=sTextLineParams.Mid(strlen(sFile)+1);
			}
			CStringSet aStrings;
			CString sRandomTextLine="";
			if(!sParams.IsEmpty()){
				if(sParams.GetAt(0)=='"'){
					sParams.TrimLeft("\"");
					sParams.TrimRight("\"");
				}
				if(sParams.GetAt(0)=='\''){
					sParams.TrimLeft("'");
					sParams.TrimRight("'");
				}
			}
			if(objFileParser().getAllValues(GetTextIncludeFilePath_forTempl(sFile),sParams, aStrings, FALSE) && aStrings.GetSize()>0){
				if(sFile.Find(PROFILE_PREFIX)==0){
					sFile=sFile.Mid(strlen(PROFILE_PREFIX));
				}
				CString sArrayName=sFile.SpanExcluding(".");
				int iSlashPos=sArrayName.Find("\\");
				if(iSlashPos!=-1){
					sArrayName=sArrayName.Mid(iSlashPos+1);
				}
				sRandomTextLine="";//sArrayName+"= new Array();";
				for(int i=0;i<aStrings.GetSize();i++){
					CString sLine=aStrings[i];
					CDataXMLSaver::Str2Xml(sLine);
					sRandomTextLine+=Format("%s[%s.length]=\"%s\";",sArrayName,sArrayName,sLine);
				}
			}
			CString sTextToReplace=Format("$=<DECLARE_TEXTFILE(%s)>",sTextLineParams);
			sBodyText.Replace(sTextToReplace,sRandomTextLine);
			iFrom+=strlen(sRandomTextLine)-strlen(sTextToReplace);
		}
	}
	// URLs
	iFrom=0;
	while(iFrom>=0){
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<DECLARE_URL(",")>",sBodyText,iFrom);
		if(sTextLineParams!=""){
			CString sFile=sTextLineParams.SpanExcluding(",");
			CString sParams="";
			if(strlen(sFile)<strlen(sTextLineParams)){
				sParams=sTextLineParams.Mid(strlen(sFile)+1);
			}
			BOOL bXMLConv=1;
			if(sParams.Find("ASIS")!=-1){
				sParams.Replace("ASIS","");
				bXMLConv=0;
			}
			CWebWorld url;
			url.SetErrorMessage(sParams==""?"Failed to load "+sFile:sParams);
			CString sPageContent=url.GetWebPage(sFile);
			if(bXMLConv){
				CDataXMLSaver::Str2Xml(sPageContent,XMLJScriptConv);
			}
			CString sTextToReplace=Format("$=<DECLARE_URL(%s)>",sTextLineParams);
			sBodyText.Replace(sTextToReplace,sPageContent);
			iFrom+=strlen(sPageContent)-strlen(sTextToReplace);
		}
	}
	// URLs test connection
	iFrom=0;
	while(iFrom>=0){
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<TEST_URL(",")>",sBodyText,iFrom);
		if(sTextLineParams!=""){
			CString sFile=sTextLineParams.SpanExcluding(",");
			CString sPageContent;
			DWORD dwTimeout=0;
			if(strlen(sFile)<strlen(sTextLineParams)){
				dwTimeout=atol(sTextLineParams.Mid(strlen(sFile)+1));
			}
			if(dwTimeout==0){
				dwTimeout=60000;
			}
			/*// Way 1
			CWebWorld url;
			url.SetErrorMessage("");
			if(url.TestWebPage(sFile,dwTimeout)){
				sPageContent="true";
			}else{
				sPageContent="false";
			}*/
			// Way 2
			DWORD dwPing1=GetTickCount();
			if(InetIsOffline(0)){
				sPageContent="false";
			}else if(MyCheckConnection(sFile)){
				sPageContent="true";
			}else{
				sPageContent="false";
			}
			DWORD dwPing2=GetTickCount();
			if(sPageContent=="true" && dwTimeout>0  && dwPing2-dwPing1>dwTimeout){
				sPageContent="false";
			}
			CString sTextToReplace=Format("$=<TEST_URL(%s)>",sTextLineParams);
			sBodyText.Replace(sTextToReplace,sPageContent);
			iFrom+=strlen(sPageContent)-strlen(sTextToReplace);
		}
	}
	// FileCopy
	iFrom=0;
	while(iFrom>=0){
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<COPYFILE(",")>",sBodyText,iFrom);
		if(sTextLineParams!=""){
			CString sFile=sTextLineParams.SpanExcluding("|");
			CString sParams="";
			if(strlen(sFile)<strlen(sTextLineParams)){
				sParams=sTextLineParams.Mid(strlen(sFile)+1);
			}
			BOOL bSkipCheck=0;
			if(sParams.Find(",NOCHECK")!=-1){
				sParams.Replace(",NOCHECK","");
				bSkipCheck=1;
			}
			if(isFileExist(sFile)){
				if(bSkipCheck || GetFileSize(sFile)!=GetFileSize(sParams)){
					DeleteFile(sParams);
					CopyFile(sFile,sParams,FALSE);
				}
			}
			CString sPageContent="";
			CString sTextToReplace=Format("$=<COPYFILE(%s)>",sTextLineParams);
			sBodyText.Replace(sTextToReplace,sPageContent);
			iFrom+=strlen(sPageContent)-strlen(sTextToReplace);
		}
	}
	// Снова. Возможно прокатят вычисления теперь!
	SubstVariables_Simple(sBodyText,FALSE,wpCommonData);
	//TIScript только на финальном этапе - когда рендерим
	if(wpCommonData && wpCommonData->hbmOutImage){
		//sBodyText=ProcessWithTiScript(sBodyText,0);
		iFrom=0;
		while((iFrom=extTag("wp:tiscript",sBodyText,&sTagValue,&sAttr))!=-1){
			if(sTagValue==""){
				sTagValue=sAttr;
			}
			CString sResult=ProcessWithTiScript(sTagValue, 1, wpCommonData);
			sBodyText.Insert(iFrom,sResult);
		}
	}
	DUMPFILE("LastTemplateRendered",sBodyText);
}

void MetaBodyProcessing(CString& sBodyText,CString sRRWPFile, CString& sBodyWP)
{
	sBodyText.Replace("$=<_WALLPAPER_FEATURED>",sRRWPFile);
	sBodyWP=CDataXMLSaver::GetInstringPart("$=<_REPLACE_BG>","$=</_REPLACE_BG>",sBodyText);
	if(sBodyWP==""){
		sBodyWP=sRRWPFile;
	}
	// Выципляем то, что нужно пометить до BODY
	int iFrom=0;
	while(iFrom>=0){
		CString sTextLineParams=CDataXMLSaver::GetInstringPart("$=<_ADDTOMETA>","$=</_ADDTOMETA>",sBodyText,iFrom);
		if(sTextLineParams!=""){
			sBodyText.Replace("<HTML>",Format("<HTML>%s",sTextLineParams));
			CString sTextToReplace=Format("$=<_ADDTOMETA>%s$=</_ADDTOMETA>",sTextLineParams);
			sBodyText.Replace(sTextToReplace,"");
			iFrom+=strlen(sTextLineParams)-strlen(sTextToReplace);
		}
	}
}

int ProcessWallpaperForeground(CSetWallPaper* wpCommonData)
{
	BOOL bExpectedAD=0;
	if(!wpCommonData->bSkipWrapperGeneration){
		// Тут рисуются те HTML шаблоны которые должны прямо в картинку врисовываться
		// Генерируем файл
		CString sFileContent;
		int iHTMLTemplCount=0;
		sFileContent=DEFAULTTEMPLATE_BEGIN;
		if(wpCommonData){
			for(int i=0;i<wpCommonData->TemplatesInWork.GetSize();i++){
				CString sPiece;
				CWPT* wpt=GetWPT(i);
				if(wpt && ReadFileWithConfig(GetWPTemplateFilePath(wpt->sWPFileName),sPiece,wpCommonData?wpCommonData->sInPreset:objSettings.sActivePreset) && wpt->sADType==""){
					iHTMLTemplCount++;
					sFileContent+="\r\n<!--";
					sFileContent+=wpCommonData->TemplatesInWork[i];
					sFileContent+=" -->\r\n";
					sFileContent+=sPiece;
					sFileContent+="\r\n\r\n";
				}
			}
		}
		if(iHTMLTemplCount==0){
			// Типа отрисовали 0 шаблонов
			return TRUE;
		}
		sFileContent+=DEFAULTTEMPLATE_END;
		TranslateTemplateAliases(sFileContent,wpCommonData);
		CString sOutDir=objSettings.sHTMLWrapperFile;
		if(!SaveFile(sOutDir,sFileContent)){
			return FALSE;
		}
		// Уведомляем заинтересованных лиц...
		if(!wpCommonData->bSuppressIntegration){
			CRegKey key;
			key.Open(HKEY_CURRENT_USER, PROG_REGKEY);
			if(key!=NULL){
				DWORD dwType=REG_SZ,dwSize=0;
				RegSetValueEx(key.m_hKey,REGWPPATH,0,REG_SZ,(BYTE*)(const char*)sOutDir,lstrlen(sOutDir)+1);
			}
			DWORD dwTarget=BSM_APPLICATIONS;
			BroadcastSystemMessage(BSF_QUERY | BSF_FORCEIFHUNG | BSF_IGNORECURRENTTASK | BSF_FLUSHDISK, &dwTarget, theApp.iWM_THIS, WPARAM(2), LPARAM(0));
		}
		// Стираем неиспользованное!
		if(!ReadFile(sOutDir,sFileContent)){
			return FALSE;
		}
		RemoveTemplateAliases(sFileContent);
		if(!SaveFile(sOutDir,sFileContent)){
			return FALSE;
		}
		bExpectedAD=1;
	}
	CSetWallPaper wpCommonData2;
	wpCommonData2.sInPreset=wpCommonData->sInPreset;
	wpCommonData2.bRegardMM=wpCommonData->bRegardMM;
	wpCommonData2.sADContent=wpCommonData->sADContent;
	wpCommonData2.bADContentInit=wpCommonData->bADContentInit;
	wpCommonData2.dwResamplingType=2;
	wpCommonData2.bSkipWrapperStep=TRUE;
	wpCommonData2.requiredSizeOfOutImage.CopyRect(wpCommonData->requiredSizeOfOutImage);
	wpCommonData2.sWallpaperFile=objSettings.sHTMLWrapperFile;
	wpCommonData2.sWallpaperThemeName=wpCommonData->sWallpaperThemeName;
	wpCommonData2.sizeOfRealImage=wpCommonData->sizeOfOutImage;
	wpCommonData2.TemplatesInWork.Copy(wpCommonData->TemplatesInWork);
	if(!PrepareWallPaper(&wpCommonData2)){
		return FALSE;
	}
	BOOL bRes=0;
	if(wpCommonData2.hbmOutImage){
		bRes=CopyBitmapToBitmap(wpCommonData2.hbmOutImage,wpCommonData->hbmOutImage,wpCommonData2.sizeOfOutImage);
	}
	return bRes;
}


int ActiveDesktopNeeded(CString& sADContent, BOOL* bRegItemsPresent=0,CSetWallPaper* wpCommonData=0)
{
	static CString sLastASC="";
	static CString sLastASCC="";
	static BOOL bRegItems=0;
	if(objSettings.bTemplatesInWorkChanged==0){
		sADContent=sLastASC;
	}else{
		objSettings.bTemplatesInWorkChanged=0;
		BOOL bADTempl=CallRenderTemplates(0,0,wpCommonData,&sADContent);
		sLastASC=sADContent;
	}
	if(bRegItemsPresent){
		*bRegItemsPresent=bRegItems;
	}
	int iAD=(sADContent!=""?1:0);
	return iAD;
}

int PrepareWallPaper(CSetWallPaper* wpCommonData)
{
	if(!wpCommonData){
		return FALSE;
	}
	if(objSettings.WPInProgress==TRUE){
		// Процесс идет, ресурсы заняты
		AddError(_l("Wallpaper not prepared yet. Please wait"));
		return FALSE;
	}
	DEBUG_INFO3("Generating wallpaper - start: %s",wpCommonData?wpCommonData->sWallpaperFile:"unknown",0,0);
	objSettings.bTemplatesInWorkChanged=1;
	DWORD dwStart=GetTickCount();
	objSettings.WPInProgress=TRUE;
	BOOL bWrapperActive=(wpCommonData->TemplatesInWork.GetSize()>0 && !wpCommonData->bSkipWrapperStep);
	wpCommonData->bWPType=IsFileHTML(wpCommonData->sWallpaperFile);
	int iRes=0;
	CString sADContent;
	BOOL bUnregClock=TRUE;
	int bActiDesk=ActiveDesktopNeeded(sADContent,&bUnregClock,wpCommonData);
	if(bUnregClock && !isScreenSaverActiveOrCLocked()){
		static DWORD dwLastTime=0;
		if(GetTickCount()-dwLastTime>20000 || dwLastTime==0){
			dwLastTime=GetTickCount();
			pMainDialogWindow->ShowBaloon(DEF_CURSELUNREG+"\n"+_l("Please, register "+AppName())+"!",_l(UNREG_FUNC_TITLE));
		}
	}
	if(!wpCommonData->bADContentInit){
		wpCommonData->sADContent=sADContent;
		wpCommonData->bADContentInit=1;
	}
	if(bActiDesk==2){
		// Просто фон
		PrepareWallpaperBg(wpCommonData);
	}else if(wpCommonData->bWPType && (objSettings.lConfModeIsON!=2)){
		PrepareHtmlWallpaper(wpCommonData);
	}else{
		PrepareImageWallpaper(wpCommonData);
	}
	if(wpCommonData->bTimedOut){
		// Показываем мессагу
		if(pMainDialogWindow && objSettings.lShowPopupsOnErr){
			pMainDialogWindow->m_STray.PopupBaloonTooltip(_l(AppName()+": Warning"),Format("%s (%lu %s)",_l(AppName()+" failed to generate\nnew wallpaper in reasonable time"),objSettings.iHTMLTimeout,_l("sec")));
		}
		objSettings.WPInProgress=FALSE;
		return FALSE;
	}
	if(bActiDesk!=2){
		iRes=FinishWallPaper(wpCommonData);
	}
	CSize scrSize=wpCommonData->requiredSizeOfOutImage.Size();
	if(scrSize.cx==0 && scrSize.cy==0){
		scrSize=GetMainMonitorRECTX().Size();
	}
	if(wpCommonData->sizeOfOutImage.cx<scrSize.cx && wpCommonData->sizeOfOutImage.cy<scrSize.cy){
		HBITMAP* pbmp=&wpCommonData->hbmOutImage;
		MakeImageCobble(pbmp,scrSize,objSettings.bTileOrCenter);
		wpCommonData->hbmOutImage=*pbmp;
		wpCommonData->sizeOfOutImage=scrSize;
	}
	objSettings.WPInProgress=FALSE;
	if(bWrapperActive){
		if(SaveBitmapToDisk(wpCommonData->hbmOutImage,getFileFullName(DEFAULT_HTMLBG))){
			iRes=ProcessWallpaperForeground(wpCommonData);
		}
	}
	if(!wpCommonData->bSkipWrapperStep || wpCommonData->bJustNotes){
		PostFilterWallpaper(wpCommonData,objSettings.bTileOrCenter);
	}
	DEBUG_INFO3("Generating wallpaper - finish: %s",wpCommonData?wpCommonData->sWallpaperFile:"unknown",0,0);
	DWORD dwStop=GetTickCount();
	return iRes;
}

void ShowWarningAboutMissedFile(const char* szFile)
{
	SmartWarning(_l("Failed to load image")+"\n'"+TrimMessage(szFile,50,2)+"'",_l("Broken image link"),&objSettings.bReportBrokenLinks,DEF_WARNTIME);
}

int PrepareWallpaperBg(CSetWallPaper* wpCommonData)
{
	wpCommonData->sizeOfRealImage=wpCommonData->sizeOfOutImage;
	ClearBitmap(wpCommonData->hbmRealImage);
	CDesktopDC dcDesk;
	wpCommonData->hbmOutImage = CreateCompatibleBitmap(dcDesk, wpCommonData->sizeOfOutImage.cx, wpCommonData->sizeOfOutImage.cy);
	CDC dcCompatible,dcCompatibleOrig;
	dcCompatible.CreateCompatibleDC(&dcDesk);
	HGDIOBJ hTmpBmp0=dcCompatible.SelectObject(wpCommonData->hbmOutImage);
	CBrush bgBrush(RGB(0x03,0x05,0x01));//COLORREF(objSettings.lWPBGColor)
	dcCompatible.FillRect(CRect(CPoint(0,0),wpCommonData->sizeOfOutImage),&bgBrush);
	dcCompatible.SelectObject(hTmpBmp0);
	return 0;
}

int PrepareImageWallpaper(CSetWallPaper* wpCommonData)
{
	CBitmap* bLoaded=_bmp().LoadBmpFromPath(wpCommonData->sWallpaperFile);
	if(!bLoaded){
		ShowWarningAboutMissedFile(wpCommonData->sWallpaperFile);
		return FALSE;
	}
	wpCommonData->sizeOfRealImage=GetBitmapSize(bLoaded);
	ClearBitmap(wpCommonData->hbmRealImage);
	wpCommonData->hbmRealImage=HBITMAP(*bLoaded);
	bLoaded->Detach();
	delete bLoaded;
	return TRUE;
}

int PrepareHtmlWallpaper(CSetWallPaper* wpCommonData)
{
	// Переинициализируем эксплорер
	BOOL bRes=FALSE;
	{
		BOOL bWasTimedOut=FALSE;
		// Удаляем браузер, если есть и нужно
		if(!objSettings.lReuseBrowser && objSettings.browser!=NULL){
			delete objSettings.browser;
			objSettings.browser=NULL;
		}
		// Создаем новый если нужно
		if(objSettings.browser==NULL){
			objSettings.browser=new CSmartWBrowser();
		}
		if(objSettings.browser){
			bRes=objSettings.browser->LoadPage(wpCommonData->sWallpaperFile,objSettings.iHTMLTimeout*1000,&bWasTimedOut);
			if(bRes){
				// Рисуем в битмап...	
				CRect rtDeskRect=theApp.GetDesktopRECT();
				if(wpCommonData && !wpCommonData->requiredSizeOfOutImage.IsRectEmpty()){
					rtDeskRect.CopyRect(wpCommonData->requiredSizeOfOutImage);
				}
				wpCommonData->sizeOfRealImage.cx=rtDeskRect.Width();
				wpCommonData->sizeOfRealImage.cy=rtDeskRect.Height();
				objSettings.browser->CopyPageImage(wpCommonData->hbmRealImage, rtDeskRect.Size());
				if(bWasTimedOut){
					// Был таймаут. Убиваем эксплорер - не нужен он нам
					if(objSettings.browser){
						delete objSettings.browser;
						objSettings.browser=NULL;
					}
					// Картинку отключаем
					wpCommonData->bTimedOut=TRUE;
				}
			}
		}
		// Удаляем браузер, если есть и нужно
		if(!objSettings.lReuseBrowser && objSettings.browser!=NULL){
			delete objSettings.browser;
			objSettings.browser=NULL;
		}
	}
	//CoFreeUnusedLibraries();
	return bRes;
}

int wrapToB_old(int iWhat, int iWithm, int iDir)
{
	iWithm-=1;
	if(iWhat>=iWithm){
		if(int(iWhat/iWithm)%2){
			return iWithm-(iWhat%iWithm);
		}else{
			return (iWhat%iWithm);
		}
	}
	return iWhat;
}

long lRandomImgBrs=0;
long lRandomImgBrsMax=0;
CSinusoid* snWraps=0;
int wrapToB(int iWhat, int iWithm, int iDir)
{
	if(!objSettings.bDotajkaRandImgBrd){
		return wrapToB_old(iWhat, iWithm, iDir);
	}
	if(snWraps){
		return wrapToB_old(iWhat+10*int(snWraps->Get1D(iWhat/double(iWithm))), iWithm, iDir);
	}
	lRandomImgBrs=abs(lRandomImgBrs);
	if(lRandomImgBrs==0){
		lRandomImgBrs=1;
	}
	if(lRandomImgBrs>lRandomImgBrsMax){
		lRandomImgBrs=lRandomImgBrsMax;
	}
	if(iWhat>=lRandomImgBrs){
		if(int(iWhat/lRandomImgBrs)%2){
			return lRandomImgBrs-(iWhat%lRandomImgBrs);//iWithm
		}else{
			return (iWhat%lRandomImgBrs);
		}
	}
	return iWhat;
}

#ifndef SM_CMONITORS
DECLARE_HANDLE(HMONITOR);
#endif
#define EFFRATE_3D	1

#ifdef _DEBUG
bool fakemulti=false;
#else
bool fakemulti=false;
#endif

BOOL bMonitorsRequested=0;
CArray<CRect, CRect&> monitors;
CRect rtMonitorsArea(0,0,0,0);
CRect rtFullDesktopRect(0,0,0,0);
CRect GetMonitorRect(int iNum);
CRect& GetMainMonitorRECTX()
{
	if(rtFullDesktopRect.IsRectEmpty()){
		GetMonitorRect(0);//GetMainMonitorRECT();
	}
	DEBUG_INFO4(">>>1>>> Virtual screen: %i:%i-%i:%i",rtFullDesktopRect.left,rtFullDesktopRect.top,rtFullDesktopRect.right,rtFullDesktopRect.bottom);
	return rtFullDesktopRect;
}

BOOL CALLBACK EnumMonitorCallback(HMONITOR,HDC,LPRECT rc,LPARAM)
{ 
	CRect rt(*rc);
	if (rc->left==0 && rc->top==0){
		monitors.InsertAt(0,rt);
	}
	else monitors.Add(rt);
	DEBUG_INFO4(">>>1>>> Found monitor: %i:%i-%i:%i",rt.left,rt.top,rt.right,rt.bottom);
	return TRUE;
}

CRect GetMonitorRect(int iNum)
{
	if(bMonitorsRequested==0){
		bMonitorsRequested=1;
		int num_monitors=GetSystemMetrics(80);
		if (fakemulti){
			CRect rc;
			//int w=GetSystemMetrics(SM_CXSCREEN), x1=w/4, x2=w*2/3, h=x2-x1; 
			rc.left=0; rc.top=0; rc.right=600; rc.bottom=768; monitors.Add(rc);
			rc.left=-424; rc.top=00; rc.right=0; rc.bottom=800; monitors.Add(rc);
		}else{ 
			if (num_monitors>1){ 
				typedef BOOL (CALLBACK *LUMONITORENUMPROC)(HMONITOR,HDC,LPRECT,LPARAM);
				typedef BOOL (WINAPI *LUENUMDISPLAYMONITORS)(HDC,LPCRECT,LUMONITORENUMPROC,LPARAM);
				HINSTANCE husr=LoadLibrary(_T("user32.dll"));
				LUENUMDISPLAYMONITORS pEnumDisplayMonitors=0;
				if (husr!=NULL) pEnumDisplayMonitors=(LUENUMDISPLAYMONITORS)GetProcAddress(husr,"EnumDisplayMonitors");
				if (pEnumDisplayMonitors!=NULL) (*pEnumDisplayMonitors)(NULL,NULL,EnumMonitorCallback,NULL);
				if (husr!=NULL) FreeLibrary(husr);
			}
			if (monitors.GetSize()==0){ 
				CRect rc; rc.left=0; rc.top=0; rc.right=GetSystemMetrics(SM_CXSCREEN); rc.bottom=GetSystemMetrics(SM_CYSCREEN);
				monitors.Add(rc);
			}
		}
		int i=0;
		// Двигаем отрицательные мониторы за пределы нулевого
		for(i=1;i<monitors.GetSize();i++){
			if(monitors[i].left<0){
				monitors[i].OffsetRect(monitors[i].Width()+monitors[0].Width(),0);
			}
			if(monitors[i].top<0){
				monitors[i].OffsetRect(0,monitors[i].Height()+monitors[0].Height());
			}
		}
		rtMonitorsArea.SetRectEmpty();
		for(i=0;i<monitors.GetSize();i++){
			CRect rt(monitors[i]);
			if(rt.left<rtMonitorsArea.left){
				rtMonitorsArea.left=rt.left;
			}
			if(rt.top<rtMonitorsArea.top){
				rtMonitorsArea.top=rt.top;
			}
			if(rt.right>rtMonitorsArea.right){
				rtMonitorsArea.right=rt.right;
			}
			if(rt.bottom>rtMonitorsArea.bottom){
				rtMonitorsArea.bottom=rt.bottom;
			}
		}
		rtFullDesktopRect.CopyRect(&rtMonitorsArea);
	}
	return monitors[iNum];
}

int GetNumberOfMons()
{
	GetMonitorRect(0);
	return monitors.GetSize();
}

int FinishWallPaper(CSetWallPaper* wpCommonData)
{
	int iRes=0;
	if(GetNumberOfMons()<=1 || wpCommonData->bRegardMM==0){
		return FinishWallPaperX(wpCommonData, -1);
	}else{
		// Первым рисуем основную обоину!!!
		CString sFileTmp2=wpCommonData->sWallpaperFile;
		DWORD dwSamplingTmp2=wpCommonData->dwResamplingType;
		for(int i=0;i<GetNumberOfMons();i++){
			if(objSettings.iCurrentMultyOrderStyle>0){
				if(i>0){
					int iAnother=objSettings.m_WChanger.GetNextWPNumber(objSettings.iCurrentMultyOrderStyle-1);
					if(iAnother>=0 && iAnother<objSettings.m_WChanger.aImagesInRotation.GetSize()){
						CString sFile;
						CString sThemeName;
						DWORD dwSampling=2;
						objSettings.m_WChanger.GetWPTitleAndSampling(objSettings.m_WChanger.aImagesInRotation[iAnother], sFile, sThemeName, dwSampling);
						if(!IsFileHTML(sFile)){
							CString sFileTmp=wpCommonData->sWallpaperFile;
							wpCommonData->sWallpaperFile=sFile;
							wpCommonData->dwResamplingType=dwSampling;
							PrepareImageWallpaper(wpCommonData);
							wpCommonData->sWallpaperFile=sFileTmp;
						}
					}
				}/*else{
					wpCommonData->sWallpaperFile=sFileTmp2;
					wpCommonData->dwResamplingType=dwSamplingTmp2;
					PrepareImageWallpaper(wpCommonData);
				}*/
			}
			int iResTmp=FinishWallPaperX(wpCommonData, i);
			if(i==0){
				iRes=iResTmp;
			}
		}
	}
	return iRes;
}

int FinishWallPaperX(CSetWallPaper* wpCommonData, int iOnMonitor)
{
	BOOL bRes=TRUE;
	if(wpCommonData->hbmRealImage!=NULL){
		objSettings.sLastWallpaperFromWC=wpCommonData->sWallpaperFile;
		FLOG1("Finishing wallpaper %s",wpCommonData->sWallpaperFile);
		// Преобразуем к выходному размеру
		CRect rtTargetScreen(CPoint(0,0),wpCommonData->sizeOfOutImage);
		BOOL bPreserveImageSize=FALSE;
		BOOL bAllowReccomendedAdjust=0;
		if(iOnMonitor==-1){
			bAllowReccomendedAdjust=1;
			if(!wpCommonData->rcRecommendedWPPosition.IsRectEmpty()){
				rtTargetScreen.CopyRect(wpCommonData->rcRecommendedWPPosition);
			}else if(wpCommonData->bApplyAntiTaskbar){
				::SystemParametersInfo(SPI_GETWORKAREA,0,&rtTargetScreen,0);
				BOOL bForActiDesk=0;//IsActiveDesctopEnable()
				bForActiDesk=(wpCommonData->sADContent!="");
				if(bForActiDesk){
					rtTargetScreen.OffsetRect(CPoint(-rtTargetScreen.left,-rtTargetScreen.top));
				}
			}
		}else{
			bAllowReccomendedAdjust=0;
			// Страшно... мультимонитор wallpaper!
			rtTargetScreen.CopyRect(&GetMonitorRect(iOnMonitor));
			if(iOnMonitor==0){
				// Тут это учтем....
				bAllowReccomendedAdjust=1;
				if(!wpCommonData->rcRecommendedWPPosition.IsRectEmpty()){
					rtTargetScreen.CopyRect(wpCommonData->rcRecommendedWPPosition);
				}
			}
		}
		DEBUG_INFO4(">>>1>>> Drawing in: %i:%i-%i:%i",rtTargetScreen.left,rtTargetScreen.top,rtTargetScreen.right,rtTargetScreen.bottom);
		CRect rcBounds(CPoint(0,0),wpCommonData->sizeOfRealImage);
		CRect rcBoundsToDrawIn(rcBounds);
		if(wpCommonData->dwResamplingType>0){
			rcBoundsToDrawIn.CopyRect(rtTargetScreen);
			if(wpCommonData->dwResamplingType!=1){
				if(wpCommonData->dwResamplingType>=2){
					double dX=double(rtTargetScreen.Width())/double(wpCommonData->sizeOfRealImage.cx);
					double dY=double(rtTargetScreen.Height())/double(wpCommonData->sizeOfRealImage.cy);
					double dRatio=(dX>dY)?dY:dX;
					if(wpCommonData->dwResamplingType==2 
						|| (wpCommonData->dwResamplingType==3 
						&& ((dRatio>=1 && (dRatio-1)*100<objSettings.dwAutoStretchLim) || (dRatio<1 && (1-dRatio)*100<objSettings.dwAutoStretchLim)))){
						//Маштабируем
						rcBoundsToDrawIn.right=rcBoundsToDrawIn.left+long(wpCommonData->sizeOfRealImage.cx*dRatio);
						rcBoundsToDrawIn.bottom=rcBoundsToDrawIn.top+long(wpCommonData->sizeOfRealImage.cy*dRatio);
						// Центруем
						rcBoundsToDrawIn.OffsetRect((rtTargetScreen.Width()-rcBoundsToDrawIn.Width())/2,(rtTargetScreen.Height()-rcBoundsToDrawIn.Height())/2);
					}else if(wpCommonData->dwResamplingType==4){
						rcBoundsToDrawIn.right=rcBoundsToDrawIn.left+wpCommonData->sizeOfRealImage.cx;
						rcBoundsToDrawIn.bottom=rcBoundsToDrawIn.top+wpCommonData->sizeOfRealImage.cy;
						// Центруем
						rcBoundsToDrawIn.OffsetRect((rtTargetScreen.Width()-rcBoundsToDrawIn.Width())/2,(rtTargetScreen.Height()-rcBoundsToDrawIn.Height())/2);
					}else{
						bPreserveImageSize=TRUE;
					}
				}else{
					bPreserveImageSize=TRUE;
				}
			}
		}else{
			bPreserveImageSize=TRUE;
		}
		if(bPreserveImageSize){
			// Мостим в заданыне размеры!
			rcBoundsToDrawIn.CopyRect(rtTargetScreen);
			HBITMAP* pbmp=&wpCommonData->hbmRealImage;
			MakeImageCobble(pbmp,rcBoundsToDrawIn.Size(),1);
			wpCommonData->sizeOfRealImage=GetBitmapSize(wpCommonData->hbmRealImage);
			bPreserveImageSize=FALSE;
		}
		{// Теперь в rcBoundsToDrawIn - куда надо врисовать картинку
			CDesktopDC dcDesk;
			CSize sizeOfOutImage=wpCommonData->sizeOfOutImage;
			// делаем то,что нужно лишь на нулевой
			if(bAllowReccomendedAdjust){
				if(bPreserveImageSize){
					rtTargetScreen.CopyRect(CRect(CPoint(0,0),wpCommonData->sizeOfOutImage));
				}
				wpCommonData->rcOutScreenOnRect=rcBoundsToDrawIn;
				ClearBitmap(wpCommonData->hbmOutImage);
				wpCommonData->hbmOutImage = CreateCompatibleBitmap(dcDesk, sizeOfOutImage.cx, sizeOfOutImage.cy);
			}
			CDC dcCompatible,dcCompatibleOrig;
			dcCompatible.CreateCompatibleDC(&dcDesk);
			dcCompatibleOrig.CreateCompatibleDC(&dcDesk);
			HGDIOBJ hTmpBmp0=dcCompatible.SelectObject(wpCommonData->hbmOutImage);
			if(bAllowReccomendedAdjust){
				CBrush bgBrush(COLORREF(objSettings.lWPBGColor));
				dcCompatible.FillRect(CRect(CPoint(0,0),sizeOfOutImage),&bgBrush);
			}
			HGDIOBJ hTmpBmp2=dcCompatibleOrig.SelectObject(wpCommonData->hbmRealImage);
			dcCompatible.SetStretchBltMode(HALFTONE);
			DWORD dwPostProcessing=objSettings.bFineStretching;
			if(dwPostProcessing==1){
				HBITMAP hbmOutputBitmapScaled = ScaleBitmapInt(wpCommonData->hbmRealImage,rcBoundsToDrawIn.Width(), rcBoundsToDrawIn.Height());
				if(hbmOutputBitmapScaled==0){// %( не получилось....
					dwPostProcessing=0;
				}else{
					CDC dcCompatibleScaled;
					dcCompatibleScaled.CreateCompatibleDC(&dcDesk);
					HGDIOBJ hTmpBmp=dcCompatibleScaled.SelectObject(hbmOutputBitmapScaled);
					dcCompatible.BitBlt(rcBoundsToDrawIn.left, rcBoundsToDrawIn.top, rcBoundsToDrawIn.Width(), rcBoundsToDrawIn.Height(), &dcCompatibleScaled, 0, 0, SRCCOPY);
					dcCompatibleScaled.SelectObject(hTmpBmp);
					ClearBitmap(hbmOutputBitmapScaled);
				}
			}
			if(dwPostProcessing==0 || wpCommonData->bSkipWrapperStep){
				// Нормальная или после генерации шаблона
				// Т.е. обычная растянутая под размер
				if(rcBoundsToDrawIn==CRect(0,0,rcBounds.Width(), rcBounds.Height())){
					dcCompatible.BitBlt(rcBoundsToDrawIn.left, rcBoundsToDrawIn.top, rcBoundsToDrawIn.Width(), rcBoundsToDrawIn.Height(), &dcCompatibleOrig, 0, 0, SRCCOPY);
				}else{
					dcCompatible.StretchBlt(rcBoundsToDrawIn.left, rcBoundsToDrawIn.top, rcBoundsToDrawIn.Width(), rcBoundsToDrawIn.Height(), &dcCompatibleOrig, 0, 0, rcBounds.Width(), rcBounds.Height(), SRCCOPY);
				}
			}
			if(objSettings.bDotajkaImages>0){// Расширяем концы картинки если ее не хватает
				BLENDFUNCTION blur;
				blur.BlendFlags=0;
				blur.AlphaFormat=0;
				blur.BlendOp=AC_SRC_OVER;
				blur.SourceConstantAlpha=125;
				long lOffsetPrev=0;
				const long lEdgeStep=1;
				double dOffsetDiv=EFFRATE_3D;
				dcCompatible.SetStretchBltMode(COLORONCOLOR);
				if(rcBoundsToDrawIn.Width()<rtTargetScreen.Width() || rcBoundsToDrawIn.Height()<rtTargetScreen.Height()){
					CRgn rgView;
					rgView.CreateRectRgn(rtTargetScreen.left,rtTargetScreen.top,rtTargetScreen.right,rtTargetScreen.bottom);
					lRandomImgBrsMax=min(rcBoundsToDrawIn.Width(),rcBoundsToDrawIn.Height());
					lRandomImgBrs=rnd(3,lRandomImgBrsMax-3);
					//snWraps = new CSinusoid(lRandomImgBrs);
					if(objSettings.bDotajkaRand<2){
						// Шуруем края...
						int i=0;
						double dwDotajkaW=(rtTargetScreen.Width()-rcBoundsToDrawIn.Width())/2;
						double dwDotajkaH=(rtTargetScreen.Height()-rcBoundsToDrawIn.Height())/2;
						if(dwDotajkaH<2 || dwDotajkaW<2){
							dOffsetDiv=4;
						}else{
							dOffsetDiv=double(dwDotajkaH)/double(dwDotajkaW);
						}
						for(i=0;i<=dwDotajkaW;i=i+lEdgeStep){
							long lOffset=long(double((objSettings.bDotajkaImages>2)?dwDotajkaW-i:0)*dOffsetDiv);
							dcCompatible.SelectClipRgn(&rgView);
							if(objSettings.bDotajkaRand==1 || objSettings.bDotajkaRand==3){
								if(dwDotajkaH>2 || dwDotajkaW>2){
									CRgn rg;
									rg.CreateRectRgn(rtTargetScreen.left+i, 
										rcBoundsToDrawIn.top-lOffset,
										rtTargetScreen.left+rtTargetScreen.Width()-i-lEdgeStep+lEdgeStep,
										rcBoundsToDrawIn.top-lOffset+rcBoundsToDrawIn.Height()+2*lOffset+lEdgeStep);
									rg.CombineRgn(&rg,&rgView,RGN_AND);
									dcCompatible.SelectClipRgn(&rg);
								}
								lOffset+=long(double(rnd(0,(objSettings.bDotajkaImages>2)?lOffset:int(dwDotajkaW/10)))*dOffsetDiv);
							}
							dcCompatible.StretchBlt(rtTargetScreen.left+i,
								rcBoundsToDrawIn.top-lOffset, lEdgeStep, rcBoundsToDrawIn.Height()+2*lOffset, &dcCompatible, rcBoundsToDrawIn.left+((objSettings.bDotajkaImages%2)==0?wrapToB(int(dwDotajkaW-i),rcBoundsToDrawIn.Width(),0):0), rcBoundsToDrawIn.top, lEdgeStep, rcBoundsToDrawIn.Height(), SRCCOPY);
							dcCompatible.StretchBlt(rtTargetScreen.left+rtTargetScreen.Width()-i-lEdgeStep,
								rcBoundsToDrawIn.top-lOffset, lEdgeStep, rcBoundsToDrawIn.Height()+2*lOffset, &dcCompatible, rcBoundsToDrawIn.right-lEdgeStep-((objSettings.bDotajkaImages%2)==0?wrapToB(int(dwDotajkaW-i),rcBoundsToDrawIn.Width(),1):0), rcBoundsToDrawIn.top, lEdgeStep, rcBoundsToDrawIn.Height(), SRCCOPY);
							if(objSettings.bDotajkaRandImgBrd && rnd(0,100)<30){
								lRandomImgBrs+=rnd(-1,1);
							}
						}
						if(dwDotajkaH<2 || dwDotajkaW<2){
							dOffsetDiv=4;
						}else{
							dOffsetDiv=double(dwDotajkaW)/double(dwDotajkaH);
						}
						for(i=0;i<=dwDotajkaH;i=i+lEdgeStep){
							long lOffset=long(double((objSettings.bDotajkaImages>2)?dwDotajkaH-i:0)*dOffsetDiv);
							dcCompatible.SelectClipRgn(&rgView);
							if(objSettings.bDotajkaRand==1 || objSettings.bDotajkaRand==3){
								if(dwDotajkaH>2 || dwDotajkaW>2){
									CRgn rg;
									rg.CreateRectRgn(rcBoundsToDrawIn.left-lOffset, 
										rtTargetScreen.top+i, 
										rcBoundsToDrawIn.left-lOffset+rcBoundsToDrawIn.Width()+2*lOffset+lEdgeStep,
										rtTargetScreen.top+rtTargetScreen.Height()-i-lEdgeStep+lEdgeStep);
									rg.CombineRgn(&rg,&rgView,RGN_AND);
									dcCompatible.SelectClipRgn(&rg);
								}
								lOffset+=long(double(rnd(0,(objSettings.bDotajkaImages>2)?lOffset:int(dwDotajkaH/10)))*dOffsetDiv);
							}
							dcCompatible.StretchBlt(rcBoundsToDrawIn.left-lOffset-1, 
								rtTargetScreen.top+i, rcBoundsToDrawIn.Width()+2*lOffset+3, lEdgeStep, &dcCompatible, rcBoundsToDrawIn.left, rcBoundsToDrawIn.top+(((objSettings.bDotajkaImages%2)==0)?wrapToB(int(dwDotajkaH-i),rcBoundsToDrawIn.Height(),0):0),  rcBoundsToDrawIn.Width(), lEdgeStep, SRCCOPY);
							dcCompatible.StretchBlt(rcBoundsToDrawIn.left-lOffset-1, 
								rtTargetScreen.top+rtTargetScreen.Height()-i-lEdgeStep, rcBoundsToDrawIn.Width()+2*lOffset+3, lEdgeStep, &dcCompatible, rcBoundsToDrawIn.left, rcBoundsToDrawIn.bottom-lEdgeStep-(((objSettings.bDotajkaImages%2)==0)?wrapToB(int(dwDotajkaH-i),rcBoundsToDrawIn.Height(),1):0), rcBoundsToDrawIn.Width(), lEdgeStep, SRCCOPY);
							if(objSettings.bDotajkaRandImgBrd && rnd(0,100)<30){
								lRandomImgBrs+=rnd(-1,1);
							}
						}
					}
					if(objSettings.bDotajkaRand>=2){
						BLENDFUNCTION blendFunction;
						blendFunction.BlendFlags=0;
						blendFunction.AlphaFormat=0;
						blendFunction.BlendOp=AC_SRC_OVER;
						blendFunction.SourceConstantAlpha=0;
						int i=0;
						long dwDotajkaW=(rtTargetScreen.Width()-rcBoundsToDrawIn.Width())/2;
						long dwDotajkaH=(rtTargetScreen.Height()-rcBoundsToDrawIn.Height())/2;
						if(dwDotajkaH<2 || dwDotajkaW<2){
							dOffsetDiv=4;
						}else{
							dOffsetDiv=double(dwDotajkaH)/double(dwDotajkaW);
						}
						for(i=0;i<=dwDotajkaW;i=i+lEdgeStep){
							long lOffset=long(double((objSettings.bDotajkaImages>2)?dwDotajkaW-i:0)*dOffsetDiv);
							dcCompatible.SelectClipRgn(&rgView);
							if(objSettings.bDotajkaRand==1 || objSettings.bDotajkaRand==3){
								if(dwDotajkaH>2 || dwDotajkaW>2){
									CRgn rg;
									rg.CreateRectRgn(rtTargetScreen.left+i, rcBoundsToDrawIn.top-lOffset,
										rtTargetScreen.left+rtTargetScreen.Width()-i-lEdgeStep+lEdgeStep,
										rcBoundsToDrawIn.top-lOffset+rcBoundsToDrawIn.Height()+2*lOffset+lEdgeStep);
									rg.CombineRgn(&rg,&rgView,RGN_AND);
									dcCompatible.SelectClipRgn(&rg);
								}
								lOffset+=long(double(rnd(0,(objSettings.bDotajkaImages>2)?lOffset:dwDotajkaW/10))*dOffsetDiv);
							}
							blendFunction.SourceConstantAlpha=(int)(double(255)*double(i)/double((rtTargetScreen.Width()-rcBoundsToDrawIn.Width())/2));
							AlphaBlendMy(dcCompatible,
								rtTargetScreen.left+i, rcBoundsToDrawIn.top-lOffset, lEdgeStep, 2*lOffset+rcBoundsToDrawIn.Height(), dcCompatible, rcBoundsToDrawIn.left+(((objSettings.bDotajkaImages%2)==0)?wrapToB(dwDotajkaW-i,  rcBoundsToDrawIn.Width(),0):0), rcBoundsToDrawIn.top, lEdgeStep, rcBoundsToDrawIn.Height(), blendFunction);
							AlphaBlendMy(dcCompatible,
								rtTargetScreen.left+rtTargetScreen.Width()-i-lEdgeStep, rcBoundsToDrawIn.top-lOffset, lEdgeStep, 2*lOffset+rcBoundsToDrawIn.Height(), dcCompatible, rcBoundsToDrawIn.right-lEdgeStep-(((objSettings.bDotajkaImages%2)==0)?wrapToB(dwDotajkaW-i,  rcBoundsToDrawIn.Width(),1):0), rcBoundsToDrawIn.top, lEdgeStep, rcBoundsToDrawIn.Height(), blendFunction);
							if(objSettings.bDotajkaRandImgBrd && rnd(0,100)<30){
								lRandomImgBrs+=rnd(-1,1);
							}
						}
						if(dwDotajkaH<2 || dwDotajkaW<2){
							dOffsetDiv=4;
						}else{
							dOffsetDiv=double(dwDotajkaW)/double(dwDotajkaH);
						}
						for(i=0;i<=dwDotajkaH;i=i+lEdgeStep){
							long lOffset=long(double((objSettings.bDotajkaImages>2)?dwDotajkaH-i:0)*dOffsetDiv);
							dcCompatible.SelectClipRgn(&rgView);
							if(objSettings.bDotajkaRand==1 || objSettings.bDotajkaRand==3){
								if(dwDotajkaH>2 || dwDotajkaW>2){
									CRgn rg;
									rg.CreateRectRgn(rcBoundsToDrawIn.left-lOffset, 
										rtTargetScreen.top+i,
										rcBoundsToDrawIn.left-lOffset+rcBoundsToDrawIn.Width()+2*lOffset+lEdgeStep,
										rtTargetScreen.top+rtTargetScreen.Height()-i-lEdgeStep+lEdgeStep);
									rg.CombineRgn(&rg,&rgView,RGN_AND);
									dcCompatible.SelectClipRgn(&rg);
								}
								lOffset+=long(double(rnd(0,(objSettings.bDotajkaImages>2)?lOffset:dwDotajkaH/10))*dOffsetDiv);
							}
							blendFunction.SourceConstantAlpha=(int)(double(255)*double(i)/double((rtTargetScreen.Height()-rcBoundsToDrawIn.Height())/2));
							AlphaBlendMy(dcCompatible,rcBoundsToDrawIn.left-lOffset-1, 
								rtTargetScreen.top+i, 2*lOffset+rcBoundsToDrawIn.Width()+3, lEdgeStep, dcCompatible, rcBoundsToDrawIn.left, rcBoundsToDrawIn.top+(((objSettings.bDotajkaImages%2)==0)?wrapToB(dwDotajkaH-i, rcBoundsToDrawIn.Height(),0):0),  rcBoundsToDrawIn.Width(), lEdgeStep, blendFunction);
							AlphaBlendMy(dcCompatible,rcBoundsToDrawIn.left-lOffset-1, 
								rtTargetScreen.top+rtTargetScreen.Height()-i-lEdgeStep, 2*lOffset+rcBoundsToDrawIn.Width()+3, lEdgeStep, dcCompatible, rcBoundsToDrawIn.left, rcBoundsToDrawIn.bottom-lEdgeStep-(((objSettings.bDotajkaImages%2)==0)?wrapToB(dwDotajkaH-i, rcBoundsToDrawIn.Height(),1):0), rcBoundsToDrawIn.Width(), lEdgeStep, blendFunction);
							if(objSettings.bDotajkaRandImgBrd && rnd(0,100)<30){
								lRandomImgBrs+=rnd(-1,1);
							}
						}
					}
					delete snWraps;
					snWraps=0;
				}
				if(bAllowReccomendedAdjust){
					wpCommonData->rcRecommendedWPPosition.CopyRect(rtTargetScreen);
				}
			}
			//
			dcCompatible.SelectObject(hTmpBmp0);
			dcCompatibleOrig.SelectObject(hTmpBmp2);
		}
	}else{
		bRes=FALSE;
	}
	return bRes;
}

BOOL IsActiveDesctopEnable()
{
	// Is Active Desktop on or off?
	SHELLFLAGSTATE shfs;
	memset(&shfs,0,sizeof(shfs));
	SHGetSettings(&shfs,SSF_DESKTOPHTML);
	BOOL bADEnabled = shfs.fDesktopHTML;
	return bADEnabled!=0?1:0;
}

BOOL SetActiveDState(BOOL bEnabled)
{
	HRESULT hr=S_FALSE;
	LPACTIVEDESKTOP pIAD=NULL;
	if(!bEnabled){
		// Вырубаем "держать элементы рабстола"
		HKEY hKey=0;
		LONG lRet = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Internet Explorer\\Desktop\\Components",0,KEY_READ|KEY_WRITE, &hKey);
		if (lRet == ERROR_SUCCESS) {
			DWORD dwType=0;
			DWORD dwOld=0;
			DWORD lSize=sizeof(dwOld);
			lRet = RegQueryValueEx(hKey, "GeneralFlags",0, &dwType,(LPBYTE)&dwOld, &lSize);
			if (lRet == ERROR_SUCCESS) {
				DWORD dwNew=(dwOld&(~0x2));
				RegSetValueEx(hKey, "GeneralFlags",0, REG_DWORD,(const BYTE*)&dwNew, sizeof(dwNew));
			}
			RegCloseKey( hKey);
		}
	}
	CoInitialize(NULL);
	// Инициализируем интерфейс IActiveDesktop
	hr = CoCreateInstance (CLSID_ActiveDesktop,NULL,CLSCTX_INPROC_SERVER,IID_IActiveDesktop,(void**) &pIAD );
	if (SUCCEEDED(hr)){
		if(pIAD){
			COMPONENTSOPT opt;
			opt.dwSize = sizeof(opt);
			opt.fActiveDesktop = opt.fEnableComponents = bEnabled;
			pIAD->SetDesktopItemOptions(&opt,0);
			pIAD->ApplyChanges(AD_APPLY_ALL);//AD_APPLY_BUFFERED_REFRESH
		}
		pIAD->Release();
	}
	CoUninitialize();
	return SUCCEEDED(hr);
}

CString GetCurrentWallpaper(BOOL* bActiveDesktopOn)
{
	CString sRes=objSettings.sLastWallpaperFromUser;
	CoInitialize(NULL);
	HRESULT hr=S_FALSE;
	LPACTIVEDESKTOP pIAD=NULL;
	hr = CoCreateInstance (CLSID_ActiveDesktop,NULL,CLSCTX_INPROC_SERVER,IID_IActiveDesktop,(void**) &pIAD );
	if (SUCCEEDED(hr)){
		if(bActiveDesktopOn){
			/*WALLPAPEROPT wpo;
			memset(&wpo,0,sizeof(wpo));
			wpo.dwSize=sizeof(wpo);
			pIAD->GetWallpaperOptions(&wpo,0);*/
			*bActiveDesktopOn=IsActiveDesctopEnable();
		}
		USES_CONVERSION;
		WCHAR szWall[MAX_PATH];
		memset(szWall,0,sizeof(szWall));
		hr=pIAD->GetWallpaper(szWall,sizeof(szWall),0);
		sRes=W2A(szWall);
		pIAD->Release();
	}
	CoUninitialize();
	return sRes;
}

BOOL SetWallpaperIntoOS(const char* szFile, CString sADContent)
{
	CoInitialize(NULL);
	//---------------------------
	BOOL bRes=0;
	CString sWallpaperFile=szFile;
	BOOL bActiveDEnabled=IsActiveDesctopEnable();
	int bNeedActiveDesktop=(sADContent!="");
	if(!bNeedActiveDesktop){
		if(bActiveDEnabled){// Выключаем ActiveDesktop если он не нужен
			//SmartWarning(_l("Active Desktop will be disabled"),_l("Active desktop"),&objSettings.bReportActiveDesktopChange,DEF_WARNTIME);
			SetActiveDState(FALSE);
		}
		SystemParametersInfo(SPI_SETDESKWALLPAPER,0,(void*)(const char*)sWallpaperFile,SPIF_UPDATEINIFILE|SPIF_SENDWININICHANGE);
		bRes=TRUE;
	}
	if(!bRes){
		// Теперь собственно занимаемся самими обоями
		HRESULT hr=S_FALSE;
		LPACTIVEDESKTOP pIAD=NULL;
		// Инициализируем интерфейс IActiveDesktop
		hr = CoCreateInstance (CLSID_ActiveDesktop,NULL,CLSCTX_INPROC_SERVER,IID_IActiveDesktop,(void**) &pIAD );
		if (SUCCEEDED(hr)){
			if(pIAD){
				if(bNeedActiveDesktop && !bActiveDEnabled){// Включаем ActiveDesktop если он нужен
					//SetActiveDState(TRUE);
					COMPONENTSOPT opt;
					opt.dwSize = sizeof(opt);
					opt.fActiveDesktop = opt.fEnableComponents = TRUE;
					hr = pIAD->SetDesktopItemOptions(&opt,0);
					hr = pIAD->ApplyChanges(AD_APPLY_ALL);
					if(objSettings.bTrasparDeskIcons==2){
						SmartWarning(_l("Warning: Transparent icons with Active Desktop enabled can cause\nserious CPU hit on some systems. If this is happening\non your computer, please consider to disable desktop icons transparency"),AppName(),&objSettings.bReportTraHit,DEF_WARNTIME);
					}
				}
				// Active wrapper если нужно
				if(bNeedActiveDesktop){
					CString sOutDir=GetUserFolder()+DEFAULT_HTMLBGTEMPL;
					if(sADContent!=""){
						CString sFileContent;
						sFileContent="<HTML><BODY bgcolor='$=<BGCOLOR>' text='$=<FGCOLOR>'>";
						sFileContent+="</BODY></HTML>";
						// НУЖНО для замены БГ солора (>)
						SubstVariables_Simple(sFileContent,FALSE,0);
						CString sBodyWP;
						CString sBody=CDataXMLSaver::GetInstringPart("<BODY",">",sFileContent);
						TranslateTemplateAliases(sFileContent,NULL);
						sFileContent.Replace("</BODY",sADContent+"</BODY");
						MetaBodyProcessing(sFileContent,sWallpaperFile,sBodyWP);
						// Заполняем финальный BODY
						sFileContent.Replace(Format("<BODY%s>",sBody),Format("<BODY SCROLL='no' bgcolor='$=<BGCOLOR>' text='$=<FGCOLOR>' style='border-style: none;' background='%s'>",sBodyWP));
						SubstVariables_Simple(sFileContent,FALSE,0);
						sFileContent.Replace("<body","<BODY");
						sFileContent.Replace("</body","</BODY");
						// Все! вычищаем - и на конвеер
						RemoveTemplateAliases(sFileContent);
						SaveFile(sOutDir,sFileContent);
						sWallpaperFile=sOutDir;
						if(sFileContent.Find("D27CDB6E-AE6D-11cf-96B8-444553540000")!=-1){
							CheckFlashPlayer();
						}
					}
				}
				//SmartWarning(_l("Desktop objects requires Active Desktop to be enabled")+"!\n"+_l("Active Desktop will be enabled automatically"),_l("Active clocks"),&objSettings.bReportActiveDesktopChange,DEF_WARNTIME);
				USES_CONVERSION;
				WALLPAPEROPT wOpt;
				wOpt.dwSize=sizeof(wOpt);
				wOpt.dwStyle=(objSettings.bTileOrCenter==0)?WPSTYLE_CENTER:WPSTYLE_TILE;
				hr = pIAD->SetWallpaperOptions(&wOpt,0);
				// Тут, думаю, все говорит само за себя 
				hr=pIAD->SetWallpaper(A2W(sWallpaperFile),0);
				hr = pIAD->ApplyChanges(AD_APPLY_ALL);//AD_APPLY_BUFFERED_REFRESH
				hr = pIAD->Release();
				bRes=TRUE;
			}else{
				hr=S_FALSE;
			}
		}
	}
	CoUninitialize();
	objSettings.noWallpaperChangesYet=0;
	ApplyDeskIconsOptions();
	objSettings.noDesktopHookProcessing=0;
	return bRes;
}

void HideDeskIcons(BOOL bNewValue)
{
	if(pMainDialogWindow){
		pMainDialogWindow->OnShowHideIcons(bNewValue);
	}
}

class CIconShifter
{
public:
	BOOL bNeedRestore;
	CIconShifter()
	{
		bNeedRestore=TRUE;
		if(pMainDialogWindow){
			pMainDialogWindow->StartAnimation();
		}
	}
	~CIconShifter()
	{
		if(bNeedRestore){
			if(pMainDialogWindow){
				pMainDialogWindow->StopAnimation(NULL);
			}
		}
	}
};

BOOL RefreshWallpaperFileName()
{
	// Определяем куда врисовывать картинку
	if(objSettings.sBMPFileName.GetLength()==0){
		if(strchr(objSettings.sWallpaperFile,'\\')==NULL){
			objSettings.sBMPFileName=GetUserFolder()+objSettings.sWallpaperFile;
		}else{
			objSettings.sBMPFileName=objSettings.sWallpaperFile;
		}
	}
	return TRUE;
}


DWORD WINAPI AskToExchangeWallpaper(LPVOID aData)
{
	if(InetIsOffline(0)){
		return 0;
	}
	if(!AddDefsInteract()){
		return 0;
	}
	CString sContest=_l("It is free and can greatly enrich your collection");
	if(objSettings.iLikStatus<2){
		sContest=DEFCONTES;
	}
	DWORD dwReply=AskYesNo(_l("Do you want to exhange your current wallpaper","Do you want to exhange one of your images for one of the public wallpapers from our web-site")+"?\n"+sContest+"!",_l("Wallpaper exchange"),&objSettings.lOnWallExchange,NULL);
	if(dwReply==IDYES){
		objSettings.lOnWallExchange=0;
		if(pMainDialogWindow){
			pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_EXCHANGEWALLPAPER,0);
		}
	}
	return 0;
}

BOOL SetWallPaper(const char* szFile, long dwFilterType, const char* szThemeName, const char* szPreset)
{
 	CSetWallPaper wpCommonData;
	wpCommonData.bRegardMM=1;
	wpCommonData.sInPreset=objSettings.sActivePreset;
	wpCommonData.sWallpaperFile=szFile;
	wpCommonData.sWallpaperThemeName=szThemeName;
	wpCommonData.bApplyAntiTaskbar=objSettings.lExcludeTaskbar;
	if(!szPreset || szPreset[0]==0){
		wpCommonData.TemplatesInWork.Copy(objSettings.TemplatesInWork);
	}else{
		wpCommonData.InitForPreset(szPreset);
	}
	if(dwFilterType<0){
		wpCommonData.bJustNotes=TRUE;
		wpCommonData.bSkipWrapperStep=TRUE;
		wpCommonData.dwResamplingType=(dwFilterType==-1?0:1);
	}else{
		wpCommonData.dwResamplingType=dwFilterType;
	}
	if(objSettings.lConfModeIsON==2){
		wpCommonData.bJustNotes=0;
		wpCommonData.bSkipWrapperStep=1;
		wpCommonData.bSkipWrapperGeneration=1;
		wpCommonData.bSuppressIntegration=1;
	}
	{// Начинаем генерацию обоев...
		CIconShifter objShifter;
		// Инициализируем рандомы...
		objSettings.lUseGlobalRandomWPTLevel=1;
		for(int i=0;i<wpCommonData.TemplatesInWork.GetSize();i++){
			CWPT* wpt=GetWPT(wpCommonData.TemplatesInWork[i]);
			if(!wpt){
				continue;
			}
			wpt->dwWeTmp=rnd(0,99);
		}
		BOOL bRes=PrepareWallPaper(&wpCommonData);
		if(bRes){
			RefreshWallpaperFileName();
			CString sFileExt=objSettings.sBMPFileName.Right(4);
			sFileExt.MakeLower();
			if(sFileExt!=".bmp"){
				objSettings.sBMPFileName+=".bmp";
			}
			DeleteFile(objSettings.sBMPFileName+".pre");
			MoveFile(objSettings.sBMPFileName,objSettings.sBMPFileName+".pre");
			if(SaveBitmapToDisk(wpCommonData.hbmOutImage,objSettings.sBMPFileName)){
				// Сохраняем в реестр для дефолтного юзера если получится
				// Сохраняем также для обычного, чтобы отловился факт смены обоины для установки фона иконок
				SetWallpaperFileInReg(objSettings.sBMPFileName, TRUE);
				SetWallpaperIntoOS(objSettings.sBMPFileName, wpCommonData.sADContent);
			}
			pMainDialogWindow->PostMessage(DESKTOP_WALLC2,0,0);
			objShifter.bNeedRestore=FALSE;
			objSettings.lChangesCount++;
			if(AddDefsInteract()){
				if(objSettings.lChangesCount%100==99 && objSettings.lConfModeIsON!=2)
				{
					FORK(AskToExchangeWallpaper,0);
				}
			}
		}
	}
	if(pMainDialogWindow){
		pMainDialogWindow->StopAnimation((objSettings.bShowTrayIconAsWP==1)?wpCommonData.hbmOutImage:NULL);
	}
	return TRUE;
}

int getRndBmpFormat()
{
	switch(rnd(1,8))
	{
	case 1:
		return SRCPAINT;
		break;
	case 2:
		return SRCAND;
		break;
	case 3:
		return SRCINVERT;
		break;
	case 4:
		return SRCERASE;
		break;
	case 5:
		return NOTSRCCOPY;
		break;
	case 6:
		return NOTSRCERASE;
		break;
	case 7:
		return MERGECOPY;
		break;
	default:
		return MERGEPAINT;
	}
	return SRCCOPY;
}

BOOL IsWPTActive(const char* szWPT)
{
	for (int i=0;i<objSettings.TemplatesInWork.GetSize();i++){
		if(objSettings.TemplatesInWork[i]==szWPT){
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SwitchSingleTemplate(const char* szTemplate, BOOL bNotify)
{
	int i=0;
	objSettings.bTemplatesInWorkChanged=1;
	// Удаляем если уже есть...
	for(i=0;i<objSettings.TemplatesInWork.GetSize();i++){
		if(objSettings.TemplatesInWork[i]==szTemplate){
			objSettings.TemplatesInWork.RemoveAt(i);
			if(pMainDialogWindow && pMainDialogWindow->dlgWChanger){
				pMainDialogWindow->dlgWChanger->PostMessage(WM_COMMAND,WM_USER+REFRTEMPL,GetWPTIndex(szTemplate));
			}
			return FALSE;
		}
	}
	// А тут можно обломаться!
	if(objSettings.iLikStatus<2){
		if(objSettings.TemplatesInWork.GetSize()>=4){
			UnregAlert(OPTION_WIDG_UNREG);
#ifndef _DEBUG
			return FALSE;
#endif
		}
	}
	CString sPiece;
	CStringArray sToRemove;
	ReadFileWithConfig(GetWPTemplateFilePath(CString(szTemplate)+".wpt"),sPiece,objSettings.sActivePreset);
	CString sExclusiveTemplates=CDataXMLSaver::GetInstringPart("$=<AUTO-EXCLUDE:",">",sPiece);
	if(sExclusiveTemplates!=""){
		ConvertComboDataToArray(sExclusiveTemplates,sToRemove,',');
	}
	for(i=0;i<objSettings.TemplatesInWork.GetSize();i++){
		if(GetIndexInArray(objSettings.TemplatesInWork[i],&sToRemove)!=-1){
			objSettings.TemplatesInWork.RemoveAt(i);
			bNotify=1;
		}
	}
	sToRemove.RemoveAll();
	CString sExclusiveTemplatesCateg=CDataXMLSaver::GetInstringPart("$=<AUTO-EXCLUDE-INTERNAL-CATEG:",">",sPiece);
	if(sExclusiveTemplatesCateg!=""){
		ConvertComboDataToArray(sExclusiveTemplatesCateg,sToRemove,',');
	}
	for(i=0;i<objSettings.TemplatesInWork.GetSize();i++){
		CString sWPName=objSettings.TemplatesInWork[i];
		CWPT* wpt=GetWPT(sWPName);
		if(wpt && GetIndexInArray(wpt->sExcCateg,&sToRemove)!=-1){
			objSettings.TemplatesInWork.RemoveAt(i);
			bNotify=1;
		}
	}
	objSettings.TemplatesInWork.InsertAt(0,szTemplate);
	CWPT* wpt=GetWPT(szTemplate);
	if(wpt->GetWeight()==0){
		wpt->SetWeight(100);
	}
	if(pMainDialogWindow && pMainDialogWindow->dlgWChanger){
		pMainDialogWindow->dlgWChanger->PostMessage(WM_COMMAND,WM_USER+REFRTEMPL,LPARAM(bNotify?-1:GetWPTIndex(szTemplate)));
	}
	return TRUE;
}

DWORD WINAPI SetWPInThread(LPVOID lpData);
void SetWallpaperBySrc(CString sSrc)
{
	BOOL bOk=0;
	// Ищем индекс обоины...
	for(int iIndex=0;iIndex<objSettings.m_WChanger.aWPapersAndThemes.GetSize();iIndex++){
		WPaper* wp=objSettings.m_WChanger.aWPapersAndThemes[iIndex];
		if(wp->sTitle==sSrc){
			bOk=1;
			FORK(SetWPInThread,iIndex);
			break;
		}
	}
	if(!bOk){
		// Ставим сами...
		objSettings.sLastWallpaperFromUser=sSrc;
		pMainDialogWindow->OrderModifierForNextWallpaper=-2;
		pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
	}
}

void CallFileForOpenOrEdit(const char* szFile,BOOL bJustTooltip)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	objSettings.bTemplatesInWorkChanged=1;
	BOOL bUseNotepad=TRUE;
	CString sString=szFile;
	SubstVariables(sString,0);
	CString sTooltip=CDataXMLSaver::GetInstringPart("<TOOLTIP>","</TOOLTIP>",sString);
	if(bJustTooltip){
		if(sTooltip!="" && pMainDialogWindow){
			pMainDialogWindow->ShowDescTooltip(sTooltip);
		}
		return;
	}
	// Отрезаем...
	sString.Replace(Format("<TOOLTIP>%s</TOOLTIP>",sTooltip),"");
	sString.TrimLeft();
	sString.TrimRight();
	if(sString==""){
		return;
	}
	CString sFileToEdit="", sEditForKey, sEditOnDate;
	if(sString.Find("WP:GENERATE-FORM")!=-1){
		CString sSrc=CDataXMLSaver::GetInstringPart("HINTS-PARAM:(",")",sString);
		GenerateForm(sSrc,sString,1);
		return;
	}
	if(sString.Find("WP:")!=-1){
		if(pMainDialogWindow && sString.Find("WP:OPEN-CAL")!=-1){
			pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_OPEN2,0);
		}else if(pMainDialogWindow && sString.Find("WP:SET-WALLPAPER-NEXT")!=-1){
			pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_NEXTWALLP,0);
		}else if(pMainDialogWindow && sString.Find("WP:SET-WALLPAPER-PREV")!=-1){
			pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_PREVWALLP,0);
		}else if(pMainDialogWindow && sString.Find("WP:SET-WALLPAPER-RANDOM")!=-1){
			pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_RANDWALLP,0);
		}else if(pMainDialogWindow && sString.Find("WP:SWITCH-ONOFF")!=-1){
			objSettings.lChangePriodically=!objSettings.lChangePriodically;
			Alert(_l(objSettings.lChangePriodically?"Automatic change started":"Automatic change stopped")+"!",9000);
		}else if(pMainDialogWindow && sString.Find("WP:EXCHANGE-URL")!=-1){
			/*if(GetApplicationLang()==1){
				pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CLANCHMONEY,0);
			}else*/
			{
				pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_EXCHANGEWALLPAPER,0);
			}
		}else if(pMainDialogWindow && sString.Find("WP:ORDER-URL")!=-1){
			BuyWC(0,0);
		}else if(pMainDialogWindow && sString.Find("WP:SHOWOPTIONS")!=-1){
			CString sSrc=CDataXMLSaver::GetInstringPart("HINTS-PARAM:(",")",sString);
			pMainDialogWindow->PostMessage(OPEN_TEMPL_PARS,WPARAM(strdup(sSrc)),0);
		}else{
			CString sSrc=CDataXMLSaver::GetInstringPart("HINTS-PARAM:(",")",sString);
			if(sSrc!=""){
				SetWallpaperBySrc(sSrc);
			}
		}
		return;
	}
	SubstVariables_Simple(sString,FALSE,0);
	if(sString.Find("USE-NOTE-AS-EDITOR")!=-1){
		// Гипер редактирование
		sFileToEdit=CDataXMLSaver::GetInstringPart("EDIT-FILE:(",")",sString);
		sEditForKey=CDataXMLSaver::GetInstringPart("EDIT-KEY:(",")",sString);
		sEditOnDate=CDataXMLSaver::GetInstringPart("EDIT-ON-DATE:(",")",sString);
		bUseNotepad=FALSE;
	}else{
		sFileToEdit=sString;
	}
	CString sExternalEdit=CDataXMLSaver::GetInstringPart("Edit[","]",sFileToEdit);
	if(sExternalEdit=="" && !IsFileHTML(sFileToEdit) && !bJustTooltip){
		sFileToEdit=GetTextIncludeFilePath_forTempl(sFileToEdit);
		if(!isFileExist(sFileToEdit)){
			SaveFile(sFileToEdit,"");
		}
	}
	if(bUseNotepad){
		int iRes=0;
		if(sExternalEdit!=""){
			CString sDLL=sExternalEdit.SpanExcluding(":");
			CString sFuncAndFile=sExternalEdit.Mid(sDLL.GetLength()+1);
			CString sRFile=Format("%s"WP_TEMPLATE"\\%s",GetApplicationDir(),sDLL);
			CString sFunc=sFuncAndFile.SpanExcluding(";");
			if(sFuncAndFile.GetLength()>sFunc.GetLength()+1){
				sFileToEdit=sFuncAndFile.Mid(sFunc.GetLength()+1);
				sFileToEdit=GetTextIncludeFilePath_forTempl(sFileToEdit);
				if(!isFileExist(sFileToEdit)){
					SaveFile(sFileToEdit,"");
				}
			}else{
				sFuncAndFile="";
			}
			HINSTANCE hRender=LoadLibrary(sRFile);
			if(hRender){
				_GetApplicationLang fp0=(_GetApplicationLang)GetProcAddress(hRender,"GetApplicationLang");
				if(fp0){
					(*fp0)()=GetApplicationLang();
				}
				__bmpX fp1=(__bmpX)GetProcAddress(hRender,"_bmpX");
				if(fp1){
					(*fp1)()=&_bmp();
				}
				_EditDateFile fp=(_EditDateFile)GetProcAddress(hRender,sFunc);
				if(fp){
					char szTmp[1024]={0},szTmp2[1024]={0};
					iRes=(*fp)(pMainDialogWindow,sFileToEdit,pMainDialogWindow->pOpenedTemplate?pMainDialogWindow->pOpenedTemplate->GetSafeHwnd():pMainDialogWindow->GetSafeHwnd())?33:0;
				}
				FreeLibrary(hRender);
			}
		}else{
			iRes=(int)::ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,NULL,sFileToEdit,NULL,NULL,SW_SHOWMAXIMIZED);
			if(iRes<=32){
				iRes=(int)::ShellExecute(pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL,NULL,"notepad.exe",sFileToEdit,NULL,SW_SHOWMAXIMIZED);
			}
			if(iRes<=32){
				Alert(_l("Failed to open")+" '"+TrimMessage(sFileToEdit,20,3)+"'!");
			}
		}
	}else{
		static CWnd* pActiveEdit=0;
		if(pActiveEdit){
			SwitchToWindow(pActiveEdit->GetSafeHwnd(),TRUE);
		}
		if((objSettings.iLikStatus==0 || objSettings.iLikStatus>2) && GetWindowDaysFromInstall()>15){
			UnregAlert(OPTION_CALE_UNREG);
			return;
		}
		// Через заметку
		CString sTitle=CDataXMLSaver::GetInstringPart("TITLE:(",")",sString);
		CString sContent,sTextToEdit,sCustomIcon;
		ReadFile(sFileToEdit,sContent);
		DWORD dwKeyTxColor=0;
		DWORD dwKeyBgColor=0;
		if(sEditForKey!=""){
			BOOL bRes=TRUE, bXCall=0;
			CString sMaybeCall=CDataXMLSaver::GetInstringPart("Call[","]",sEditForKey);
			if(sMaybeCall==""){
				sMaybeCall=CDataXMLSaver::GetInstringPart("CallX[","]",sEditForKey);
				bXCall=1;
			}
			if(sMaybeCall!=""){
				CString sDLL=sMaybeCall.SpanExcluding(":");
				CString sFunc=sMaybeCall.Mid(sDLL.GetLength()+1);
				CString sRFile=Format("%s"WP_TEMPLATE"\\%s",GetApplicationDir(),sDLL);
				HINSTANCE hRender=LoadLibrary(sRFile);
				if(hRender){
					_GetApplicationLang fp0=(_GetApplicationLang)GetProcAddress(hRender,"GetApplicationLang");
					if(fp0){
						(*fp0)()=GetApplicationLang();
					}
					__bmpX fp1=(__bmpX)GetProcAddress(hRender,"_bmpX");
					if(fp1){
						(*fp1)()=&_bmp();
					}
					char szTmp[1024]={0},szTmp2[1024]={0},szTmp3[1024]={0};
					if(bXCall){
						_GetEditKeyX fp=(_GetEditKeyX)GetProcAddress(hRender,sFunc);
						if(fp){
							bRes=(*fp)(pMainDialogWindow,szTmp,sizeof(szTmp),szTmp2,sizeof(szTmp2),szTmp3,sizeof(szTmp3));
							sEditForKey=szTmp;
							sTitle=szTmp2;
							sEditOnDate=szTmp3;
						}
					}else{
						_GetEditKey fp=(_GetEditKey)GetProcAddress(hRender,sFunc);
						if(fp){
							bRes=(*fp)(pMainDialogWindow,szTmp,sizeof(szTmp),szTmp2,sizeof(szTmp2));
							sEditForKey=szTmp;
							sTitle=szTmp2;
						}
					}
					FreeLibrary(hRender);
				}
			}
			if(!bRes){
				return;
			}
			sContent.Replace("\r\n","\n");
			sContent=CString("\n")+sContent+"\n";
			CString sSingleLine=" ";
			while(sSingleLine!=""){
				sSingleLine=CDataXMLSaver::GetInstringPart(CString("\n")+sEditForKey,"\n",sContent);
				sContent.Replace(Format("%s%s\n",sEditForKey,sSingleLine),"");
				sTextToEdit+=sSingleLine+"\n";
			}
			sTextToEdit.TrimLeft();
			sTextToEdit.TrimRight();
			sTextToEdit.Replace("\r\n","\n");
			sTextToEdit.Replace("<br>","\n");
			// Системная инфа
			CString sSysInfo=CDataXMLSaver::GetInstringPart("[?CDATA{","}?DATAC:)]",sTextToEdit);
			if(sSysInfo!=""){
				dwKeyTxColor=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("FG:(",")",sSysInfo));
				dwKeyBgColor=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("BG:(",")",sSysInfo));
				sCustomIcon=CDataXMLSaver::GetInstringPart("ICON:(",")",sSysInfo);
				sTextToEdit.Replace(Format("[?CDATA{%s}?DATAC:)]",sSysInfo),"");
			}
		}else{
			sTextToEdit=sContent;
		}
		CNote nt;
		CDLG_Note dlg;
		pActiveEdit=&dlg;
		dlg.sNoteOnDate=sEditOnDate;
		dlg.bForceFocus=TRUE;
		nt.sTitle=sTitle;
		nt.SetRealTextFromNote(sTextToEdit);
		nt.sNoteFile=sEditForKey;
		if(sCustomIcon!=""){
			nt.sCustomIcon=sCustomIcon;
			nt.iIcon=-1;
		}else{
			nt.iIcon=999;
		}
		dlg.pNote=&nt;
		dlg.bNoFont=1;
		dlg.bJustEdit=1;
		dlg.bNoLayout=1;
		dlg.bNoBGImage=1;
		dlg.bNoChangeView=1;
		if(sString.Find("NO-COLOR-SELECT")!=-1){
			dlg.bNoChangeView=TRUE;
			dlg.bNoColorSelect=TRUE;
		}
		if(sString.Find("TITLE-DISABLED")!=-1){
			dlg.bTitleDisabled=TRUE;
		}
		if(sString.Find("NOSIZE")!=-1){
			dlg.bNoSize=TRUE;
		}
		if(sString.Find("NOMOVE")!=-1){
			dlg.bNoMove=TRUE;
		}
		if(sString.Find("FULL-WND-EDIT")!=-1){
			dlg.bJustEdit=2;
		}
		if(sString.Find("NO-ICON")!=-1){
			dlg.bNoChangeIcon=1;
		}else{
			if(objSettings.lVeryFirstStart){
				static BOOL bNotice=2;
				if(bNotice){
					bNotice=0;
					objSettings.bFirstJustEditNote=2;
				}
			}
		}
		nt.left=atol(CDataXMLSaver::GetInstringPart("X:(",")",sString));
		nt.top=atol(CDataXMLSaver::GetInstringPart("Y:(",")",sString));
		nt.right=nt.left+atol(CDataXMLSaver::GetInstringPart("W:(",")",sString));
		nt.bottom=nt.top+atol(CDataXMLSaver::GetInstringPart("H:(",")",sString));
		DWORD dwDefTxColor=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("FG:(",")",sString));
		DWORD dwDefBgColor=GetRGBFromHTMLString(CDataXMLSaver::GetInstringPart("BG:(",")",sString));
		BOOL bColorInfoWasPresent=0;
		if(dwKeyTxColor!=dwKeyBgColor){
			bColorInfoWasPresent=1;
			nt.dwTColor=dwKeyTxColor;
			nt.dwBColor=dwKeyBgColor;
		}else if(dwDefTxColor!=dwDefBgColor){
			nt.dwTColor=dwDefTxColor;
			nt.dwBColor=dwDefBgColor;
		}else{
			// Сами разберутся
			nt.dwTColor=0;
			nt.dwBColor=0;
		}
		int iRes=dlg.DoModal();
		DWORD dwErr=GetLastError();
		if(iRes!=IDOK){
			nt.SetRealTextFromNote("");
		}
		{//----------------------
			sTextToEdit=nt.GetRealTextFromNote();
			if(sEditForKey!=""){
				sTextToEdit.Replace("\r\n","\n");
				sTextToEdit.Replace("\n","<br>");
				CString sCData="";
				if(dlg.dwTX!=dlg.dwBG && (dlg.dwTX!=dwDefBgColor || dlg.dwBG!=dwDefTxColor)){
					sCData=Format("FG:(%lu),BG:(%lu)",dlg.dwTX,dlg.dwBG);
				}
				if(nt.sCustomIcon!=""){
					sCData+=Format("ICON:(%s)",nt.sCustomIcon);
				}else{
					if(_bmp().GetRaw(GetNSkinBmp(objSettings.lNoteSkin,"ICO",nt.iIcon))!=0){
						CString sFileNoExt=Format("%sicon%lu",objSettings.aNoteSkin[objSettings.lNoteSkin].sFolder,nt.iIcon);
						if(isFileExist(sFileNoExt+".gif")){
							sFileNoExt+=".gif";
							sCData+=Format("ICON:(%s)",sFileNoExt);
							_bmp().AddBmpFromDiskIfNotPresent(sFileNoExt,sFileNoExt+DEFNOTEPRF,DEFNOTEICONSIZE);
							_bmp().AddBmpFromDiskIfNotPresent(sFileNoExt,sFileNoExt+DEFNOTEPRFM,DEFNOTEICONSIZM);
						}
						if(isFileExist(sFileNoExt+".bmp")){
							sFileNoExt+=".bmp";
							sCData+=Format("ICON:(%s)",sFileNoExt);
							_bmp().AddBmpFromDiskIfNotPresent(sFileNoExt,sFileNoExt+DEFNOTEPRF,DEFNOTEICONSIZE);
							_bmp().AddBmpFromDiskIfNotPresent(sFileNoExt,sFileNoExt+DEFNOTEPRFM,DEFNOTEICONSIZM);
						}
						if(isFileExist(sFileNoExt+".jpg")){
							sFileNoExt+=".jpg";
							sCData+=Format("ICON:(%s)",sFileNoExt);
							_bmp().AddBmpFromDiskIfNotPresent(sFileNoExt,sFileNoExt+DEFNOTEPRF,DEFNOTEICONSIZE);
							_bmp().AddBmpFromDiskIfNotPresent(sFileNoExt,sFileNoExt+DEFNOTEPRFM,DEFNOTEICONSIZM);
						}
					}
				}
				if(nt.GetRealTextFromNote()==""){
					sCData="";
				}
				if(sCData!=""){
					sTextToEdit+=Format("[?CDATA{%s}?DATAC:)]",sCData);
				}
				sContent+=Format("%s%s\n",sEditForKey,sTextToEdit);
			}else{
				sContent=sTextToEdit;
			}
			
			CString sContentOk="";
			if(sEditForKey!=""){
				// Убиваем мусорные значения!!!
				while(sContent!=""){
					CString sLine=sContent.SpanExcluding("\n");
					if(sLine!=""){
						CStringArray aList;
						ConvertComboDataToArray(sLine,aList,'/');
						if(aList.GetSize()==3 && aList[2].GetLength()>5){
							CString sData=aList[2].Mid(5);
							CString sDovesok=CDataXMLSaver::GetInstringPart("[?CDATA{","}?DATAC:)]",sData);
							sData.Replace(Format("[?CDATA{%s}?DATAC:)]",sDovesok),"");
							sData.TrimLeft();
							sData.TrimRight();
							if(sData!="" && atol(aList[2])>0 && atol(aList[1])>0 && atol(aList[0])>0){
								COleDateTime dt(atol(aList[2]),atol(aList[1]),atol(aList[0]),0,0,0);
								if(dt.GetStatus()==COleDateTime::valid){
									sContentOk+="\n";
									sContentOk+=sLine;
									sContentOk.TrimRight();
								}
							}
						}
						if(sContent.GetLength()>sLine.GetLength()){
							sContent=sContent.Mid(sLine.GetLength()+1);
						}else{
							sContent="";
						}
					}
					sContent.TrimLeft();
				}
			}else{
				sContentOk=sContent;
			}
			sContentOk.TrimLeft();
			sContentOk.TrimRight();
			SaveFile(sFileToEdit,sContentOk);
		}
		pActiveEdit=0;
		if(pMainDialogWindow && sString.Find("REDRAW-AFTER-EDIT")!=-1){
			pMainDialogWindow->OrderModifierForNextWallpaper=-2;
			pMainDialogWindow->PostMessage(WM_COMMAND,ID_SYSTRAY_CHANGEWALLPAPER,0);
		}
	}
	if(sFileToEdit.Find(".txt")!=-1){
		// Финт ушами для очистки кешей при редактировании текстовых файлов
		objFileParser().ClearCashes();
	}
}

CString GetTextIncludeFilePath_forTempl(CString sFile)
{
	CString sRes;
	if(sFile.Find(PROFILE_PREFIX)==0){
		sFile=sFile.Mid(strlen(PROFILE_PREFIX));
		sRes=Format("%s%s",GetUserFolder(),sFile);
		CString sEtalon=Format("%s"WP_TEMPLATE"\\%s",GetApplicationDir(),sFile);
		if(!isFileExist(sRes) && isFileExist(sEtalon)){
			::CopyFile(sEtalon,sRes,TRUE);
		}
	}else{
		if(sFile.Find(":\\")==1 || sFile.Find(":/")==1){
			sRes=sFile;
		}else{
			sRes=Format("%s"WP_TEMPLATE"\\%s",GetApplicationDir(),sFile);
			if(!isFileExist(sRes) && !isFileExist(sRes+".wpt")){
				sRes=GetUserFolder()+WP_IMPORTS+"\\"+sFile;
			}
		}
	}
	return sRes;
}

CString GetWPTemplateFilePath(const char* szWPT)
{
	return GetTextIncludeFilePath_forTempl(szWPT);
}

CString GetHTMLConfigFile(const char* szFile, const char* szPreset)
{
	char szDisk[MAX_PATH]="C:\\", szPath[MAX_PATH]="", szName[MAX_PATH]="", szExt[MAX_PATH]="";
	_splitpath(szFile, szDisk, szPath, szName, szExt);
	CString sOut=szName;
	if(szExt[0]=='.' && strcmp(szExt,CONFIG_EXT)!=0){
		sOut+="_";
		sOut+=(const char*)(szExt+1);
	}
	sOut+=CONFIG_EXT;
	if(szPreset/*objSettings.sActivePreset*/!=""){
		CString sP=GetPresetsFolder(szPreset);
		sOut=sP+sOut;
	}
	return getFileFullName(sOut);
}


BOOL ReadFileWithConfig(CString sFile, CString& sContent, const char* szPreset, BOOL bPerformSimpleCalcs)
{
	CString sConfig;
	ReadFile(GetHTMLConfigFile(sFile,szPreset), sConfig);
	if(!ReadFile(sFile, sContent)){
		return FALSE;
	}
	sContent=sConfig+sContent;
	sContent.TrimLeft();
	// Нужно именно в контексте файла!
	sContent.Replace("{wpt_id}",GetHash(sFile));
	if(bPerformSimpleCalcs){
		SubstVariables_Simple(sContent,FALSE,0);
	}
	return (sContent!="");
}

CString GetDesktopPrefix()
{
	CString sDesk;
	if(!IsDefaultDesktop(&sDesk) && sDesk!=""){
		return sDesk+"_";
	}
	return "";
}

static long g_lUniqueCounter=0;
CNote::CNote()
{
	iIcon=0;
	font=NULL;
	lTmpPreviousTileBG=-1;
	lTmpPreviousSize=CSize(0,0);
	lShowMode=objSettings.lDefautNoteType;
	bUnderEdit=0;
	lTileBG=1;
	lOpacity=0;
	left=top=right=bottom=0;
	dwTColor=RGB(255,255,255);
	dwBColor=RGB(0,0,0);
	lUniqueCounter=++g_lUniqueCounter;
	sTitle="";
	sNoteOriginalText="";
};

CNote::~CNote()
{
	if(font){
		delete font;
		font=NULL;
	}
};

BOOL CNote::SaveToFile(const char* szFile)
{
	CString sFile=szFile;
	if(sFile==""){
		sFile=GetNotesFolder()+sNoteFile;
	}
	if(sFile==""){
		return FALSE;
	}
	CDataXMLSaver note(sFile,"desktop-note",FALSE);
	note.putValue("icon",iIcon);
	note.putValue("mode",lShowMode);
	note.putValue("left",left);
	note.putValue("top",top);
	note.putValue("right",right);
	note.putValue("bottom",bottom);
	note.putValue("tcolor",dwTColor);
	note.putValue("bcolor",dwBColor);
	note.putValue("cIcon",sCustomIcon);
	note.putValue("cFont",sCustomFont);
	note.putValue("cBg",sCustomBg);
	note.putValue("tileBg",lTileBG);
	note.putValue("opacity",lOpacity);
	note.putValue("title",sTitle);
	note.putValue("bindfile",sNoteOriginalTextFilePath);
	note.putValue("text",sNoteOriginalText);
	return TRUE;
}

CString CNote::GetReminderKey()
{
	CString sRes=sNoteFile;
	sRes.TrimRight();
	sRes.TrimLeft();
	MakeSafeFileName(sRes);
	return sRes;
}

BOOL CNote::LoadFromFile(const char* szFile)
{
	CString sFile=szFile;
	if(sFile==""){
		sFile=GetNotesFolder()+sNoteFile;
	}
	if(sFile=="" || !isFileExist(sFile)){
		return FALSE;
	}
	//Читаем настройки
	CDataXMLSaver note(sFile,"desktop-note",TRUE);
	note.getValue("icon",iIcon);
	if(sCustomIcon!=""){
		_bmp().AddBmpFromDiskIfNotPresent(sCustomIcon,sCustomIcon+DEFNOTEPRF,DEFNOTEICONSIZE);
		_bmp().AddBmpFromDiskIfNotPresent(sCustomIcon,sCustomIcon+DEFNOTEPRFM,DEFNOTEICONSIZM);
	}
	note.getValue("mode",lShowMode);
	note.getValue("left",left);
	note.getValue("top",top);
	note.getValue("right",right);
	note.getValue("bottom",bottom);
	note.getValue("tcolor",dwTColor,RGB(255,255,255));
	note.getValue("bcolor",dwBColor,RGB(0,0,0));
	note.getValue("cIcon",sCustomIcon);
	note.getValue("cFont",sCustomFont);
	note.getValue("cBg",sCustomBg);
	note.getValue("tileBg",lTileBG,1);
	note.getValue("opacity",lOpacity);
	note.getValue("title",sTitle);
	note.getValue("bindfile",sNoteOriginalTextFilePath);
	note.getValue("text",sNoteOriginalText);
	return TRUE;
}

BOOL ReKolbassWallpaper(const char* sNewWPFile, int iTile)
{
	static long lDWPLock=0;
	if(lDWPLock>0){
		return FALSE;
	}
	SimpleTracker Track(lDWPLock);
	if(sNewWPFile==NULL || strlen(sNewWPFile)==0 || IsFileHTML(sNewWPFile) || !isFileExist(sNewWPFile)){
		// Не наш клиент
		return FALSE;
	}
	BOOL bRes=FALSE;
	{
		if(pMainDialogWindow){
			pMainDialogWindow->StartAnimation();
		}
		CBitmap* bmpLoaded=_bmp().LoadBmpFromPath(sNewWPFile);
		{// Генерим преобразованную обоину
			CSetWallPaper wpCommonData;
			if(bmpLoaded){
				wpCommonData.bRegardMM=1;
				wpCommonData.sInPreset=objSettings.sActivePreset;
				wpCommonData.TemplatesInWork.Copy(objSettings.TemplatesInWork);
				wpCommonData.hbmOutImage=(HBITMAP)bmpLoaded->Detach();
				PostFilterWallpaper(&wpCommonData,iTile);
				DeleteFile(objSettings.sBMPFileName+".pre");
				MoveFile(objSettings.sBMPFileName,objSettings.sBMPFileName+".pre");
				// Удалится в деструкторе...
				if(SaveBitmapToDisk(wpCommonData.hbmOutImage,objSettings.sBMPFileName)){
					SetWallpaperFileInReg(objSettings.sBMPFileName, FALSE);
					SetWallpaperIntoOS(objSettings.sBMPFileName, wpCommonData.sADContent);
					bRes=TRUE;
				}
			}
			if(pMainDialogWindow){
				pMainDialogWindow->StopAnimation((objSettings.bShowTrayIconAsWP==1)?wpCommonData.hbmOutImage:NULL);
			}
		}
		if(bmpLoaded){
			delete bmpLoaded;
			bmpLoaded=NULL;
		}
	}
	return bRes;
}

void CallAndFreeRender(HINSTANCE hRender,HBITMAP* hbmOutImage,const char* sPiece, _RenderScript fp, const char*  sLibName)
{
	__try{
		(*fp)(*hbmOutImage,(const char*)sPiece,pMainDialogWindow);
		FreeLibrary(hRender);
	}__except(EXCEPTION_EXECUTE_HANDLER){_XLOG_
		AfxMessageBox("Error in plugin library call! crash Log saved in c:\\wccrash.txt");
		char sErr[100000]="";
		strcpy(sErr,sLibName);
		strcat(sErr,"\n");
		if(strlen(sPiece)>99999){
			((char*)sPiece)[99999]=0;
		}
		strcat(sErr,sPiece);
		SaveFile("c:\\wccrash.txt",sErr);
		//AfxMessageBox(sPiece);
		//AfxMessageBox();
	}
}

BOOL RenderWPT(CString sWPTName, HBITMAP* hbmOutImage,CString* sADTextOut,CString* sCachedPiece, CSetWallPaper* wpCommonData, BOOL bCheckHash=0)
{
	DEBUG_INFO3("Rendering widget: %s, %i*%i",sWPTName,wpCommonData?wpCommonData->rcRecommendedWPPosition.Width():0,wpCommonData?wpCommonData->rcRecommendedWPPosition.Height():0)
	CString sPiece=sCachedPiece?(*sCachedPiece):"";
	CWPT* wpt=GetWPT(sWPTName);
	if(wpt==0){
		return 0;
	}
	if(!isFileExist(GetWPTemplateFilePath(wpt->sWPFileName))){
		return 0;
	}
	objSettings.lSetWallaperLockDesc=Format("%s %s",_l("Rendering"),_l(wpt->sTitle));
	pMainDialogWindow->SetTrayTooltip();
	if(sPiece==""){
		ReadFileWithConfig(GetWPTemplateFilePath(wpt->sWPFileName),sPiece,wpCommonData?wpCommonData->sInPreset:objSettings.sActivePreset);
	}
	if(bCheckHash){
		CString sData2;
		_tag(CUTLINE_TAG,sPiece,&sData2);
		sData2.TrimLeft();
		sData2.TrimRight();
		CString sHash2=MD5_HexString(sData2+"WC3UNREG");
		if(sHash2.CompareNoCase(wpt->sHash)!=0){
#ifdef _DEBUG
			AfxMessageBox(Format("Wrong hash!!! t=%s, righth=%s",wpt->sWPFileName,sHash2));
#endif
			return 0;
		}
	}
	CString sRender=wpt->sADType;
	if(sRender!=""){
		CString sRenderAlt=CDataXMLSaver::GetInstringPart("$=<ALTRENDER:",">",sPiece);
		if(sRenderAlt==""){
			sRenderAlt=_atr("alt-render",sPiece);
		}
		if(objSettings.isLight){
			if(sPiece.Find("FO!")==-1){
				return 0;
			}
		}
#if	defined(USE_LITE) && !defined(ART_VERSION)
		if(sPiece.Find("LO!")==-1){
			return 0;
		}
#endif
		CString sRFile=Format("%s"WP_TEMPLATE"\\%s",GetApplicationDir(),sRender);
		HINSTANCE hRender=0;
		if(sRender!="ActiveDesktop" && hbmOutImage!=0){
			hRender=LoadLibrary(sRFile);
		}else{
			hRender=HINSTANCE(1);
		}
		if(hRender){
			if(wpCommonData){
				wpCommonData->sCurrentWPT=sWPTName;
			}
			TranslateTemplateAliases(sPiece,wpCommonData);
			RemoveTemplateAliases(sPiece,(sADTextOut!=0));
			if(sRender=="ActiveDesktop" || sRenderAlt=="ActiveDesktop"){
				if(sADTextOut){
					*sADTextOut+=sPiece;
				}
			}
			if(sRender!="ActiveDesktop"){
				if(hbmOutImage==0){
					return 0;
				}
				_GetApplicationLang fp0=(_GetApplicationLang)GetProcAddress(hRender,"GetApplicationLang");
				if(fp0){
					(*fp0)()=GetApplicationLang();
				}
				__bmpX fp1=(__bmpX)GetProcAddress(hRender,"_bmpX");
				if(fp1){
					(*fp1)()=&_bmp();
				}
				_RenderScript fp=(_RenderScript)GetProcAddress(hRender,"RenderScript");
				if(fp){
					CallAndFreeRender(hRender,hbmOutImage,(const char*)sPiece,fp, (const char*) sRFile);
					//(*fp)(*hbmOutImage,(const char*)sPiece,pMainDialogWindow);
				}else{
					AfxMessageBox(Format("Invalid plugin file! %s",sRFile));
					FreeLibrary(hRender);
				}
			}
		}else{
			return 0;
		}
	}
	return TRUE;
}

BOOL CallRenderTemplates(const char* szLayer,HBITMAP* hbmOutImage,CSetWallPaper* wpCommonData,CString* sADTextOut)
{// Втыкаем все, что генерится через рендеры
	if(objSettings.lTempDisableAllWidgets){
		if(sADTextOut){
			*sADTextOut="";
		}
		return FALSE;
	}
	CStringArray* TemplatesInWorkTmp=wpCommonData?&wpCommonData->TemplatesInWork:&objSettings.TemplatesInWork;
	objSettings.lRefreshOnNewDay=0;
	int iHashedOkItems=0,iTotalItems=0;
	for(int i=0;i<TemplatesInWorkTmp->GetSize();i++){
		CWPT* wpt=GetWPT(TemplatesInWorkTmp->GetAt(i));
		if(!wpt){
			continue;
		}
		if(objSettings.lUseGlobalRandomWPTLevel){
			long dwWe=wpt->GetWeight(wpCommonData?(const char*)wpCommonData->sInPreset:0);
			if(dwWe<(wpt->dwWeTmp)){
				continue;
			}
		}
		CString sWPT=wpt->sWPFileName;
		CString sPiece;
		ReadFileWithConfig(GetWPTemplateFilePath(sWPT),sPiece,wpCommonData?wpCommonData->sInPreset:objSettings.sActivePreset);
		if(sPiece==""){
			continue;
		}
		CString sHints=CDataXMLSaver::GetInstringPart("$=<HINTS:",">",sPiece);
		if(sHints==""){
			sHints=_atr("hints",sPiece);
		}
		if(sHints.Find("REFRESH-ON-NEW-DAY")!=-1){
			objSettings.lRefreshOnNewDay=1;
		}
		if(wpCommonData!=0 && wpCommonData->lScrMode && (sHints.Find("NO-SCREENSAVER")!=-1 || wpt->sADType=="ActiveDesktop")){
			continue;
		}
		if(objSettings.lConfModeIsON && sPiece.Find("CONTAIN-CONFIDENTIAL-INFO")!=-1){
			continue;
		}
		if(objSettings.lSkipMorthOnGeneration && sHints.Find("CONTAIN-MORTHING")!=-1){
			continue;
		}
		CString sRType=CDataXMLSaver::GetInstringPart("RENDER-LAYER=(",")",sHints);
		if(sRType==""){
			sRType="fg";
		}
		if(szLayer!=0 && sRType.CompareNoCase(szLayer)!=0){
			continue;
		}
		CString sOnlineType=CDataXMLSaver::GetInstringPart("ONLINE-STATE=(",")",sHints);
		if(sOnlineType==""){
			sOnlineType="DONTCARE";
		}
		if(sOnlineType.CompareNoCase("DONTCARE")!=0){
			if(objSettings.lAdForPrevModeIsON){
				continue;
			}
			if(InetIsOffline(0)){
				// Нужен инет, а его нет...
				continue;
			}
		}
		RenderWPT(TemplatesInWorkTmp->GetAt(i), hbmOutImage, sADTextOut, &sPiece, wpCommonData);
	}
	return TRUE;
}

void DrawUnRegs(HBITMAP& hbm,CSetWallPaper* wpCommonData)
{
#ifdef ART_VERSION
	return;
#endif
	if(objSettings.iLikStatus==2){
		return;
	}
	if(objSettings.TemplatesInWork.GetSize()<=2 && objSettings.aDesktopNotes.GetSize()<=1){
		if(rnd(0,100)<30){
			// Не будем раздражать....
			return;
		}
	}
	if(RenderWPT("Unregs", &hbm, 0, 0, wpCommonData,1)==0){
		// Кто-кто сломал все!
		AfxMessageBox("Some important files are missing. Please reinstall application to fix the problem",MB_OK|MB_ICONINFORMATION|MB_TOPMOST);
		TerminateProcess(GetCurrentProcess(),1);
	}
}

CString PREtext(CString sText)
{
	sText.Replace("&","&amp;");
	sText.Replace("<","&lt;");
	sText.Replace(">","&gt;");
	sText.Replace("\n","<br>");
	return sText;
}

CString sFont2HTMLConvert(CString& sMyFont);
BOOL LoadNoteSkin(int iSkin,CString szSkinDir, BOOL bWithIcons);
void RenderHTMLNote(HDC dc, RECT* rtAreaIn, CNoteSkin& skin, CNote& nt)
{
#ifdef _DEBUG
	LoadNoteSkin(objSettings.lNoteSkin,skin.sFolder, 1);
#endif
	CString sTemplate=skin.sHTML_Full;
	if(nt.lShowMode==1 && skin.sHTML_Titleonly.GetLength()>0){
		sTemplate=skin.sHTML_Titleonly;
	}
	if(nt.lShowMode==2 && skin.sHTML_Iconic.GetLength()>0){
		sTemplate=skin.sHTML_Iconic;
	}
	if(nt.lShowMode==3 && skin.sHTML_Text.GetLength()>0){
		sTemplate=skin.sHTML_Text;
	}
	sTemplate=Format("<html><style>%s</style><base url=\"%s\"><body>%s</body></html>",skin.sHTML_Css,skin.sFolder,sTemplate);
	DWORD dwTColor=nt.dwTColor;
	DWORD dwBColor=nt.dwBColor;
	CString sCssBg;
	sCssBg = CString("background-color: ")+Format("#%02X%02X%02X",GetRValue(dwBColor),GetGValue(dwBColor),GetBValue(dwBColor))+";";
	sCssBg += "overflow: none;";
	if(nt.sCustomBg!=""){
		sCssBg += CString("background-image: url("+nt.sCustomBg+");");
		if(!nt.lTileBG){
			sCssBg+="background-repeat: no-repeat;";
		}
	}
	sTemplate.Replace("$=<CSS-BACKGROUND>",sCssBg);

	CFont* pNoteFont=objSettings.pNoteFont;
	CString sFontdOriginal=CreateStrFromFont(pNoteFont, "???", 0, 0, 0, 0, 0);
	CString sTextColor=CString("color: ")+Format("#%02X%02X%02X",GetRValue(dwTColor),GetGValue(dwTColor),GetBValue(dwTColor))+";";
	CString sCssTextTitle=sTextColor+"font:"+sFont2HTMLConvert(sFontdOriginal)+";white-space:pre;";
	sTemplate.Replace("$=<CSS-TITLE>",sCssTextTitle);
	CString sFontd;
	if(nt.sCustomFont!=""){
		sFontd=nt.sCustomFont;
	}else{
		sFontd=sFontdOriginal;
	}
	CString sCssText=sTextColor+"font:"+sFont2HTMLConvert(sFontd)+";white-space:pre;";
	sTemplate.Replace("$=<CSS-TEXT>",sCssText);
	CString sTitla=Format("<span style=\"%s\">%s</span>",sCssTextTitle,PREtext(nt.sTitle));
	sTemplate.Replace("$=<PAR:TITLE>",sTitla);
	CString sTexta=Format("<span style=\"%s\">%s</span>",sCssText,PREtext(nt.GetRealTextFromNote()));
	sTemplate.Replace("$=<PAR:TEXT>",sTexta);
	CString sIcon=nt.sCustomIcon;
	if(nt.iIcon>=0){
		sIcon=skin.sFolder+"icon"+Format(nt.iIcon)+".png";
	}
	sTemplate.Replace("$=<PAR:ICON>",sIcon);
	sTemplate.Replace("$=<PAR:HEIGHT>",Format(rtAreaIn->bottom-rtAreaIn->top));
	sTemplate.Replace("$=<PAR:WIDTH>",Format(rtAreaIn->right-rtAreaIn->left));
	SubstVariables_Simple(sTemplate,TRUE,0);
	pMainDialogWindow->RenderHTML(dc,*rtAreaIn,sTemplate, FALSE);
}

CCriticalSection csActZones;
BOOL PostFilterWallpaper(CSetWallPaper* wpCommonData, int iTile)
{
	HBITMAP& hbmOutImage=wpCommonData->hbmOutImage;
	if(objSettings.lConfModeIsON==2){
		// Выход из программы-вообще ничего не рендерим
		return 1;
	}
	if(hbmOutImage){
		if(objSettings.lChangeInLowP){
			SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_IDLE);
		}
		if(!wpCommonData->bADContentInit){
			ActiveDesktopNeeded(wpCommonData->sADContent);
			wpCommonData->bADContentInit=1;
		}
		// Мостим вручную если нужно
		CSize bmpSize=GetBitmapSize(hbmOutImage);
		CSize scrSize=theApp.GetDesktopRECT().Size();
		if(wpCommonData && !wpCommonData->requiredSizeOfOutImage.IsRectEmpty()){
			scrSize=wpCommonData->requiredSizeOfOutImage.Size();
		}
		if(bmpSize.cx<scrSize.cx && bmpSize.cy<scrSize.cy){
			HBITMAP* pbmp=&hbmOutImage;
			MakeImageCobble(pbmp,scrSize,iTile);
			hbmOutImage=*pbmp;
		}
		// Перед первым вызовом очищаем активные зоны...
		{// Удаляем в защищенном режиме
			CSmartLock lActZones(&csActZones,TRUE);
			objSettings.aActiveZonesRect.RemoveAll();
			objSettings.aActiveZonesActions.RemoveAll();
		}
		{
			if(wpCommonData->sSingleWPTMode!=""){ 
				RenderWPT(wpCommonData->sSingleWPTMode, &hbmOutImage,0,0, wpCommonData);
			}else{
				CallRenderTemplates("bg",&hbmOutImage,wpCommonData);
				CallRenderTemplates("bg-1",&hbmOutImage,wpCommonData);
				CallRenderTemplates("bg-2",&hbmOutImage,wpCommonData);
				CallRenderTemplates("bg-3",&hbmOutImage,wpCommonData);
				CallRenderTemplates("bg-last",&hbmOutImage,wpCommonData);
				if(objSettings.hEnliver && objSettings.lShowAnimationItems){
					UpdateEnviverState();
					if(objSettings.lEnliverON){
						_ChangeBackground fp=(_ChangeBackground)GetProcAddress(objSettings.hEnliver,"ChangeBackground");
						if(fp){
							// Картинка подменяется на подходящую для overlay
							// в библиотеке работает поток, который сам все будет делать
							DWORD dwNewBGColor=0;
							if((*fp)(hbmOutImage,dwNewBGColor)){
								CSafeDC  dc;
								CDesktopDC dcDesk;
								dc.CreateCompatibleDC(&dcDesk);
								CBitmap bmpT;
								bmpT.Attach(hbmOutImage);
								CBitmap* bmOld=dc.SelectObject(&bmpT);
								CBrush bgBrush;
								bgBrush.CreateSolidBrush(dwNewBGColor);
								dc.FillRect(CRect(CPoint(0,0),bmpSize),&bgBrush);
								dc.SelectObject(bmOld);
								bmpT.Detach();
							}
						}else{
							objSettings.lEnliverON=0;
						}
					}
				}
				CallRenderTemplates("fg",&hbmOutImage,wpCommonData);
				CallRenderTemplates("fg-last",&hbmOutImage,wpCommonData);
			}
		}
#if	!defined(USE_LITE) && !defined(ART_VERSION)
		BOOL bDisable=objSettings.lDisableNotes;
		if(objSettings.lConfModeIsON){
			bDisable=1;
		}
		if(objSettings.aDesktopNotes.GetSize()>0 && !bDisable && wpCommonData->lScrMode==0){
			CSmartLock lN(&objSettings.cNoteLock,TRUE,-1);
			// Подготавливаем канву
			CSafeDC  dc;
			CDesktopDC dcDesk;
			dc.CreateCompatibleDC(&dcDesk);
			CBitmap bmpT;
			bmpT.Attach(hbmOutImage);
			CBitmap* bmOld=dc.SelectObject(&bmpT);
			CBitmap* bmTitle=_bmp().GetRaw(GetNSkinBmp(objSettings.lNoteSkin,"TITLE"));
			if(!objSettings.bUnderWindowsNT){
				CSize sz=GetBitmapSize(bmTitle);
				bmTitle=_bmp().Clone(bmTitle,sz);
			}
			CBrush brTitle;
			brTitle.CreatePatternBrush(bmTitle);
			CSize sizeIcon=DEFNOTEICONSIZE;
			CSize sizeTitle=GetBitmapSize(bmTitle);
			if(sizeTitle.cy==0 || sizeTitle.cx==0){
				sizeTitle=sizeIcon;
			}
			if(objSettings.lNoteSkin>=0 && objSettings.lNoteSkin<objSettings.aNoteSkin.GetSize()){// Рисуем в цикле
				for(int i=0;i<objSettings.aDesktopNotes.GetSize();i++){
					CNote& nt=objSettings.aDesktopNotes[i];
					if(nt.lShowMode==4){
						continue;
					}
					CRect rtFull(nt.left,nt.top,nt.right,nt.bottom);
					BLENDFUNCTION blendFunction;
					blendFunction.BlendFlags=0;
					blendFunction.AlphaFormat=0;
					blendFunction.BlendOp=AC_SRC_OVER;
					blendFunction.SourceConstantAlpha=(int)(double(255)*double(nt.lOpacity?nt.lOpacity:objSettings.dwDefNoteOpacity)/double(100));

					if(objSettings.aNoteSkin[objSettings.lNoteSkin].bIsHTML){
						/*if(nt.lShowMode==1){
							rtFull.bottom=rtFull.top+sizeIcon.cy*1.1f;
						}
						if(nt.lShowMode==2){
							rtFull.right=rtFull.left+sizeIcon.cx*1.1f;
							rtFull.bottom=rtFull.top+sizeIcon.cy*1.1f;
						}*/
						CSafeDC  dc2;
						CBitmap bmp;
						dc2.CreateCompatibleDC(&dc);
						bmp.CreateCompatibleBitmap(&dc, rtFull.Width(), rtFull.Height());
						CBitmap* bmOld2=dc2.SelectObject(&bmp);
						::StretchBlt(dc2, 0, 0, rtFull.Width(), rtFull.Height(), dc, rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height(), SRCCOPY);
						CRect rtFull2(rtFull);
						rtFull2.right=rtFull2.Width();
						rtFull2.left=0;
						rtFull2.bottom=rtFull2.Height();
						rtFull2.top=0;
						RenderHTMLNote(dc2.GetSafeHdc(),rtFull,objSettings.aNoteSkin[objSettings.lNoteSkin],nt);
						// Рисуем фон с полупрозрачностью
						AlphaBlendMy(dc,rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height(),dc2,0,0,rtFull.Width(), rtFull.Height(),blendFunction);
						dc2.SelectObject(&bmOld2);
						continue;
					}
					// Вычисляем
					BOOL iSkinYAlign=objSettings.aNoteSkin[objSettings.lNoteSkin].bYPos;
					BOOL iSkinXAlign=objSettings.aNoteSkin[objSettings.lNoteSkin].bXPos;
					double dSkinYDelta=objSettings.aNoteSkin[objSettings.lNoteSkin].dYDelta;
					double dSkinXDelta=objSettings.aNoteSkin[objSettings.lNoteSkin].dXDelta;
					if(nt.lShowMode==1){
						rtFull.bottom=rtFull.top+sizeTitle.cy;
					}
					if(nt.lShowMode==2){
						rtFull.right=rtFull.left+sizeIcon.cx;
						rtFull.bottom=rtFull.top+sizeIcon.cy;
						iSkinYAlign=1;
						iSkinXAlign=1;
						dSkinXDelta=0.5;
						dSkinYDelta=0.5;
					}
					BOOL bNoTitle=FALSE;
					CRect rtTitle(rtFull),rtText(rtFull);
					if(nt.lShowMode!=3){
						rtTitle.bottom=rtTitle.top+sizeTitle.cy;
						rtText.top=rtTitle.bottom-1;
					}else{
						bNoTitle=TRUE;
					}
					{// Рисуем фон
						if(nt.lShowMode!=2){
							CSafeDC  dc2;
							CBitmap bmp;
							dc2.CreateCompatibleDC(&dc);
							bmp.CreateCompatibleBitmap(&dc, rtFull.Width(), rtFull.Height());
							CBitmap* bmOld2=dc2.SelectObject(&bmp);
							CRect rtTitle2(rtTitle),rtText2(rtText),rtFull2(rtFull);
							rtTitle2.OffsetRect(-rtFull.left,-rtFull.top);
							rtTitle2.DeflateRect(1,1);
							rtText2.OffsetRect(-rtFull.left,-rtFull.top);
							rtText2.DeflateRect(1,1);
							rtFull2.OffsetRect(-rtFull.left,-rtFull.top);
							dc2.FillSolidRect(&rtFull2,nt.dwBColor);
							// Рисуем фон с полупрозрачностью
							AlphaBlendMy(dc,rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height(),dc2,0,0,rtFull.Width(), rtFull.Height(),blendFunction);
							if(!bNoTitle){
								dc2.FillRect(&rtTitle2,&brTitle);
							}
							// Фон...
							CBrush brText;
							CSize bgSize(0,0);
							if(!nt.lTileBG){
								bgSize.cx=rtText2.Width();
								bgSize.cy=rtText2.Height();
							}
							if(nt.lShowMode==0 || nt.lShowMode==3){
								BOOL bHaveToClone=(!objSettings.bUnderWindowsNT);
								CBitmap* bmText=0;
								if(nt.sCustomBg!=""){
									if((bgSize.cx!=0 && nt.lTmpPreviousSize!=bgSize) || nt.lTmpPreviousTileBG!=nt.lTileBG){
										_bmp().DelBmp(nt.sCustomBg+DEFNOTEPRFB);
									}
									bmText=_bmp().AddBmpFromDiskIfNotPresent(nt.sCustomBg,nt.sCustomBg+DEFNOTEPRFB,bgSize);
									nt.lTmpPreviousTileBG=nt.lTileBG;
									nt.lTmpPreviousSize=bgSize;
								}else{
									bmText=_bmp().GetRaw(GetNSkinBmp(objSettings.lNoteSkin,"TEXT"),bgSize);
								}
								CSize sz=GetBitmapSize(bmText);
								bmText=_bmp().Clone(bmText,sz);
								if(!bNoTitle){
									if((rtText2.left>2 || rtText2.top>2) && rtText2.top<sz.cy){
										CDib db;
										db.Create(sz.cx,sz.cy);
										db.SetBitmap(&dc2,bmText);
										// Копируем
										DWORD dwLenOfMovedPart=rtText2.top*sz.cx*4;
										BYTE* dwTempBMPPart=new BYTE[dwLenOfMovedPart];
										BYTE* dwTempBMPFull=(BYTE*)db.m_Bits;
										memmove(dwTempBMPPart,dwTempBMPFull,dwLenOfMovedPart);
										memmove(dwTempBMPFull,dwTempBMPFull+dwLenOfMovedPart,db.m_Info.bmiHeader.biSizeImage-dwLenOfMovedPart);
										memmove(dwTempBMPFull+(db.m_Info.bmiHeader.biSizeImage-dwLenOfMovedPart),dwTempBMPPart,dwLenOfMovedPart);
										db.Render(&dc2,bmText);
										delete[] dwTempBMPPart;
									}
								}
								brText.CreatePatternBrush(bmText);
								dc2.FillRect(&rtText2,&brText);
								delete bmText;
							}
							// Рисуем в оригинал с полупрозрачностью
							if(!AlphaBlendMy(dc,rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height(),dc2,0,0,rtFull.Width(), rtFull.Height(),blendFunction)){
								// Без прозрачности
								dc.BitBlt(rtFull.left, rtFull.top, rtFull.Width(), rtFull.Height(), &dc2, 0, 0, SRCCOPY);
							}
							dc2.SelectObject(bmOld2);
						}
						HICON m_hIcon=0;
#ifndef ART_VERSION
						if(nt.iIcon>=0){
							m_hIcon=CreateIconFromBitmap(_bmp().GetRaw(GetNSkinBmp(objSettings.lNoteSkin,"ICO",nt.iIcon)),&sizeIcon,TRUE);
						}else{
							CBitmap* pIcon=_bmp().AddBmpFromDiskIfNotPresent(nt.sCustomIcon,nt.sCustomIcon+DEFNOTEPRF,DEFNOTEICONSIZE);
							m_hIcon=CreateIconFromBitmap(pIcon,&sizeIcon,TRUE);//,nt.sCustomIcon.Find(".png")>=0?0:RGB(255,255,255)
						}
#endif
						if(m_hIcon){
							// Справа-внизу...
							int iIconX=0,iIconY=0;
							if(iSkinXAlign){
								iIconX=rtFull.left-sizeIcon.cx/2+int((double)sizeIcon.cx*dSkinXDelta);
								if(iSkinYAlign){
									rtTitle.left=iIconX+sizeIcon.cx+1;
								}
							}else{
								iIconX=rtFull.right-sizeIcon.cx/2+int((double)sizeIcon.cx*dSkinXDelta)-1;
								if(iSkinYAlign){
									rtTitle.right=iIconX-1;
								}
							}
							if(iSkinYAlign){
								iIconY=rtFull.top-sizeIcon.cy/2+int((double)sizeIcon.cy*dSkinYDelta);
							}else{
								iIconY=rtFull.bottom-sizeIcon.cy/2+int((double)sizeIcon.cy*dSkinYDelta)-1;
							}
							::DrawIconEx(dc.GetSafeHdc(), iIconX, iIconY, m_hIcon, sizeIcon.cx, sizeIcon.cy, 0, NULL, DI_NORMAL);
							ClearIcon(m_hIcon);
						}
					}
					rtText.DeflateRect(3,3);
					rtTitle.DeflateRect(3,3);
					if(nt.lShowMode<2  || nt.lShowMode>=3){// Пишем текст
						if(objSettings.lShowRIconOnNotes){
							CString sRMKey=nt.GetReminderKey();
							_IsReminderFamilyPresent fpPr=(_IsReminderFamilyPresent)GetProcAddress(objSettings.hRemin,"IsReminderFamilyPresent");
							if(fpPr){
								int iMaxFNum=0;
								if((*fpPr)(sRMKey,&iMaxFNum)){
									CBitmap* bmpIco=_bmp().Get("REMINDER");
									CSize sDD(12,12);
									HICON m_hIcon=CreateIconFromBitmap(bmpIco,&sDD,TRUE);
									::DrawIconEx(dc.GetSafeHdc(), rtTitle.left, rtTitle.top+rtTitle.Height()/2-sDD.cy/2+2, m_hIcon, sDD.cx, sDD.cy, 0, NULL, DI_NORMAL);
									rtTitle.left+=sDD.cx+1;
									ClearIcon(m_hIcon);
								}
							}
						}
						if(!bNoTitle){
							CFont* pNoteTFont=objSettings.pNoteTFont;
							CFont* fnOld=0;
							if(pNoteTFont){
								fnOld=dc.SelectObject(pNoteTFont);
							}
							SmartDrawText(dc,nt.sTitle,rtTitle,NULL,nt.dwTColor,nt.dwBColor,(objSettings.lUseNoteTitleShadow?TXSHADOW_GLOW:0)|TXSHADOW_MIXBG|TXSHADOW_VCENT,((nt.lShowMode==1)?DT_LEFT:DT_CENTER)|DT_TOP);
							if(fnOld){
								dc.SelectObject(fnOld);
							}
						}
						if(nt.lShowMode==0 || nt.lShowMode==3){
							CFont* pNoteFont=objSettings.pNoteFont;
							if(nt.sCustomFont!=""){
								pNoteFont=CreateFontFromStr(nt.sCustomFont);
							}
							CFont* fnOld=dc.SelectObject(pNoteFont);
							SmartDrawText(dc,nt.GetRealTextFromNote(),rtText,NULL,nt.dwTColor,nt.dwBColor,TXSHADOW_SIMPLE|TXSHADOW_MIXBG,DT_LEFT|DT_TOP|(objSettings.lAutoWrapNote?DT_WORDBREAK:0));
							dc.SelectObject(fnOld);
							if(nt.sCustomFont!=""){
								delete pNoteFont;
							}
						}
						/*if(objSettings.iLikStatus==1){
							// Подарок незарегистрированным пользователям
							SmartDrawText(dc,_l("Unregistered"),rtText,NULL,RGB(255,254,253),RGB(255,2,1),TXSHADOW_SIMPLE|TXSHADOW_MIXBG,DT_CENTER|DT_BOTTOM|DT_SINGLELINE);
						}*/
					}
				}
			}
			dc.SelectObject(bmOld);
			if(!objSettings.bUnderWindowsNT){
				delete bmTitle;
			}
			bmpT.Detach();
		}
#endif
		DrawUnRegs(hbmOutImage, wpCommonData);
		if(objSettings.lChangeInLowP){
			SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_NORMAL);
		}
	}
	return TRUE;
}

/*
void OnActiveDelClock()
{
	int iActiveClock=GetActiveClockIndex();
	if(iActiveClock>=0 && iActiveClock<objSettings.aLoadedActiveClocksFiles.GetSize()){
		DWORD dwReply=AskYesNo(_l("Do you really want to delete this clock")+"?"+"\n"+_l("It can not be undone")+"!",_l("Confirmation"),&objSettings.lDelClockWarn,pMainDialogWindow?(pMainDialogWindow->dlgWChanger?(CWnd*)pMainDialogWindow->dlgWChanger:(CWnd*)pMainDialogWindow):NULL);
		if(dwReply!=IDYES){
			objSettings.lDelClockWarn=0;
			return;
		}
		objSettings.sActiveClock="";
		CString sFile=objSettings.aLoadedActiveClocksFiles[iActiveClock];
		CString sClocksName=objSettings.aLoadedActiveClocks[iActiveClock];
		char szDisk[MAX_PATH]="C://Temp", szPath[MAX_PATH+2]="", szFile[MAX_PATH]="", szExt[MAX_PATH]="";
		_splitpath(sFile, szDisk, szPath, szFile, szExt);
		objSettings.aLoadedActiveClocksFiles.RemoveAt(iActiveClock);
		objSettings.aLoadedActiveClocks.RemoveAt(iActiveClock);
		BOOL bRes=DeleteFile(sFile);
		sFile=szDisk;
		sFile+=szPath;
		CString sFileMask=szFile;
		MakeSafeFileName(sFileMask);
		sFileMask+="*";
		sFile+=sFileMask;
		sFile+=".*";
		//bRes|=DeleteFile(sFile);
		SHFILEOPSTRUCT str;
		memset(&str,0,sizeof(str));
		str.hwnd=pMainDialogWindow?pMainDialogWindow->GetSafeHwnd():NULL;
		str.wFunc=FO_DELETE;
		szPath[0]=0;
		strcpy(szPath,sFile);
		str.pFrom=szPath;
		str.fFlags=FOF_NOCONFIRMATION|FOF_NOERRORUI|FOF_SILENT;
		if(SHFileOperation(&str)!=0){
			bRes=FALSE;
		}else{
			bRes=TRUE;
		}
		if(bRes){
			Alert(CString("'")+sClocksName+"' "+_l("Deleted successfully")+"!");
		}else{
			Alert(_l("Can`t delete")+CString(" '")+sClocksName+"'"+"!");
		}
	}
}*/

BOOL UpdateEnviverState()
{
	static long lPreviousEnliverState=0;
	if(objSettings.lEnliverON!=lPreviousEnliverState){
		_ChangeState fp=(_ChangeState)GetProcAddress(objSettings.hEnliver,"ChangeState");
		if(fp){
			BOOL bChangedOK=(*fp)(objSettings.lEnliverON);
			if(bChangedOK){
				lPreviousEnliverState=objSettings.lEnliverON;
			}else{
				if(objSettings.lEnliverON){
					Alert(_l("Can not start Desktop animation")+"!",_l("Desktop animation error"));
				}else{
					Alert(_l("Can not stop Desktop animation")+"!",_l("Desktop animation error"));
				}
				objSettings.lEnliverON=lPreviousEnliverState;
			}
		}
	}
	return TRUE;
}

CString WPaper::GetWPar(const char* szName)
{
	if(szName==0){
		return sWallpaperParameters;
	}
	if(strlen(szName)==0){
		int iStop=sWallpaperParameters.Find("<");
		if(iStop!=-1){
			return sWallpaperParameters.Left(iStop);
		}
		return sWallpaperParameters;
	}
	return CDataXMLSaver::GetInstringPart(Format("<%s:",szName),">",sWallpaperParameters);
}

void WPaper::SetWPar(const char* szName, const char* szValue)
{
	if(szName==0 || strlen(szName)==0){
		sWallpaperParameters=szValue;
		return;
	}
	CDataXMLSaver::StripInstringPart(Format("<%s:",szName),">",sWallpaperParameters);
	sWallpaperParameters+=Format("<%s:%s>",szName,szValue);
}

void GenerateForm(CString sSrc,CString sAddParam, BOOL bStrick)
{
	static long lDWPLock=0;
	if(lDWPLock>0){
		return;
	}
	SimpleTracker Track(lDWPLock);
	CString sFile;
	ReadFile(sSrc,sFile);
	BOOL bBaloon=0;
	if(sFile.Find("URL")!=-1 || sFile.Find("url")!=-1){
		bBaloon=1;
	}
	if(bBaloon){
		pMainDialogWindow->ShowBaloon(_l("Checking connection")+"...",_l("Please wait"),30000);
	}
	if(bStrick){
		while(1){
			if(sFile.Replace("< ","<")==0){
				break;
			}
		}
		while(1){
			if(sFile.Replace("<\n","<")==0){
				break;
			}
		}
		while(1){
			if(sFile.Replace("<\r","<")==0){
				break;
			}
		}
		while(1){
			if(sFile.Replace("<\t","<")==0){
				break;
			}
		}
		sFile.Replace("<f","<_f");
		sFile.Replace("<F","<_f");
		sFile.Replace("<_font","<font");// А тут осечка...
		sFile.Replace("<_form action='http://www.wiredplane","<form action='http://www.wiredplane");
	}
	CString sNewAdd=CDataXMLSaver::GetInstringPart("ADDINFO:(",")",sAddParam);
	sFile.Replace("{form_add_param}",sNewAdd);
	CSetWallPaper wpCommonData;
	wpCommonData.sWallpaperFile=CDataXMLSaver::GetInstringPart("HINTS-PARAM2:(",")",sAddParam);
	TranslateTemplateAliases(sFile, &wpCommonData);
	CString sOut=GetUserFolder()+GetPathPart(sSrc,0,0,1,1);
	SaveFile(sOut,sFile);
	CString sOurViewer=Format("%shtml_forms.exe",GetApplicationDir());
	if((int)ShellExecute(0,_T("open"),sOurViewer,"url=\""+sOut+"\"",NULL,SW_SHOWNORMAL)<=32){
		if((int)ShellExecute(0,_T("open"),"iexplore.exe","-nohome \""+sOut+"\"",NULL,SW_SHOWNORMAL)<=32){
			ShellExecute(0,_T("open"),sOut,NULL,NULL,SW_SHOWNORMAL);
		}
	}
	if(bBaloon){
		pMainDialogWindow->ShowBaloon("","");
	}
}

CString CNote::GetRealTextFromNote()
{
	CString sDef=sNoteOriginalText;
	if(sNoteOriginalTextFilePath!=""){
		if(isFileExist(sNoteOriginalTextFilePath)){
			ReadFile(sNoteOriginalTextFilePath,sDef,2);
		}
	}
	return sDef;
}

CString CNote::SetRealTextFromNote(CString sDef)
{
	sNoteOriginalText=sDef;
	if(sNoteOriginalTextFilePath!=""){
		SaveFile(sNoteOriginalTextFilePath,sDef);
		sNoteOriginalText="";
	}
	return sDef;
}


