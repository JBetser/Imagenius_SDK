using System;
using System.Collections;
using System.Linq;
using System.Web;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Xml.Linq;
using System.Drawing;
using System.Text.RegularExpressions;
using IGPE;
using IGSMLib;
using IGMadam;
using System.Web.Script.Services;

/// <summary>
/// Summary description for IGWSWorkspace
/// </summary>
[WebService(Namespace = "https://imageniuspaint.com/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
// To allow this Web Service to be called from script, using ASP.NET AJAX, uncomment the following line. 
[System.Web.Script.Services.ScriptService]
public class IGWSWorkspace : System.Web.Services.WebService {

    public IGWSWorkspace () {
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string NewImage(string Width, string Height)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestWorkspaceNew(sUser, Width, Height, "0", "0");
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string CloseImage(string FrameId)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameClose(sUser, FrameId);
        if (FrameId == "-1") // all frames
            return IGPEWebServer.ProcessUserCommand(Session, req);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string UpdateImage(string FrameId)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameUpdate(sUser, FrameId);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string SaveImage(string FrameId, string ImageName)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameSave(sUser, FrameId, ImageName);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string SaveImageBM(string FrameId, string ImageName)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameSaveBeetleMorph(sUser, FrameId, ImageName);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string FilterImage(string FrameId, int LayerId, int FilterId, bool Original, string Params)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req;
        if (Params == "")
            req = new IGRequestFrameFilterLayer(sUser, FrameId, LayerId.ToString(), FilterId.ToString(), Original);
        else
            req = new IGRequestFrameFilterLayer(sUser, FrameId, LayerId.ToString(), FilterId.ToString(), Params.Split(',').ToDictionary(str => str.Split('_')[0], str => str.Split('_')[1]), Original);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string DrawLines(string FrameId, int LayerId, string TabPoints, string Transparency, string BrushSize)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameDrawLines(sUser, FrameId, LayerId.ToString(), TabPoints, Transparency, BrushSize);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string Rubber(string FrameId, int LayerId, string TabPoints, string Transparency, string BrushSize)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameRubber(sUser, FrameId, LayerId.ToString(), TabPoints, Transparency, BrushSize);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string PickColor(string FrameId, int LayerId, string Coords)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFramePickColor(sUser, FrameId, LayerId.ToString(), Coords);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string Copy(string FrameId)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameCopy(sUser, FrameId);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string Cut(string FrameId)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameCut(sUser, FrameId);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string Paste(string FrameId)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFramePaste(sUser, FrameId);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string Delete(string FrameId)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameDelete(sUser, FrameId);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string SmartDelete(string FrameId)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameSmartDelete(sUser, FrameId);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string SetViewport(string FrameId, double CenterX, double CenterY, double Zoom)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        if (Session[IGAnswer.IGANSWER_CURRENTVIEWPORTMAP] != null)
        {
            IGViewportMap viewportMap = (IGViewportMap)Session[IGAnswer.IGANSWER_CURRENTVIEWPORTMAP];
            viewportMap.SetViewport(FrameId, new IGViewport(new PointF((float)CenterX, (float)CenterY), Zoom));
            return IGPEWebServer.WEBSERVICE_RESULT_OK;
        }
        return IGPEWebServer.WEBSERVICE_RESULT_ERROR;
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string LayerAdd(string FrameId)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameAddLayer(sUser, FrameId);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string LayerRemove(string FrameId, int LayerId)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameRemoveLayer(sUser, FrameId, LayerId.ToString());
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string LayerMove(string FrameId, string PosLayerFrom, string PosLayerDest)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameMoveLayer(sUser, FrameId, PosLayerFrom, PosLayerDest);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string LayerMerge(string FrameId, string PosLayers)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        Regex reg = new Regex(@"[,]+");
        string[] tPosLayers = reg.Split(PosLayers);
        if (tPosLayers.Length < 2)
            return IGPEWebServer.WEBSERVICE_RESULT_ERROR;
        ArrayList listPosLayer = new ArrayList(tPosLayers);
        listPosLayer.Sort();
        int nPosMin = int.Parse((string)listPosLayer[0]);
        string sSortedPos = "";
        for (int idxPosLayer = 0; idxPosLayer < listPosLayer.Count; idxPosLayer++)
        {
            // avoid several occurences of same layer pos
            for (int idxItPosLayer = 0; idxItPosLayer < listPosLayer.Count; idxItPosLayer++)
            {
                if (idxItPosLayer != idxPosLayer)
                {
                    if (listPosLayer[idxItPosLayer] == listPosLayer[idxPosLayer])
                        return IGPEWebServer.WEBSERVICE_RESULT_ERROR;
                }
            }
            int nCurPosMin = int.Parse((string)listPosLayer[idxPosLayer]);
            if (nCurPosMin < nPosMin)
                nPosMin = nCurPosMin;
            if (sSortedPos != "")
                sSortedPos += ",";
            sSortedPos += listPosLayer[idxPosLayer];
        }
        IGRequest req = new IGRequestFrameMergeLayer(sUser, FrameId, sSortedPos, nPosMin.ToString());
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string LayerSetVisible(string FrameId, int LayerId, string Visible)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameSetLayerVisible(sUser, FrameId, LayerId.ToString(), Visible);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string GotoHistory(string FrameId, string HistoryId)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameGotoHistory(sUser, FrameId, HistoryId);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string ChangeSelection(string FrameId, int LayerId, string SelectionType, string TabPoints)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameSelect(sUser, FrameId, LayerId.ToString(), SelectionType, TabPoints);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string ChangeProperty(string FrameId, string PropIds, string PropVals)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestChangeProperty(sUser, FrameId, PropIds, PropVals);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string MovePixels(string FrameId, int LayerId, string DirectionX, string DirectionY)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameMove(sUser, FrameId, LayerId.ToString(), DirectionX, DirectionY);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string RotateAndResize(string FrameId, int LayerId, string Angle, string Rate)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameRotateAndResize(sUser, FrameId, LayerId.ToString(), Angle, Rate);
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }

    [ScriptMethod(ResponseFormat = ResponseFormat.Json)]
    [WebMethod(EnableSession = true)]
    public string SelectLayer(string FrameId, int LayerId)
    {
        if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null)
            return IGPEWebServer.WEBSERVICE_RESULT_DISCONNECTED;
        string sUser = (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
        IGRequest req = new IGRequestFrameSelectLayer(sUser, FrameId, LayerId.ToString());
        return IGPEWebServer.ProcessUserCommand(Session, req);
    }
}

