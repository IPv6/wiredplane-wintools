// StoreCard.cpp : Defines the class behaviors for the application.
//
#ifndef _UTILS_H_123
#define _UTILS_H_123

#include <string>
#include <vector>
#include <iostream>

typedef std::vector<std::string> array_s;

BOOL ReadFile(const char* szFileName, std::string& sFileContent);
BOOL SaveFile(const char* sStartDir, const char* sFileContent);
BOOL isFileExist(const char* szFile);
BOOL isFolder(const char* szFile);
DWORD GetFileSize(const char* szFileName);
std::string GetAppDir(const char* subPath=NULL);
std::wstring _A2W(std::string in);
#include "fsenum.h"
array_s GetFolderList(const char* szFolder, const char* szMask=NULL);
std::string _W2A(const WCHAR* in);
int strReplace(std::string& where, std::string what, std::string with);
std::string getInstringPart(std::string text, std::string begin, std::string end);
void strTrim(std::string& str);
#endif