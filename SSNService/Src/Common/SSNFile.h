#pragma once
#ifndef SSN_File_h
#define SSN_File_h
namespace SSN{
	class CFile
	{
	public:
		static bool WriteToFile(const wchar_t * filePath, const  wchar_t * msg);

	public:
		CFile();
		~CFile();
	};
}
#endif

