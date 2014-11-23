using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Web.SessionState;
using System.Threading;
using System.Runtime.Serialization;

namespace IGSMLib
{
    [DataContract]
    public class IGRequest : IGXmlMessage
    {
        [DataMember(Name = "RequestId", Order = 0)]
        protected int m_nId = -1;

        public const int IGREQUEST_WORKSPACE = 100;
        public const int IGREQUEST_FRAME = 101;
        public const int IGREQUEST_SERVER = 102;

        public const int IGREQUEST_WORKSPACE_CONNECT = 1000;
        public const int IGREQUEST_WORKSPACE_PING = 1001;
        public const int IGREQUEST_WORKSPACE_DISCONNECT = 1002;
        public const int IGREQUEST_WORKSPACE_NEWIMAGE = 1003;
        public const int IGREQUEST_WORKSPACE_LOADIMAGE = 1004;
        public const int IGREQUEST_WORKSPACE_TERMINATE = 1005;
        public const int IGREQUEST_WORKSPACE_SHOW = 1006;
		public const int IGREQUEST_WORKSPACE_DESTROYACCOUNT = 1007;

        public const int IGREQUEST_FRAME_SAVE = 2000;
        public const int IGREQUEST_FRAME_ADDLAYER = 2001;
        public const int IGREQUEST_FRAME_REMOVELAYER = 2002;
        public const int IGREQUEST_FRAME_MOVELAYER = 2003;
        public const int IGREQUEST_FRAME_MERGELAYER = 2004;
        public const int IGREQUEST_FRAME_FILTER = 2005;
        public const int IGREQUEST_FRAME_CHANGEPROPERTY = 2006;
        public const int IGREQUEST_FRAME_CLOSE = 2007;
        public const int IGREQUEST_FRAME_UPDATE = 2008;
        public const int IGREQUEST_FRAME_SELECT = 2009;
        public const int IGREQUEST_FRAME_SETLAYERVISIBLE = 2010;
        public const int IGREQUEST_FRAME_GOTOHISTORY = 2011;
        public const int IGREQUEST_FRAME_DRAWLINES = 2012;
        public const int IGREQUEST_FRAME_PICKCOLOR = 2013;
        public const int IGREQUEST_FRAME_INDEX = 2014;
        public const int IGREQUEST_FRAME_COPY = 2015;
        public const int IGREQUEST_FRAME_CUT = 2016;
        public const int IGREQUEST_FRAME_PASTE = 2017;
        public const int IGREQUEST_FRAME_DELETE = 2018;
        public const int IGREQUEST_FRAME_SMARTDELETE = 2019;
        public const int IGREQUEST_FRAME_MOVE = 2020;
        public const int IGREQUEST_FRAME_ROTATEANDRESIZE = 2021;
        public const int IGREQUEST_FRAME_SELECTLAYER = 2022;
        public const int IGREQUEST_FRAME_RUBBER = 2023;
        public const int IGREQUEST_FRAME_SAVE_BM = 2024;

        public const int IGREQUEST_FRAME_SELECT_MAGIC = 64;

        public const string IGREQUEST_NODENAME = "Request";
        public const string IGREQUEST_NODETYPE = "Type";
        public const string IGREQUEST_NODEID = "Id";
        public const string IGREQUEST_GUID = "Guid";
        public const string IGREQUEST_FRAMEID = "FrameId";
        public const string IGREQUEST_LAYERID = "LayerId";
        public const string IGREQUEST_LAYERIDTO = "LayerIdTo";
        public const string IGREQUEST_LAYERFROMIDS = "LayerFromIds";
        public const string IGREQUEST_PROCESSID = "ProcessId";
        public const string IGREQUEST_PROPIDS = "PropIds";
        public const string IGREQUEST_FILTERID = "FilterId";
        public const string IGREQUEST_ORIGINAL = "Original";
        public const string IGREQUEST_FACEEFFECTID = "FaceEffectId";
        public const string IGREQUEST_COLOR	= "Color";
        public const string IGREQUEST_COLOR2	=	"Color2";
        public const string IGREQUEST_PARAM	= "Param";
        public const string IGREQUEST_PARAM2 = "Param2";
        public const string IGREQUEST_PARAM3 = "Param3";
        public const string IGREQUEST_PATH = "Path";
        public const string IGREQUEST_PATH2 = "Path2";
        public const string IGREQUEST_USERLOGIN = "UserLogin";
        public const string IGREQUEST_WIDTH = "Width";
        public const string IGREQUEST_HEIGHT = "Height";
        public const string IGREQUEST_BACKGROUNDMODE = "BackgroundMode";
        public const string IGREQUEST_COLORMODE = "ColorMode";
        public const string IGREQUEST_SELECTIONTYPE = "SelectionType";
        public const string IGREQUEST_TOLERANCE = "Tolerance";
        public const string IGREQUEST_POINTS = "Points";
        public const string IGREQUEST_POINTS2 = "Points2";
        public const string IGREQUEST_MARKERTYPE = "MarkerType";
        public const string IGREQUEST_MARKERTYPE2 = "MarkerType2";
        public const string IGREQUEST_VISIBLE = "Visible";
        public const string IGREQUEST_STATUSTYPE = "StatusType";
        public const string IGREQUEST_SIZE = "Size";
        public const string IGREQUEST_HISTORYID = "HistoryId";
        public const string IGREQUEST_ISFRAME = "IsFrame";
        public const string IGREQUEST_RETRY = "Retry";
        public const string IGREQUEST_STRENGTH = "Strength";
        public const string IGREQUEST_POSX = "PosX";
        public const string IGREQUEST_POSY = "PosY";
        public const string IGREQUEST_DIRECTIONX = "DirectionX";
        public const string IGREQUEST_DIRECTIONY = "DirectionY";
        public const string IGREQUEST_ANGLE = "Angle";
        public const string IGREQUEST_RATE = "Rate";
        public const string IGREQUEST_TRANSPARENCY = "Transparency";
        public const string IGREQUEST_LOADAS = "LoadAs";
        public const string IGREQUEST_BRUSHSIZE = "BrushSize";
        public const string IGREQUEST_AUTOROTATE = "AutoRotate";
        
        public static string sRequestWorkspaceXMLTemplate = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Request Type=\"" + IGREQUEST_WORKSPACE.ToString() + "\" Id=\"0\" Guid=\"\">\n<Params/>\n</Request>";
        public static string sRequestFrameXMLTemplate = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Request Type=\"" + IGREQUEST_FRAME.ToString() + "\" Id=\"0\" Guid=\"\">\n<Params FrameId=\"0\"/>\n</Request>";
        public static string sRequestServerXMLTemplate = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Request Type=\"" + IGREQUEST_SERVER.ToString() + "\" Id=\"0\" Guid=\"\">\n<Params/>\n</Request>";
        protected string m_sGuid = null;
        protected string m_sResult = "No result";
        protected XmlNode m_xmlNodeRequest = null;
        protected int m_nTypeId = -1;        
        protected bool m_bRetry = false;
        protected IGRequestProcessing m_reqProc = null;
        protected IGConnection m_connection = null;
        public IGConnection Connection { get { return m_connection; } set { m_connection = value; } }
        public IGConnectionLocal UserConnection { get { return (IGConnectionLocal)m_connection; } }
        protected AutoResetEvent m_stopWaitHandle = null;

        public IGRequest(string sName, int nType, int nReqId)
            : base((nType == IGREQUEST_WORKSPACE) ? sRequestWorkspaceXMLTemplate
                    : (nType == IGREQUEST_FRAME) ? sRequestFrameXMLTemplate
                    : sRequestServerXMLTemplate)
        {
            m_xmlNodeRequest = m_xmlDoc.SelectSingleNode(IGREQUEST_NODENAME);
            XmlNode xmlNodeRequestId = m_xmlNodeRequest.Attributes.GetNamedItem(IGREQUEST_NODEID);
            xmlNodeRequestId.Value = nReqId.ToString();
            m_sGuid = Guid.NewGuid().ToString();
            XmlNode xmlNodeRequestGuid = m_xmlNodeRequest.Attributes.GetNamedItem(IGREQUEST_GUID);
            xmlNodeRequestGuid.Value = m_sGuid;
            XmlNode xmlNodeTypeId = m_xmlNodeRequest.Attributes.GetNamedItem(IGREQUEST_NODETYPE);
            m_nTypeId = Convert.ToInt32(xmlNodeTypeId.Value);
            m_sString = sName;
            m_nId = nReqId;
            m_bRetry = (GetParameter(IGREQUEST_RETRY.ToString()) != null);
        }

        public IGRequest(string sReqString, XmlDocument xmlDoc)
            : base(xmlDoc, sReqString)
        {
            m_xmlNodeRequest = m_xmlDoc.SelectSingleNode(IGREQUEST_NODENAME);
            if (m_xmlNodeRequest != null)
            {
                XmlNode xmlNodeRequestGuid = m_xmlNodeRequest.Attributes.GetNamedItem(IGREQUEST_GUID);
                if (xmlNodeRequestGuid != null)
                    m_sGuid = xmlNodeRequestGuid.Value;
                XmlNode xmlNodeTypeId = m_xmlNodeRequest.Attributes.GetNamedItem(IGREQUEST_NODETYPE);
                if (xmlNodeTypeId != null)
                    m_nTypeId = Convert.ToInt32(xmlNodeTypeId.Value);
                XmlNode xmlNodeId = m_xmlNodeRequest.Attributes.GetNamedItem(IGREQUEST_NODEID);
                if (xmlNodeId != null)
                    m_nId = Convert.ToInt32(xmlNodeId.Value);
                m_bRetry = (GetParameter(IGREQUEST_RETRY.ToString()) != null);
            }
        }

        public void SetEventDone(AutoResetEvent stopWaitHandle)
        {
            m_stopWaitHandle = stopWaitHandle;
        }

        public void FireEventDone(string answer)
        {
            if (m_stopWaitHandle != null)
            {
                SetResult(answer);
                m_stopWaitHandle.Set();
            }
        }

        public string GetResult()
        {
            return m_sResult;
        }
        
        public void SetResult(string Str)
        {
            m_sResult = Str;
        }

        protected static string getParameter(XmlDocument xmlDoc, string sName)
        {
            XmlNode xmlNodeRequest = xmlDoc.SelectSingleNode(IGREQUEST_NODENAME);
            if (xmlNodeRequest == null)
                return null;
            XmlNode paramNode = xmlNodeRequest.FirstChild;
            if (paramNode == null)
                return null;
            XmlNode attNode = paramNode.Attributes.GetNamedItem(sName);
            if (attNode == null)
                return null;
            return attNode.Value;
        }

        public void InitReqProcAndConnection()
        {
            m_reqProc = IGRequestProcessing.GetRequest(m_sGuid);
            if (m_reqProc != null)
            {
                if (m_serverMgr.IsLocalServer())
                    m_serverMgr.GetConnection(GetParameterValue(IGREQUEST_USERLOGIN), out m_connection, true);
            }
        }        

        public bool Valid
        {
            get
            {
                return ((m_xmlNodeRequest != null) &&
                    (m_sGuid != null) && (m_sGuid != "") &&
                    IsTypeValid(m_nTypeId) &&
                    (m_xmlNodeRequest.FirstChild != null) &&
                    (m_xmlNodeRequest.FirstChild.Attributes != null));
            }
        } 

        public virtual bool NeedAccount()
        {
            return false;
        }        

        public int GetId()
        {
            return m_nId;
        }

        public void SetId(int id)
        {
            m_nId = id;
        }

        public override string GetXml()
        {
            return (m_xmlNodeRequest == null) ? "" : m_xmlDoc.OuterXml;
        }

        public override XmlNode GetXmlNode()
        {
            return m_xmlNodeRequest;
        }

        public string GetGuid()
        {
            return m_sGuid;
        }

        public void SetGuid(string guid)
        {
            m_sGuid = guid;
            SetAttribute(IGREQUEST_GUID, guid);
        }

        public int GetTypeId()
        {
            return m_nTypeId;
        }

        public static bool IsTypeValid(int nTypeId)
        {
            return ((nTypeId == IGREQUEST_WORKSPACE) ||
                (nTypeId == IGREQUEST_FRAME) ||
                (nTypeId == IGREQUEST_SERVER));
        }

        public static int GetRequestId(XmlDocument xmlRequestDoc, out XmlNode xmlRequestNode)
        {
            return GetRequestId(null, ref xmlRequestDoc, out xmlRequestNode);
        }

        public static int GetRequestId(string sXml, ref XmlDocument xmlRequestDoc, out XmlNode xmlRequestNode)
        {
            if (xmlRequestDoc == null)
            {
                xmlRequestDoc = new XmlDocument();
                try
                {
                    xmlRequestDoc.LoadXml(sXml.Trim());
                }
                catch (Exception exc)
                {
                    IGServerManager.Instance.AppendError(exc.ToString());
                    xmlRequestDoc = null;
                    xmlRequestNode = null;
                    return -1;
                }
            }
            xmlRequestNode = xmlRequestDoc.SelectSingleNode(IGREQUEST_NODENAME);
            XmlNode xmlRequestNodeId = xmlRequestNode.Attributes.GetNamedItem(IGREQUEST_NODEID);
            return Convert.ToInt32(xmlRequestNodeId.Value);
        }

        public bool IsSMRequest()
        {
            return IGSMRequest.IsSMRequest(m_nId);
        }

        // called by IGAnswer
        public virtual bool ProcessAnswer(IGAnswer answer)
        {
            return true;
        }

        // called by IGAnswer
        public virtual void GetParams()
        {
        }

        // called by IGAnswer
        // this function is called before GetParams(), it may be used to
        // store some of the session param or answer param in the request xml
        public virtual void GetParams(IGAnswer answer, HttpSessionState session)
        {
        }
    }

    [DataContract]
    public class IGRequestUnknown : IGRequest
    {
        public IGRequestUnknown(XmlDocument xmlDoc)
            : base("Unknown request", xmlDoc)
        {
        }
    }        
}
