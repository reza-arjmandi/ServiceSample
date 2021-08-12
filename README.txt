# ServiceSample

## Prerequisites:

1- Change Windows settings in order to have permission to create report files 
in the C drive root directory:

	This service needs to create report files in the C drive root directory. 
	In Windows 10, users (even administrators) are not allowed to create files 
	in the C drive root directory, otherwise, an error message will be prompted.
	To fix it, just turn off the User Account Control (UAC). In Windows 10, 
	I’ve done the following steps:
	- Open up regedit.
	- Locate HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System\EnableLUA
	- Update the EnableLUA value to 0
	- Restart Windows.

2- Visual Studio 2019

## Install Service:

1- Build ServiceSmaple project(or use ServiceSample.exe in Binary directory)
2- Run following command with Administrator privilege to install service:
	ServiceSample.exe install

## Description:

The service object is created with SERVICE_USER_OWN_PROCESS as it’s ServiceType, it means 
this service runs in its own process under every logged-on user account. 
Because of that if there are more logged-in users, it logs info for all of them.
For each item that was mentioned in the task, an asynchronous(active) class 
is implemented as follows:

1- ActiveApplicationReporter.h:
This class records what application is currently in focus and logs
information in file C:\log-app.txt and every time the currently 
in focus application will changed it will update the log file.

2- RegKeyChangeReporter.h:
In the SvcInit.h header file, we have the init_services function,
in this function following objects from RegKeyChangeReporter are
instantiated in order to log information if the camera or microphone 
becomes active or inactive, and every time the states will changed, 
the logs also will be updated.

1-webcam_watcher object
2-microphone_watcher object

These two objects log information in files: 
C:\log-camera.txt and C:\log-microphone.txt

3-FileWatcher.h:
This class records every time C:\log-app.txt is open, it logs this 
information in C:\log-open-log-file.txt with the user who opened 
the file and the time when it was open.

## Concurrency pattern

### Active object(Active.h)
The active object design pattern decouples method execution from method 
invocation for objects that each reside in their own thread of control. 
The goal is to introduce concurrency, by using asynchronous method invocation 
and a scheduler for handling requests.
