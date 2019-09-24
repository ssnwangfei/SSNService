#pragma once
#ifndef SSN_String_h
#define SSN_String_h

namespace SSN {
	class CString
	{
	public:
		static wchar_t* AllocAndCopyString(const wchar_t* sourceStr);
		static char* AllocAndCopyString(const char* sourceStr);
		static char* AllocAndWtoA(const wchar_t* wStr);
		static wchar_t* AllocAndAtoW(const char* ansiStr);
		static wchar_t* AllocAndCategate(const wchar_t* sub_items[], int length);
	public:
		CString();
		~CString();
	};
}
#endif
