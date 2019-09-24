//#include "pch.h"
#include "SSNFile.h"
#include <iostream>
using namespace SSN;
bool SSN::CFile::WriteToFile(const wchar_t * filePath, const wchar_t * msg)
{
	if (filePath == nullptr || msg == nullptr) return false;
	FILE* pfile;
	_wfopen_s(&pfile, filePath, L"a+,ccs=UTF-16LE");
	if (pfile == NULL)
	{
		return false;
	}
	fwprintf_s(pfile, L"%s\n", msg);
	fclose(pfile);
	return true;
}
SSN::CFile::CFile()
{
}

SSN::CFile::~CFile()
{
}
