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
		const filesystem::path& target_file)
		:_reporter{ reporter },
		_target_file{ target_file }
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
					case Event::modified:
						_reporter->println(
							_report_file, L"The file is opened");
						break;
					default:
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
	filesystem::path _report_file{ "C:\\log-open-log-file.txt" };
	shared_ptr<IReporter> _reporter;
	unique_ptr<FileWatch<filesystem::path>> _watch{ nullptr };

};