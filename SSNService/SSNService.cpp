// test_console.cpp : 定义控制台应用程序的入口点。
//

//#include "pch.h"
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include ".\Src\CServiceManager.h"
using namespace std;

int main(int argc, TCHAR* argv[])
{
	CServiceManager::_StartServiceCtrlDispatcher();

	return 0;
}