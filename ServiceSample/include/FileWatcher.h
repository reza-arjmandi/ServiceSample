#pragma once

#include <memory>
#include <vector>
#include <Windows.h>

#include "IMicroService.h"
#include "FileWatch.hpp"
#include "IReporter.h"
#include "UserInformationParser.h"
#include "ImpersonateUser.h"

using namespace std;
using namespace filewatch;
using namespace darka;

class FileWatcher : public IMicroService
{

public:

	FileWatcher(shared_ptr<IReporter> reporter,
		const filesystem::path& target_file,
		const filesystem::path& report_file,
		shared_ptr<UserInformationParser> user_info_parser)
		:_reporter{ reporter },
		_target_file{ target_file },
		_report_file{ report_file },
		_user_info_parser{ user_info_parser }
	{
	}

	void start() final
	{
		if (!_watchers.empty())
		{
			return;
		}
		auto user_info_list{ _user_info_parser->get_user_informations() };
		for (const auto& info : user_info_list)
		{
			_impersonate_user.Logon(info.user_name, info.domain, info.password);
			auto watcher{ make_shared<FileWatch<filesystem::path>>(
			_target_file,
				[&, user_info_{info}](const filesystem::path& path,
					const Event change_type) {
						switch (change_type)
						{
						case Event::added:
							_reporter->println(
								_report_file,
								L"The file was added to the directory.",
								user_info_.user_name);
							break;
						case Event::removed:
							_reporter->println(
								_report_file,
								L"The file was removed from the directory.",
								user_info_.user_name);
							break;
						case Event::modified:
							_reporter->println(
								_report_file,
								L"The file was modified. This can be a change "
								"in the time stamp or attributes.",
								user_info_.user_name);
							break;
						case Event::renamed_old:
							_reporter->println(
								_report_file,
								L"The file was renamed and this is the old name.",
								user_info_.user_name);
							break;
						case Event::renamed_new:
							_reporter->println(
								_report_file,
								L"The file was renamed and this is the new name.",
								user_info_.user_name);
							break;
						};
			}) };
			_watchers.push_back(watcher);
		}

	}

	void stop() final
	{
		if (_watchers.empty())
		{
			return;
		}
		_watchers.clear();
	}

private:

	ImpersonateUser _impersonate_user;
	shared_ptr<UserInformationParser> _user_info_parser;
	filesystem::path _target_file;
	filesystem::path _report_file;
	shared_ptr<IReporter> _reporter;
	vector<shared_ptr<FileWatch<filesystem::path>>> _watchers;

};