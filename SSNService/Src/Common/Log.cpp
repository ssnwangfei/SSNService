//#include "pch.h"
#include "Log.h"
#include "SSNFile.h"
#include "SSNString.h"
SSN::Log* SSN::Log::instance = nullptr;
SSN::Log * SSN::Log::getInstance()
{
	if (Log::instance == nullptr) Log::instance = new Log(LOG_FILE, DEBUG_FILE);
	return Log::instance;
}

bool SSN::Log::writeToDebug(const wchar_t * msg)
{
	return this->writeToFile(this->_debugFilePath, msg);
}

bool SSN::Log::writeToLog(const wchar_t * msg)
{
	return this->writeToFile(this->_logFilePath, msg);
}

bool SSN::Log::writeToDebug(const char * msg)
{
	wchar_t* wmsg = SSN::CString::AllocAndAtoW(msg);
	bool bRet = this->writeToFile(this->_debugFilePath, wmsg);
	if (wmsg != nullptr)delete wmsg;
	return bRet;
}

bool SSN::Log::writeToLog(const char * msg)
{
	wchar_t* wmsg = SSN::CString::AllocAndAtoW(msg);
	bool bRet = this->writeToFile(this->_logFilePath, wmsg);
	if (wmsg != nullptr)delete wmsg;
	return bRet;
}


bool SSN::Log::writeToFile(const wchar_t * filePath, const wchar_t * msg)
{
	return SSN::CFile::WriteToFile(filePath, msg);
}

SSN::Log::Log(const wchar_t* logFilePath,const wchar_t* debugFilePath)
{
	this->_debugFilePath = SSN::CString::AllocAndCopyString(debugFilePath);
	this->_logFilePath = SSN::CString::AllocAndCopyString(logFilePath);
}


SSN::Log::~Log()
{
	if (this->_debugFilePath != nullptr) delete this->_debugFilePath;
	if (this->_logFilePath != nullptr) delete this->_logFilePath;
}

bool SSN::writeToDebug(const char * msg)
{
	return SSN::writeToDebug(SSN::CString::AllocAndAtoW(msg));
}

bool SSN::writeToLog(const char * msg)
{
	return SSN::writeToLog(SSN::CString::AllocAndAtoW(msg));
}

bool SSN::writeToDebug(const wchar_t * msg)
{
	return SSN::Log::getInstance()->writeToDebug(msg);
}

bool SSN::writeToLog(const wchar_t * msg)
{
	return SSN::Log::getInstance()->writeToLog(msg);
}

bool SSN::writeToDebug(std::wstring  msg)
{
	return SSN::writeToDebug(msg.c_str());
}

bool SSN::writeToLog(std::wstring msg)
{
	return SSN::writeToLog(msg.c_str());
}
