taskkill /F /T /IM icegridnode.exe
taskkill /F /T /IM iceboxd.exe
START "Face detection server" /D"C:\Imagenius\Testing\IGServer\IGFaceDetector" "C:\Program Files (x86)\ZeroC\Ice-3.5.0\bin\icegridnode.exe" --Ice.Config=config.grid
