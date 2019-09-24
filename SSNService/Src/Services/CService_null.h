#pragma once

#include "CServiceBase.h"
class CService_null :
	public CServiceBase
{
public:
	CService_null(CServiceManager* sm, PSERVICE_INFO si);
	int _work();
};

