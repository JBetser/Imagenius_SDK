To start Imagenius debug platform locally:
- install ice 3.5.0 from Zeroc.com website (Click and then install to default folder: http://www.zeroc.com/download/Ice/3.5/Ice-3.5.0-1.msi)
- add "C:\Program Files (x86)\ZeroC\Ice-3.5.0\bin" and "C:\Program Files (x86)\ZeroC\Ice-3.5.0\bin\x64" to your environment variable PATH
- Edit Imagenius\IGSMDesktopIce\IGSMDesktopIce.exe.debug.config and specify the full path of folder Imagenius_SDK (comprised) in SDK_PATH
- Open Imagenius/ImageniusSDK.sln in Visual studio 2010 as admin and build solution
- Set the full control permission to Everyone to the folder ./WebDebug (right click the folder -> Properties -> Sharing -> Advanced Sharing -> Tick Share this folder, type WebDebug, and select full control)
- Run a cmd prompt as admin in this path, and execute "DeployWebSite Debug.cmd" -> 3 window processes should appear and stay alive, and a web page should be opened in your browser 
- If the firewall asks to unblock IGServer, unblock it for the local network
- Click on "Load Image", and then on an effect

To debug an effect:
- Attach visual studio to IGServer.exe
- Set a break point in IGLibrary project, file IGIPFilter.cpp, function OnImageProcessing

To activate the email to our support, you can specify your smtp parameters in web.config file in TI_Website:
<add key="CONTACTUS_EMAIL" value="contactus@imageniuspaint.com"></add>
<add key="ADMIN_EMAIL" value="admin@imageniuspaint.com"></add>
<add key="ADMIN_PWD" value=""></add>
<add key="ADMIN_SMTP" value="auth.smtp.1and1.fr"></add>

When using the contact us page, the test page can send emails to us on your behalf (always with your confirmation!) using that account


