using System;
using System.Collections;
using System.Linq;
using System.Web;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Xml;
using System.Xml.Linq;
using System.IO;
using IGPE;
using IGMadam;

/// <summary>
/// Summary description for ImageniusWS
/// </summary>
[WebService(Namespace = "https://imageniuspaint.com/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
// To allow this Web Service to be called from script, using ASP.NET AJAX, uncomment the following line. 
// [System.Web.Script.Services.ScriptService]
public class ImageniusWS : System.Web.Services.WebService {

    public ImageniusWS () {
    }

    [WebMethod]
    public string Ping() {
        return IGPEWebServer.WEBSERVICE_RESULT_CONNECTED;
    }        

    [WebMethod(EnableSession = true)]
    public string GetServerList() {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        IGSMStatusTreeView treeViewStatus = (IGSMStatusTreeView)Session[IGPEMultiplexing.SESSIONMEMBER_SERVERSTATUS];
        if (treeViewStatus == null)
            return IGPEWebServer.WEBSERVICE_RESULT_ERROR;
        return treeViewStatus.GetServerList();
    }

    [WebMethod(EnableSession = true)]
    public string GetServerStatus(string ServerName)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        if (Session[IGPEMultiplexing.SESSIONMEMBER_SERVERSTATUS] == null)
            return IGPEWebServer.WEBSERVICE_RESULT_ERROR;
        IGSMStatusTreeView treeViewStatus = ((IGSMStatusTreeView)Session[IGPEMultiplexing.SESSIONMEMBER_SERVERSTATUS]).GetCopy();
        return treeViewStatus.GetServerStatus(ServerName);
    }

    [WebMethod(EnableSession = true)]
    public string DownloadOutput(bool ViewErrorsOnly)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        IGPEWebServer.Hearthbeat(Session);
        if (Session[IGPEMultiplexing.SESSIONMEMBER_OUTPUT] == null)
            return "No output";        
        IGPEOutput output = (IGPEOutput)Session[IGPEMultiplexing.SESSIONMEMBER_OUTPUT];
        return output.Download(ViewErrorsOnly);
    }

    [WebMethod(EnableSession = true)]
    public string GetUserStatus(string Server, string User)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        IGSMStatusTreeView treeViewStatus = (IGSMStatusTreeView)Session[IGPEMultiplexing.SESSIONMEMBER_SERVERSTATUS];
        if (treeViewStatus == null)
            return IGPEWebServer.WEBSERVICE_RESULT_ERROR;
        return treeViewStatus.GetUserStatus(Server, User);
    }

    [WebMethod(EnableSession = true)]
    public string SendCommand(string CommandLine)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        if ((string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE] != DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE_ADMIN)
            return IGPEWebServer.WEBSERVICE_RESULT_ACCESSDENIED;
        return IGPEWebServer.ProcessCommandLine(CommandLine);
    }
}

