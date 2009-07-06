// Stuff.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"


CString Format(const char* szFormat,...)
{
	va_list vaList;
	va_start(vaList,szFormat);
	CString sBuffer;
	sBuffer.FormatV(szFormat,vaList);
	va_end (vaList);
	return sBuffer; 
}