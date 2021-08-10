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
		shared_ptr<IReporter> reporter)
		: _name{ name },
		_reg_key{ regkey },
		_reporter{ reporter }
	{
		_watcher = make_unique<RegKeyWatcher<Active>>(
			_active, _reg_key);
	}

	void start() final
	{
		_is_stopped = false;
		init_watch();
	}

	void stop() final
	{
		if (_is_stopped) 
		{
			return;
		}
		_is_stopped = true;
		_active.stop();
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
				_reporter->println(_name + L" is enabled");
				_watcher->watch_change();
			}
			else
			{
				_reporter->println(_name + L" is not enabled");
			}
			init_watch();
		});
	}

	wstring _name;
	bool _is_stopped{ false };
	bool _is_resource_enabled{ false };
	shared_ptr<RegKey> _reg_key;
	unique_ptr<RegKeyWatcher<Active>> _watcher;
	shared_ptr<IReporter> _reporter;
	Active _active;

};