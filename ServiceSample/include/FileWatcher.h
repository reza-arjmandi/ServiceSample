#pragma once

#include <memory>
#include <Windows.h>

#include "IMicroService.h"
#include "FileWatch.hpp"
#include "IReporter.h"

using namespace std;
using namespace filewatch;

class FileWatcher : public IMicroService
{

public:

	FileWatcher(shared_ptr<IReporter> reporter,
		const filesystem::path& target_file,
		const filesystem::path& report_file)
		:_reporter{ reporter },
		_target_file{ target_file },
		_report_file{ report_file }
	{
	}

	void start() final
	{
		if (_watch == nullptr)
		{
			_watch = make_unique<FileWatch<filesystem::path>>(
				_target_file,
				[&](const filesystem::path& path, 
					const Event change_type) {
					switch (change_type)
					{
					case Event::added:
						_reporter->println(
							_report_file, 
							L"The file was added to the directory.");
						break;
					case Event::removed:
						_reporter->println(
							_report_file,
							L"The file was removed from the directory.");
						break;
					case Event::modified:
						_reporter->println(
							_report_file,
							L"The file was modified. This can be a change "
							"in the time stamp or attributes.");
						break;
					case Event::renamed_old:
						_reporter->println(
							_report_file,
							L"The file was renamed and this is the old name.");
						break;
					case Event::renamed_new:
						_reporter->println(
							_report_file,
							L"The file was renamed and this is the new name.");
						break;
					};
				});
		}
	}

	void stop() final
	{
		if (_watch == nullptr)
		{
			return;
		}
		_watch.reset();
	}

private:

	filesystem::path _target_file;
	filesystem::path _report_file;
	shared_ptr<IReporter> _reporter;
	unique_ptr<FileWatch<filesystem::path>> _watch{ nullptr };

};