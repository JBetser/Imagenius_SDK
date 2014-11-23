using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Xml;
using System.Timers;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;
using System.Configuration;

namespace IGSMLib
{
    public class IGServerManagerRemote : IGServerManager
    {
        protected IGConfigManagerRemote m_configMgr = IGConfigManagerRemote.GetInstance();

        public IGServerManagerRemote(Dictionary<string, string> appSettings)
            : base(appSettings, new IPEndPoint(IPAddress.Parse(appSettings["IP_LOCAL"]), 0))
        {
            m_logMgr = new EventLog("IGSMService", Environment.MachineName, "Web Server");
        }

        public override void UpdateLogPath()
        {
            string sDate = string.Format("{0:u}", DateTime.Today);
            sDate = sDate.Substring(0, sDate.IndexOf(' '));
            if (sDate != m_sCurDate)
            {
                m_sCurDate = sDate;
                string sIP = (m_endPoint.Address == null ? "NullIP" : m_endPoint.Address.ToString().Replace('.', '_'));
                string logFileName = "SMimageniusRemote_trace" + sDate + "_" + sIP + ".txt";
#if DEBUG
                logFileName = "DEBUG_" + logFileName;
#endif
                m_sLogPath = HC.PATH_REMOTESERVERLOG + logFileName;

            }
        }
        
        public override bool Initialize()
        {
            try
            {
                if (m_bIsInitialized)
                    return true;
                lock (m_lockObject)
                {
                    if (!m_configMgr.Init())
                        return false;
                    List<IGServer> lServers = m_configMgr.GetListServers();
                    if (!Initialize(appSettings["SERVERMGR_IPSHARE"], lServers))
                        return false;
                    UpdateLogPath();
                    foreach (IGServer server in lServers)
                    {

                        if (!(from srv in m_lServers select srv.m_sIpEndPoint).Contains(server.m_sIpEndPoint))
                            m_lServers.Add(server);
                    }
                    ConnectToAllServers();
                    m_bIsInitialized = true;
                }
            }
            finally
            {
                if (m_lServers.Count == 0)
                    IGServerManager.Instance.AppendError("IGServerManagerRemote init failed");

            }
            return true;
        }

        public override int GetConnection(string sServer, out IGConnection retConnection, bool bConnectingAllowed)
        {
            retConnection = null;
            foreach (IGConnection connection in m_lConnections)
            {
                if (connection.m_sUser == sServer)
                {
                    retConnection = connection;
                    return IGSMAnswer.IGSMANSWER_ERROR_NONE;
                }
            }
            return (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_SERVERNOTFOUND;
        }

        public bool Ping(IPEndPoint endPoint)
        {
            try
            {
                IGConnection conn = null;
                GetConnection(m_configMgr.GetServerName(endPoint.ToString()), out conn);
                return conn.Ping();
            }
            catch (Exception exc)
            {
                AppendError("IGServerManagerRemote - Ping failed. Exception: " + exc.ToString());
                return false;
            }
        }
        
        public override bool IsLocalServer()
        {
            return false;
        }
                
        public override bool DisconnectServer(IPEndPoint endPoint, bool bForce)
        {
            try
            {
                IGConnection conn = null;
                GetConnection(m_configMgr.GetServerName(endPoint.ToString()), out conn);
                conn.Terminate(IGSERVERMANAGER_AUTHORITY);
                return true;
            }
            catch (Exception exc)
            {
                AppendError("IGServerManagerRemote - Ping failed. Exception: " + exc.ToString());
                return false;
            }
        }

        public bool Reset(IPEndPoint endPoint)
        {
            try
            {
                IGConnection conn = null;
                GetConnection(m_configMgr.GetServerName(endPoint.ToString()), out conn);
                ((IGConnectionRemote)conn).Reset();
                return true;
            }
            catch (Exception exc)
            {
                AppendError("IGServerManagerRemote - Ping failed. Exception: " + exc.ToString());
                return false;
            }
        }

        public bool DisconnectServer(IPEndPoint endPoint)
        {
            return DisconnectServer(endPoint, false);
        }

        public virtual bool DisconnectUser(string sUser, IPEndPoint serverEndPoint)
        {
            return false;
        }

        public virtual bool DisconnectUser(string sUser, bool bSendRequest)
        {
            return false;
        }

        protected override IGAnswer createAnswer(int nAnswerId, XmlDocument xmlDoc, XmlNode xmlNodeAnswer, IGConnection userConnection)
        {
            if (IGSMAnswer.IsSMAnswer(nAnswerId))
            {
                switch (nAnswerId)
                {
                    case IGSMAnswer.IGSMANSWER_PONG:
                        return new IGSMAnswerPing(xmlDoc);
                    case IGSMAnswer.IGSMANSWER_GETSTATUS:
                        return new IGSMStatus(xmlDoc);
                    case IGSMAnswer.IGSMANSWER_TERMINATED:
                        return new IGSMAnswerTerminate(xmlDoc);
                    case IGSMAnswer.IGSMANSWER_INITIALIZED:
                        return new IGSMAnswerInitialized(xmlDoc);
                    case IGSMAnswer.IGSMANSWER_ERROR:
                        return new IGSMAnswerError(xmlDoc);
                    case IGSMAnswer.IGSMANSWER_ACTIONDONE:
                        return new IGSMAnswerActionDone(xmlDoc);
                    case IGSMAnswer.IGSMANSWER_UPLOADED:
                        return new IGSMAnswerUploadSucceeded(xmlDoc);
                    case IGSMAnswer.IGSMANSWER_DELETED:
                        return new IGSMAnswerDeleted(xmlDoc);
                }
            }
            else
            {
                return base.createAnswer(nAnswerId, xmlDoc, xmlNodeAnswer, userConnection);
            }
            return new IGAnswerUnknown(xmlDoc);
        }        
    }
}
