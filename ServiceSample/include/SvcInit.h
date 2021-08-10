#pragma once

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <string>
#include <memory>
#include <filesystem>

#include "Globals.h"
#include "ReportSvcStatus.h"
#include "RegKeyChangeReporter.h"
#include "FileWatcher.h"
#include "FileReporter.h"
#include "StartMicroServiceGuard.h"
#include "SvcReportEvent.h"

using namespace std;

auto init_services()
{
    try {
        wstring main_key{ L"HKCU" };
        wstring webcam_key{
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion"
            "\\CapabilityAccessManager\\ConsentStore\\webcam" };
        wstring microphone_key{
            L"SOFTWARE\\Microsoft\\Windows\\"
            "CurrentVersion\\CapabilityAccessManager"
            "\\ConsentStore\\microphone" };

        auto webcam_reg_key{
            make_shared<RegKey>(main_key, webcam_key) };
        auto microphone_reg_key{
            make_shared<RegKey>(main_key, microphone_key) };
        auto file_reporter{ make_shared<FileReporter>(
            filesystem::path{"D:\\report.txt"}) };

        auto webcam_watcher{ make_shared<RegKeyChangeReporter>(
            L"webcam", webcam_reg_key, file_reporter) };
        auto microphone_watcher{ make_shared<RegKeyChangeReporter>(
            L"microphone", microphone_reg_key, file_reporter) };
        auto file_watcher{ make_shared<FileWatcher>(
            file_reporter,
            filesystem::path{"D:\\log-app.txt"},
            filesystem::path{"D:\\log-open-log-file.txt"}
        ) };

        return make_tuple(
            make_shared<StartMicroServiceGuard>(webcam_watcher),
            make_shared<StartMicroServiceGuard>(microphone_watcher),
            make_shared<StartMicroServiceGuard>(file_watcher)
        );
    }
    catch (const exception& ex) {
        auto size_{ strlen(ex.what()) };
        wstring wc(size_, L'#');
        mbstowcs(&wc[0], ex.what(), size_);
        SvcReportEvent(wc.data());

        return make_tuple(
            make_shared<StartMicroServiceGuard>(nullptr),
            make_shared<StartMicroServiceGuard>(nullptr),
            make_shared<StartMicroServiceGuard>(nullptr)
        );
    }
}

//
// Purpose: 
//   The service code
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None
//
VOID SvcInit(DWORD dwArgc, LPTSTR* lpszArgv)
{
    // TO_DO: Declare and set any required variables.
    //   Be sure to periodically call ReportSvcStatus() with 
    //   SERVICE_START_PENDING. If initialization fails, call
    //   ReportSvcStatus with SERVICE_STOPPED.

    // Create an event. The control handler function, SvcCtrlHandler,
    // signals this event when it receives the stop control code.

    ghSvcStopEvent = CreateEvent(
        NULL,    // default security attributes
        TRUE,    // manual reset event
        FALSE,   // not signaled
        NULL);   // no name

    if (ghSvcStopEvent == NULL)
    {
        ReportSvcStatus(SERVICE_STOPPED, GetLastError(), 0);
        return;
    }

    // Report running status when initialization is complete.
    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

    // TO_DO: Perform work until service stops.
    auto [start_webcam, start_mic, start_file] = init_services();
    if (start_webcam == nullptr 
        || start_mic == nullptr 
        || start_file == nullptr)
    {
        ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
        return;
    }

    while (1)
    {
        // Check whether to stop the service.
        WaitForSingleObject(ghSvcStopEvent, INFINITE);
        ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);

        return;
    }
}