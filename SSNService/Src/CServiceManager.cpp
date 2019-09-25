//#include "pch.h"
#include <string>
#include "CServiceManager.h"
#include ".\Services\CServiceBase.h"
#include ".\Services\CService_null.h"
#include ".\Services\CService_test.h"
#include ".\Services\CService_log.h"
#include ".\Common\Log.h"
//定义服务信息
//定义于此数组的服务会被注册成服务
BEGIN_DEFINE_SERVICE_INFO
DEFINE_SERVICE_INFO(test,test)
DEFINE_SERVICE_INFO(testnull,null)
DEFINE_SERVICE_INFO(ssnlog,log)
END_DEFINE_SERVICE_INFO

//用于CServiceManager::CreateService(serviceName)中
#define CREATE_SERVICE_IF(className) if (strcmp(si->name, #className) == 0) {	\
		pRet = new CService_##className(sm, si);}

#define CREATE_SERVICE_ELSE_IF(className) else if (strcmp(si->name, #className) == 0) {	\
		pRet = new CService_##className(sm, si);}

#define CREATE_SERVICE_ELSE 	else {	\
		pRet = nullptr;	}				\

//每一个服务类需要定义一个实例化入口
#define CREATE_SERVICE_IF_ELSE_ALL CREATE_SERVICE_IF(null) \
		CREATE_SERVICE_ELSE_IF(test)					   \
		CREATE_SERVICE_ELSE_IF(log)					   \
		CREATE_SERVICE_ELSE

#define DEFAULT_WORK_SLEEP_TIME 3000 //间隔时间执行一次


void WINAPI  CServiceManager::ServiceMain(int argc, char ** argv)
{
	//Sleep(30 * 1000);
	PSERVICE_INFO si = CServiceManager::getServiceInfo((wchar_t*)argv[0]);

	//服务名
	std::wstring serviceName = (wchar_t*)argv[0];
	//初始化指示
	bool init_ok = false;
	int sleepTime = DEFAULT_WORK_SLEEP_TIME;
	DEBUG_PRINT(serviceName + L": 服务启动...");

	if (si == nullptr) {
		//没有定义，不可能！！！
		DEBUG_PRINT(serviceName + L": 服务没有定义...");
		return;
	}
	CServiceBase* pService;
	try {
		pService = CServiceManager::createService(si->wname);
	}
	catch (...) {
		pService = nullptr;
	}
	if (pService == nullptr) {
		//创建失败
		DEBUG_PRINT(serviceName + L": 创建服务失败...");
		return;
	}
	// 线程Id
	DWORD threadId = pService->getThreadId();
	//注册服务控制器
	si->hStatus = ::RegisterServiceCtrlHandlerEx(si->wname, si->pfunCtrlHandler, si);
	if (si->hStatus == 0)
	{

		try { pService->_register_fail(); }
		catch (...) { 
			DEBUG_PRINT(serviceName + L":回调函数抛出异常[_register_fail]"); 
		}
		delete pService;
		DEBUG_PRINT(serviceName + L": 注册服务失败...");
		return;

	}
	DEBUG_PRINT(serviceName + L": 服务注册成功...");

	SERVICE_STATUS& servicestatus = si->serviceStatus;
	try { pService->_register_ok(); }
	catch (...) {
		DEBUG_PRINT(serviceName + L":回调函数抛出异常[_register_ok]");
		goto exit;
	}

	//回调服务初始化函数
	try {
		init_ok = pService->_init();
	}
	catch (...) {
		DEBUG_PRINT(serviceName + L":回调函数抛出异常[_init]");
		goto exit;
	}
	if (!init_ok) {
		DEBUG_PRINT(serviceName + L": 初始化失败...");
		goto exit;
	}

	si->pService = pService;
	servicestatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(si->hStatus, &servicestatus);
	DEBUG_PRINT(serviceName + L": 初始化完成...");
	DEBUG_PRINT(serviceName + L": 线程Id[" + std::to_wstring(threadId) + L"]");
	//下面就开始任务循环了，你可以添加你自己希望服务做的工作
	si->bRun = true;
	while (si->bRun)
	{

		DEBUG_PRINT(serviceName + L": 服务运行中...");
		try {
			sleepTime = pService->_work();
		}
		catch (...) {
			DEBUG_PRINT(serviceName + L": 工作函数出错，默认时间后重新调用...");
			sleepTime = DEFAULT_WORK_SLEEP_TIME;
		}
		sleepTime = sleepTime == -1 ? DEFAULT_WORK_SLEEP_TIME : sleepTime;
		if (si->bRun) Sleep(sleepTime);
	}

	try {
		pService->_uninit();
	}
	catch (...) {
		DEBUG_PRINT(serviceName + L":回调函数抛出异常[_uninit]");
		goto exit;
	}
exit:
	delete pService;
	si->pService = nullptr;
	DEBUG_PRINT(serviceName + L": 服务退出...");
	si->serviceStatus.dwCurrentState = SERVICE_STOP;
	SetServiceStatus(si->hStatus, &(si->serviceStatus));
}

DWORD WINAPI CServiceManager::ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	PSERVICE_INFO si = (PSERVICE_INFO)lpContext;
	std::wstring serviceName = si->wname;
	if (dwControl == SERVICE_CONTROL_INTERROGATE) return NO_ERROR;
	DWORD rCode = ERROR_CALL_NOT_IMPLEMENTED;
	try{
		rCode = si->pService->_ctrl(dwControl, dwEventType, lpEventData, lpContext);
	}
	catch (...) {
		DEBUG_PRINT(serviceName + L": 回调函数抛出异常[_ctrl]");
	}
	if (rCode == ERROR_CALL_NOT_IMPLEMENTED) {
		switch (dwControl)
		{
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			try {
				si->pService->_request_stop();
			}
			catch (...) {
				DEBUG_PRINT(serviceName + L": 回调函数抛出异常[_stop]");
			}
			si->bRun = false;
			si->serviceStatus.dwCurrentState = SERVICE_STOPPED;
			rCode = NO_ERROR;
			break;
		default:
			rCode = ERROR_CALL_NOT_IMPLEMENTED;
			break;
		}
	}
	return rCode;
}

CServiceManager* CServiceManager::instance = nullptr;
CServiceManager* CServiceManager::getInstance()
{
	if (CServiceManager::instance == nullptr) CServiceManager::instance = new CServiceManager();
	return CServiceManager::instance;
}

PSERVICE_INFO CServiceManager::getServices()
{
	return CServiceManager::services;
}

int CServiceManager::getServiceCount()
{
	int iCount = 0;
	PSERVICE_INFO psi = CServiceManager::getServices();
	while (psi->pfunCtrlHandler != nullptr) { psi++; iCount++; }
	return iCount;
}

PSERVICE_INFO CServiceManager::getServiceInfo(const wchar_t* serviceName)
{
	PSERVICE_INFO psi = CServiceManager::getServices();
	while (psi->pfunCtrlHandler != nullptr) 
	{ 
		if (wcscmp(serviceName,psi->wname) == 0) {
			return psi;
		}
		psi = psi + 1;
	}
	return nullptr;
}

		
		
CServiceBase* CServiceManager::createService(const wchar_t* serviceName)
{
	//返回服务实例
	CServiceManager* sm = CServiceManager::getInstance();
	PSERVICE_INFO si = CServiceManager::getServiceInfo(serviceName);
	CServiceBase* pRet = nullptr;
	CREATE_SERVICE_IF_ELSE_ALL
	return pRet;
}

void CServiceManager::_StartServiceCtrlDispatcher()
{
	int iServices = CServiceManager::getServiceCount();
	SERVICE_TABLE_ENTRY* entrys = new SERVICE_TABLE_ENTRY[iServices + 1];
	SERVICE_INFO* pCurrService = CServiceManager::getServices();
	for (int i = 0; i < iServices; i++) {
		entrys[i].lpServiceName = pCurrService->wname;
		entrys[i].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)CServiceManager::ServiceMain;
		pCurrService ++;
	}

	entrys[iServices].lpServiceName = NULL;
	entrys[iServices].lpServiceProc = NULL;

	::StartServiceCtrlDispatcher(entrys);
}


CServiceManager::CServiceManager()
{

}


CServiceManager::~CServiceManager()
{
}
