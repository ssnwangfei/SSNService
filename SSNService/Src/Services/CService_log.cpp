//#include "pch.h"
#include "CService_log.h"
#include "..\Common\SSNFile.h"
CService_log::CService_log(CServiceManager* sm, PSERVICE_INFO si):CServiceBase(sm, si)
{

}

int CService_log::_work()
{
	MSG msg;
	GetMessage(&msg, NULL, 0, 0);
	SSN::CFile::WriteToFile(L"c:\\temp\\log.txt",L"GET A MESSAGE...");
	return 100;
}
