using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Net;
using System.IO;
using System.Net.Sockets;
using System.Web.SessionState;
using System.Drawing;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;

namespace IGSMLib
{
    [DataContract]
    [KnownType(typeof(IGRequestFrameSave))]
    [KnownType(typeof(IGRequestFrameSaveBeetleMorph))]
    [KnownType(typeof(IGRequestFrameAddLayer))]
    [KnownType(typeof(IGRequestFrameCopy))]
    [KnownType(typeof(IGRequestFrameCut))]
    [KnownType(typeof(IGRequestFramePaste))]
    [KnownType(typeof(IGRequestFrameDelete))]
    [KnownType(typeof(IGRequestFrameSmartDelete))]
    [KnownType(typeof(IGRequestFrameRemoveLayer))]
    [KnownType(typeof(IGRequestFrameMoveLayer))]
    [KnownType(typeof(IGRequestFrameMove))]
    [KnownType(typeof(IGRequestFrameRotateAndResize))]
    [KnownType(typeof(IGRequestFrameSelectLayer))]
    [KnownType(typeof(IGRequestFrameMergeLayer))]
    [KnownType(typeof(IGRequestFrameFilterLayer))]
    [KnownType(typeof(IGRequestFrameFilterLayerBrightness))]
    [KnownType(typeof(IGRequestFrameFilterLayerMorphing))]
    [KnownType(typeof(IGRequestFrameFilterLayerFace))]
    [KnownType(typeof(IGRequestFrameIndexLayer))]
    [KnownType(typeof(IGRequestChangeProperty))]
    [KnownType(typeof(IGRequestFrameClose))]
    [KnownType(typeof(IGRequestFrameUpdate))]
    [KnownType(typeof(IGRequestFrameGotoHistory))]
    [KnownType(typeof(IGRequestFrameSelect))]
    [KnownType(typeof(IGRequestFrameSetLayerVisible))]
    [KnownType(typeof(IGRequestFrameDrawLines))]
    [KnownType(typeof(IGRequestFrameRubber))]
    [KnownType(typeof(IGRequestFramePickColor))]
    [KnownType(typeof(IGRequestWorkspaceConnect))]
    [KnownType(typeof(IGRequestWorkspaceDisconnect))]
    [KnownType(typeof(IGRequestWorkspacePing))]
    [KnownType(typeof(IGRequestWorkspaceLoad))]
    [KnownType(typeof(IGRequestWorkspaceNew))]
    [KnownType(typeof(IGRequestWorkspaceShow))]
    [KnownType(typeof(IGSMRequestPing))]
    [KnownType(typeof(IGSMRequestGetStatus))]
    [KnownType(typeof(IGSMRequestTerminate))]
    [KnownType(typeof(IGSMRequestAddIGApps))]
    [KnownType(typeof(IGSMRequestDestroyAccount))]
    [KnownType(typeof(IGSMRequestDeleteImage))]
    [KnownType(typeof(IGSMRequestDownload))]
    [KnownType(typeof(IGSMRequestUpload))]
    public class IGAnswer : IGXmlMessage
    {
        public const string  IGANSWER_XMLTEMPLATE       =   "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Answer Id=\"\" UserLogin=\"\" ReqGuid=\"\">\n<Params/>\n</Answer>";
        public const string  IGANSWER_NODENAME			=	"Answer";
        public const string  IGANSWER_ANSWERID			=	"Id";
        public const string  IGANSWER_REQGUID			=	"ReqGuid";
        public const string  IGANSWER_USERLOGIN			=	"UserLogin";
        public const string  IGANSWER_RESTOREDFRAMEIDS	=   "RestoredFrameIds";
        public const string  IGANSWER_SELECTEDFRAMEID   =   "SelectedFrameId";
        public const string  IGANSWER_SERVERIP          =   "ServerIP";
        public const string  IGANSWER_SERVERPORT        =   "ServerPort";
        public const string  IGANSWER_REQNAME           =   "ReqName";
        public const string  IGANSWER_USEROUTPUT        =   "UserOutput";
        public const string  IGANSWER_IMAGELIBRARY      =   "ImageLibrary";
        public const string  IGANSWER_PICTURENAME       =   "PictureName";
        public const string  IGANSWER_FRAMENAMES        =   "FrameNames";
        public const string  IGANSWER_REQGUIDS          =   "ReqGuids";
        public const string  IGANSWER_SELECTEDREQGUID   =   "SelectedReqGuid";
        public const string  IGANSWER_CURRENTVIEW       =   "CurrentView";
        public const string  IGANSWER_CURRENTVIEWPORTMAP=   "CurrentViewportMap";
        public const string  IGANSWER_NBLAYERS          =   "NbLayers";
        public const string  IGANSWER_LAYERVISIBILITY   =   "LayerVisibility";
        public const string  IGANSWER_ALLNBLAYERS       =   "AllNbLayers";
        public const string  IGANSWER_ALLLAYERVISIBILITY=   "AllLayerVisibility";
        public const string  IGANSWER_ALLSTEPIDS        =   "AllStepIds";
        public const string  IGANSWER_STEPIDS           =   "StepIds";
        public const string  IGANSWER_WORKSPACEPROPERTIES = "WorkspaceProperties";
        public const string  IGANSWER_FRAMEPROPERTIES   =   "FrameProperties";
        public const string  IGANSWER_ALLFRAMEPROPERTIES =  "AllFrameProperties";
        public const string  IGANSWER_ISFRAME           =   "IsFrame";
        public const string  IGANSWER_RELOADPAGE        =   "ReloadPage";
        
        public enum IGANSWER_ID
        {
            IGANSWER_WORKSPACE_CONNECTED = 3000,
			IGANSWER_WORKSPACE_DISCONNECTED = 3001,
			IGANSWER_WORKSPACE_ACTIONFAILED	= 3002,
			IGANSWER_WORKSPACE_PONG = 3003,
            IGANSWER_WORKSPACE_HEARTHBEAT = 3004,
            IGANSWER_WORKSPACE_PROPERTYCHANGED = 3005,
            IGANSWER_FRAME_CHANGED = 4000,
			IGANSWER_FRAME_ACTIONDONE = 4001,
			IGANSWER_FRAME_ACTIONFAILED	= 4002,
            IGANSWER_FRAME_ACTIONINPROGRESS = 4003 };

        [DataMember(Name = "Status", Order = 0)]
        protected string status = "Error";

        [DataMember(Name = "AnswerId", Order = 1)]
        protected int m_nId = -1;

        [DataMember(Name = "RequestGuid", Order = 2)]
        protected string m_sGuid = null;

        [DataMember(Name = "RequestParams", Order = 3)]
        IGRequest request = null;

        protected static InterThreadHashtable mg_hashAnswerIds = new InterThreadHashtable();
        protected XmlNode m_xmlNodeAnswer = null;        
        protected IGRequestProcessing m_reqProc = null;
        
        protected IGAnswer(XmlNode xmlRequest, int nReqId, string sAnswerString)
            : base(IGANSWER_XMLTEMPLATE)
        {
            m_xmlNodeAnswer = m_xmlDoc.SelectSingleNode(IGANSWER_NODENAME);
            XmlNode xmlId = m_xmlNodeAnswer.Attributes.GetNamedItem(IGANSWER_ANSWERID);
            xmlId.Value = nReqId.ToString();
            m_nId = Convert.ToInt32(xmlId.Value);
            m_sString = sAnswerString;
            XmlNode xmlUser = m_xmlNodeAnswer.Attributes.GetNamedItem(IGANSWER_USERLOGIN);
            XmlNode xmlFromReqLogin = xmlRequest.FirstChild.Attributes.GetNamedItem(IGRequest.IGREQUEST_USERLOGIN);
            xmlUser.Value = (xmlFromReqLogin == null ? "" : xmlFromReqLogin.Value);
            XmlNode xmlReqGuid = m_xmlNodeAnswer.Attributes.GetNamedItem(IGANSWER_REQGUID);
            XmlNode xmlFromReqGuid = xmlRequest.FirstChild.Attributes.GetNamedItem(IGRequest.IGREQUEST_GUID);
            xmlReqGuid.Value = (xmlFromReqGuid == null ? "" : xmlFromReqGuid.Value);
            if (m_serverMgr.IsLocalServer())
            {
                SetParameter(IGANSWER_SERVERIP, m_serverMgr.ServerIP.ToString());
                SetParameter(IGANSWER_SERVERPORT, ((IGServerManagerLocal)m_serverMgr).ServerPort.ToString());
            }
        }
        
        protected IGAnswer(XmlDocument xmlDocAnswer, string sAnswerString)
            : base(xmlDocAnswer, sAnswerString)
        {
            m_xmlNodeAnswer = m_xmlDoc.SelectSingleNode(IGANSWER_NODENAME);
            XmlNode xmlId = m_xmlNodeAnswer.Attributes.GetNamedItem(IGANSWER_ANSWERID);
            m_nId = Convert.ToInt32(xmlId.Value);
            m_reqProc = IGRequestProcessing.GetRequest(GetReqGuid());
            if (m_serverMgr.IsLocalServer())
            {
                SetParameter(IGANSWER_SERVERIP, m_serverMgr.ServerIP.ToString());
                SetParameter(IGANSWER_SERVERPORT, ((IGServerManagerLocal)m_serverMgr).ServerPort.ToString());
            }   
        }

        protected IGAnswer(string sXMLTemplate, string sAnswerString)
            : base(sXMLTemplate)
        {
            m_xmlNodeAnswer = m_xmlDoc.SelectSingleNode(IGANSWER_NODENAME);
            XmlNode xmlId = m_xmlNodeAnswer.Attributes.GetNamedItem(IGANSWER_ANSWERID);
            m_nId = Convert.ToInt32(xmlId.Value); 
            m_sString = sAnswerString;
            if (m_serverMgr.IsLocalServer())
            {
                SetParameter(IGANSWER_SERVERIP, m_serverMgr.ServerIP.ToString());
                SetParameter(IGANSWER_SERVERPORT, ((IGServerManagerLocal)m_serverMgr).ServerPort.ToString());
            }
        }

        protected IGAnswer(string sXMLTemplate, int nId, string sAnswerString)
            : base(sXMLTemplate)
        {
            m_xmlNodeAnswer = m_xmlDoc.SelectSingleNode(IGANSWER_NODENAME);
            XmlNode xmlId = m_xmlNodeAnswer.Attributes.GetNamedItem(IGANSWER_ANSWERID);
            xmlId.Value = nId.ToString();
            m_nId = nId;
            m_sString = sAnswerString;
            if (m_serverMgr.IsLocalServer())
            {
                if (m_serverMgr.ServerIP != null)
                    SetParameter(IGANSWER_SERVERIP, m_serverMgr.ServerIP.ToString());
                SetParameter(IGANSWER_SERVERPORT, ((IGServerManagerLocal)m_serverMgr).ServerPort.ToString());
            }
        }

        public int GetId()
        {
            return m_nId;
        }

        public string GetReqGuid()
        {
            return GetAttributeValue(IGAnswer.IGANSWER_REQGUID);
        }

        public void SetReqGuid(string reqGuid)
        {
            SetAttribute(IGAnswer.IGANSWER_REQGUID, reqGuid);
        }

        public virtual bool IsError()
        {
            return false;
        }

        public override string GetXml()
        {
            return m_xmlDoc.InnerXml;
        }

        public override XmlNode GetXmlNode()
        {
            return m_xmlNodeAnswer;
        }

        public void Init(string sGuid, string sReqName)
        {
            XmlNode xmlNodeGuid = GetAttribute(IGANSWER_REQGUID);
            xmlNodeGuid.Value = sGuid;
            XmlNode xmlNodeReqName = GetAttribute(IGANSWER_REQNAME);
            xmlNodeReqName.Value = sReqName;
        }

        public void SetRequest(IGRequest req)
        {
            if (req.ProcessAnswer(this))
                request = req;
        }

        public static int GetAnswerId(string sXml, out XmlDocument xmlAnswerDoc, out XmlNode xmlAnswerNode)
        {
            xmlAnswerDoc = new XmlDocument();
            try
            {
                xmlAnswerDoc.LoadXml(sXml.Trim());
            }
            catch (Exception exc)
            {
                IGServerManager.Instance.AppendError(exc.ToString());
                xmlAnswerDoc = null;
                xmlAnswerNode = null;
                return -1;
            }
            xmlAnswerNode = xmlAnswerDoc.SelectSingleNode(IGANSWER_NODENAME);
            XmlNode xmlNodeAnswerId = xmlAnswerNode.Attributes.GetNamedItem(IGANSWER_ANSWERID);
            return Convert.ToInt32(xmlNodeAnswerId.Value);
        }

        public static int GetAnswerId(string sXml)
        {
            XmlDocument xmlDoc;
            XmlNode xmlNodeAnswer;
            return GetAnswerId(sXml, out xmlDoc, out xmlNodeAnswer);
        }

        public bool IsSMAnswer()
        {
            return IGSMAnswer.IsSMAnswer(m_nId);
        }

        public bool IsSMError()
        {
            return IGSMAnswer.IsSMError(m_nId);
        }

        public virtual bool Execute()
        {
            if ((m_xmlDoc == null) ||
                (m_xmlNodeAnswer == null) ||
                (m_nId < (int)IGANSWER_ID.IGANSWER_WORKSPACE_CONNECTED))
                return false;
            // Server answer processing steps
            if (PreProcess())
                return true;
            bool bRet = true;
            if ((m_serverMgr.IsLocalServer() && !IsSMAnswer()) ||
                (!m_serverMgr.IsLocalServer() && IsSMAnswer()) ||
                (m_nId == (int)IGAnswer.IGANSWER_ID.IGANSWER_FRAME_ACTIONINPROGRESS) ||
                IsSMError())
            {
                // Answer processing
                bRet = InternalProcess();
                if (!bRet)
                    m_serverMgr.AppendError("Error during internal processing of Answer: " + GetReqGuid());
            }
            if (!m_serverMgr.ProcessAnswer(this))
            {
                m_serverMgr.AppendError("Error during server manager processing of Answer: " + m_sString + " Id: " + GetReqGuid());
                return false;
            }
            if (!PostProcess())
            {
                m_serverMgr.AppendError("Error during post processing of Answer: " + GetReqGuid());
                bRet = false;
            }
            return bRet;
        }

        protected virtual bool PreProcess()
        {
            // answer filtering
            return false;
        }

        protected virtual bool InternalProcess()
        {
            string sGuid = GetAttributeValue(IGAnswer.IGANSWER_REQGUID);
            if (sGuid == null)
                return true;
            IGRequestProcessing reqProc = IGRequestProcessing.GetRequest(sGuid);
            if (reqProc == null)
                return true;
            IGRequest request = reqProc.GetRequest();
            request.InitReqProcAndConnection();
            return request.ProcessAnswer(this);
        }

        public virtual bool PostProcess()
        {
            return true;
        }

        public string ToClientOutput()
        {
            GetParams();
            status = "OK";
            MemoryStream stream = new MemoryStream();
            DataContractJsonSerializer serializer = new 
              DataContractJsonSerializer(GetType());
            serializer.WriteObject(stream, this);
            stream.Position = 0;
            StreamReader streamReader = new StreamReader(stream);
            return streamReader.ReadToEnd();
        }            

        public virtual void GetParams()
        {
            m_sGuid = GetReqGuid();
            if (request != null)
                request.GetParams();

        }

        // this function is called before GetParams(), it may be used to
        // store some of the session param in the answer xml
        public virtual void GetParams(HttpSessionState session)
        {
        }        
    }

    [DataContract]
    public class IGAnswerUnknown : IGAnswer
    {
        public IGAnswerUnknown(XmlDocument xmlDoc)
            : base(xmlDoc, "Unknown answer") {  }
    }        
}
