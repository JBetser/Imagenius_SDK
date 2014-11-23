using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Configuration;

namespace IGSMLib
{
    public class HC
    {        
#if DEBUG
        public static string PATH_APPLICATIONROOT = IGServerManager.AppSettings["DRIVE_APPLI"] + @"/Imagenius/Testing";
        public static string PATH_SHAREROOT = IGServerManager.AppSettings["DRIVE_SHARED"] + @"/Imagenius/Testing";
#else
        public static string PATH_APPLICATIONROOT = IGServerManager.AppSettings["DRIVE_APPLI"] + @"/Imagenius";
        public static string PATH_SHAREROOT = IGServerManager.AppSettings["DRIVE_SHARED"] + @"/Imagenius";
#endif
        public static string PATH_REMOTESERVERLOG = IGServerManager.AppSettings["DRIVE_APPLI"] + @"/Logs/";
        public static string PATH_IGSERVERFOLDER = PATH_APPLICATIONROOT + @"/IGServer/IGServerApplication";
        public static string PATH_LOCALSERVERLOG = PATH_SHAREROOT + @"/IGData/Logs/";
        public static string PATH_USERACCOUNT = PATH_SHAREROOT + @"/IGData/UserAccounts/";
        public const string PATH_IGSERVEREXECUTABLE = @"IGServer.exe";
        public const string PATH_USERROOT = @"/Imagenius";
        public const string PATH_USERUPLOAD = PATH_USERROOT + @"/Upload/";
        public const string PATH_USERIMAGES = PATH_USERROOT + @"/Images/";
        public const string PATH_USERMINI = PATH_USERROOT + @"/Images/Mini/";
        public const string PATH_USERIMAGES_BEETLEMORPH = @"BeetleMorph";

        public static string PATH_INPUT = IGServerManager.AppSettings["SDK_PATH"] + IGServerManager.AppSettings["DRIVE_INPUT"] + @"/";
        public const string PATH_INPUTPHYSICAL = @"\IGInputFiles";
        public const string PATH_INPUTVIRTUAL = "/iginput/";
        public static string PATH_OUTPUT = IGServerManager.AppSettings["SDK_PATH"] + IGServerManager.AppSettings["DRIVE_OUTPUT"] + @"/";
        public const string PATH_OUTPUTDOWNLOADS = @"Downloads/";
        public const string PATH_OUTPUTPHYSICAL = @"\IGOutputFiles";
        public const string PATH_OUTPUTVIRTUAL = @"igoutput";
        public static string PATH_SHARED = IGServerManager.AppSettings["SDK_PATH"] + IGServerManager.AppSettings["DRIVE_SHARED"] + @"/";
        public const string PATH_SHAREDPHYSICAL = @"\Shared";
        public const string PATH_OUTPUTMINI = @"/Mini/";
        public const string PATH_PREFIXMINI = @"Mini";
        public const string PATH_LASTSAVEDPICTURE = @"LastSavedPicture.jpg";

        public const float MINIPICTURE_MAXSIZE = 160.0f;
                        
#if DEBUG
        public const int HEARTHBEAT_LOCAL_TIMEOUT_NOTRESPONDING = 200000;
        public const int HEARTHBEAT_LOCAL_TIMEOUT_DISCONNECTING = 1000000;
        public const int HEARTHBEAT_REMOTE_IDLEUSERTIMEOUT_DISCONNECTING = 25000;
		public const int REQUEST_PROCESSING_TIMEOUT = 120000;        
#else
        public const int HEARTHBEAT_LOCAL_TIMEOUT_NOTRESPONDING = 8000;
        public const int HEARTHBEAT_LOCAL_TIMEOUT_DISCONNECTING = 15000;
        public const int HEARTHBEAT_REMOTE_IDLEUSERTIMEOUT_DISCONNECTING = 300000;
		public const int REQUEST_PROCESSING_TIMEOUT = 10000;
#endif
        public const int HEARTHBEAT_REMOTE_USERTIMEOUT_DISCONNECTING = 60000;
        public const int HEARTHBEAT_REMOTE_SERVERTIMEOUT_NOTRESPONDING = REQUEST_PROCESSING_TIMEOUT + 20000;

        public const int PROGRESS_TIMEOUT_LOCAL = REQUEST_PROCESSING_TIMEOUT + 7000;
        public const int PROGRESS_TIMEOUT_REMOTE = REQUEST_PROCESSING_TIMEOUT + 10000;

        public const int IGREQPROC_PROGRESS_CHECKTIME = 1000;

        public const string CREDENTIAL_WEBACTIVATOR = "webActivator";
        public const string CREDENTIAL_WEBACTIVATOR_PWD = "wEb@ct1v@T10N";
        public const string CREDENTIAL_DEMO_HEADER = "Demo@";
        public const string CREDENTIAL_DEMOPWD = "DeMo123456789";
        public const string CREDENTIAL_FBEMAIL = "@facebook.com";
        public const string CREDENTIAL_TWITTEREMAIL = "@twitter.com";
    }
}
