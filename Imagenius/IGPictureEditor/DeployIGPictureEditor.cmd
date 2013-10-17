IF %1==ServerRelease GOTO RELEASE
SET targetdir="C:\Imagenius\Testing\IGServer"
GOTO MAIN
:RELEASE
SET targetdir="C:\Imagenius\IGServer"
:MAIN

md "C:\Imagenius"
md "C:\Imagenius\IGServer"
md "C:\Imagenius\Testing
md "C:\Imagenius\Testing\IGServer"
md "%targetdir%\IGServerApplication"
md "%targetdir%\IGServerApplication\IGPictureEditor.dll"

copy /Y "..\IGPictureEditor\%1\IGPictureEditor.dll" "%targetdir%\IGServerApplication\IGPictureEditor.dll"
copy /Y "..\IGPictureEditor\%1\IGPictureEditor.pdb" "%targetdir%\IGServerApplication\IGPictureEditor.dll"
copy /Y "..\IGServer\IGPictureEditor.dll.manifest" "%targetdir%\IGServerApplication\IGPictureEditor.dll"
