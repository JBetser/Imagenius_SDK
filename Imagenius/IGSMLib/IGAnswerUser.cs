using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Web.SessionState;
using System.Text.RegularExpressions;
using System.Runtime.Serialization;

namespace IGSMLib
{
    [DataContract]
    public class IGAnswerUser : IGAnswer
    {
        protected string m_sUser;
        protected IGConnectionLocal m_connection;
        public IGAnswerUser(IGConnectionLocal connection, XmlDocument xmlDoc, string sAnswerString)
            : base(xmlDoc, sAnswerString)
        {
            m_connection = connection;
            XmlNode xmlNodeAnswer = m_xmlDoc.SelectSingleNode(IGANSWER_NODENAME);
            XmlNode xmlNodeUser = xmlNodeAnswer.Attributes.GetNamedItem(IGANSWER_USERLOGIN);
            m_sUser = xmlNodeUser.Value;
        }
        public IGAnswerUser(XmlNode xmlRequest, int nReqId, string sAnswerString)
            : base(xmlRequest, nReqId, sAnswerString) { }
        public IGAnswerUser(IGConnectionLocal connection, int nAnswerId, string sAnswerString)
            : base(IGANSWER_XMLTEMPLATE, nAnswerId, sAnswerString)
        {
            m_connection = connection;
            if (m_connection != null)
            {
                m_sUser = m_connection.GetName();
                SetAttribute(IGANSWER_USERLOGIN, m_sUser);
            }
        }
        public override string ToString()
        {
            return m_sUser + ": " + base.ToString();
        }        
    }

    [DataContract]
    public class IGAnswerDisconnected : IGAnswerUser
    {
        public IGAnswerDisconnected(IGConnectionLocal connection, XmlDocument xmlDoc)
            : base(connection, xmlDoc, "disconnected from server") { }
        public IGAnswerDisconnected(IGConnectionLocal connection)
            : base(connection, (int)IGANSWER_ID.IGANSWER_WORKSPACE_DISCONNECTED, "") { }
        protected override bool PreProcess()
        {
            m_serverMgr.ProcessAnswer(this);
            InternalProcess();
            return true;
        }
        protected override bool InternalProcess()
        {
            if (m_connection != null)
                m_connection.OnUserDisconnected();
            return true;
        }
    }

    [DataContract]
    public class IGAnswerHearthbeat : IGAnswerUser
    {
        public IGAnswerHearthbeat(IGConnectionLocal connection, XmlDocument xmlDoc)
            : base(connection, xmlDoc, "hearthbeat") { }
        protected override bool PreProcess()
        {
            InternalProcess();
            return true;
        }
        protected override bool InternalProcess()
        {
            m_connection.Hearthbeat();
            return true;
        }
    }

    [DataContract]
    public class IGAnswerPong : IGAnswerUser
    {
        public IGAnswerPong(IGConnectionLocal connection, XmlDocument xmlDoc)
            : base(connection, xmlDoc, "pong") { }
    }

    [DataContract]
    public class IGAnswerFailed : IGAnswerUser
    {
        public IGAnswerFailed(IGConnectionLocal connection, XmlDocument xmlDoc)
            : base(connection, xmlDoc, "request failed") { }
        protected override bool PreProcess()
        {
            if (m_reqProc == null)
                return false;
            if (m_reqProc.GetReqId() == IGRequest.IGREQUEST_WORKSPACE_CONNECT)
            {
                IGAnswerDisconnected answerDisconnected = new IGAnswerDisconnected(m_connection);
                answerDisconnected.Execute();
                return true;
            }
            return false;
        }
        public override void GetParams()
        {
            base.GetParams();
        }
        protected override bool InternalProcess()
        {
            return true;
        }
    }

    [DataContract]    
    public class IGAnswerFrameDone : IGAnswerUser
    {        
        [DataMember(Name = "History", Order = 0)]
        string history;
        [DataMember(Name = "FrameProperties", Order = 1)]
        string frameProp;

        public IGAnswerFrameDone(IGConnectionLocal connection, XmlDocument xmlDoc)
            : base(connection, xmlDoc, "done") { }
        public IGAnswerFrameDone(IGConnectionLocal connection, XmlDocument xmlDoc, string sAnswerName)
            : base(connection, xmlDoc, sAnswerName) { }
        public override void GetParams()
        {
            base.GetParams();
            history = GetParameterValue(IGANSWER_ALLSTEPIDS);
            frameProp = GetParameterValue(IGANSWER_FRAMEPROPERTIES);
        }

        public override void GetParams(HttpSessionState session)
        {
            string sFrameId = GetParameterValue(IGRequest.IGREQUEST_FRAMEID);
            bool bFrameExists = ((session[IGANSWER_RESTOREDFRAMEIDS] != null) && ((string)session[IGANSWER_RESTOREDFRAMEIDS] != "")
                                    && ((string)session[IGANSWER_RESTOREDFRAMEIDS]).Contains(sFrameId));

            if (bFrameExists)
            {                             
                string sCurStepIds = GetParameterValue(IGANSWER_STEPIDS);
                if ((sCurStepIds != null) && (sCurStepIds != ""))
                {
                    // get Frame index
                    SetParameter(IGANSWER_RESTOREDFRAMEIDS, (string)session[IGANSWER_RESTOREDFRAMEIDS]);
                    List<string> lsFrameIds = new List<string>();
                    splitParamToList(lsFrameIds, IGANSWER_RESTOREDFRAMEIDS);
                    int nIdxFrame = lsFrameIds.IndexOf(sFrameId);
                    
                    // step ids
                    SetParameter(IGANSWER_ALLSTEPIDS, (string)session[IGANSWER_ALLSTEPIDS]);
                    List<string> lsStepIds = new List<string>();
                    splitParamToList(lsStepIds, IGANSWER_ALLSTEPIDS);
                    lsStepIds[nIdxFrame] = sCurStepIds;
                    lsStepIds.RemoveAt(0);
                    session[IGANSWER_ALLSTEPIDS] = createParamFromList(lsStepIds);

                    // Frame properties
                    string sCurFrameProperties = GetParameterValue(IGANSWER_FRAMEPROPERTIES);
                    SetParameter(IGANSWER_FRAMEPROPERTIES, (string)session[IGANSWER_FRAMEPROPERTIES]);
                    List<string> lsFrameProperties = new List<string>();
                    splitParamToList(lsFrameProperties, IGANSWER_FRAMEPROPERTIES);
                    lsFrameProperties[nIdxFrame] = sCurFrameProperties;
                    lsFrameProperties.RemoveAt(0);
                    session[IGANSWER_FRAMEPROPERTIES] = createParamFromList(lsFrameProperties);
                }
            }
            else
            {
                if ((session[IGANSWER_ALLSTEPIDS] == null) || ((string)session[IGANSWER_ALLSTEPIDS] == ""))
                {
                    session[IGANSWER_ALLSTEPIDS] = "1";
                    session[IGANSWER_FRAMEPROPERTIES] = GetParameterValue(IGANSWER_FRAMEPROPERTIES);
                }
                else
                {
                    session[IGANSWER_ALLSTEPIDS] += ",1";
                    session[IGANSWER_FRAMEPROPERTIES] += "," + GetParameterValue(IGANSWER_FRAMEPROPERTIES);
                }                
            }
            SetParameter(IGANSWER_ALLSTEPIDS, (string)session[IGANSWER_ALLSTEPIDS]);
            SetParameter(IGANSWER_FRAMEPROPERTIES, (string)session[IGANSWER_FRAMEPROPERTIES]);

            AddRequestParams(session);
        }

        public virtual void AddRequestParams(HttpSessionState session)
        {
            GetRequestParams(session);
        }

        public void GetRequestParams(HttpSessionState session)
        {
            // Request get param overriding
            XmlNode xmlReqGuid = GetAttribute(IGANSWER_REQGUID);
            if (xmlReqGuid == null)
                return;
            IGRequestProcessing reqProc = IGRequestProcessing.GetRequest(xmlReqGuid.Value);
            if (reqProc == null)
                return;
            reqProc.GetRequest().GetParams(this, session);
        }
    }

    [DataContract]
    public class IGAnswerProgress : IGAnswerUser
    {
        [DataMember(Name = "Progress", Order = 0)]
        string progress;

        public IGAnswerProgress(IGConnectionLocal connection, XmlDocument xmlDoc)
            : base(connection, xmlDoc, "progressing") { }
        protected override bool InternalProcess()
        {
            IGRequestProcessing.Progress(m_xmlNodeAnswer);
            return true;    // not critical
        }
        public override void GetParams()
        {
            base.GetParams();
            List<string> lsParams = new List<string>();
            progress = GetParameterValue(IGSMStatus.IGSMSTATUS_PROGRESS);
        }
    }

    [DataContract]
    public class IGAnswerPropertyChanged : IGAnswerUser
    {
        [DataMember(Name = "IsFrame", Order = 1)]
        bool isFrame;
        [DataMember(Name = "Properties", Order = 2)]
        string props;

        public IGAnswerPropertyChanged(IGConnectionLocal connection, XmlDocument xmlDoc)
            : base(connection, xmlDoc, "Property changed") { }
        public override void GetParams()
        {
            base.GetParams();
            isFrame = GetParameterValue(IGANSWER_ISFRAME) == "1";
            props = GetParameterValue(isFrame ? IGANSWER_FRAMEPROPERTIES : IGANSWER_WORKSPACEPROPERTIES);
        }
        public override void GetParams(HttpSessionState session)
        {
            string sIsFrame = GetParameterValue(IGANSWER_ISFRAME);
            if (sIsFrame == "1")
            {
                string sFrameId = GetParameterValue(IGRequest.IGREQUEST_FRAMEID);
                bool bFrameExists = ((session[IGANSWER_RESTOREDFRAMEIDS] != null) && ((string)session[IGANSWER_RESTOREDFRAMEIDS] != "")
                                        && ((string)session[IGANSWER_RESTOREDFRAMEIDS]).Contains(sFrameId));
                if (bFrameExists)
                {
                    // get Frame index
                    SetParameter(IGANSWER_RESTOREDFRAMEIDS, (string)session[IGANSWER_RESTOREDFRAMEIDS]);
                    List<string> lsFrameIds = new List<string>();
                    splitParamToList(lsFrameIds, IGANSWER_RESTOREDFRAMEIDS);
                    int nIdxFrame = lsFrameIds.IndexOf(sFrameId);

                    // Frame properties
                    string sCurFrameProperties = GetParameterValue(IGANSWER_FRAMEPROPERTIES);
                    SetParameter(IGANSWER_FRAMEPROPERTIES, (string)session[IGANSWER_FRAMEPROPERTIES]);
                    List<string> lsFrameProperties = new List<string>();
                    splitParamToList(lsFrameProperties, IGANSWER_FRAMEPROPERTIES);
                    lsFrameProperties[nIdxFrame] = sCurFrameProperties;
                    lsFrameProperties.RemoveAt(0);
                    session[IGANSWER_FRAMEPROPERTIES] = createParamFromList(lsFrameProperties);
                }
            }
            else
                session[IGANSWER_WORKSPACEPROPERTIES] = GetParameterValue(IGANSWER_WORKSPACEPROPERTIES);
        }
    }
}
