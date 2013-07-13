rd /S /Q WebDebug
md WebDebug
xcopy /Y /E "./TI_WebSite" "./WebDebug"
md "WebDebug/iginput"
md "WebDebug/igoutput127.0.0.1"
md "C:\Imagenius\Testing\IGServer/IGServerApplication/Logs"
md "C:\Imagenius\Testing\IGServer/IGServerApplication/UserAccounts"
md "C:\Imagenius\Testing\IGServer/IGServerApplication/UserAccounts/Demo"
md "C:\Imagenius\Testing\IGData
md "C:\Imagenius\Testing\IGData\Logs
xcopy /Y /E "./Demo" "C:\Imagenius\Testing\IGServer/IGServerApplication/UserAccounts/Demo"
copy /Y "TI_WebSite\App_Data\Imagenius.mdb" "C:\Imagenius\Testing\IGServer/IGServerApplication/UserAccounts"
copy /Y "Imagenius\IGSMDesktopIce\IGSMDesktopIce.exe.debug.config" "C:\Imagenius\Testing\IGSMDesktop\IGSMDesktopIce.exe.config"
.\Imagenius\Tools\InstallUtil.exe .\Imagenius\EventLogSourceInstaller\bin\Debug\EventLogSourceInstaller.dll
call "Launch Server Debug.cmd"