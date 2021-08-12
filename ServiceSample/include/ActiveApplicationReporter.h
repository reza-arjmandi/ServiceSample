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
#include "IMicroService.h"
#include "ImpersonateUser.h"
#include "IReporter.h"
#include "ProcessUtil.h"

using namespace darka;

class ActiveApplicationReporter : public IMicroService
{

public:

	ActiveApplicationReporter(
		shared_ptr<IReporter> reporter)
		: _reporter{ reporter }
	{
		ofstream{ _report_path };
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
			wstring window_title{ get_active_window_title() };
			if(window_title != _last_window_title)
			{
				_last_window_title = window_title;
				_reporter->println(_report_path, L" active window is : " + window_title);
			}
			init_timer();
		});
	}

	wstring get_active_window_title() const
	{
		HWND window_handle = GetForegroundWindow();
		wchar_t window_title[1024] = TEXT("Sample Text");
		GetWindowText(window_handle, window_title, 1024);
		return wstring { window_title };
	}

	filesystem::path _report_path{ "C:\\log-app.txt" };
	shared_ptr<IReporter> _reporter;
	wstring _last_window_title{ L"" };
	bool _is_started{ false };
	unique_ptr<DeadlineTimer<Active>> _timer;
	unique_ptr<Active> _active;

};