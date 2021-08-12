// Tests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "RegKey.h"
#include "FileReporter.h"
#include "RegKeyChangeReporter.h"

#include "UserInformation.h"
#include "UserInformationParser.h"
#include "FileWatcher.h"
#include "ImpersonateUser.h"
#include "ActiveApplicationReporter.h"

int main()
{
	//auto parser{ make_shared<UserInformationParser>("D:\\info.txt") };

	//wstring main_key{ L"HKCU" };
	//wstring microphone_key{
	//	L"SOFTWARE\\Microsoft\\Windows\\"
	//	"CurrentVersion\\CapabilityAccessManager"
	//	"\\ConsentStore\\webcam" };

	//auto microphone_reg_key{
	//	make_shared<RegKey>(main_key, microphone_key) };
	//auto file_reporter{ make_shared<FileReporter>(
	//	filesystem::path{"D:\\report.txt"}) };
	//auto microphone_watcher{ make_shared<RegKeyChangeReporter>(
	//	L"webcam", microphone_reg_key, file_reporter, parser) };
	//microphone_watcher->start();

	//cin.get();

	//microphone_watcher->stop();


	HWND res =GetForegroundWindow();
	wchar_t lpString[1024] = TEXT("Sample Text");
	GetWindowText(res, lpString, 1024);
	wcout << lpString << endl;

	//auto file_reporter{ make_shared<FileReporter>(
	//	filesystem::path{"D:\\report.txt"}) };
	//auto parser{ make_shared<UserInformationParser>("D:\\info.txt") };
	//auto file_watcher{ make_shared<FileWatcher>(
	//	file_reporter,
	//	filesystem::path{"D:\\log-app.txt"},
	//	filesystem::path{"D:\\log-open-log-file.txt"},
	//	parser
	//) };

	//file_watcher->start();
	//cin.get();

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
