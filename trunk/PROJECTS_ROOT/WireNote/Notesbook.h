// Notesbook.h: interface for the CNotesbook class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTESBOOK_H__8B0B5B01_96B4_4E2C_B0F6_EF38C06D585E__INCLUDED_)
#define AFX_NOTESBOOK_H__8B0B5B01_96B4_4E2C_B0F6_EF38C06D585E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNotesBook  
{
public:
	int GetMessageCount(BOOL bIncomingOnly=FALSE, int* iUnRead=NULL, int* iFilterPersonTo=NULL, int* iLastMessageNum=NULL, int* iLastUnreadMessageNum=NULL);
	void DeleteAllNotes();
	void UnregisterNoteById(const char* szID);
	void SaveNotes();
	void LoadNotes(BOOL bShared=0);
	CArray<CDLG_Note*,CDLG_Note*> aNotes;
	CNotesBook();
	~CNotesBook();
	int GetSize();
	CDLG_Note* FindNoteByID(const char* sID);
	CDLG_Note* GetNote(int iIndex);
	int Add(CDLG_Note* pNote);
	CDLG_Note* FindMessageByRecAndText(const char* szFrom,const char* szMessage);
};

#endif // !defined(AFX_NOTESBOOK_H__8B0B5B01_96B4_4E2C_B0F6_EF38C06D585E__INCLUDED_)
