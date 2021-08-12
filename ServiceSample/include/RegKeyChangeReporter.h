#pragma once

#include <memory>

#include "IReporter.h"
#include "Active.h"
#include "RegKeyWatcher.h"
#include "IMicroService.h"

using namespace std;

class RegKeyChangeReporter : public IMicroService
{

public:

	RegKeyChangeReporter(
		const wstring& name,
		shared_ptr<RegKey> regkey,
		const filesystem::path& report_file_path,
		shared_ptr<IReporter> reporter
	)
		: _name{ name },
		_reg_key{ regkey },
		_report_file_path{ report_file_path },
		_reporter{ reporter }
	{
	}

	void start() final
	{
		_is_stopped = false;

		_active = make_shared<Active>();
		_watcher = 
			make_shared<RegKeyWatcher<Active>>(*_active, _reg_key);
		init_watch();
	}

	void stop() final
	{
		if (_is_stopped)
		{
			return;
		}
		_is_stopped = true;
		_watcher->release_wait();
		_active->stop();
	}

private:

	void init_watch()
	{
		if (_is_stopped) {
			return;
		}
		_watcher->async_watch_change([&]() {
			_is_resource_enabled = !_is_resource_enabled;
			if (_is_resource_enabled)
			{
				_reporter->println(_report_file_path, _name + L" is active");
			}
			else
			{
				_reporter->println(_report_file_path, _name + L" becomes inactive");
			}
			init_watch();
		});
	}

	filesystem::path _report_file_path;
	wstring _name;
	bool _is_stopped{ false };
	bool _is_resource_enabled{ false };
	shared_ptr<RegKey> _reg_key;
	shared_ptr<RegKeyWatcher<Active>> _watcher;
	shared_ptr<IReporter> _reporter;
	shared_ptr<Active> _active;

};