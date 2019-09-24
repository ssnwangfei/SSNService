#pragma once

#include "CServiceBase.h"
class CService_log :
	public CServiceBase
{
public:
	CService_log(CServiceManager* sm, PSERVICE_INFO si);
	int _work();
};

