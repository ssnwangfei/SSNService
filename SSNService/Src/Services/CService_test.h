#pragma once

#ifndef CSERVICE_BASE_test_H
#define CSERVICE_BASE_test_H

#include "CServiceBase.h"

class CService_test :
	public CServiceBase
{
public:
	CService_test(CServiceManager* sm, PSERVICE_INFO si);
	void _register_fail();
	void _register_ok();
	bool _init(SERVICE_STATUS& servicestatus);
	int  _work();
	bool _uninit();
	DWORD WINAPI _ctrl(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);
	int WriteToLog(const char* str);
	~CService_test();
private:
	wchar_t* _filePath;
};

#endif