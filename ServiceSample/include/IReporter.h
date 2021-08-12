#pragma once

#include <string>
#include <filesystem>

using namespace std;

class IReporter
{

public:

	virtual ~IReporter() = default;
	
	virtual void println(
		const filesystem::path& file_path, 
		const wstring& line) = 0;

};