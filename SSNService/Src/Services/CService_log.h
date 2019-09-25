#pragma once

#include "CServiceBase.h"
class CService_log :
	public CServiceBase
{
public:
	CService_log(CServiceManager* sm, PSERVICE_INFO si);
	DWORD WINAPI _ctrl(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);
	int _work();
};

