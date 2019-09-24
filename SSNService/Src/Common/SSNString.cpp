//#include "pch.h"
#include "SSNString.h"
#include <Windows.h>

using namespace SSN;
wchar_t* SSN::CString::AllocAndCopyString(const wchar_t * sourceStr)
{
	wchar_t* destStr = nullptr;
	size_t len = wcslen(sourceStr);
	try {
		destStr = new wchar_t[len + 1];
	}
	catch (...) {
		return destStr;
	}
	wcscpy_s(destStr, len + 1, sourceStr);
	return destStr;
}

char* SSN::CString::AllocAndCopyString(const char * sourceStr)
{
	char* destStr = nullptr;
	size_t len = strlen(sourceStr);
	try {
		destStr = new char[len + 1];
	}
	catch (...) {
		return destStr;
	}
	strcpy_s(destStr, len + 1, sourceStr);
	return destStr;
}

char* SSN::CString::AllocAndWtoA(const wchar_t * wStr)
{
	int size = WideCharToMultiByte(CP_ACP, 0, wStr, -1, 0, 0, 0, 0);
	char* pRet = nullptr;
	try {
		pRet = new char[size];
	}
	catch (...) {
		return nullptr;
	}
	memset(pRet, 0, size * sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, wStr, -1, pRet, size, 0, 0);
	return pRet;
}

wchar_t * SSN::CString::AllocAndAtoW(const char * ansiStr)
{
	int size = MultiByteToWideChar(CP_ACP, 0, ansiStr, -1, 0, 0);
	wchar_t* pRet = nullptr;
	try {
		pRet = new WCHAR[size];
	}
	catch (...) {
		return pRet;
	}
	memset(pRet, 0, size * sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, ansiStr, -1, pRet, size);
	return pRet;
}

wchar_t * SSN::CString::AllocAndCategate(const wchar_t * sub_items[], int length)
{
	size_t buffer_len = 0;
	for (int i = 0; i < length; i++) {
		buffer_len += wcslen(sub_items[i]);
	}
	buffer_len++;

	wchar_t* pRet = nullptr;
	try {
		pRet = new wchar_t[buffer_len];
	}
	catch (...) {
		return pRet;
	}
	pRet[0] = '\0';

	for (int i = 0; i < length; i++) {
		wcscat_s(pRet,buffer_len,sub_items[i]);
	}
	return pRet;
}

SSN::CString::CString()
{
}

SSN::CString::~CString()
{
}

