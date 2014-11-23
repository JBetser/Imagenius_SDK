using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Xml;
using System.Timers;
using System.Threading;
using System.IO;
using System.Diagnostics;
using System.Configuration;

namespace IGSMLib
{
    public class IGServerManagerLocal : IGServerManager
    {
        public const int HEARTHBEATTIME_NBCYCLES = 30;
        public const int HEARTHBEATTIME_APP_NBCYCLES = 1200; // 2mn timeout for no app hearthbeat
        public const int MAXNBCONNECTIONS = 100;
        public const int ADDSERVERINTERVAL = 1000;

        protected IPAddress m_ipWebServer = null;
        public string WebServerIP
        {
            get { return m_ipWebServer.ToString(); }
            set { m_ipWebServer = IPAddress.Parse(value); }
        }
        private const int MAXUSERLENGTH = 64;
        private long m_nHearthbeatTimeLastReceived = -1;    
        private int m_nNbInitialServers = 0;
        protected readonly int m_nServerManagerPort = -1;   // necessary to create local server applications
        private System.Timers.Timer m_timerAddServer;
        private Thread threadLocalCleaner;

        public static IGServerManagerLocal LocalInstance
        {
            get
            {
                return (IGServerManagerLocal)s_serverManager;
            }
        }

        public IGServerManagerLocal(Dictionary<string, string> appSettings) : base(appSettings, new IPEndPoint(IPAddress.Parse(appSettings["IP_LOCAL"]), int.Parse(appSettings["SERVERMGR_PORT_INPUT"])))
        {
            threadLocalCleaner = new Thread(new ThreadStart(IGLocalCleaner.ThreadProc));
            threadLocalCleaner.Priority = ThreadPriority.Lowest;
            threadLocalCleaner.SetApartmentState(ApartmentState.STA);
            threadLocalCleaner.Start();
        }

        public IGServerManagerLocal(Dictionary<string, string> appSettings, int nServerManagerPort)
            : base(appSettings, new IPEndPoint(IPAddress.Parse(appSettings["IP_LOCAL"]), nServerManagerPort))
        {
            m_nServerManagerPort = nServerManagerPort;
        }        

        public int ServerPort
        {
            get
            {
                return m_nServerManagerPort;
            }
        }

        public override void UpdateLogPath()
        {
            string sDate = string.Format("{0:u}", DateTime.Today);
            sDate = sDate.Substring(0, sDate.IndexOf(' '));
            if (sDate != m_sCurDate)
            {
                m_sCurDate = sDate;
                string sIP = (m_endPoint.Address != null ? m_endPoint.Address.ToString().Replace('.', '_') : "0_0_0_0");
                m_sLogPath = HC.PATH_LOCALSERVERLOG + "SMimageniusLocal_trace" + sDate + "_" + sIP + string.Format("_port{0:G}.txt", m_nServerManagerPort);
            }
        }

        public bool Initialize(string sIPWebServer, string sIPShare, List<IGServer> lServerPorts)
        {
            if (m_bIsInitialized)
                return true;
            lock (m_lockObject)
            {
                if (!base.Initialize(sIPShare, lServerPorts))
                    return false;
                m_ipWebServer = IPAddress.Parse(sIPWebServer);
                if (sIPShare.Substring(0, sIPWebServer.LastIndexOf('.')) != m_sSubNetwork)
                    throw new ApplicationException("starting: Share and Client must be in the same subnetwork.");
                UpdateLogPath();
                if (lServerPorts != null)
                {
                    m_nNbInitialServers = lServerPorts.Count;
                    m_lServers = new SynchronizedCollection<IGServer>();
                    foreach (IGServer server in lServerPorts)
                        m_lServers.Add(new IGServerLocal(m_endPoint.Address.ToString(), server.m_endPoint.Port, sIPWebServer));
                    m_timer.Start();
                }
                m_timerAddServer = new System.Timers.Timer();
                m_timerAddServer.Elapsed += new ElapsedEventHandler(onTimedEvent_addServer);
                m_timerAddServer.Interval = ADDSERVERINTERVAL;
                m_timerAddServer.Start();
                if (IGServerManager.AppSettings["DO_MOUNT"] == "Yes")
                {
                    if (!mountDrives())
                        return false;
                }
                if (!ConnectToAllServers())
                    return false;
                m_bIsInitialized = true;
                return true;
            }
        }        

        public int GetNbConnections()
        {
            return m_lConnections.Count;
        }
        
        private void readAnswers()
        {
            List<IGConnection> tmpconnectArray = new List<IGConnection>(m_lConnections);
            foreach (IGConnection connection in tmpconnectArray)
            {
                connection.ProcessAnswers();
            }
        }

        private void onTimedEvent_addServer(object source, ElapsedEventArgs e)
        {
            if (m_lServers.Count < int.Parse(appSettings["SERVERMGR_NBSERVERS"]))
            {
                if (!AddServer())
                {
                    AppendError("- IGServerManagerLocal failed to add a new server");
                    return;
                }
            }
            else
                m_timerAddServer.Stop();
        }

        protected override void onTimedEvent(object source, ElapsedEventArgs e)
        {
            if (m_bTimerProcessing)
                return;
            m_bTimerProcessing = true;
            lock (m_lockTimer)
            {
                try
                {                    
                    // no request processing, just need to take care of the IGServer answers
                    readAnswers();
                }
                catch (Exception exc)
                {
                    m_bTimerProcessing = false;
                    AppendError("- IGServerManagerLocal failed reading buffer from socket port " + m_nServerManagerPort.ToString() + " exception:" + exc.ToString());
                    return;
                }
            }
            m_bTimerProcessing = false;
        }

        public void ProcessRequest(IGRequest request)
        {
            ProcessRequest(request.GetXml(), null);
        }

        public IGRequest ProcessRequest(string sText, AutoResetEvent stopWaitHandle)
        {
            IGRequest request = CreateRequest(sText, stopWaitHandle);
            if (m_processedRequests.ContainsKey(request.GetGuid()))
            {
                m_processedRequests[request.GetGuid()].Key.Execute();
                return request;
            }
            if (IGRequestProcessing.GetRequest(request.GetGuid()) != null)
                return request;
            if (!request.Valid)
            {
                IGSMAnswer.IGSMANSWER_ERROR_CODE errCode = IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_CORRUPTEDREQUEST;
                IGSMAnswerError answerFailure = new IGSMAnswerError(request, errCode);
                request.SetId((int)errCode);
                request.SetResult(answerFailure.GetXml());
                return request;
            }
            switch (request.GetTypeId())
            {
                case IGRequest.IGREQUEST_SERVER:
                    process((IGSMRequest)request);
                    break;

                // Local application request
                case IGRequest.IGREQUEST_FRAME:
                case IGRequest.IGREQUEST_WORKSPACE:
                    int nRes = process(request);
                    if (nRes != IGSMAnswer.IGSMANSWER_ERROR_NONE)
                    {
                        IGSMAnswer.IGSMANSWER_ERROR_CODE errCode = (IGSMAnswer.IGSMANSWER_ERROR_CODE)nRes;
                        IGSMAnswerError answerFailure = new IGSMAnswerError(request, errCode);
                        request.SetId((int)errCode);
                        request.SetResult(answerFailure.GetXml());
                    }
                    break;
            }            
            return request;
        }

        private void process(IGSMRequest request)
        {
            IGRequestProcessing.Add(request, this);
            IGAnswer answer = request.CreateAnswer();
            int nRes = IGSMAnswer.IGSMANSWER_ERROR_NONE;
            if (answer != null)
            {
                answer.Init(request.GetGuid(), request.GetTitle());
                if (answer.Execute())
                    request.SetResult(answer.GetXml());
                else
                    nRes = (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_ANSWEREXECUTION;
            }
            if (IGSMAnswer.IsSMError(nRes))
            {
                IGSMAnswerError answerFailure = new IGSMAnswerError(request, (IGSMAnswer.IGSMANSWER_ERROR_CODE)nRes);
                request.SetId(nRes);
                request.SetResult(answerFailure.GetXml());
            }
        }

        private int process(IGRequest request)
        {
            // Server application request
            string sUser = request.GetParameterValue(IGRequest.IGREQUEST_USERLOGIN);
            string sRequestId = request.GetAttributeValue(IGRequest.IGREQUEST_NODEID);
            string sGuid = request.GetAttributeValue(IGRequest.IGREQUEST_GUID);
            if ((sUser == null) ||
                (sRequestId == null) ||
                (sGuid == null))
                return (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_CORRUPTEDREQUEST;
            int nReqId = Convert.ToInt32(sRequestId);
            int nRes = IGSMAnswer.IGSMANSWER_ERROR_NONE;
            IGConnectionLocal userConnection = null;
            switch (nReqId)
            {
                case IGRequest.IGREQUEST_WORKSPACE_CONNECT:
                    nRes = ConnectUser(sUser, sGuid, out userConnection);
                    if (nRes == (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERALREADYCONNECTED)
                    {
                        AppendInfo("Reconnecting user: " + sUser);
                        nRes = IGSMAnswer.IGSMANSWER_ERROR_NONE;
                    }
                    break;
                case IGRequest.IGREQUEST_WORKSPACE_DISCONNECT:
                    nRes = DisconnectUser(sUser, sGuid, out userConnection);
                    break;
                default:
                    IGConnection conn;
                    nRes = GetConnection(sUser, out conn);
                    userConnection = (IGConnectionLocal)conn;
                    break;
            }
            request.Connection = userConnection;
            if (nRes != IGSMAnswer.IGSMANSWER_ERROR_NONE)
                return nRes;
            if (request.GetTypeId() == IGRequest.IGREQUEST_FRAME)
            {
                string sFrameId = request.GetParameterValue(IGRequest.IGREQUEST_FRAMEID);
                string sRetry = request.GetParameterValue(IGRequest.IGREQUEST_RETRY);
                if (sFrameId != null && sRetry == null)
                {
                    if (IGRequestProcessing.IsFrameBusy(sUser, sFrameId))
                        return (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_FRAMEBUSY;
                }
            }
            userConnection.GetUserStatus().m_nLastReqId = nReqId;
            return userConnection.SendRequest(request) ? IGSMAnswer.IGSMANSWER_ERROR_NONE : (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_SENDREQUEST;            
        }
        
        public override int GetConnection(string sUser, out IGConnection retConnection, bool bConnectingAllowed)
        {
            retConnection = null;
            foreach (IGConnectionLocal connection in m_lConnections)
            {
                if (connection.m_sUser == sUser)
                {
                    if (!bConnectingAllowed)
                    {
                        if (connection.IsUserConnecting())
                            return (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERCONNECTING;
                        if (connection.IsUserDisconnecting())
                        {
                            retConnection = connection;
                            return (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERDISCONNECTING;
                        }
                    }
                    retConnection = connection;
                    return IGSMAnswer.IGSMANSWER_ERROR_NONE;
                }
            }
            return (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERNOTFOUND;
        }

        public override bool Reset()
        {
            foreach (IGConnection connection in m_lConnections)
                connection.Reset(IGSERVERMANAGER_AUTHORITY);
            if (m_lConnections.Count == 0)
                ConnectToAllServers();
            m_nHearthbeatTimeLastReceived = DateTime.UtcNow.Ticks;
            return true;
        }
        
        public override bool IsLocalServer()
        {
            return true;
        }

        public bool AddServer()
        {
            lock (m_lockObject)
            {
                int nNewPort = m_nServerManagerPort + m_lServers.Count + 1;
                IGServerLocal igServer = new IGServerLocal(m_endPoint.Address.ToString(), nNewPort, m_ipWebServer.ToString());
                try
                {
                    igServer.Initialize();
                }
                catch (Exception exc)
                {
                    AppendError("- IGServerManagerLocal failed adding server " + m_endPoint.Address.ToString() + " exception: " + exc.ToString());
                    return false;
                }
                return ConnectServer(igServer);
            }
        }

        public override SynchronizedCollection<IGConnection> GetConnections()
        {
            SynchronizedCollection<IGConnection> lUserConnections = new SynchronizedCollection<IGConnection>();
            foreach (IGConnectionLocal connection in m_lConnections)
            {
                if ((connection.IsUserConnected()) || connection.IsUserConnecting())
                    lUserConnections.Add(connection);
            }
            return lUserConnections;
        }

        public int GetNbUserConnections()
        {
            int nNbUserConnections = 0;
            foreach (IGConnectionLocal connection in m_lConnections)
            {
                if (connection.IsUserConnected())
                    nNbUserConnections++;
            }
            return nNbUserConnections;
        }

        public override int GetNbAvailableConnections()
        {
            return m_lConnections.Count - GetNbUserConnections();
        }

        public int GetMaxNbConnections()
        {
            return MAXNBCONNECTIONS;
        }
        
        public override string GetStatus()
        {
            lock (m_lockObject)
            {
                string status = base.GetStatus().Substring(0, base.GetStatus().Length-2);
                status += string.Format(",\"NbMaxConnections\":\"{0}\"", GetMaxNbConnections().ToString());
                status += string.Format(",\"NbUserConnections\":\"{0}\"", GetNbUserConnections().ToString());
                status += string.Format(",\"NbAvailConnections\":\"{0}\"}}}}", GetNbAvailableConnections().ToString());
                return status;
            }
        }

        public override bool Hearthbeat(IPEndPoint serverEndPoint)
        {
            m_nHearthbeatTimeLastReceived = DateTime.UtcNow.Ticks;
            return true;
        }        

        public void ChangePortList(int nNbPorts)
        {
            lock (m_lockObject)
            {
                int nNewPorts = nNbPorts - m_lServers.Count;
                int nLastPort = m_lServers[m_lServers.Count - 1].m_endPoint.Port;
                while (nNewPorts-- >= 0)
                    m_lServers.Add(new IGServerLocal(ServerIP.ToString(), ++nLastPort, m_ipWebServer.ToString()));
            }
        }

        public int ConnectUser(string sUser, string sGuid)
        {
            IGConnectionLocal localConnection;
            return ConnectUser(sUser, sGuid, out localConnection);
        }

        public int ConnectUser(string sUser, string sGuid, out IGConnectionLocal userConnection)
        {
            userConnection = null;
            if (!checkUserName(ref sUser))
                return (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_WRONGUSERNAME;
            IGConnection connection;
            int nResult = GetConnection(sUser, out connection);
            if (nResult != (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERNOTFOUND)
            {
                userConnection = (IGConnectionLocal)connection;
                if (nResult == (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERCONNECTING)
                    return (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERCONNECTING;
                else
                    return (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERALREADYCONNECTED;
            }
            foreach (IGConnectionLocal localConnection in m_lConnections)
            {
                if (!localConnection.IsUserConnected())
                {
                    userConnection = localConnection;
                    return localConnection.ConnectUser(sUser, sGuid);
                }
            }
            return (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_TOOMANYUSERS;
        }

        public int DisconnectUser(string sUser, string sGuid)
        {
            IGConnectionLocal localConnection;
            return DisconnectUser(sUser, sGuid, out localConnection);
        }

        public int DisconnectUser(string sUser, string sGuid, out IGConnectionLocal userConnection)
        {
            userConnection = null;
            IGConnection connection = null;
            int nResult = GetConnection(sUser, out connection);
            if (nResult != IGSMAnswer.IGSMANSWER_ERROR_NONE)
                return nResult;
            userConnection = (IGConnectionLocal)connection;
            IGSMStatusUser statusUser = userConnection.GetUserStatus();
            if (statusUser.m_eStatus != IGSMStatusUser.IGState.IGSMSTATUS_READY)
            {
                nResult = (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_SERVERBUSY;
                userConnection.Reset(sGuid);
            }
            else
                userConnection.DisconnectUser(sGuid);
            return nResult;
        }

        public bool Hearthbeat(string sUser)
        {
            foreach (IGConnectionLocal connection in m_lConnections)
            {
                if (connection.m_sUser == sUser)
                {
                    connection.GetUserStatus().m_nHearthbeatTime = DateTime.UtcNow.Ticks;
                    return true;
                }
            }
            return false;
        }

        public bool IsUserConnected()
        {
            foreach (IGConnectionLocal connection in m_lConnections)
            {
                if (connection.IsUserConnected())
                    return true;
            }
            return false;
        }

        public bool IsUserConnected(string sUser)
        {
            IGConnection connection = null;
            return (GetConnection(sUser, out connection) == IGSMAnswer.IGSMANSWER_ERROR_NONE);
        }

        public bool RemoveLastConnection(string sGuid)
        {
            int nNbConnections = m_lConnections.Count;
            if (nNbConnections == 0)
                return false;
            IGConnectionLocal removeConnection = (IGConnectionLocal)m_lConnections[nNbConnections - 1];
            if (removeConnection.IsUserConnected())
                return false;
            removeConnection.Terminate(sGuid);
            m_lConnections.Remove(removeConnection);
            return true;
        }

        public override void Terminate(string sGuid)
        {
            base.Terminate(sGuid);            
            if (m_lServers != null)
            {
                while (m_lServers.Count > m_nNbInitialServers)
                    m_lServers.RemoveAt(m_lServers.Count - 1);
            }
        }
        
        private bool checkUserName(ref string sUser)
        {
            sUser.ToUpperInvariant();
            if ((sUser.Length <= 0 || sUser.Length > MAXUSERLENGTH) || !sUser.IsNormalized())
                return false;
            foreach (char c in Path.GetInvalidFileNameChars())
                if (sUser.Contains(c))
                    return false;
            return true;
        }
            
        unsafe private bool mountDrives()
        {
            string sCmd = "use " + IGServerManager.AppSettings["DRIVE_OUTPUT"] + @" \\" + m_ipWebServer.ToString() + HC.PATH_OUTPUTPHYSICAL + ServerIP + @" /USER:" + IGServerManager.AppSettings["USER_DOMAIN"] + @"\" + IGServerManager.AppSettings["USER_NAME"] + " " + IGServerManager.AppSettings["USER_PASSWORD"];
            Process procOutputFolder = Process.Start("net.exe", sCmd);
            if (!procOutputFolder.WaitForExit(MAPNETWORKDRIVE_MAXWAITTIME))
            {
                AppendError(MAPNETWORKDRIVE_ERROR + "Command: " + sCmd);
                return false;
            }
            sCmd = "use " + IGServerManager.AppSettings["DRIVE_INPUT"] + @" \\" + m_ipWebServer.ToString() + HC.PATH_INPUTPHYSICAL + @" /USER:" + IGServerManager.AppSettings["USER_DOMAIN"] + @"\" + IGServerManager.AppSettings["USER_NAME"] + " " + IGServerManager.AppSettings["USER_PASSWORD"];
            Process procInputFolder = Process.Start("net.exe", sCmd);
            if (!procInputFolder.WaitForExit(MAPNETWORKDRIVE_MAXWAITTIME))
            {
                AppendError(MAPNETWORKDRIVE_ERROR + "Command: " + sCmd);
                return false;
            }
            sCmd = "use " + IGServerManager.AppSettings["DRIVE_SHARED"] + @" \\" + m_ipShareAddress.ToString() + HC.PATH_SHAREDPHYSICAL + @" /USER:" + IGServerManager.AppSettings["USER_DOMAIN"] + @"\" + IGServerManager.AppSettings["USER_NAME"] + " " + IGServerManager.AppSettings["USER_PASSWORD"];
            Process procSharedFolder = Process.Start("net.exe", sCmd);
            if (!procSharedFolder.WaitForExit(MAPNETWORKDRIVE_MAXWAITTIME))
            {
                AppendError(MAPNETWORKDRIVE_ERROR + "Command: " + sCmd);
                return false;
            }
            return true;
        }
    }
}
