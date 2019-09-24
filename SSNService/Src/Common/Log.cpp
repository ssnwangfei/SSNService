//#include "pch.h"
#include "Log.h"
#include "SSNFile.h"
#include "SSNString.h"

Log* Log::instance = nullptr;
Log * Log::getInstance()
{
	if (Log::instance == nullptr) Log::instance = new Log(LOG_FILE, DEBUG_FILE);
	return Log::instance;
}

bool Log::writeToDebug(const wchar_t * msg)
{
	return this->writeToFile(this->_debugFilePath, msg);
}

bool Log::writeToLog(const wchar_t * msg)
{
	return this->writeToFile(this->_logFilePath, msg);
}

bool Log::writeToDebug(const char * msg)
{
	wchar_t* wmsg = SSN::CString::AllocAndAtoW(msg);
	bool bRet = this->writeToFile(this->_debugFilePath, wmsg);
	if (wmsg != nullptr)delete wmsg;
	return bRet;
}

bool Log::writeToLog(const char * msg)
{
	wchar_t* wmsg = SSN::CString::AllocAndAtoW(msg);
	bool bRet = this->writeToFile(this->_logFilePath, wmsg);
	if (wmsg != nullptr)delete wmsg;
	return bRet;
}


bool Log::writeToFile(const wchar_t * filePath, const wchar_t * msg)
{
	return SSN::CFile::WriteToFile(filePath, msg);
}

Log::Log(const wchar_t* logFilePath,const wchar_t* debugFilePath)
{
	this->_debugFilePath = SSN::CString::AllocAndCopyString(debugFilePath);
	this->_logFilePath = SSN::CString::AllocAndCopyString(logFilePath);
}


Log::~Log()
{
	if (this->_debugFilePath != nullptr) delete this->_debugFilePath;
	if (this->_logFilePath != nullptr) delete this->_logFilePath;
}

bool writeToDebug(const char * msg)
{
	return writeToDebug(SSN::CString::AllocAndAtoW(msg));
}

bool writeToLog(const char * msg)
{
	return writeToLog(SSN::CString::AllocAndAtoW(msg));
}

bool writeToDebug(const wchar_t * msg)
{
	return Log::getInstance()->writeToDebug(msg);
}

bool writeToLog(const wchar_t * msg)
{
	return Log::getInstance()->writeToLog(msg);
}

bool writeToDebug(std::wstring  msg)
{
	return writeToDebug(msg.c_str());
}

bool writeToLog(std::wstring msg)
{
	return writeToLog(msg.c_str());
}
