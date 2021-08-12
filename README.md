# ServiceSample

## Prerequisites:

1- Change Windows settings to be able to create report files in the C drive root directory:
	This service needs to create report files in the C drive root directory. 
	In Windows 10, users (even administrators) are not allowed to create files 
	in the C drive root directory, otherwise, an error message like 
	“A required privilege is not held by the client” or “access is denied” 
	will be prompted.
	To fix it, just turn off the User Account Control (UAC). In Windows 10, 
	I’ve changed following registry key.
	- Open up regedit.
	- Locate HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System\EnableLUA
	- Update the EnableLUA value to 0 (turn if off)
	- Restart Windows.

2- Visual Studio 2019

## Install Service:

1- Build ServiceSmaple project
2- Run following command with Administrator privilege to install service:
	-ServiceSample.exe install

## Description:

This service is created with SERVICE_USER_OWN_PROCESS as ServiceType, it means service runs in its own process under every logged-on user account. And because of that if there are more logged-in users, it logs info for all of them.
For each item that was mentioned in the task, an asynchronous(active) class is implemented as follows:

1- ActiveApplicationReporter.h:
This class records what application is currently in focus and logs
information in file C:\log-app.txt and every time the currently in focus application will changed it will update the log file.

2- RegKeyChangeReporter.h:
In the SvcInit.h header file, we have the init_services function, in this function following objects from RegKeyChangeReporter is instantiated to logs information if the camera or microphone is active or inactive and every time the states will changed it will update the log file.
1-webcam_watcher object
2-microphone_watcher object
This class and logs in files: C:\log-camera.txt and C:\log-microphone.txt

3-FileWatcher.h:
This class records every time C:\log-app.txt is open, logs this information in C:\log-open-log-file.txt with the user who opened the file and the time when it was open.

