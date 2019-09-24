#pragma once
#ifndef LOG_H
#define LOG_H

#include <string>
#define LOG_FILE L"c:\\temp\\log.txt"
#define DEBUG_FILE L"c:\\temp\\debug.txt"
class Log
{
private:
	static Log* instance;
private:
	wchar_t* _logFilePath;
	wchar_t* _debugFilePath;
public:
	static Log* getInstance();
	bool writeToDebug(const wchar_t* msg);
	bool writeToLog(const wchar_t* msg);
	bool writeToDebug(const char* msg);
	bool writeToLog(const char* msg);
	bool writeToFile(const wchar_t* filePath, const  wchar_t* msg);
	Log(const wchar_t* logFilePath,const wchar_t* debugFilePath);
	~Log();
};

bool writeToDebug(const char* msg);
bool writeToLog(const char* msg);
bool writeToDebug(const wchar_t* msg);
bool writeToLog(const wchar_t* msg);
bool writeToDebug(std::wstring msg);
bool writeToLog(std::wstring msg);
#ifdef _DEBUG
#define DEBUG_PRINT(msg) writeToDebug(msg)
#elif
#define DEBUG_PRINT(msg)
#endif

#define LOG_PRINT(msg) writeToLog(msg)

#endif