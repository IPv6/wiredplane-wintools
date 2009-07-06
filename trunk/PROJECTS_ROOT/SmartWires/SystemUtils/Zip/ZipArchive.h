#if !defined(AFX_ZIPARCHIVE_H__20010602_3EE9_91B0_80F5_0080AD509054__INCLUDED_)
#define AFX_ZIPARCHIVE_H__20010602_3EE9_91B0_80F5_0080AD509054__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// ZipArchive - Wrapper classes for ZIP file access
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// ZLIB by Jean-loup Gailly and Mark Adler.
//
// Only reading from ZIP files is supported.
// Supports decrypting files as well.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//
#ifndef __cplusplus
#error ZipArchive requires C++ compilation (use a .cpp suffix)
#endif

#if (_WIN32_IE < 0x0400)
// See note below
#error ZipArchive.h requires _WIN32_IE >= 0x0400
#endif

// We use this IE4 dependant library for the PathMatchSpec() method
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

// Include ZLIB stuff. ZLIB is maintained by Jean-loup Gailly and Mark Adler.
// It is a general GZIP and PKZIP compatible compression library.
// Define ZLIB_DLL to link with the DLL version of ZLIB.
#include "zlib.h"
// Наведенные структуры
#include <sys/stat.h>
#include "cryptutil.h"
#include "zip.h"
#include "unzip.h"

#define ZIP_OK				(0)
#define ZIP_ERRNO			(Z_ERRNO)
#define ZIP_PARAMERROR		(-102)
#define ZIP_INTERNALERROR	(-104)
#define ZIPRES_ERROR		0
#define ZIPRES_OK			1
#define ZIPRES_NOTSRCFILE	-1
#define FILEBUFFERSIZE		2048

#ifndef ATLTRY
#define ATLTRY(x) x
#endif

///////////////////////////////////////////////////////////////
// Structures and defines

typedef struct
{
	TCHAR    szFileName[MAX_PATH];
	TCHAR    szComment[64];
	DWORD    nFileSizeCompressed;
	DWORD    nFileSizeUncompressed;
	FILETIME ftCreationDate;
	DWORD    dwFlags;
	int      nDirIndex;
} ZIP_FIND_DATA, *LPZIP_FIND_DATA;

class CZipArchive;
///////////////////////////////////////////////////////////////
// Classic ZIP file
class CZipFile
{
	friend CZipArchive;
protected:
	// Stream data members
	LPBYTE m_pData;
	DWORD  m_dwPos;
	DWORD  m_dwSize;
	// Decryption
	const DWORD* m_pCrcTable;
	DWORD  m_dwKey[3];
	
public:
	CZipFile() : m_pData(NULL), m_dwSize(0UL), m_dwPos(0UL), m_pCrcTable(NULL)
	{
	}
	~CZipFile()
	{
		Close();
	}
	BOOL CreatePathIfNE(const char* szFile)
	{
		if(int(GetFileAttributes(szFile))==-1){
			return CreateDirectory(szFile,NULL);
		}
		return TRUE;
	}
	BOOL Read(LPVOID pVoid, DWORD dwSize, LPDWORD pdwRead=NULL)
	{
		_ASSERTE(m_pData);
		if( pdwRead != NULL ) *pdwRead = 0;
		if( m_pData == NULL ) return FALSE;
		if( dwSize == 0 || m_dwPos >= m_dwSize ) return FALSE;
		if( m_dwPos+dwSize > m_dwSize ) dwSize = m_dwSize - m_dwPos; // Thanks to Shaun Wilde for fixing this!
		::CopyMemory(pVoid, m_pData + m_dwPos, dwSize);
		m_dwPos += dwSize;
		if( pdwRead!=NULL ) *pdwRead = dwSize;
		return TRUE;
	}
	BOOL SafeSaveToDosk(const char* szTargetName, BOOL bThroughtRBin=0)
	{
		BOOL bRes=0;
		CString sTargetName=szTargetName;
		CString sOrigName=sTargetName;
		if(bThroughtRBin && int(::GetFileAttributes(sTargetName))!=-1){
			sTargetName+=".tmp";
		}
		TCHAR szDrive[MAX_PATH];
		TCHAR szDir[MAX_PATH];
		TCHAR szFile[MAX_PATH];
		_splitpath(sTargetName,szDrive,szDir,szFile,0);
		strcat(szDrive,szDir);
		if (!CreatePathIfNE(szDrive)){
			return FALSE;
		}
		// если только дерриктория - то след.
		if (strlen(szFile) == 0){
			return TRUE;
		}
		bRes=1;
		FILE* f=fopen(sTargetName,"w+b");
		if (f==NULL){
			DeleteFile(sTargetName);
			return FALSE;
		}
		if(m_dwSize>0){
			if (fwrite(m_pData,1,m_dwSize,f)!=m_dwSize){
				bRes=0;
			};
		}
		fclose(f);
		if(sTargetName!=sOrigName){
			if(DeleteFileUsingRBin(sOrigName)){
				if(MoveFile(sTargetName,sOrigName)){
					bRes=1;
				}
			}
		}
		return bRes;
	}
	const char* GetAsString()
	{
		return (const char*)m_pData;
	}
	BOOL Close()
	{
		m_dwPos = 0;
		m_dwSize = 0;
		if( m_pData == NULL ) return TRUE;
		ATLTRY(delete [] m_pData);
		m_pData = NULL;
		return TRUE;
	}
	BOOL IsOpen() const
	{
		return m_pData!=NULL;
	}
	DWORD GetSize() const
	{
		_ASSERTE(IsOpen());
		return m_dwSize;
	}
	DWORD GetPosition() const
	{
		_ASSERTE(IsOpen());
		return m_dwPos;
	}
	DWORD Seek(DWORD dwOffset, UINT nFrom)
	{
		_ASSERTE(IsOpen());
		DWORD dwPos = m_dwPos;
		switch( nFrom ) {
		case FILE_BEGIN:
			m_dwPos = dwOffset;
			break;
		case FILE_END:
			m_dwPos = m_dwSize + dwOffset;
			break;
		case FILE_CURRENT:
			m_dwPos += dwOffset;
			break;
		}
		if( m_dwPos < 0 ) m_dwPos = 0;
		if( m_dwPos >= m_dwSize ) m_dwPos = m_dwSize;
		return dwPos;
	}
	
protected:
	void _Initialize(LPBYTE pData, DWORD dwSize)
	{
		_ASSERTE(pData);
		_ASSERTE(!::IsBadReadPtr(pData,dwSize));
		Close();
		m_pData = pData;
		m_dwSize = dwSize;
	}
	BOOL _DecryptFile(LPCSTR pstrPassword, LPBYTE& pData, DWORD& dwSize, DWORD crc32)
	{
		_ASSERTE(pData);
		_ASSERTE(!::IsBadStringPtrA(pstrPassword,-1));
		
		if( !_InitKeys(pstrPassword) ) return FALSE;
		if( !_DecryptHeader(pData, dwSize, crc32) ) return FALSE;
		if( !_DecryptData(pData, dwSize) ) return FALSE;
		return TRUE;
	}
	inline DWORD _crc32(DWORD c, BYTE b)
	{
		_ASSERTE(m_pCrcTable);
		return m_pCrcTable[((DWORD) (c) ^ (b)) & 0xFF] ^ ((c) >> 8);
	}
	BOOL _InitKeys(LPCSTR pstrPassword)
	{
		m_pCrcTable = get_crc_table();
		m_dwKey[0] = 305419896;
		m_dwKey[1] = 591751049;
		m_dwKey[2] = 878082192;
		int nLen = ::lstrlenA(pstrPassword);
		if( nLen == 0 ) return FALSE;
		for( int i = 0; i < nLen; i++ ) _UpdateKeys(pstrPassword[i]);
		return TRUE;
	}
	void _UpdateKeys(BYTE c)
	{      
		m_dwKey[0] = _crc32(m_dwKey[0], c);
		m_dwKey[1] = m_dwKey[1] + (m_dwKey[0] & 0x000000FF);
		m_dwKey[1] = m_dwKey[1] * 134775813 + 1;
		BYTE b = (BYTE) (m_dwKey[1] >> 24);
		m_dwKey[2] = _crc32(m_dwKey[2], b);
	}
	BOOL _DecryptHeader(LPBYTE pData, DWORD dwSize, DWORD crc32)
	{
		if( dwSize < 12 ) return FALSE;
		BYTE header[12];
		::CopyMemory(&header, pData, 12);
		for( int i = 0; i < 12; i++ ) {
			BYTE c = (BYTE)( header[i] ^ _DecryptByte() );
			_UpdateKeys(c);
			header[i] = c;
		}
		// Password check
		if( header[11] != (BYTE)(crc32 >> 24) ) return FALSE;
		return TRUE;
	}
	inline BYTE _DecryptByte() const
	{
		DWORD temp = (WORD) (m_dwKey[2] | 2);
		return (BYTE)((temp * (temp ^ 1)) >> 8);
	}
	BOOL _DecryptData(LPBYTE& pData, DWORD& dwSize)
	{
		LPBYTE pRawData;
		ATLTRY(pRawData = new BYTE[ dwSize - 12 ]);
		_ASSERTE(pRawData);
		if( pRawData == NULL ) return FALSE;
		LPBYTE p = pRawData;
		for( DWORD i = 12; i < dwSize; i++ ) {
			BYTE c = (BYTE) (pData[i] ^ _DecryptByte());
			_UpdateKeys(c);
			*p++ = c;
		}
		ATLTRY(delete [] pData);
		pData = pRawData;
		dwSize -= 12;
		return TRUE;
	}
};



///////////////////////////////////////////////////////////////
// ZIP Archive class

class CZipArchive
{
protected:
	
#pragma pack(2)
	
	enum
	{
		DIR_SIGNATURE = 0x06054b50,
	};
	
	struct ZipDirHeader
	{
		DWORD   sig;
		WORD    nDisk;
		WORD    nStartDisk;
		WORD    nDirEntries;
		WORD    totalDirEntries;
		DWORD   dirSize;
		DWORD   dirOffset;
		WORD    cmntLen;
	};
	
	enum
	{
		FILE_SIGNATURE = 0x02014b50,
			FILE_COMP_STORE   = 0,
			FILE_COMP_DEFLAT  = 8,
	};
	
	struct ZipDirFileHeader
	{
		DWORD   sig;
		WORD    verMade;
		WORD    verNeeded;
		WORD    flag;
		WORD    compression;      // COMP_xxxx
		WORD    modTime;
		WORD    modDate;
		DWORD   crc32;
		DWORD   cSize;            // Compressed size
		DWORD   ucSize;           // Uncompressed size
		WORD    fnameLen;         // Filename string follows header.
		WORD    xtraLen;          // Extra field follows filename.
		WORD    cmntLen;          // Comment field follows extra field.
		WORD    diskStart;
		WORD    intAttr;
		DWORD   extAttr;
		DWORD   hdrOffset;
		
		LPSTR GetName() const { return (LPSTR)(this + 1); }
		LPSTR GetExtra() const { return GetName() + fnameLen; }
		LPSTR GetComment() const { return GetExtra() + xtraLen; }
	};
	
	enum
	{
		LOCAL_SIGNATURE = 0x04034b50,
			LOCAL_COMP_STORE  = 0,
			LOCAL_COMP_DEFLAT = 8,
	};
	
	struct ZipLocalHeader
	{
		DWORD   sig;
		WORD    version;
		WORD    flag;
		WORD    compression;      // COMP_xxxx
		WORD    modTime;
		WORD    modDate;
		DWORD   crc32;
		DWORD   cSize;
		DWORD   ucSize;
		WORD    fnameLen;         // Filename string follows header.
		WORD    xtraLen;          // Extra field follows filename.
	};
	
#pragma pack()
	
	struct FindFileHandle
	{
		TCHAR szSearch[MAX_PATH];
		int nPos;
	};
	
	HANDLE             m_hFile;
	ZipDirHeader       m_Header;
	ZipDirFileHeader** m_Files;
	LPBYTE             m_DirData;
	CHAR               m_szPassword[64];
	
	public:
		
		CZipArchive() : m_hFile(INVALID_HANDLE_VALUE), m_Files(NULL), m_DirData(NULL)
		{
		}
		~CZipArchive()
		{
			Close();
		}
		
		BOOL Open(LPCTSTR pstrFileName)
		{
			_ASSERTE(!::IsBadStringPtr(pstrFileName,MAX_PATH));
			Close();
			m_hFile = ::CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if( m_hFile == INVALID_HANDLE_VALUE ) return FALSE;
			
			::SetFilePointer(m_hFile, -(LONG)sizeof(m_Header), NULL, FILE_END);
			DWORD dwRead;
			::ReadFile(m_hFile, &m_Header, sizeof(m_Header), &dwRead, NULL);
			_ASSERTE(dwRead==sizeof(m_Header));
			if( dwRead != sizeof(m_Header) ) { Close(); return FALSE; };
			if( m_Header.sig != DIR_SIGNATURE ) { Close(); return FALSE; }
			
			_ASSERTE(m_Header.nDirEntries<1000); // Sanity check
			ATLTRY(m_DirData = (LPBYTE) malloc(m_Header.dirSize));
			_ASSERTE(m_DirData);
			ATLTRY(m_Files = new ZipDirFileHeader*[m_Header.nDirEntries]);
			_ASSERTE(m_Files);
			if( m_Files == NULL ) { Close(); return FALSE; };
			if( m_DirData == NULL ) { Close(); return FALSE; }
			
			::SetFilePointer(m_hFile, -(LONG)(sizeof(m_Header) + m_Header.dirSize), NULL, FILE_END);
			::ReadFile(m_hFile, m_DirData, m_Header.dirSize, &dwRead, NULL);
			_ASSERTE(dwRead==m_Header.dirSize);
			if( dwRead != m_Header.dirSize ) { Close(); return FALSE; }
			
			LPBYTE pData = m_DirData;
			for( int i = 0; i < m_Header.nDirEntries; i++ ) {
				// Set the header pointer in the m_Files array
				ZipDirFileHeader* fh = (ZipDirFileHeader*) pData;
				m_Files[i] = fh;
				if( fh->sig != FILE_SIGNATURE ) { Close(); return FALSE; }
				// Convert UNIX slash to Windows backslash in ANSI string
				LPSTR pstrName = fh->GetName();
				for( int i = 0; i < fh->fnameLen; i++, pstrName++ ) if( *pstrName == '/' ) *pstrName = '\\';
				// Get next header
				pData += sizeof(ZipDirFileHeader) + fh->fnameLen + fh->xtraLen + fh->cmntLen;
			}
			
			m_szPassword[0] = '\0';
			return TRUE;
		}
		void Close()
		{
			if( m_hFile!=INVALID_HANDLE_VALUE ) {
				::CloseHandle(m_hFile);
				m_hFile = INVALID_HANDLE_VALUE;
			}
			if( m_Files!=NULL ) {
				ATLTRY(delete [] m_Files);
				m_Files = NULL;
			}
			if( m_DirData!=NULL ) {
				free(m_DirData);
				m_DirData = NULL;
			}
		}
		BOOL IsOpen() const
		{
			return m_hFile!=INVALID_HANDLE_VALUE;
		}
		BOOL SetPassword(LPCSTR pstrPassword)
		{
			_ASSERTE(IsOpen());
			_ASSERTE(!::IsBadStringPtrA(pstrPassword,-1));
			_ASSERTE(::lstrlenA(pstrPassword)<sizeof(m_szPassword)-1);
			if( ::lstrlenA(pstrPassword) >= sizeof(m_szPassword) - 1 ) return FALSE;
			::lstrcpyA(m_szPassword, pstrPassword);
			return TRUE;
		}
		
		// ZIP File API
		
		BOOL GetFile(LPCTSTR pstrName, CZipFile* pFile) const
		{
			_ASSERTE(IsOpen());
			_ASSERTE(!::IsBadStringPtr(pstrName,MAX_PATH));
			ZIP_FIND_DATA fd;
			HANDLE hFindFile = FindFirstFile(pstrName, &fd);
			if( hFindFile == INVALID_HANDLE_VALUE ) return FALSE;
			BOOL bRes = GetFile(fd.nDirIndex, pFile);
			FindClose(hFindFile);
			return bRes;
		}
		BOOL GetFile(int iIndex, CZipFile* pFile) const
		{
			_ASSERTE(IsOpen());
			_ASSERTE(pFile);
			_ASSERTE(iIndex>=0 && iIndex<m_Header.nDirEntries);
			if( m_hFile == INVALID_HANDLE_VALUE ) return FALSE;
			if( iIndex < 0 || iIndex >= m_Header.nDirEntries ) return FALSE;
			
			ZipLocalHeader hdr;
			::SetFilePointer(m_hFile, m_Files[iIndex]->hdrOffset, NULL, FILE_BEGIN);
			DWORD dwRead;
			::ReadFile(m_hFile, &hdr, sizeof(hdr), &dwRead, NULL);
			if( dwRead!=sizeof(hdr) ) return FALSE;
			if( hdr.sig != LOCAL_SIGNATURE ) return FALSE;
			::SetFilePointer(m_hFile, hdr.fnameLen+hdr.xtraLen, NULL, FILE_CURRENT);
			if(hdr.ucSize==0){
				// Empty file, that is ok;
				return TRUE;
			}
			// Prepare decompression file of file
			LPBYTE pData;
			ATLTRY(pData = new BYTE[hdr.cSize]);
			_ASSERTE(pData);
			if( pData == NULL ) return FALSE;
			
			::ReadFile(m_hFile, pData, hdr.cSize, &dwRead, NULL);
			if( dwRead != hdr.cSize ) {
				ATLTRY(delete [] pData);
				return FALSE;
			}
			
			// Decrypt option
			DWORD dwSize = hdr.cSize;
			if( hdr.flag & 1 ) {
				if( ::lstrlenA(m_szPassword) == 0 ) return FALSE;
				if( !pFile->_DecryptFile(m_szPassword, pData, dwSize, hdr.crc32) ) return FALSE;
			}
			
			// Decompress file
			switch( hdr.compression ) {
			case LOCAL_COMP_STORE:
				_ASSERTE(hdr.cSize==hdr.ucSize);
				break;
			case LOCAL_COMP_DEFLAT: 
				{
					LPBYTE pTarget;
					ATLTRY(pTarget = new BYTE[hdr.ucSize]);
					_ASSERTE(pTarget);
					if( pTarget == NULL ) return FALSE;
					
					z_stream stream = { 0 };
					stream.next_in = (Bytef*) pData;
					stream.avail_in = (uInt) dwSize;
					stream.next_out = (Bytef*) pTarget;
					stream.avail_out = hdr.ucSize;
					stream.zalloc = (alloc_func) NULL;
					stream.zfree = (free_func) NULL;
					// Perform inflation; wbits < 0 indicates no zlib header inside the data.
					int err = inflateInit2(&stream, -MAX_WBITS);
					if( err == Z_OK ) {
						err = inflate(&stream, Z_NO_FLUSH);
						if( err == Z_STREAM_END) err = Z_OK;
						inflateEnd(&stream);
					}
					ATLTRY(delete [] pData);
					if( err!=Z_OK ) {
						ATLTRY(delete [] pTarget);
						return FALSE;
					}
					pData = pTarget;
				}
				break;
			default:
				_ASSERTE(false); // unsupported compression scheme
				return FALSE;
			}
			
			// The memory we allocated is passed to the file, which
			// takes ownership of it.
			pFile->_Initialize(pData, hdr.ucSize);
			return TRUE;
		}
		
		// FindFile API
		
		HANDLE FindFirstFile(LPCTSTR pstrFileName, LPZIP_FIND_DATA lpFindFileData) const
		{
			_ASSERTE(IsOpen());
			_ASSERTE(!::IsBadStringPtr(pstrFileName,MAX_PATH));
			_ASSERTE(!::IsBadWritePtr(lpFindFileData, sizeof(ZIP_FIND_DATA)));
			
			::ZeroMemory(lpFindFileData, sizeof(ZIP_FIND_DATA));
			
			FindFileHandle* pFF;
			ATLTRY(pFF = new FindFileHandle);
			if( pFF == NULL ) return INVALID_HANDLE_VALUE;
			::lstrcpy(pFF->szSearch, pstrFileName);
			pFF->nPos = 0;
			
			BOOL bSuccess = FindNextFile( (HANDLE) pFF, lpFindFileData );
			if( !bSuccess ) {
				ATLTRY(delete pFF);
				return INVALID_HANDLE_VALUE;
			}
			else {
				return (HANDLE)pFF;
			}
		}
		BOOL FindNextFile(HANDLE hFindFile, LPZIP_FIND_DATA lpFindFileData) const
		{
			_ASSERTE(IsOpen());
			if( m_hFile == INVALID_HANDLE_VALUE ) return FALSE;
			if( hFindFile == INVALID_HANDLE_VALUE || hFindFile == NULL ) return FALSE;
			
			FindFileHandle* pFF = reinterpret_cast<FindFileHandle*>(hFindFile);
			while( true ) {
				if( pFF->nPos >= m_Header.nDirEntries ) return FALSE;
				// Extract filename and match with pattern
				const ZipDirFileHeader* fh = m_Files[pFF->nPos];
				TCHAR szFile[MAX_PATH] = { 0 };
				::OemToCharBuff(fh->GetName(), szFile, fh->fnameLen);
				if( ::PathMatchSpec(szFile, pFF->szSearch) != NULL ) {
					// Copy data to the ZIP_FIND_DATA structure
					lpFindFileData->nDirIndex = pFF->nPos;
					::lstrcpy( lpFindFileData->szFileName, szFile );
					lpFindFileData->szComment[0] = _T('\0'); // unsupported right now
					lpFindFileData->nFileSizeCompressed = fh->cSize;
					lpFindFileData->nFileSizeUncompressed = fh->ucSize;
					::DosDateTimeToFileTime(fh->modDate, fh->modTime, &lpFindFileData->ftCreationDate);
					// Figure out the file attributes
					DWORD& dwFlags = lpFindFileData->dwFlags = 0;
					if( fh->flag & 1 ) dwFlags |= FILE_ATTRIBUTE_ENCRYPTED;
					if( fh->flag & (2|4) ) dwFlags |= FILE_ATTRIBUTE_COMPRESSED;
					if( fh->flag & (16|32|64) ) dwFlags |= FILE_ATTRIBUTE_OFFLINE; // unsupported compression used
					if( dwFlags == 0 ) dwFlags = FILE_ATTRIBUTE_NORMAL;
					// Ready for next entry...
					pFF->nPos++;
					return TRUE;
				}
				pFF->nPos++;
			}
			return FALSE;
		}
		BOOL FindClose(HANDLE hFindFile) const
		{
			if( hFindFile == INVALID_HANDLE_VALUE || hFindFile == NULL ) return FALSE;
			FindFileHandle* pFF = reinterpret_cast<FindFileHandle*>(hFindFile);
			ATLTRY(delete pFF);
			return TRUE;
		}
};

class CCreateZipArchive
{
public:
	int m_ikind;
	void* m_fzip;
	char m_szPwd[64];
	// имя zip файла
	CString m_szZipFileName;
	// сколько файлов в архиве
	int iInnerZipFileNumber;
	// информация об открытом внутреннем файле
	unz_file_info m_unzFI;
	
	CCreateZipArchive()
	{
		m_ikind=0;
		m_szPwd[0]=0;
		iInnerZipFileNumber=0;
	}
	
	~CCreateZipArchive()
	{
		Close();
	}
	
	LPTSTR MakeRealName(LPTSTR newFileNAME, LPCTSTR FileName, int SavePath)
	{
		LPTSTR from=(LPTSTR)FileName;
		LPTSTR tmp;
		if ( (tmp=_tcsrchr(from,':'))!=NULL){
			from=tmp+1;
			if (*from=='\\')
				from++;
		};
		if (!SavePath)
			if ( (tmp=_tcsrchr(from,'\\'))!=NULL)
				from=tmp+1;
			tmp=newFileNAME;
			while (*from!=0){
				if (*from=='\\')
					*tmp='/';
				else
					*tmp=*from;
				from++;
				tmp++;
			}
			*tmp=0;
			TCHAR szTmpBuff[2*MAX_PATH];
			_tcscpy(szTmpBuff,newFileNAME);
			CharToOem(szTmpBuff,newFileNAME);
			//strcpy(newFileNAME,szTmpBuff,
			//ConvertToDosChar(newFileNAME);
			return newFileNAME;
	};
	void SetZipPassword(const char* szNewPasw)
	{
		strcpy(m_szPwd,szNewPasw);
	}
	/** Получить время создания файла хранящегося в архиве		
	*	@param		time_t& tTime - время
	*	@return		int - если ZIPRES_OK то все ok
	*/
	int GetCurrentInzipFileDate(time_t& tTime)
	{
		tm tmTime;
		unz_file_info unzFI;
		if (unzGetCurrentFileInfo(m_fzip,&unzFI,NULL,0,NULL,0,NULL,0) != UNZ_OK)
			return ZIPRES_ERROR;
		
		// Секунды
		tmTime.tm_sec = (unzFI.dosDate & 0xf)*2;
		// Минуты
		tmTime.tm_min = (unzFI.dosDate & 0x7f0)/32;
		// Часы 
		tmTime.tm_hour = (unzFI.dosDate & 0xf800)/2048;
		
		unzFI.dosDate >>= 16; 
		
		//день
		tmTime.tm_mday= unzFI.dosDate & 0xf;
		// месяц
		tmTime.tm_mon= (unzFI.dosDate & 0x1f0)/32 - 1;
		// год
		tmTime.tm_year = (unzFI.dosDate & 0xfe00)/512 + 80;
		
		tmTime.tm_isdst = 0;
		tTime = mktime(&tmTime);
		return ZIPRES_OK;
	}
	
	
	/** Распаковать текуций файл в szToFileName		
	*	@param		LPCSTR szToFileName - имя файла в который распаковываем 
	*				текущий файл из архива
	*	@return		int  - если ZIPRES_OK то все ok
	*/
	int UnpackCurrent(LPCTSTR szToFileName)
	{
		if (m_ikind != 2 || szToFileName == NULL)
			return ZIPRES_ERROR;
		
		// создать dir для выходного файла
		TCHAR szDrive[MAX_PATH];
		TCHAR szDir[MAX_PATH];
		TCHAR szFile[MAX_PATH];
		_splitpath(szToFileName,szDrive,szDir,szFile,0);
		strcat(szDrive,szDir);
		if (!CreatePathIfNE(szDrive))
			return ZIPRES_ERROR;
		
		// если только дерриктория - то след.
		if (strlen(szFile) == 0)
			return ZIPRES_OK;
		
		FILE* f=fopen(szToFileName,"w+b");
		if (f==NULL)
		{
			DeleteFile(szToFileName);
			return ZIPRES_ERROR;
		}
		
		
		DWORD dwRes=1;
		if (unzOpenCurrentFile(m_fzip,(LPTSTR)m_szPwd)==UNZ_OK){
			// start zipping
			unsigned char buff[FILEBUFFERSIZE];
			while (!unzeof(m_fzip)){
				size_t len=unzReadCurrentFile(m_fzip,buff,FILEBUFFERSIZE);
				if (len>0){
					if ( fwrite(buff,1,len,f)!=len){
						dwRes=0;
						break;
					};
				}
				else{
					dwRes=0;
					break;
				}
			};
			unzCloseCurrentFile(m_fzip);
		}
		else
			dwRes=0;
		fclose(f);
		
		if (!dwRes)
			return ZIPRES_ERROR;
		
		return ZIPRES_OK;
	}
	
	
	/** получить имя следующего файла		
	*	@param		LPSTR szFileName - сюда имя следующего файла
	*	@return		int - если ZIPRES_OK то все ok
	*/
	int GetNext(LPTSTR szFileName)
	{
		if (m_ikind != 2 || szFileName == NULL)
			return ZIPRES_ERROR;
		if (unzGoToNextFile(m_fzip)!=UNZ_OK)
			return ZIPRES_ERROR;
		unzGetCurrentFileInfo(m_fzip,&m_unzFI,szFileName,512,NULL,0,NULL,0);
		return ZIPRES_OK;
	}
	
	
	/** получить имя первого файла
	*	@param		LPSTR szFileName - сюда имя первого файла
	*	@return		int - если ZIPRES_OK то все ok
	*/
	int GetFirst(LPTSTR szFileName)
	{
		if (m_ikind != 2 || szFileName == NULL)
			return ZIPRES_ERROR;
		if (unzGoToFirstFile(m_fzip)!=UNZ_OK)
			return ZIPRES_ERROR;
		unzGetCurrentFileInfo(m_fzip,&m_unzFI,szFileName,512,NULL,0,NULL,0);
		return ZIPRES_OK;
	}
	
	/** Открыть для разархивирования		
	*	@param		LPCSTR szZipFileName - имя архива
	*	@param		LPCSTR szPwd=NULL - пароль
	*	@return		int - если ZIPRES_OK то все ok
	*/
	int OpenToUnpack(LPCTSTR szZipFileName, LPCTSTR szPwd=NULL)
	{
		if (m_ikind != 0)
			return ZIPRES_ERROR;
		// Не должны быть NULL 
		if (!szZipFileName)
			return ZIPRES_ERROR;
		m_szZipFileName = szZipFileName;
		if (szPwd && szPwd[0]!=0){
			strcpy(m_szPwd, szPwd);
		}
		
		// открыть на unpack
		m_fzip=unzOpen (m_szZipFileName);
		if (m_fzip==NULL)
			return ZIPRES_ERROR;
		
		
		unz_global_info pglobal_info;
		unzGetGlobalInfo(m_fzip,&pglobal_info);
		iInnerZipFileNumber = pglobal_info.number_entry;
		
		m_ikind = 2; // открыт на unpack
		return ZIPRES_OK;
	}
	/** Добавить файл szFileName в архив под именем szInZipFileName		
	*	@param		LPCSTR szFileName - имя добавляемого файла
	*	@param		LPCSTR szInZipFileName - имя файла в zip
	*	@return		int  - если ZIPRES_OK то все ok
	*/
	int Add(LPCTSTR szFileName, LPCTSTR szInZipFileName)
	{
		if(m_ikind!=1){
			return ZIPRES_ERROR;
		}
		
		FILE* f=fopen(szFileName,"rb");
		if (f==NULL)
			return ZIPRES_NOTSRCFILE;
		
		crypt_info cr;
		cr.pwd[0]=0;
		if (strlen(m_szPwd)>0){
			cr.isCalcCRC=1;
			if (cr.isCalcCRC)
				cr.CalcCRC=GetCRCForFile(f);
			strcpy(cr.pwd, m_szPwd);
			rewind(f);
		}
		int dwRes=1;
		zip_fileinfo zfi;
		struct stat fst;
		fstat(fileno(f), &fst);
		zfi.dosDate=ToZipTime(fst.st_mtime);
		zfi.internal_fa=0;
		zfi.external_fa=0;
		TCHAR szNewFileNAME[_MAX_FNAME];
		//OemToChar(szFileName,szNewFileNAME);
		MakeRealName(szNewFileNAME,szInZipFileName,1);
		int i=zipOpenNewFileInZip (m_fzip,szNewFileNAME,&zfi, 
			NULL, 0,
			NULL, 0,
			NULL,Z_DEFLATED, Z_DEFAULT_COMPRESSION,&cr);
		if (i==ZIP_OK){
			// start zipping
			unsigned char buff[FILEBUFFERSIZE];
			while (!feof(f)){
				size_t len=fread(buff,1,FILEBUFFERSIZE,f);
				if (zipWriteInFileInZip(m_fzip,buff,len)!=ZIP_OK){
					dwRes=0;
					break;
				};
			};
		}
		else
			dwRes=0;
		
		if (zipCloseFileInZip(m_fzip)!=ZIP_OK)
			dwRes=0;
		fclose(f);
		
		if (!dwRes)
			return ZIPRES_ERROR;
		return ZIPRES_OK;
	}
	
	/** Создать архив с именем szZipFileName и паролем szPwd
	*	@param		LPCSTR szZipFileName - имя zip файла
	*	@param		LPCSTR szPwd=NULL - пароль
	*	@return		int - если ZIPRES_OK то все ok
	*/
	int Create(LPCTSTR szZipFileName, LPCTSTR szPwd=NULL, BOOL bAppend=0)
	{
		// Не должны быть NULL 
		if (!szZipFileName)
			return ZIPRES_ERROR;
		m_szZipFileName = szZipFileName;
		if (szPwd && szPwd[0]!=0){
			strcpy(m_szPwd, szPwd);
		}
		// Создаем zip-файл
		m_fzip=(void*)zipOpen(m_szZipFileName, bAppend);
		if (m_fzip==NULL){
			return ZIPRES_ERROR;
		}
		m_ikind=1;
		return ZIPRES_OK;
	}
	
	int Close(const char* szComments=0)
	{
		if (!m_fzip)
			return ZIPRES_ERROR;
		if(m_ikind!=1){
			if (unzClose(m_fzip)!=ZIP_OK)
				return ZIPRES_ERROR;
		}else{
			if (zipClose(m_fzip,szComments)!=ZIP_OK)
				return ZIPRES_ERROR;
		}
		m_fzip = NULL;
		return ZIPRES_OK;
	}
	
	BOOL CreatePathIfNE(const char* szFile)
	{
		if(int(GetFileAttributes(szFile))==-1){
			return CreateDirectory(szFile,NULL);
		}
		return TRUE;
	}
};

class CZipCacher
{
public:
	int iTodayCount;
	CString sTempPath;
	CMapStringToString aFiles;
	DWORD dwCheckTime;
	CZipCacher()
	{
		iTodayCount=0;
		dwCheckTime=GetTickCount();
		char sTempFolder[MAX_PATH]={0};
		GetTempPath(sizeof(sTempFolder),sTempFolder);
		sTempPath=sTempFolder;
	};
	void RemoveAll()
	{
		// Чистим каталог
		dwCheckTime=GetTickCount();
		POSITION pos=aFiles.GetStartPosition();
		CString sKey,sVal;
		while(pos){
			aFiles.GetNextAssoc(pos,sKey,sVal);
			DeleteFile(sVal);
		}
		aFiles.RemoveAll();
	}
	~CZipCacher()
	{
		RemoveAll();
	}
	CString UnpackAndRetName(const char* szFile)
	{
		// Проверка времени...
		if(GetTickCount()-dwCheckTime>(60*100)*1000){
			RemoveAll();
		}
		CString szUnpackedFile;
		if(aFiles.Lookup(szFile,szUnpackedFile) && int(GetFileAttributes(szUnpackedFile))!=-1){
			return szUnpackedFile;
		}
		// Вычленяем имя архива
		CString sPackedFile=szFile;
		CString sPackedFileLow=sPackedFile;
		sPackedFileLow.MakeLower();
		int iFStart=sPackedFileLow.Find(".zip\\");
		if(iFStart==-1){
			iFStart=sPackedFileLow.Find(".wsc\\");
		}
		if(iFStart==-1){
			szUnpackedFile=szFile;
		}else{
			CString sArch=sPackedFile.Left(iFStart+4);
			CString sInArch=sPackedFile.Mid(iFStart+5);
			szUnpackedFile.Format("WCZ%i_%s",iTodayCount++,sInArch);
			szUnpackedFile.Replace("\\","#");
			szUnpackedFile.Replace("/","#");
			szUnpackedFile=sTempPath+szUnpackedFile;
			CZipArchive zp;
			if(zp.Open(sArch)){
				CZipFile zI;
				if(zp.GetFile(sInArch,&zI)){
					zI.SafeSaveToDosk(szUnpackedFile,0);
				}
				zp.Close();
			}
		}
		aFiles.SetAt(szFile,szUnpackedFile);
		return szUnpackedFile;
	}
	static CString zipCache(const char* szFileInZip)
	{
		static CZipCacher zp;
		return zp.UnpackAndRetName(szFileInZip);
	}
};

#endif // !defined(AFX_ZIPARCHIVE_H__20010602_3EE9_91B0_80F5_0080AD509054__INCLUDED_)
