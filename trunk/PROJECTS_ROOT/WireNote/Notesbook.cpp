// Notesbook.cpp: implementation of the CNotesbook class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_common.h"
#include "_externs.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNotesBook::CNotesBook(){};

void CNotesBook::DeleteAllNotes()
{
	//Deleting notes...
	for(int i=0; i<aNotes.GetSize();i++){
		CDLG_Note* pNote=aNotes[i];
		if(!pNote){
			continue;
		}
		aNotes[i]=NULL;
		pNote->DestroyWindow();
	}
	aNotes.RemoveAll();
}

CDLG_Note* CNotesBook::FindMessageByRecAndText(const char* szFrom,const char* szMessage)
{
	if(szFrom==NULL){
		return NULL;
	}
	for(int i=0; i<aNotes.GetSize();i++){
		CDLG_Note* pNote=aNotes[i];
		if(!pNote){
			continue;
		}
		BOOL bSameSender=szFrom && ((strcmp(szFrom,pNote->Sets.sReplyAddressName)==0) || (strcmp(objSettings.AddrBook.TranslateIPToNick(szFrom),pNote->Sets.sReplyAddressName)==0));
		BOOL bSameText=szMessage && (strcmp(szMessage,pNote->Sets.sText)==0);
		if(szMessage){
			// И сендер и текст
			if(bSameSender && bSameText){
				return pNote;
			}
		}else{
			// Только сендер
			if(bSameSender){// && (pNote->Sets.bIncomingMessage)>0//но лишь для входящих сообщений
				return pNote;
			}
		}
	}
	return NULL;
}

CNotesBook::~CNotesBook()
{
	DeleteAllNotes();
}

int CNotesBook::GetSize()
{
	return aNotes.GetSize();
}

CDLG_Note* CNotesBook::GetNote(int iIndex)
{
	if(iIndex<0 || iIndex>=GetSize())
		return NULL;
	return aNotes[iIndex];
}

int CNotesBook::Add(CDLG_Note* pNote)
{
	return aNotes.Add(pNote);
}

void CNotesBook::SaveNotes()
{
	CreateDirIfNotExist(getFileFullName(objSettings.sNotesDirectory,TRUE));
	for(int i=0;i<aNotes.GetSize();i++){
		CDLG_Note* pNote=GetNote(i);
		if(!pNote){
			continue;
		}
		pNote->SaveNoteToFile();
	}
}

void CNotesBook::LoadNotes(BOOL bShared)
{
	BOOL bSomeNotesNotLoaded=0;
	CString sDirectoryPath=getFileFullName(objSettings.sNotesDirectory,TRUE);
	CFileInfoArray dir;
	if(bShared){
		for(int i=0;i<objSettings.aSubsBB.GetSize();i++){
			sDirectoryPath=GetFBFolder(DEF_BB_NDIR,objSettings.aSubsBB[i]);
			dir.AddDir(sDirectoryPath,"*."NOTE_EXTENSION,TRUE,CFileInfoArray::AP_SORTBYTIME | CFileInfoArray::AP_SORTDESCENDING,FALSE);
		}
	}else{
		dir.AddDir(sDirectoryPath,"*."NOTE_EXTENSION,TRUE,CFileInfoArray::AP_SORTBYTIME | CFileInfoArray::AP_SORTDESCENDING,FALSE);
	}
	for (int i=0;i<dir.GetSize();i++){
		if(bShared && !CheckSharedItem(dir[i])){
			continue;
		}
		CString sItemPath=dir[i].GetFilePath();
		CNoteSettings* Sets=new CNoteSettings();
		BOOL bRes=Sets->LoadFromFile(sItemPath);
		CDLG_Note* pOldNote=FindNoteByID(Sets->GetID());
		if(!bRes || (pOldNote && !bShared)){
			bSomeNotesNotLoaded=TRUE;
			delete Sets;
			continue;
		}
		if(bShared){
			CString sIDD=Sets->GetID();
			if(pOldNote){
				Sets->SetRevision(atol(CDataXMLSaver::GetInstringPart("[","]",sItemPath)));
				if(pOldNote->Sets.GetRevision()>Sets->GetRevision()){
					AddBBExcl(GetPathPart(sItemPath,0,0,1,1));
					delete Sets;
					continue;
				}else{
					pOldNote->Sets=*Sets;
					delete Sets;
					Sets=&pOldNote->Sets;
					pOldNote->Sets.hHotkey=CIHotkey(0,0);
					objSettings.lFBStat_UpdN++;
				}
			}else{
				objSettings.lFBStat_NewN++;
				Sets->SetPath(BB_PATH);//NOTE_PATH"/"
				if(pMainDialogWindow){
					::SendMessage(pMainDialogWindow->m_hWnd,CREATE_NEWNOTE,0,(LPARAM)Sets);
				}
			}
			NotifyAboutSharedItem(sIDD,sItemPath);
		}else{
			OpenNote(Sets);
		}
	}
	if(bSomeNotesNotLoaded){
		SmartWarning(_l("Some Notes are not loaded: they are corrupted"),_l("WireNote: Warning"),0);
	}
}

CDLG_Note* CNotesBook::FindNoteByID(const char* sID)
{
	for(int i=0;i<aNotes.GetSize();i++){
		CDLG_Note* pCurNote=aNotes[i];
		if(!pCurNote){
			continue;
		}
		if(strcmp(pCurNote->Sets.GetID(),sID)==0){
			return pCurNote;
		}
	}
	return NULL;
}

void CNotesBook::UnregisterNoteById(const char *szID)
{
	for(int i=0;i<aNotes.GetSize();i++){
		CDLG_Note* pCurNote=aNotes[i];
		if(!pCurNote){
			continue;
		}
		if(strcmp(pCurNote->Sets.GetID(),szID)==0){
			aNotes.RemoveAt(i);
			return;
		}
	}
	return;
}

int CNotesBook::GetMessageCount(BOOL bIncomingOnly, int* iUnRead, int* iFilterPersonTo, int* iLastMessageNum, int* iLastUnreadMessageNum)
{
	int iRes=0;
	if(iUnRead){
		*iUnRead=0;
	}
	for(int i=0;i<aNotes.GetSize();i++){
		CDLG_Note* pCurNote=aNotes[i];
		if(!pCurNote){
			continue;
		}
		if(iFilterPersonTo!=NULL){
			int iNotePerson=objSettings.AddrBook.IsPersonKnown(pCurNote->Sets.sSenderPerson);
			if(iNotePerson!=(*iFilterPersonTo)){
				continue;
			}
		}
		if(pCurNote->Sets.bIncomingMessage==2 && iUnRead){
			(*iUnRead)++;
		}
		if(bIncomingOnly){
			if(pCurNote->Sets.bIncomingMessage>0){
				if(iLastMessageNum){
					*iLastMessageNum=i;
				}
				if(iLastUnreadMessageNum && pCurNote->Sets.bIncomingMessage==2){
					*iLastUnreadMessageNum=i;
				}
				iRes++;
			}
		}else if(pCurNote->IsMessage() || pCurNote->Sets.bIncomingMessage>0){
			if(iLastMessageNum){
				*iLastMessageNum=i;
			}
			if(iLastUnreadMessageNum && pCurNote->Sets.bIncomingMessage==2){
				*iLastUnreadMessageNum=i;
			}
			iRes++;
		}
	}
	return iRes;
}