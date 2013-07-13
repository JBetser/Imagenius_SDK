using System;
using System.Collections;
using System.Linq;
using System.Web;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Xml.Linq;
using IGPE;
using IGSMLib;
using IGMadam;
using System.Web.Script.Services;

/// <summary>
/// Summary description for IGWSAccount
/// </summary>
[WebService(Namespace = "https://imageniuspaint.com/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
[System.Web.Script.Services.ScriptService]
public class IGWSAccount : System.Web.Services.WebService {

    public IGWSAccount () {
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string DeleteImage(string ImageName)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGSMRequestDeleteImage(sUser, ImageName);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string OpenImage(string ImageName, string LoadAs, bool AutoRotate, bool CloseAll)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        if (CloseAll)
            IGPEWebServer.ProcessUserCommand(Session, new IGRequestFrameClose(sUser, "-1"));
        IGRequest req = new IGRequestWorkspaceLoad(sUser, ImageName, LoadAs.ToString(), AutoRotate ? "1" : "0");
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string UploadImage(string ImageURL)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        return IGPEWebServer.UploadImageFromUrl(ImageURL, Session);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string DownloadImage(string ImageName)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGSMRequestDownload req = new IGSMRequestDownload(sUser, ImageName);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string SetCurrentView(string CurrentView)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        Session[IGAnswer.IGANSWER_CURRENTVIEW] = CurrentView;
        return IGPEWebServer.WEBSERVICE_RESULT_OK;
    }
}

