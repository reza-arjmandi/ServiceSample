# ServiceSample

## Prerequisites:

This service needs to create report files in the C drive root directory. 
In Windows 10, users (even administrators) are not allowed to create files in the C drive root directory, otherwise, an error message like “A required privilege is not held by the client” or “access is denied” will be prompted.
To fix it, just turn off the User Account Control (UAC). In Windows 10, I’ve changed following registry key.
Open up regedit.
Locate HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System\EnableLUA
Update the EnableLUA value to 0 (turn if off)
Restart Windows.


## Install Service:
