#pragma once

#ifndef CSERVICE_MANAGER_H
#define CSERVICE_MANAGER_H

#include <Windows.h>
class CServiceBase;

constexpr auto MAX_SERVICE_NAME_LENGTH = 100;			    //服务名长度;
constexpr auto MAX_CLASS_NAME_LENGTH = 20;
//服务信息
typedef struct tag_SERVICE_INFO {
	wchar_t wname[MAX_SERVICE_NAME_LENGTH];
	char name[MAX_CLASS_NAME_LENGTH];
	LPHANDLER_FUNCTION_EX pfunCtrlHandler;			    //服务控制处理函数指针
	bool bRun;									    //控制是否运行
	SERVICE_STATUS serviceStatus;					//服务状态
	SERVICE_STATUS_HANDLE hStatus;					//状态句柄  
	CServiceBase* pService;								//服务
}SERVICE_INFO, *PSERVICE_INFO;


/*#define SERVICE_CTRL_HANDLER(serviceName)									\
	static void WINAPI ServiceCtrlHandler_##serviceName(DWORD dwControl){			\
		PSERVICE_INFO si = getServiceInfo(L#serviceName);		\
		ServiceCtrlHandler(si,dwControl);				\
    }*/

#define BEGIN_DEFINE_SERVICE_INFO		\
SERVICE_INFO CServiceManager::services[] = {

#define END_DEFINE_SERVICE_INFO		\
	{L"","",nullptr,false,{},0}					\
};

#define DEFINE_SERVICE_INFO(serviceName,className) \
	{L#serviceName, #className, ServiceCtrlHandler,false,{},0},

class CServiceManager
{
public:
	//定义服务控制函数
	//每个服务需要定义一个控制处理器
	//SERVICE_CTRL_HANDLER(test)
	//SERVICE_CTRL_HANDLER(testnull)

	static CServiceManager* getInstance();
	static PSERVICE_INFO getServices();
	static int getServiceCount();
	static PSERVICE_INFO getServiceInfo(const wchar_t* serviceName);
	static CServiceBase* createService(const wchar_t* className);
	static void _StartServiceCtrlDispatcher();

protected:
	CServiceManager();
	static CServiceManager* instance;
	static SERVICE_INFO services[];
	static void WINAPI ServiceMain(int argc, char** argv);
	static DWORD WINAPI ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

public:
	~CServiceManager();

private:

};


#endif

