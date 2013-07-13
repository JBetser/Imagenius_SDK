taskkill /F /T /IM WebDev.WebServer40.EXE
taskkill /F /T /IM icegridnode.exe
taskkill /F /T /IM iceboxd.exe
taskkill /F /T /IM IGSMDesktopIce.exe
taskkill /F /T /IM IGServer.exe
START "Face detection server" /D"C:\Imagenius\Testing\IGServer\IGFaceDetector" "C:\Program Files (x86)\ZeroC\Ice-3.5.0\bin\icegridnode.exe" --Ice.Config=config.grid
START "Imagenius server" /D"C:\Imagenius\Testing\IGSMDesktop" "C:\Program Files (x86)\ZeroC\Ice-3.5.0\bin\icegridnode.exe" --Ice.Config=config.grid
START "IIS Web server" "C:\Program Files (x86)\Common Files\microsoft shared\DevServer\10.0\WebDev.WebServer40.exe" /path:"%~dp0WebDebug" /vpath:"/WebDebug" /port:8080
START http://localhost:8080/WebDebug/Test.aspx