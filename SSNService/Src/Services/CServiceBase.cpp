//#include "pch.h"
#include "CServiceBase.h"

CServiceBase::CServiceBase(CServiceManager* sm, PSERVICE_INFO si):_sm(sm), _si(si)
{
	this->_threadId = GetCurrentThreadId();
}

CServiceBase::~CServiceBase()
{

}

DWORD CServiceBase::getThreadId()
{
	return this->_threadId;
}
