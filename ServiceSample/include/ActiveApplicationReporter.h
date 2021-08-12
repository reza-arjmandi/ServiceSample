#pragma once

#include <winuser.h>

#include <fstream>
#include <memory>
#include <string>
#include <map>
#include <filesystem>

#include "atlstr.h"
#include "Active.h"
#include "DeadlineTimer.h"
#include "UserInformationParser.h"
#include "IMicroService.h"
#include "ImpersonateUser.h"
#include "IReporter.h"

using namespace darka;


#define BUFSIZE 4096

TCHAR szCmdline[] = TEXT("Test.exe");

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;

class ActiveApplicationReporter : public IMicroService
{

public:

	ActiveApplicationReporter(
		shared_ptr<UserInformationParser> user_info_parser,
		shared_ptr<IReporter> reporter)
		:_user_info_parser{ user_info_parser },
		_reporter{ reporter }
	{
		init();
	}

	void start() final
	{
		if (_is_started) {
			return;
		}
		_is_started = true;
		init_timer();
	}

	void stop() final
	{
		if (!_is_started)
		{
			return;
		}
		_is_started = false;
		_active->stop();
	}

private:

	void init()
	{
		_active = make_unique<Active>();
		_timer = make_unique<DeadlineTimer<Active>>(*_active);
	}

	void init_timer()
	{
		_timer->expires_from_now(chrono::milliseconds{ 500 });
		_timer->async_wait([&]() {
			auto users_info{ _user_info_parser->get_user_informations() };
			for (const auto& info : users_info)
			{
				_impersonate_user.Logon(info.user_name, info.domain, info.password);

				SECURITY_ATTRIBUTES saAttr;

				// Set the bInheritHandle flag so pipe handles are inherited.
				saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
				saAttr.bInheritHandle = TRUE;
				saAttr.lpSecurityDescriptor = NULL;

				// Create a pipe for the child process's STDOUT.
				if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
					ErrorExit("StdoutRd CreatePipe");

				// Ensure the read handle to the pipe for STDOUT is not inherited.
				if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
					ErrorExit("Stdout SetHandleInformation");

				CreateChildProcess();
				auto app_title_{ ReadFromPipe() };

				auto search_user = _user_apptitle.find(info.user_name);
				auto app_title{ wstring{ app_title_.begin(), app_title_.end() } };
				if (search_user != _user_apptitle.end()) {
					if (search_user->second == app_title) {
						break;
					}
				}
				_user_apptitle[info.user_name] = app_title;

				_reporter->println(report_path, L" active window is : " + app_title, info.user_name);


				_impersonate_user.Logoff();
			}
			init_timer();
			});
	}

	void CreateChildProcess()
		// Create a child process that uses the previously created pipes for STDIN and STDOUT.
	{
		PROCESS_INFORMATION piProcInfo;
		STARTUPINFO siStartInfo;
		BOOL bSuccess = FALSE;

		// Set up members of the PROCESS_INFORMATION structure.
		ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

		// Set up members of the STARTUPINFO structure.
		// This structure specifies the STDIN and STDOUT handles for redirection.

		ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
		siStartInfo.cb = sizeof(STARTUPINFO);
		siStartInfo.hStdError = g_hChildStd_OUT_Wr;
		siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
		siStartInfo.hStdInput = g_hChildStd_IN_Rd;
		siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

		// Create the child process.
		bSuccess = CreateProcess(NULL,
			szCmdline,     // command line
			NULL,          // process security attributes
			NULL,          // primary thread security attributes
			TRUE,          // handles are inherited
			0,             // creation flags
			NULL,          // use parent's environment
			NULL,          // use parent's current directory
			&siStartInfo,  // STARTUPINFO pointer
			&piProcInfo);  // receives PROCESS_INFORMATION

		 // If an error occurs, exit the application.
		if (!bSuccess)
			ErrorExit("CreateProcess");
		else
		{
			// Close handles to the child process and its primary thread.
			// Some applications might keep these handles to monitor the status
			// of the child process, for example.

			CloseHandle(piProcInfo.hProcess);
			CloseHandle(piProcInfo.hThread);
		}
	}

	string ReadFromPipe(void)

		// Read output from the child process's pipe for STDOUT
		// and write to the parent process's pipe for STDOUT.
		// Stop when there is no more data.
	{
		DWORD dwRead, dwWritten;
		CHAR chBuf[BUFSIZE];
		BOOL bSuccess = FALSE;
		HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		return string{ chBuf };
	}

	void ErrorExit(char* msg)

		// Format a readable error message, display a message box,
		// and exit from the application.
	{
		//printf("%s\n", msg);
		//exit(1);
	}

	filesystem::path report_path{ "D:\\log-app.txt" };
	shared_ptr<IReporter> _reporter;
	ImpersonateUser _impersonate_user;
	map<string, wstring> _user_apptitle;
	bool _is_started{ false };
	shared_ptr<UserInformationParser> _user_info_parser;
	unique_ptr<DeadlineTimer<Active>> _timer;
	unique_ptr<Active> _active;

};