#pragma once
#ifndef CSERVICE_BASE_H
#define CSERVICE_BASE_H

#include "..\CServiceManager.h"
class CServiceBase
{
public:
	CServiceBase(CServiceManager* sm, PSERVICE_INFO si);
	~CServiceBase();

	//ע��ʧ�ܵ��ú���
	virtual void _register_fail() {}
	//ע��ɹ����ú���
	virtual void _register_ok() {}
	//��ʼ������
	virtual bool _init() { return true; }
	//��������������ֵ��ʾ��Ҫ˯�߶��֮���ٴε��á�
	// -1   �� Ĭ��
	// 0    �� ���ϵ���
	// ���� �� ΢��
	virtual int _work() { return -1; }
	//����ʼ������
	virtual bool _uninit() { return true; }
	//���ƺ���
	virtual DWORD WINAPI _ctrl(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext) { return ERROR_CALL_NOT_IMPLEMENTED; }
protected:
	CServiceManager* _sm;
	PSERVICE_INFO _si;
};
#endif