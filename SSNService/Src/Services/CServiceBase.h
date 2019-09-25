#pragma once
#ifndef CSERVICE_BASE_H
#define CSERVICE_BASE_H

#include "..\CServiceManager.h"
class CServiceBase
{
public:
	CServiceBase(CServiceManager* sm, PSERVICE_INFO si);
	~CServiceBase();

	//注册失败调用函数
	virtual void _register_fail() {}
	//注册成功调用函数
	virtual void _register_ok() {}
	//初始化函数
	virtual bool _init() { return true; }
	//工作函数，返回值表示需要睡眠多久之后再次调用。
	// -1   ： 默认
	// 0    ： 马上调用
	// 正数 ： 微秒
	virtual int _work() { return -1; }
	//反初始化函数
	virtual bool _uninit() { return true; }
	//控制函数
	virtual DWORD WINAPI _ctrl(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) { return ERROR_CALL_NOT_IMPLEMENTED; }
protected:
	CServiceManager* _sm;
	PSERVICE_INFO _si;
};
#endif