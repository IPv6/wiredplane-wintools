#ifndef CFileMap_classes_H
#define CFileMap_classes_H

/********************************************************************
created:    30:10:2001   15:27
filename:   filemap.h
author:     Vitali Brusentsev
e-mail:     retalik@mail.ru
version:    1.1
purpose:    memory-mapped files stuff
*********************************************************************/

#define CFILE_INIT hFile(INVALID_HANDLE_VALUE), hMap(0), lpMapView(0), dwSize(0)

class CFileMap
{
public:
	CFileMap(): CFILE_INIT {}
	CFileMap(LPCTSTR path, bool write, DWORD size): CFILE_INIT
	{
		Open(path, write, size);
	}
	CFileMap(LPCTSTR path, DWORD size): CFILE_INIT
	{ 
		if(size) Create(path, size);
	}
	~CFileMap(){ Close(); }

	operator bool(){ return (hFile!=INVALID_HANDLE_VALUE); }
	bool    Open(LPCTSTR path, bool write, DWORD size)
	{
		if(write) return OpenInternal(path, GENERIC_READ|GENERIC_WRITE, OPEN_EXISTING, 
			PAGE_READWRITE, FILE_MAP_READ|FILE_MAP_WRITE, size);
		else return OpenInternal(path, GENERIC_READ, OPEN_EXISTING, PAGE_READONLY, FILE_MAP_READ, size);
	}


	bool Create(LPCTSTR path, DWORD size)
	{
		return OpenInternal(path, GENERIC_READ|GENERIC_WRITE, CREATE_ALWAYS, 
			PAGE_READWRITE, FILE_MAP_WRITE, size);
	}
	void Close()
	{
		if(hMap) CloseHandle(hMap);
		hMap=0;
		if(lpMapView) UnmapViewOfFile(lpMapView);
		lpMapView=0;
		if(hFile!=INVALID_HANDLE_VALUE) CloseHandle(hFile);
		hFile=INVALID_HANDLE_VALUE;
		dwSize=0;
	}

	LPVOID   Base(){ return lpMapView; }
	DWORD   Size(){ return dwSize; }
	bool OpenInternal(LPCTSTR pathPart, DWORD dwAccess, DWORD dwCreation, 
		DWORD flProtect, DWORD dwPageAccess, DWORD size=0)
	{
		char path[MAX_PATH]={0};
		/*if(pathPart[1]!=':'){
		GetTempPath(sizeof(path),path);
		strcat(path,pathPart);
		}else{
		strcpy(path,pathPart);
		}*/
		strcpy(path,pathPart);
		if(hFile!=INVALID_HANDLE_VALUE) Close();
		/*
		hFile=CreateFile(path, dwAccess, FILE_SHARE_READ, 0, dwCreation, 
		FILE_ATTRIBUTE_NORMAL, 0);
		if(hFile==INVALID_HANDLE_VALUE)
		{
		return false;
		}
		*/
		if(!size) size=GetFileSize(hFile, 0);
		hMap=CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, size, path);//hFile
		if(!hMap)
		{
			Close();
			return false;
		}

		lpMapView=MapViewOfFile(hMap, dwPageAccess, 0, 0, size);
		if(!lpMapView)
		{
			Close();
			return false;
		}
		dwSize=size;
		return true;
	}
private:
	HANDLE  hFile, hMap;
	LPVOID  lpMapView;
	DWORD   dwSize;
};

#endif //CFileMap_classes_H
