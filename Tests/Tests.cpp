//// Tests.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
//
#include <iostream>

#include "RegKey.h"
#include "FileReporter.h"
#include "RegKeyChangeReporter.h"

#include "UserInformation.h"
#include "UserInformationParser.h"
#include "FileWatcher.h"
#include "ImpersonateUser.h"
//#include "ActiveApplicationReporter.h"

int main()
{
	wstring main_key{ L"HKCU" };
	wstring microphone_key{
		L"SOFTWARE\\Microsoft\\Windows\\"
		"CurrentVersion\\CapabilityAccessManager"
		"\\ConsentStore\\microphone" };

	auto microphone_reg_key{
		make_shared<RegKey>(main_key, microphone_key) };
	auto file_reporter{ make_shared<FileReporter>() };
	auto microphone_watcher{ make_shared<RegKeyChangeReporter>(
		L"microphone", microphone_reg_key, filesystem::path{"C:\\log-microphone.txt"}, file_reporter) };
	microphone_watcher->start();

	cin.get();

}

