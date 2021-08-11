#pragma once

#include <memory>
#include <vector>

#include "IReporter.h"
#include "Active.h"
#include "RegKeyWatcher.h"
#include "IMicroService.h"
#include "UserInformationParser.h"
#include "ImpersonateUser.h"

using namespace std;
using namespace darka;

class RegKeyChangeReporter : public IMicroService
{

public:

	RegKeyChangeReporter(
		const wstring& name,
		shared_ptr<RegKey> regkey, 
		shared_ptr<IReporter> reporter,
		shared_ptr<UserInformationParser> user_info_parser
		)
		: _name{ name },
		_reg_key{ regkey },
		_reporter{ reporter },
		_user_info_parser{ user_info_parser }
	{
	}

	void start() final
	{
		_is_stopped = false;

		auto users{ _user_info_parser->get_user_informations() };
		for (const auto& user : users)
		{
			_actives.push_back(make_shared<Active>());
			_watchers.push_back(
				make_shared<RegKeyWatcher<Active>>(
					*_actives.back(), _reg_key));
			init_watch(_watchers.back(), user);
		}
	}

	void stop() final
	{
		if (_is_stopped) 
		{
			return;
		}
		_is_stopped = true;
		for (auto& active : _actives)
		{
			active->stop();
		}
	}

private:

	void init_watch(
		shared_ptr<RegKeyWatcher<Active>> watcher, 
		const UserInformation& user_info)
	{
		if (_is_stopped) {
			return;
		}
		watcher->async_watch_change([&, 
			watcher_{ watcher }, 
			user_info_{ user_info }]() {
			_is_resource_enabled = !_is_resource_enabled;
			if (_is_resource_enabled)
			{
				_reporter->println(_name + L" is enabled");
				watcher_->watch_change(user_info_);
			}
			else
			{
				_reporter->println(_name + L" is not enabled");
			}
			init_watch(watcher_, user_info_);
		},
		user_info);
	}

	shared_ptr<UserInformationParser> _user_info_parser;
	wstring _name;
	bool _is_stopped{ false };
	bool _is_resource_enabled{ false };
	shared_ptr<RegKey> _reg_key;
	vector<shared_ptr<RegKeyWatcher<Active>>> _watchers;
	shared_ptr<IReporter> _reporter;
	vector<shared_ptr<Active>> _actives;

};