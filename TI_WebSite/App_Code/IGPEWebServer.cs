using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Xml;
using System.Net;
using System.Net.Sockets;
using System.Web.SessionState;
using System.Threading;
using System.Text.RegularExpressions;
using System.IO;
using System.Web.UI;
using IGMadam;
using IGSMLib;
using System.Drawing.Imaging;
using System.Drawing;
using System.Configuration;
using System.Web.Configuration;
using System.Net.Mail;
using System.Security;

namespace IGPE
{
    /// <summary>
    /// Summary description for IGPEWebServer
    /// </summary>
    public static class IGPEWebServer
    {
        public static readonly string WEBSERVICE_RESULT_OK = "{\"Status\":\"OK\"}";
        public static readonly string WEBSERVICE_RESULT_ERROR = "{\"Status\":\"Error\"}";
        public static readonly string WEBSERVICE_RESULT_CONNECTED = "{\"Status\":\"Connected\"}";
        public static readonly string WEBSERVICE_RESULT_DISCONNECTED = "{\"Status\":\"Disconnected\"}";
        public static readonly string WEBSERVICE_RESULT_ACCESSDENIED = "{\"Status\":\"Access Denied\"}";
        public static readonly string WEBSERVICE_RESULT_INITCONNECTION = "{\"Realm\":\"ImageniusAuthentication\",\"PublicKey\":\"" + DatabaseUserSecurityAuthority.IGMADAM_PUBLICKEY + "\"}";
        public static readonly string ERROR_UPLOADFAILED = "The file upload has failed. <br/>Check that the path you specified is valid.";
        public static readonly string ERROR_DISCONNECTED = "Session has been disconnected";
        
        private static IGPEMultiplexing mg_multiplexingMgr = null;
        private static IGPEServerStatus mg_serverStatus = null;

        public static bool OpenSession()
        {
            if (checkServerConnections())
                return true;
            try
            {
                Configuration rootWebConfig = null;
                try
                {
                    rootWebConfig = WebConfigurationManager.OpenWebConfiguration("/Web.config");
                }
                catch (Exception)
                {
                    rootWebConfig = WebConfigurationManager.OpenWebConfiguration("/WebDebug/Web.config");
                }

                Dictionary<string, string> dicSettings = new Dictionary<string, string>();
                foreach (KeyValueConfigurationElement prop in rootWebConfig.AppSettings.Settings)
                    dicSettings.Add(prop.Key, prop.Value);

                mg_multiplexingMgr = new IGPEMultiplexing(dicSettings);
                mg_multiplexingMgr.Initialize();
                mg_serverStatus = IGPEServerStatus.GetInstance();
                mg_serverStatus.Init();
            }
            catch (Exception exc)
            {
                if (mg_multiplexingMgr != null)
                    mg_multiplexingMgr.AppendError(exc.ToString());
            }
            return true;
        }

        public static bool IsStarted()
        {
            return (mg_multiplexingMgr != null && mg_serverStatus != null);
        }

        private static bool checkServerConnections()
        {
            if (IsStarted())
                return mg_multiplexingMgr.Initialize();
            return false;
        }

        public static bool ConnectDemo(string userIP, HttpSessionState userSession)
        {
            if (!OpenSession())
                return false;
            return IGPEWebServer.connectMember(HC.CREDENTIAL_DEMO_HEADER + userIP, HC.CREDENTIAL_DEMOPWD, userSession);
        }        

        public static bool ConnectGuest(string userName, string userEmail, string pwd, HttpSessionState userSession)
        {
            if (!OpenSession())
                return false;
            if (!mg_multiplexingMgr.AuthenticateMember(userEmail, pwd))
                RegisterUser(HC.CREDENTIAL_WEBACTIVATOR, HC.CREDENTIAL_WEBACTIVATOR_PWD, userName, userEmail, pwd);
            return connectMember(userEmail, pwd, userSession);
        }

        public static bool ConnectMember(string User, string Pwd, HttpSessionState userSession)
        {
            if (!OpenSession())
                return false;
            return connectMember(User, Pwd, userSession);
        }

        private static bool connectMember(string User, string Pwd, HttpSessionState userSession)
        {
            Hearthbeat(userSession);
            bool bIsConnected = mg_multiplexingMgr.ConnectMember(User, Pwd, userSession);
            if (bIsConnected)
                mg_serverStatus.AddServerStatusToSession(User, userSession);
            return bIsConnected;
        } 

        public static bool DisconnectUser(HttpSessionState userSession)
        {
            string sUser = (string)userSession[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
            if (!IsStarted() || (sUser == null))
            {
                mg_multiplexingMgr.AppendError(userSession, ERROR_DISCONNECTED);
                return false;
            }
            IPEndPoint serverEndPoint = (IPEndPoint)userSession[IGPEMultiplexing.SESSIONMEMBER_SERVERIP];
            mg_multiplexingMgr.DisconnectUser(sUser, serverEndPoint);
            return true;
        }

        public static string RegisterUser(string AdminLogin, string AdminPwd, string UserName, string UserEmail, string UserPassword)
        {
            try
            {
                if ((UserSecurityAuthority.FromConfig().Authenticate(AdminLogin, AdminPwd, null, "Forms") == null))
                    return "Login failed";
            }
            catch (PasswordFormatNotSupportedException)
            {
                return "Login failed";
            }
            string userPrivilege = UserSecurityAuthority.FromConfig().GetUserPrivilege(AdminLogin);
            if (userPrivilege != DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE_ADMIN &&
                userPrivilege != DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE_ACTIVATOR)
                return "This account does not have this privilege";
            if (!UserEmail.Contains('@'))
                return "Bad User e-mail format";
            if (!UserEmail.Split('@')[1].Contains('.'))
                return "Bad User e-mail format";
            // Create the account detail hash table.
            Hashtable accountDetails = new Hashtable();
            accountDetails.Add(DatabaseUserSecurityAuthority.IGMADAM_USERSHORTNAME, UserName);
            accountDetails.Add(DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE, DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE_GUEST);
            accountDetails.Add(DatabaseUserSecurityAuthority.IGMADAM_USERIP, HttpContext.Current.Request.UserHostAddress);
            string res = UserSecurityAuthority.FromConfig().CreateAccount(UserEmail.Trim(), UserPassword.Trim(), PasswordFormat.DigestAuthA1, accountDetails);
            if ((res != IGMadam.AccountCreationResult.ACCOUNT_CREATION_SUCCEEDED.ToString()) &&
                (res != IGMadam.AccountCreationResult.ACCOUNT_ALREADY_EXISTS.ToString()))
                return res;
            if (userPrivilege == DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE_ACTIVATOR)
                res = UserSecurityAuthority.FromConfig().ActivateAccount(UserEmail.Trim());
            return res;
        }

        public static string ActivateUser(string AdminLogin, string AdminPwd, string UserEmail)
        {
            try
            {
                if ((UserSecurityAuthority.FromConfig().Authenticate(AdminLogin, AdminPwd, null, "Forms") == null))
                    return "Login failed";
            }
            catch (PasswordFormatNotSupportedException)
            {
                return "Login failed";
            }
            string userPrivilege = UserSecurityAuthority.FromConfig().GetUserPrivilege(AdminLogin);
            if (userPrivilege != DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE_ADMIN &&
                userPrivilege != DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE_ACTIVATOR)
                return "This account does not have this privilege";
            if (!UserEmail.Contains('@'))
                return "Bad User e-mail format";
            if (!UserEmail.Split('@')[1].Contains('.'))
                return "Bad User e-mail format";
            return UserSecurityAuthority.FromConfig().ActivateAccount(UserEmail.Trim());
        }

        public static bool IsConnected(string sUser)
        {
            if (!IsStarted())
                return false;
            return mg_multiplexingMgr.IsConnected(sUser);
        }

        public static bool IsDisconnected(HttpSessionState userSession)
        {
            if (userSession[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] == null)
                return false;
            return true;
        }

        public static bool Hearthbeat(HttpSessionState userSession)
        {
            if (!IsStarted())
                return false;
            mg_multiplexingMgr.Hearthbeat(userSession);
            return true;
        }

        public static string ProcessUserCommand(HttpSessionState userSession, IGRequest request)
        {
            if (!IsStarted())
                return IGPEMultiplexing.IGPEMULTIPLEXING_ERROR_DISCONNECTED;
            if (!mg_multiplexingMgr.ProcessUserCommand(userSession, request))
                return IGPEWebServer.WEBSERVICE_RESULT_ERROR;
            IGAnswer answer = IGServerManager.Instance.CreateAnswer(request);
            if (userSession != null)
            {
                if (userSession[IGAnswer.IGANSWER_ALLNBLAYERS] != null)
                    answer.SetParameter(IGAnswer.IGANSWER_ALLNBLAYERS, (string)userSession[IGAnswer.IGANSWER_ALLNBLAYERS]);
                if (userSession[IGAnswer.IGANSWER_ALLLAYERVISIBILITY] != null)
                    answer.SetParameter(IGAnswer.IGANSWER_ALLLAYERVISIBILITY, (string)userSession[IGAnswer.IGANSWER_ALLLAYERVISIBILITY]);
                if (userSession[IGAnswer.IGANSWER_ALLSTEPIDS] != null)
                    answer.SetParameter(IGAnswer.IGANSWER_ALLSTEPIDS, (string)userSession[IGAnswer.IGANSWER_ALLSTEPIDS]);
                if (userSession[IGAnswer.IGANSWER_SERVERIP] != null)
                    answer.SetParameter(IGAnswer.IGANSWER_SERVERIP, ((IPEndPoint)userSession[IGAnswer.IGANSWER_SERVERIP]).Address.ToString());
            }
            return answer.ToClientOutput();
        }

        public static void AppendError(HttpSessionState userSession, string sError)
        {
            if (IsStarted())
                mg_multiplexingMgr.AppendError(userSession, sError);
        }

        public enum enumCmdLine
        {
                                    WEBSERVER_CMDLINE_UNKNOWN = 0,
                                    WEBSERVER_CMDLINE_PING = 1, 
                                    WEBSERVER_CMDLINE_DISCONNECT = 2,
                                    WEBSERVER_CMDLINE_RESET = 3,
                                    WEBSERVER_CMDLINE_ADDIGAPP = 4,
                                    WEBSERVER_CMDLINE_REGISTERUSER = 5 };

        public enum enumCmdLineModifier {   WEBSERVER_MODIFIER_NONE = 0,
                                            WEBSERVER_MODIFIER_FORCE = 1,
                                            WEBSERVER_MODIFIER_USER = 2 };

        public static string ProcessCommandLine(string commandLine)
        {
            string result = "Server ";
            string [] separators = {" ", "\t", "\n"};
            string [] commands = commandLine.Split(separators, StringSplitOptions.RemoveEmptyEntries);
            string errorReason = "";
            string success = "no information";
            enumCmdLine curCmdLine = enumCmdLine.WEBSERVER_CMDLINE_UNKNOWN;
            enumCmdLineModifier curCmdLineModifier = enumCmdLineModifier.WEBSERVER_MODIFIER_NONE;
            bool res = false;
            for (int idxComd = 0; idxComd < commands.Length; idxComd++)
            {
                commands[idxComd] = commands[idxComd].ToLower();
                if (curCmdLine == enumCmdLine.WEBSERVER_CMDLINE_UNKNOWN)
                {
                    if (commands[idxComd] == "ping"){
                        if (commands.Length == 2)
                            curCmdLine = enumCmdLine.WEBSERVER_CMDLINE_PING;
                    }
                    else if (commands[idxComd] == "disconnect"){
                        if (commands.Length == 2)
                            curCmdLine = enumCmdLine.WEBSERVER_CMDLINE_DISCONNECT;
                    }
                    else if (commands[idxComd] == "reset")
                    {
                        if (commands.Length == 2)
                            curCmdLine = enumCmdLine.WEBSERVER_CMDLINE_RESET;
                    }
                    else if (commands[idxComd] == "addigapp")
                    {
                        if (commands.Length == 3)
                            curCmdLine = enumCmdLine.WEBSERVER_CMDLINE_ADDIGAPP;
                    }
                    else if (commands[idxComd] == "register")
                    {
                        if (commands.Length == 6)
                            curCmdLine = enumCmdLine.WEBSERVER_CMDLINE_REGISTERUSER;
                    }
                    else
                    {
                        errorReason = "unknown command";
                        break;
                    }
                }
                else{
                    if (commands[idxComd].StartsWith("-"))
                    {
                        // Modifier management
                        if (commands[idxComd] == "-f")
                        {
                            curCmdLineModifier |= enumCmdLineModifier.WEBSERVER_MODIFIER_FORCE;
                            continue;
                        }
                        else if (commands[idxComd] == "-u")
                        {
                            curCmdLineModifier |= enumCmdLineModifier.WEBSERVER_MODIFIER_USER;
                            continue;
                        }
                        else
                        {
                            errorReason = "unknown modifier";
                            break;
                        }
                    }
                    try
                    {
                        switch (curCmdLine)
                        {
                            case enumCmdLine.WEBSERVER_CMDLINE_PING:
                                {
                                    string[] endPoint = commands[1].Split(':');
                                    res = mg_multiplexingMgr.Ping(new IPEndPoint(IPAddress.Parse(endPoint[0]), int.Parse(endPoint[1])));
                                }
                                break;
                            case enumCmdLine.WEBSERVER_CMDLINE_DISCONNECT:
                                {
                                    string[] endPoint = commands[1].Split(':');
                                    res = mg_multiplexingMgr.DisconnectServer(new IPEndPoint(IPAddress.Parse(endPoint[0]), int.Parse(endPoint[1])));
                                }
                                break;
                            case enumCmdLine.WEBSERVER_CMDLINE_RESET:
                                {
                                    string[] endPoint = commands[1].Split(':');
                                    res = mg_multiplexingMgr.Reset(new IPEndPoint(IPAddress.Parse(endPoint[0]), int.Parse(endPoint[1])));
                                }
                                break;
                            case enumCmdLine.WEBSERVER_CMDLINE_ADDIGAPP:                                
                                {
                                    string[] endPoint = commands[1].Split(':');
                                    ushort nNbApps = Convert.ToUInt16(commands[2]);
                                    res = IGPERequestSender.SendRequest(new IPEndPoint(IPAddress.Parse(endPoint[0]), int.Parse(endPoint[1])), new IGSMRequestAddIGApps(nNbApps));
                                }
                                break;
                            case enumCmdLine.WEBSERVER_CMDLINE_REGISTERUSER:
                                success = "user account " + commands[4] + " added";
                                errorReason = RegisterUser(commands[1], commands[2], commands[3], commands[4], commands[5]);
                                res = (errorReason == IGMadam.AccountCreationResult.ACCOUNT_CREATION_SUCCEEDED.ToString());
                                break;
                        }
                        if (!res && errorReason == "")
                            errorReason = "an unknown error occured";
                        if (!res)
                            break;
                    }
                    catch (Exception exc)
                    {
                        errorReason = "IGPEWebServer process command line error. Exception: " + exc.ToString();
                        res = false;
                    }                
                }
            }
            if (!res)
                result += "error: " + errorReason;
            else if (curCmdLine == enumCmdLine.WEBSERVER_CMDLINE_UNKNOWN)
                result += "unknown request: " + errorReason;
            else
                result += "done: " + success;
            result += "\n";
            return result;
        }

        public static string UploadImageFromUrl(string url, HttpSessionState session)
        {
            string destPath = "";
            string destFileName = Path.GetFileName(url);
            try
            {
                var buffer = 1024;
                ImageFormat format = ImageFormat.Jpeg;
                // We prefix the file name with the Page.Session.SessionID so
                // it will be deleted when the session expires
                destPath = @"\\" + mg_multiplexingMgr.ServerIP + HC.PATH_INPUTPHYSICAL + @"\" + session.SessionID + "$" + destFileName;
                switch (Path.GetExtension(destPath).ToUpper())
                {
                    case ".JPG":
                        format = ImageFormat.Jpeg;
                        break;
                    case ".PNG":
                        format = ImageFormat.Png;
                        break;
                    case ".BMP":
                        format = ImageFormat.Bmp;
                        break;
                    default:
                        throw new FormatException("the image format is not supported");
                }

                if (!Uri.IsWellFormedUriString(url, UriKind.Absolute))
                    throw new FormatException("the url is not valid");

                using (var ms = new MemoryStream())
                {
                    var req = WebRequest.Create(url);

                    using (var resp = req.GetResponse())
                    {
                        using (var stream = resp.GetResponseStream())
                        {
                            var bytes = new byte[buffer];
                            var n = 0;

                            while ((n = stream.Read(bytes, 0, buffer)) != 0)
                                ms.Write(bytes, 0, n);
                        }
                    }

                    Bitmap image = Bitmap.FromStream(ms) as Bitmap;
                    image.Save(destPath, format);
                }
                string sUser = (string)session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
                IGSMRequestUpload request = new IGSMRequestUpload(sUser, destPath);
                return IGPEWebServer.ProcessUserCommand(session, request);
            }
            catch (Exception exc)
            {
                IGPEWebServer.AppendError(session, ERROR_UPLOADFAILED + " - File name: " + destFileName + " - File destination: " + destPath + ". Exception:" + exc.ToString());
                return IGPEWebServer.WEBSERVICE_RESULT_ERROR;
            }
        }

        public static bool DownloadFile(Page page, HttpResponse response, HttpSessionState userSession)
        {
            Regex RE = new Regex(@"[,]+");
            string[] tImagePathDef = RE.Split((string)userSession[IGSMRequest.IGSMREQUEST_PARAM_LISTPATH]);
            
            string sImageFileName = Path.GetFileName(tImagePathDef[0]);
            string sFilePath = page.MapPath(tImagePathDef[0]);            
            response.ContentType = "application/octet-stream";
            response.AddHeader("Content-Disposition", "attachment; filename=" + sImageFileName);
            response.WriteFile(sFilePath);
            userSession.Remove(IGSMRequest.IGSMREQUEST_PARAM_LISTPATH);
            return true;
        }

        public static bool SendEmail(HttpSessionState userSession, string destEmail, string userName, string subject, string body)
        {
            MailMessage message = new MailMessage(IGServerManager.AppSettings["ADMIN_EMAIL"], destEmail, subject, body);
            message.IsBodyHtml = true;
            SmtpClient client = new SmtpClient(IGServerManager.AppSettings["ADMIN_SMTP"], 25);
            // Credentials are necessary if the server requires the client  
            // to authenticate before it will send e-mail on the client's behalf.
            client.UseDefaultCredentials = false;
            client.Credentials = new NetworkCredential(IGServerManager.AppSettings["ADMIN_EMAIL"], IGServerManager.AppSettings["ADMIN_PWD"].Aggregate(new SecureString(), (ss, c) => { ss.AppendChar(c); return ss; }));
            try
            {
                client.Send(message);
            }
            catch (Exception ex)
            {
                IGPEWebServer.AppendError(userSession, string.Format("Imagenius server error, please retry later.\nTechnical information: {0}", ex.ToString()));
                return false;
            }
            return true;
        }  
    }    
}