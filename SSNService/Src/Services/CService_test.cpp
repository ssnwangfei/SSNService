//#include "pch.h"
#include "CService_test.h"

#include "..\Common\SSNString.h"
#include "..\Common\SSNFile.h"


CService_test::CService_test(CServiceManager* sm, PSERVICE_INFO si):CServiceBase(sm, si)
{
	const wchar_t* path_items[3] = {L"c:\\temp\\",this->_si->wname,L".txt"};
	this->_filePath = SSN::CString::AllocAndCategate(path_items, 3);
}

CService_test::~CService_test()
{
	if (this->_filePath == nullptr) delete this->_filePath;
}

void CService_test::_register_fail() {
	this->WriteToLog("RegisterServiceCtrlHandler failed");
}

void CService_test::_register_ok()
{
	this->WriteToLog("RegisterServiceCtrlHandler success");
}

bool CService_test::_init(SERVICE_STATUS& servicestatus)
{
	return true;
}

int CService_test::_work()
{
	char* pName = SSN::CString::AllocAndWtoA(this->_si->wname);
	this->WriteToLog(pName);
	delete pName;
	return -1;
}

bool CService_test::_uninit()
{
	this->WriteToLog("service stopped");
	return true;
}
DWORD WINAPI CService_test::_ctrl(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{ 
	return ERROR_CALL_NOT_IMPLEMENTED; 
}


int CService_test::WriteToLog(const char* str)
{
	wchar_t* msg = SSN::CString::AllocAndAtoW(str);
	return SSN::CFile::WriteToFile(this->_filePath , msg);
}
