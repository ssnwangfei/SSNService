//#include "pch.h"
#include "CService_null.h"

CService_null::CService_null(CServiceManager* sm, PSERVICE_INFO si):CServiceBase(sm, si)
{

}

int CService_null::_work()
{
	return 100;
}
