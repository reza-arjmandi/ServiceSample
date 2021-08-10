#pragma once

#include <winuser.h>

#include <fstream>
#include <memory>
#include <string>
#include "atlstr.h"

#include "Active.h"
#include "DeadlineTimer.h"

using namespace std;

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <stdio.h>

#include <assert.h>

#include <windows.h>

#include <lm.h>

class ReportActiveApplication
{

public:
	
	void init()
	{
		_active = make_unique<Active>();
		_timer = make_unique<DeadlineTimer<Active>>(*_active);
		init_timer();
	}


private:

	void init_timer()
	{
		_timer->expires_from_now(chrono::milliseconds{ 3500 });
		_timer->async_wait([&]() {
			HWND res = GetCapture();
			init_timer();
		});
	}

	ofstream _report_file{ "C:\\report.txt" };
	unique_ptr<DeadlineTimer<Active>> _timer;
	unique_ptr<Active> _active;

};