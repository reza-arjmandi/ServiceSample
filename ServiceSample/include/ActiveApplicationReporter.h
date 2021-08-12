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
#include "ProcessUtil.h"

using namespace darka;

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

				//wchar_t cmd_[1024] = L"D:\\GetActiveWindow.exe";
				//auto app_title_{ ProcessUtil::run_command(cmd_, info) };

				auto search_user = _user_apptitle.find(info.user_name);

				HWND res =GetForegroundWindow();
				wchar_t lpString[1024] = TEXT("Sample Text");
				GetWindowText(res, lpString, 1024);

				wstring app_title{ lpString  };
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

	filesystem::path report_path{ "D:\\log-app.txt" };
	shared_ptr<IReporter> _reporter;
	ImpersonateUser _impersonate_user;
	map<string, wstring> _user_apptitle;
	bool _is_started{ false };
	shared_ptr<UserInformationParser> _user_info_parser;
	unique_ptr<DeadlineTimer<Active>> _timer;
	unique_ptr<Active> _active;

};