#include <iostream>

#include "RegKey.h"
#include "FileReporter.h"
#include "RegKeyChangeReporter.h"
#include "ActiveApplicationReporter.h"

int main()
{
    auto file_reporter{ make_shared<FileReporter>() };
    auto _active_app_reporter{ make_shared<ActiveApplicationReporter>(file_reporter) };
    _active_app_reporter->start();

    cin.get();

    _active_app_reporter->stop();
}

