using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Services;
using IGPE;
using IGMadam;
using System.Collections;
using System.Web.Script.Services;
using IGSMLib;

/// <summary>
/// Summary description for ImageniusPublicWS
/// </summary>
[WebService(Namespace = "https://imageniuspaint.com/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
// To allow this Web Service to be called from script, using ASP.NET AJAX, uncomment the following line. 
[System.Web.Script.Services.ScriptService]
public class ImageniusPublicWS : System.Web.Services.WebService {

    public ImageniusPublicWS () {
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod]
    public string InitConnection()
    {
        return IGPEWebServer.WEBSERVICE_RESULT_INITCONNECTION;
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string CheckConnection()
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_RELOADPAGE] != null)
        {
            Session.Remove(IGPEMultiplexing.SESSIONMEMBER_RELOADPAGE);
            return "NeedReload";
        }
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        IGPEWebServer.Hearthbeat(Session);
        return (Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME] != null) ? IGPEWebServer.WEBSERVICE_RESULT_CONNECTED : IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string ConnectDemo()
    {
        lock (Session)
        {
            Session.Remove(IGPEMultiplexing.SESSIONMEMBER_CONNECTRESULT);
            if (!IGPEWebServer.ConnectDemo(Context.Request.UserHostAddress.StartsWith("::") ? "127.0.0.1" : Context.Request.UserHostAddress, Session))
                return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
            if (Session[IGPEMultiplexing.SESSIONMEMBER_CONNECTRESULT] == null)
                return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
            return (string)Session[IGPEMultiplexing.SESSIONMEMBER_CONNECTRESULT];
        }
    }    

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string ConnectUser(string UserName, string Password)
    {
        lock (Session)
        {
            Session.Remove(IGPEMultiplexing.SESSIONMEMBER_CONNECTRESULT);
            if (!IGPEWebServer.ConnectMember(UserName, Password, Session))
                return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
            if (Session[IGPEMultiplexing.SESSIONMEMBER_CONNECTRESULT] == null)
                return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
            return (string)Session[IGPEMultiplexing.SESSIONMEMBER_CONNECTRESULT];
        }
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string ConnectFbGuest(string UserName, string UserEmail, string Password)
    {
        lock (Session)
        {
            Session.Remove(IGPEMultiplexing.SESSIONMEMBER_CONNECTRESULT);
            if (!UserEmail.EndsWith(HC.CREDENTIAL_FBEMAIL))
                return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
            if (!IGPEWebServer.ConnectGuest(UserName, UserEmail, Password, Session))
                return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
            if (Session[IGPEMultiplexing.SESSIONMEMBER_CONNECTRESULT] == null)
                return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
            return (string)Session[IGPEMultiplexing.SESSIONMEMBER_CONNECTRESULT];
        }
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string ConnectTwitterGuest(string UserName, string UserEmail, string Password)
    {
        lock (Session)
        {
            Session.Remove(IGPEMultiplexing.SESSIONMEMBER_CONNECTRESULT);
            if (!UserEmail.EndsWith(HC.CREDENTIAL_FBEMAIL))
                return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
            if (!IGPEWebServer.ConnectGuest(UserName, UserEmail, Password, Session))
                return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
            if (Session[IGPEMultiplexing.SESSIONMEMBER_CONNECTRESULT] == null)
                return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
            return (string)Session[IGPEMultiplexing.SESSIONMEMBER_CONNECTRESULT];
        }
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string DisconnectUser()
    {
        return IGPEWebServer.DisconnectUser(Session) ? IGPEWebServer.WEBSERVICE_RESULT_OK : IGPEWebServer.WEBSERVICE_RESULT_ERROR;
    }

    [WebMethod(EnableSession = false)]
    public string RegisterUser(string AdminLogin, string AdminPwd, string UserName, string UserEmail, string UserPassword)
    {
        return IGPEWebServer.RegisterUser(AdminLogin, AdminPwd, UserName, UserEmail, UserPassword);
    } 
}

