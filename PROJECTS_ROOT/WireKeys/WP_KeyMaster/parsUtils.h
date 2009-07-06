#if !defined(AFX_DLG_KMASTER_H__17101BF6_D941_4AF2_9421_B2C000ADB783__INCLUDED_2)
#define AFX_DLG_KMASTER_H__17101BF6_D941_4AF2_9421_B2C000ADB783__INCLUDED_2

CString ReadFile(const char* szFile);
BOOL IsVista();
BOOL isWinXP();
DWORD GetFileSize(const char* szFileName);
BOOL SaveFile(const char* sHidIni,const char* sData);
DWORD atolx(const char* szStr);
class CStringSubstr
{
public:
	
	static CString getTag(const char* szStart, const char* szEnd, const char* szString, int& iFrom, BOOL bRecursive)
	{
		CString sRes="";
		const char* szSearchFrom=szString+iFrom;
		char* szFrom=strstr(szSearchFrom,szStart);
		if(szFrom){
			sRes=(const char*)(szFrom+strlen(szStart));
			iFrom=(szFrom-szString)+strlen(szStart);
			const char* szSearchFromAfterStart=szString+iFrom;
		}else{
			iFrom=-1;
			return "";
		}
		if(szEnd!=NULL){
			int iEnd=sRes.Find(szEnd);
			if(iEnd!=-1){
				// Niio?ei aei?aiiinoe
				int iRecuFind=sRes.Find(szStart);
				if(bRecursive && iRecuFind!=-1 && iRecuFind<iEnd){
					//int iStCount=sRes.Replace(szStart,szStart);
					int iStCount=0,iStOffset=0;
					while((iStOffset=sRes.Find(szStart,iStOffset+strlen(szStart)))!=-1){
						iStCount++;
					}
					while(iStCount>0){
						iEnd=sRes.Find(szEnd,iEnd+strlen(szEnd));
						iStCount--;
					}
				}
				sRes=sRes.Left(iEnd);
				iFrom+=strlen(sRes)+strlen(szEnd);
				const char* szSearchFromEnd=szString+iFrom;
			}else{
				sRes="";
			}
		}else{
			sRes.TrimRight();
			iFrom+=strlen(sRes);
		}
		return sRes;
	}
	
	
	static CString getTag(const char* szStart, const char* szString)
	{
		int i=0;
		return getTag(szStart, NULL, szString,i, FALSE);
	}
	
	static CString getTag(const char* szStart, const char* szEnd, const char* szString)
	{
		int i=0;
		return getTag(szStart, szEnd, szString,i, FALSE);
	}
};

bool PatternMatch(const char* s, const char* mask);
void ConvertComboDataToArray(const char* szData, CStringArray& aStrings, const CString& sChar, char cComment);
bool PatternMatchList(const char* s, const char* mask,const char* szDelimiter);
void SavePairedValues(CString& sData,CStringArray& a1,CStringArray& a2);
void LoadPairedValues(CString sData,CStringArray& a1,CStringArray& a2);
#endif // !defined(AFX_DLG_KMASTER_H__17101BF6_D941_4AF2_9421_B2C000ADB783__INCLUDED_)
