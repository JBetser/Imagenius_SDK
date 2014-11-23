using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Timers;

namespace IGSMLib
{
    public class IGRequestProcessing
    {
        protected XmlNode m_xmlNodeRequest = null;
        protected int m_nProgress = -1;
        protected int m_nTypeId = -1;
        protected string m_sGuid = null;
        protected string m_sUser = null;
        protected string m_sFrameId = null;
        protected IGRequest m_request = null;
        protected System.Timers.Timer m_timer;
        protected long m_nProgressTime = -1;
        protected int m_nReqId = -1;
        protected IGSMStatus.IGState m_eStatus = IGSMStatus.IGState.IGSMSTATUS_NOTRESPONDING;
        protected IGServer m_server = null;
        protected int m_nNbRetry = 0;

        private static SynchronizedCollection<IGRequestProcessing> s_lProcessingRequests = new SynchronizedCollection<IGRequestProcessing>();

        public IGRequestProcessing(IGRequest request, IGServer server)
        {
            m_server = server;
            m_request = request;
            m_xmlNodeRequest = m_request.GetXmlNode();
            XmlNode xmlNodeTypeId = request.GetAttribute(IGRequest.IGREQUEST_NODETYPE);
            if (xmlNodeTypeId != null)
                m_nTypeId = Convert.ToInt32(xmlNodeTypeId.Value);
            XmlNode xmlNodeReqId = request.GetAttribute(IGRequest.IGREQUEST_NODEID);
            if (xmlNodeReqId != null)
                m_nReqId = Convert.ToInt32(xmlNodeReqId.Value);
            XmlNode xmlNodeGuid = request.GetAttribute(IGRequest.IGREQUEST_GUID);
            if (xmlNodeGuid != null)
                m_sGuid = xmlNodeGuid.Value;
            XmlNode xmlNodeUser = request.GetParameter(IGRequest.IGREQUEST_USERLOGIN);
            if (xmlNodeUser != null)
                m_sUser = xmlNodeUser.Value;
            XmlNode xmlNodeFrameId = request.GetParameter(IGRequest.IGREQUEST_FRAMEID);
            if (xmlNodeFrameId != null)
                m_sFrameId = xmlNodeFrameId.Value;
            m_nProgressTime = DateTime.UtcNow.Ticks;
            m_nProgress = 0;
            m_eStatus = IGSMStatusUser.IGState.IGSMSTATUS_WORKING;
            m_timer = new System.Timers.Timer();
            m_timer.Elapsed += new ElapsedEventHandler(OnSurveyProcessingEvent);
            m_timer.Interval = HC.IGREQPROC_PROGRESS_CHECKTIME;
            m_timer.Start();
        }

        public override string ToString()
        {
            return m_request.ToString();
        }

        public string GetUser()
        {
            return m_sUser;
        }

        public string GetGuid()
        {
            return m_sGuid;
        }

        public IGRequest GetRequest()
        {
            return m_request;
        }

        public int GetReqId()
        {
            return m_nReqId;
        }

        public IGSMStatus.IGState GetStatus()
        {
            return m_eStatus;
        }

        public string GetFrameId()
        {
            return m_sFrameId;
        }

        public int GetProgress()
        {
            return m_nProgress;
        }

        public void OnSurveyProcessingEvent(object source, ElapsedEventArgs e)
        {
            lock (this)
            {
                if (((DateTime.UtcNow.Ticks - m_nProgressTime) / 10000) > HC.PROGRESS_TIMEOUT_LOCAL)
                {
                    if (m_eStatus != IGSMStatusUser.IGState.IGSMSTATUS_NOTRESPONDING)
                    {
                        IGServerManager serverMgr = IGServerManager.Instance;
                        if (m_nNbRetry++ >= 1)
                        {
                            //m_request.SetParameter(IGRequest.IGREQUEST_RETRY, m_nNbRetry.ToString());
                            //m_nProgressTime = DateTime.UtcNow.Ticks;
                            m_eStatus = IGSMStatusUser.IGState.IGSMSTATUS_NOTRESPONDING;
                            serverMgr.AppendError("TIMEOUT, request not responding: " + m_request.ToString());
                            IGRequestProcessing.Remove(this);
                        }
                    }
                }
                else
                {
                    m_eStatus = IGSMStatusUser.IGState.IGSMSTATUS_WORKING;
                }
            }
        }

        public static bool IsEmpty()
        {
            return (s_lProcessingRequests.Count == 0);
        }

        public static bool IsBusy(string sUser)
        {
            foreach (IGRequestProcessing reqProc in s_lProcessingRequests)
            {
                if (reqProc.m_sUser == sUser)
                    return true;
            }
            return false;
        }

        public static bool IsFrameBusy(string sUser, string sFrameId)
        {
            foreach (IGRequestProcessing reqProc in s_lProcessingRequests)
            {
                if ((reqProc.m_sUser == sUser) &&
                    (reqProc.m_sFrameId == sFrameId))
                    return true;
            }
            return false;
        }

        public static void Add(IGRequest request, IGServer server)
        {            
            if (request.GetId() == IGRequest.IGREQUEST_WORKSPACE_DISCONNECT)
            {
                // server does not wait for a disconnection answer
                IGServerManager.Instance.AppendInfo("To server " + server.ToString() + ": Disconnecting User Request " + request.ToString());
                return;
            }
            else
            {
                if (!request.Silent)
                    IGServerManager.Instance.AppendInfo("To server " + server.ToString() + ": Processing Request " + request.ToString());
                s_lProcessingRequests.Add(new IGRequestProcessing(request, server));
            }
        }

        protected delegate void SetFrameId(IGRequestProcessing reqProc, string sText);

        private static void SetRequestFrameId(IGRequestProcessing reqProc, string sText)
        {
            reqProc.m_sFrameId = sText;
        }

        public static bool Remove(IGAnswer answer)
        {
            if (answer.GetId() == (int)IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_DISCONNECTED)
            {
                // server does not wait for a disconnection answer
                return true;
            }
            else
            {
                bool bRes = true;
                XmlNode xmlNodeRequestGuid = answer.GetAttribute(IGAnswer.IGANSWER_REQGUID);
                if (xmlNodeRequestGuid == null)
                    bRes = false;
                IGRequestProcessing reqProc = GetRequest(xmlNodeRequestGuid.Value);
                if (reqProc == null)
                    bRes = false;
                IGServerManager serverMgr = IGServerManager.Instance;
                if (!bRes)
                {
                    serverMgr.AppendError("- IGRequestProcessing failed to match this answer with a request: \"" + answer.ToString() + "\"");
                    return false;
                }                
                if (answer.IsError() || answer.IsSMError())
                {
                    serverMgr.AppendError("- IGRequestProcessing Error " + answer.ToString());
                    if (answer.IsSMError())
                        ((IGServerManagerRemote)serverMgr).DisconnectUser(answer.GetParameterValue(IGRequest.IGREQUEST_USERLOGIN), true);
                }
                else if (!answer.Silent)
                    serverMgr.AppendInfo("IGRequestProcessing Answer " + answer.ToString());
                IGRequest finishedReq = reqProc.GetRequest();
                if (finishedReq != null)
                    finishedReq.FireEventDone(answer.GetXml());
                return remove(xmlNodeRequestGuid.Value);
            }
        }

        private static bool remove(string sGuid)
        {
            IGRequestProcessing reqFound = null;
            lock (s_lProcessingRequests.SyncRoot)
            {
                foreach (IGRequestProcessing reqProc in s_lProcessingRequests)
                {
                    if (reqProc.m_sGuid == sGuid)
                    {
                        reqFound = reqProc;
                        break;
                    }
                }
                return Remove(reqFound);
            }
        }

        public static void Cleanup(IGConnection connection)
        {
            lock (s_lProcessingRequests.SyncRoot)
            {
                List<IGRequestProcessing> reqToRemove = new List<IGRequestProcessing>();
                foreach (IGRequestProcessing reqProc in s_lProcessingRequests)
                {
                    if (reqProc.m_request.Connection == connection)
                    {
                        reqProc.m_request.FireEventDone(new IGSMAnswer((int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_REQUESTPROCESSING, "Following request crashed in IGServer: " + reqProc.m_request.ToString()).GetXml());
                        reqToRemove.Add(reqProc);
                    }
                }
                foreach (var req in reqToRemove)
                    Remove(req);
            }
        }

        public static bool Remove(IGRequestProcessing reqProc)
        {
            if (reqProc != null)
            {
                reqProc.m_timer.Stop();
                s_lProcessingRequests.Remove(reqProc);
                return true;
            }
            return false;
        }

        public static IGRequestProcessing GetImageRequest(string sGuid)
        {
            foreach (IGRequestProcessing reqProc in s_lProcessingRequests)
            {
                if (reqProc.m_sFrameId == sGuid)
                    return reqProc;
            }
            return null;
        }

        public static IGRequestProcessing GetUserRequest(string sUser, int nReqId)
        {
            foreach (IGRequestProcessing reqProc in s_lProcessingRequests)
            {
                if ((reqProc.m_sUser == sUser) && (reqProc.m_nReqId == nReqId))
                    return reqProc;
            }
            return null;
        }

        public static IGRequestProcessing GetUserRequest(string sUser)
        {
            foreach (IGRequestProcessing reqProc in s_lProcessingRequests)
            {
                if ((reqProc.m_sUser == sUser) && (reqProc.m_sFrameId == null))
                    return reqProc;
            }
            return null;
        }

        public static IGRequestProcessing GetRequest(string sGuid)
        {
            foreach (IGRequestProcessing reqProc in s_lProcessingRequests)
            {
                if (reqProc.m_sGuid == sGuid)
                    return reqProc;
            }
            return null;
        }

        public static bool Progress(XmlNode xmlNodeAnswer)
        {
            XmlNode xmlGuid = xmlNodeAnswer.Attributes.GetNamedItem(IGAnswer.IGANSWER_REQGUID);
            if (xmlGuid == null)
                return false;
            string sGuid = xmlGuid.Value;
            XmlNode xmlNodeProgress = xmlNodeAnswer.FirstChild.Attributes.GetNamedItem(IGSMStatus.IGSMSTATUS_PROGRESS);
            if (xmlNodeProgress == null)
                return false;
            IGRequestProcessing reqProcess = GetRequest(sGuid);
            if (reqProcess == null)
                return false;
            reqProcess.m_nProgressTime = DateTime.UtcNow.Ticks;
            reqProcess.m_nProgress = Convert.ToInt32(xmlNodeProgress.Value);
            if ((reqProcess.m_nProgress < 0) ||
                (reqProcess.m_nProgress > 100))
            {
                reqProcess.m_nProgress = -1;
                return false;
            }            
            return true;
        }
    }
}
