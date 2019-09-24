//#include "pch.h"
#include "CServiceManager.h"
#include ".\Services\CServiceBase.h"
#include ".\Services\CService_null.h"
#include ".\Services\CService_test.h"
#include ".\Services\CService_log.h"
#include <string>
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

	//
	std::wstring serviceName = (wchar_t*)argv[0];
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

	//注册服务控制器
	si->hStatus = ::RegisterServiceCtrlHandlerEx(si->wname, si->pfunCtrlHandler,si);
	if (si->hStatus == 0)
	{

		pService->_register_fail();
		delete pService;
		DEBUG_PRINT(serviceName + L": 注册服务失败...");
		return;

	}
	pService->_register_ok();
	DEBUG_PRINT(serviceName + L": 服务注册成功...");
	SERVICE_STATUS& servicestatus = si->serviceStatus;
	servicestatus.dwServiceType = SERVICE_WIN32;
	servicestatus.dwCurrentState = SERVICE_START_PENDING;
	servicestatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;//在本例中只接受系统关机和停止服务两种控制命令
	servicestatus.dwWin32ExitCode = 0;
	servicestatus.dwServiceSpecificExitCode = 0;
	servicestatus.dwCheckPoint = 0;
	servicestatus.dwWaitHint = 0;
	
	//向SCM 报告运行状态
	if (!pService->_init(servicestatus)) {

		servicestatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(si->hStatus, &servicestatus);
		delete pService;
		DEBUG_PRINT(serviceName + L": 初始化失败...");
		return;
	}

	si->pService = pService;
	servicestatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(si->hStatus, &servicestatus);
	DEBUG_PRINT(serviceName + L": 初始化完成，开始运行...");
	//下面就开始任务循环了，你可以添加你自己希望服务做的工作
	si->bRun = true;
	while (si->bRun)
	{
		DEBUG_PRINT(serviceName + L": 服务运行中...");
		int sleepTime = pService->_work();
		sleepTime = sleepTime == -1 ? DEFAULT_WORK_SLEEP_TIME : sleepTime;
		if (si->bRun) Sleep(sleepTime);
	}
	pService->_uninit();
	delete pService;
	si->pService = nullptr;
	DEBUG_PRINT(serviceName + L": 服务退出...");
	SetServiceStatus(si->hStatus, &(si->serviceStatus));
}

DWORD WINAPI CServiceManager::ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	PSERVICE_INFO si = (PSERVICE_INFO)lpContext;
	if (dwControl == SERVICE_CONTROL_INTERROGATE) return NO_ERROR;
	DWORD rCode = si->pService->_ctrl(dwControl, dwEventType, lpEventData, lpContext);
	
	if (rCode == ERROR_CALL_NOT_IMPLEMENTED) {
		switch (dwControl)
		{
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
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
