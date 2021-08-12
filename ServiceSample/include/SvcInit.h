#pragma once

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <string>
#include <memory>
#include <filesystem>
#include <vector>

#include "Globals.h"
#include "ReportSvcStatus.h"
#include "RegKeyChangeReporter.h"
#include "FileWatcher.h"
#include "FileReporter.h"
#include "StartMicroServiceGuard.h"
#include "SvcReportEvent.h"
#include "UserInformationParser.h"
#include "ActiveApplicationReporter.h"

using namespace std;

vector<shared_ptr<StartMicroServiceGuard>> init_services()
{
    vector<shared_ptr<StartMicroServiceGuard>> services;
    try {
        auto file_reporter{ make_shared<FileReporter>() };
        
        auto active_app_reporter{
            make_shared<ActiveApplicationReporter>(
                file_reporter)
        };
        services.push_back(
            make_shared<StartMicroServiceGuard>(active_app_reporter)
        );

        wstring main_key{ L"HKCU" };
        wstring webcam_key{
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion"
            "\\CapabilityAccessManager\\ConsentStore\\webcam" };
        auto webcam_reg_key{
            make_shared<RegKey>(main_key, webcam_key) };
        auto webcam_watcher{
            make_shared<RegKeyChangeReporter>(
                L"webcam",
                webcam_reg_key,
                filesystem::path{"C:\\log-camera.txt"},
                file_reporter
            )
        };
        services.push_back(
            make_shared<StartMicroServiceGuard>(webcam_watcher)
        );

        wstring microphone_key{
            L"SOFTWARE\\Microsoft\\Windows\\"
            "CurrentVersion\\CapabilityAccessManager"
            "\\ConsentStore\\microphone" };
        auto microphone_reg_key{
            make_shared<RegKey>(main_key, microphone_key) };
        auto microphone_watcher{ 
            make_shared<RegKeyChangeReporter>(
                L"microphone", 
                microphone_reg_key, 
                filesystem::path{"C:\\log-microphone.txt"}, 
                file_reporter
            ) 
        };
        services.push_back(
            make_shared<StartMicroServiceGuard>(microphone_watcher)
        );
        
        auto file_watcher{ 
            make_shared<FileWatcher>(
                file_reporter,
                filesystem::path{"C:\\log-app.txt"}
            ) 
        };
        services.push_back(
            make_shared<StartMicroServiceGuard>(file_watcher)
        );

        return services;
    }
    catch (const exception& ex) {
        auto size_{ strlen(ex.what()) };
        wstring wc(size_, L'#');
        mbstowcs(&wc[0], ex.what(), size_);
        SvcReportEvent(wc.data());
        return services;
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
    auto services{ init_services() };
    if (services.empty())
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