#pragma once

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#include <filesystem>
#include <fstream>
#include <mutex>
#include <iostream>

#include "IReporter.h"
#include "FileReporterException.h"

#define INFO_BUFFER_SIZE 32767

using namespace std;

class FileReporter : public IReporter
{

public:

	FileReporter(const filesystem::path& file_path)
		:_file{ file_path, ios::app }
	{
	}

	void println(const wstring& line, const string& username) final
	{
		lock_guard<mutex> lock{ _mutex };
		_file << "username: "
			<< username
			<< " | "
			<< line 
			<< " | time: "
			<< get_time()
			<< endl;
		_file.flush();
	}

	void println(
		const filesystem::path& file_path, 
		const wstring& line,
		const string& username) final
	{
		lock_guard<mutex> lock{ _mutex };
		wofstream file{ file_path, ios::app };
		file << "username: "
			<< wstring{ username.c_str() }
			<< " | "
			<< line
			<< " | time: "
			<< get_time()
			<< endl;
		file.flush();
	}

private:

	//wstring get_user_name() const
	//{
	//	TCHAR _user_name[INFO_BUFFER_SIZE];
	//	DWORD bufCharCount{ INFO_BUFFER_SIZE };
	//	if (!GetUserName(_user_name, &bufCharCount))
	//	{
	//		throw FileReporterException(
	//			"GetUserName error.");
	//	}
	//	return wstring{ _user_name };
	//}

	wstring get_time() const
	{
		time_t rawtime;
		struct tm timeinfo;
		wchar_t buffer[20];
		time(&rawtime);
		localtime_s(&timeinfo, &rawtime);
		wcsftime(buffer, 20, L"%Y-%m-%d %H:%M:%S", &timeinfo);
		return wstring{ buffer };
	}

	wofstream _file;
	mutex _mutex;

};