//#include "pch.h"
#include <string>
#include "CServiceManager.h"
#include ".\Services\CServiceBase.h"
#include ".\Services\CService_null.h"
#include ".\Services\CService_test.h"
#include ".\Services\CService_log.h"
#include ".\Common\Log.h"
//���������Ϣ
//�����ڴ�����ķ���ᱻע��ɷ���
BEGIN_DEFINE_SERVICE_INFO
DEFINE_SERVICE_INFO(test,test)
DEFINE_SERVICE_INFO(testnull,null)
DEFINE_SERVICE_INFO(ssnlog,log)
END_DEFINE_SERVICE_INFO

//����CServiceManager::CreateService(serviceName)��
#define CREATE_SERVICE_IF(className) if (strcmp(si->name, #className) == 0) {	\
		pRet = new CService_##className(sm, si);}

#define CREATE_SERVICE_ELSE_IF(className) else if (strcmp(si->name, #className) == 0) {	\
		pRet = new CService_##className(sm, si);}

#define CREATE_SERVICE_ELSE 	else {	\
		pRet = nullptr;	}				\

//ÿһ����������Ҫ����һ��ʵ�������
#define CREATE_SERVICE_IF_ELSE_ALL CREATE_SERVICE_IF(null) \
		CREATE_SERVICE_ELSE_IF(test)					   \
		CREATE_SERVICE_ELSE_IF(log)					   \
		CREATE_SERVICE_ELSE

#define DEFAULT_WORK_SLEEP_TIME 3000 //���ʱ��ִ��һ��


void WINAPI  CServiceManager::ServiceMain(int argc, char ** argv)
{
	//Sleep(30 * 1000);
	PSERVICE_INFO si = CServiceManager::getServiceInfo((wchar_t*)argv[0]);

	//������
	std::wstring serviceName = (wchar_t*)argv[0];
	//��ʼ��ָʾ
	bool init_ok = false;
	int sleepTime = DEFAULT_WORK_SLEEP_TIME;
	DEBUG_PRINT(serviceName + L": ��������...");

	if (si == nullptr) {
		//û�ж��壬�����ܣ�����
		DEBUG_PRINT(serviceName + L": ����û�ж���...");
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
		//����ʧ��
		DEBUG_PRINT(serviceName + L": ��������ʧ��...");
		return;
	}
	// �߳�Id
	DWORD threadId = pService->getThreadId();
	//ע����������
	si->hStatus = ::RegisterServiceCtrlHandlerEx(si->wname, si->pfunCtrlHandler, si);
	if (si->hStatus == 0)
	{

		try { pService->_register_fail(); }
		catch (...) { 
			DEBUG_PRINT(serviceName + L":�ص������׳��쳣[_register_fail]"); 
		}
		delete pService;
		DEBUG_PRINT(serviceName + L": ע�����ʧ��...");
		return;

	}
	DEBUG_PRINT(serviceName + L": ����ע��ɹ�...");

	SERVICE_STATUS& servicestatus = si->serviceStatus;
	try { pService->_register_ok(); }
	catch (...) {
		DEBUG_PRINT(serviceName + L":�ص������׳��쳣[_register_ok]");
		goto exit;
	}

	//�ص������ʼ������
	try {
		init_ok = pService->_init();
	}
	catch (...) {
		DEBUG_PRINT(serviceName + L":�ص������׳��쳣[_init]");
		goto exit;
	}
	if (!init_ok) {
		DEBUG_PRINT(serviceName + L": ��ʼ��ʧ��...");
		goto exit;
	}

	si->pService = pService;
	servicestatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(si->hStatus, &servicestatus);
	DEBUG_PRINT(serviceName + L": ��ʼ�����...");
	DEBUG_PRINT(serviceName + L": �߳�Id[" + std::to_wstring(threadId) + L"]");
	//����Ϳ�ʼ����ѭ���ˣ������������Լ�ϣ���������Ĺ���
	si->bRun = true;
	while (si->bRun)
	{

		DEBUG_PRINT(serviceName + L": ����������...");
		try {
			sleepTime = pService->_work();
		}
		catch (...) {
			DEBUG_PRINT(serviceName + L": ������������Ĭ��ʱ������µ���...");
			sleepTime = DEFAULT_WORK_SLEEP_TIME;
		}
		sleepTime = sleepTime == -1 ? DEFAULT_WORK_SLEEP_TIME : sleepTime;
		if (si->bRun) Sleep(sleepTime);
	}

	try {
		pService->_uninit();
	}
	catch (...) {
		DEBUG_PRINT(serviceName + L":�ص������׳��쳣[_uninit]");
		goto exit;
	}
exit:
	delete pService;
	si->pService = nullptr;
	DEBUG_PRINT(serviceName + L": �����˳�...");
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
		DEBUG_PRINT(serviceName + L": �ص������׳��쳣[_ctrl]");
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
				DEBUG_PRINT(serviceName + L": �ص������׳��쳣[_stop]");
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
	//���ط���ʵ��
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
