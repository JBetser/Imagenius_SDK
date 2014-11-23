using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Xml;
using System.Threading;
using System.Timers;
using System.IO;
using System.Diagnostics;
using System.Configuration;

namespace IGSMLib
{
    public abstract class IGServerManager : IGServer
    {
        public const string IGSERVERMANAGER_AUTHORITY = "##ServerManagerAuthority##";
        public const string LOGNAME = "Application";
        public const int ANSWER_BUFSIZE = 65536;
        public const int PACKETDATA_SIZE = 65500;
        public const int REQUEST_BUFSIZE = 65536;
        public const int CYCLETIME = 100;
        
        public Dictionary<string, string> appSettings;        

        protected const int MAPNETWORKDRIVE_MAXWAITTIME = 5000;
        protected const string MAPNETWORKDRIVE_ERROR = "starting: cannot mount file server drive.";       
                
        //singleton
        protected static IGServerManager s_serverManager = null;

        //connections
        protected SynchronizedCollection<IGConnection> m_lConnections = new SynchronizedCollection<IGConnection>();
        protected object m_lockObject = new object();
        protected object m_lockFile = new object();
        protected bool m_bTimerProcessing = false;
        protected object m_lockTimer = new object();
        protected SynchronizedCollection<IGServer> m_lServers = new SynchronizedCollection<IGServer>();
        protected System.Timers.Timer m_timer;   
        protected IPAddress m_ipShareAddress;
        protected string m_sLogPath = "";
        protected string m_sCurDate = "";
        protected string m_sSubNetwork = null;
        protected SortedDictionary<string, KeyValuePair<IGAnswer, DateTime>> m_processedRequests = new SortedDictionary<string, KeyValuePair<IGAnswer, DateTime>>();
        protected EventLog m_logMgr = null;
        protected bool m_bIsInitialized = false;

        #region Singleton instanciation
        /** Instantiate a server manager, without input port **/
        public static IGServerManager CreateInstanceRemote(Dictionary<string, string> appSettings)
        {
            return new IGServerManagerRemote(appSettings); 
        }

        /** Instantiate a server manager, listening to an input port: "localhost:nServerManagerPort" **/
        public static IGServerManager CreateInstanceLocal(Dictionary<string, string> appSettings, int nServerManagerPort)
        {
            return new IGServerManagerLocal(appSettings, nServerManagerPort);
        }
        
        public static Dictionary<string, string> AppSettings
        {
            get
            {
                return Instance.appSettings;
            }
        }

        public static IGServerManager Instance
        {
            get
            {
                return s_serverManager;
            }
        }
        #endregion        

        protected IGServerManager(Dictionary<string, string> appSettings, IPEndPoint endPoint) : base(endPoint)
        {
            this.appSettings = appSettings;
            s_serverManager = this;            
            m_timer = new System.Timers.Timer();
            m_timer.Elapsed += new ElapsedEventHandler(onTimedEvent);
            m_timer.Interval = CYCLETIME;
            m_logMgr = new EventLog("IGSMService", Environment.MachineName, "Session Manager");
        }

        #region Abstract declarations
        public abstract void UpdateLogPath();
        public abstract bool IsLocalServer();
        public abstract int GetConnection(string sUser, out IGConnection retConnection, bool bConnectingAllowed);
        #endregion

        #region Event declarations
        public delegate void ErrorHandler(object sender, string textError);
        public event ErrorHandler ErrorEvent = null;
        protected void raiseErrorEvent(string textError)
        {
            if (ErrorEvent != null)
                ErrorEvent(this, textError);
        }
        protected virtual void onTimedEvent(object source, ElapsedEventArgs e)
        {
        }
        #endregion

        public virtual bool Initialize(string sIPShare, List<IGServer> lServers)
        {
            if (m_bIsInitialized)
                return true;
            lock (m_lockObject)
            {
                m_sSubNetwork = sIPShare.Substring(0, sIPShare.LastIndexOf('.'));
                IPHostEntry ipEntry = Dns.GetHostEntry(Dns.GetHostName());
                bool bFound = false;
                int idxAddress = 0;
                foreach (IPAddress ipAddress in ipEntry.AddressList)
                {
                    if (ipAddress.ToString().StartsWith(m_sSubNetwork + "."))
                    {
                        if (bFound)
                            throw new Exception("Could not determine current IP Address");
                        bFound = true;
                        m_endPoint.Address = ipEntry.AddressList[idxAddress];
                    }
                    idxAddress++;
                }
                if (!bFound)
                {
                    AppendError("No local IP is within the same subnetwork as the file share server. Taking the local IP address as the current one.");
                    m_endPoint.Address = IPAddress.Parse(appSettings["IP_LOCAL"]);
                }
                if (sIPShare != null)
                    m_ipShareAddress = IPAddress.Parse(sIPShare);
                m_bIsInitialized = true;
                return true;
            }
        }

        public override IGServer Clone()
        {
            throw new NotImplementedException();
        }

        public int GetConnection(string sServer, out IGConnection retConnection)
        {
            return GetConnection(sServer, out retConnection, false);
        }  

        public int AppendError(IGSMAnswer.IGSMANSWER_ERROR_CODE nErrorCode)
        {
            string sError;
            return AppendError(nErrorCode, out sError);
        }

        public int AppendError(IGSMAnswer.IGSMANSWER_ERROR_CODE nErrorCode, out string sError)
        {            
            IGSMAnswerError answer = new IGSMAnswerError(null, nErrorCode);
            sError = answer.ToString();
            AppendError(sError);
            return (int)nErrorCode;
        }

        public virtual void AppendError(string sError)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
#if DEBUG
            sError = string.Format(@"DEBUG {0:T} - ", DateTime.Now) + sError;
            m_logMgr.WriteEntry(sError, EventLogEntryType.Warning);
            addToFile(sError);
#else
            sError = string.Format(@"{0:T} - ", DateTime.Now) + sError;
            m_logMgr.WriteEntry(sError, EventLogEntryType.Error);
#endif                        
            raiseErrorEvent(sError);
        }

        public virtual void AppendInfo(string sInfo)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
#if DEBUG
            sInfo = "DEBUG - " + sInfo;
            addToFile(sInfo);
#endif
            sInfo = string.Format(@"{0:T} - Info - ", DateTime.Now) + sInfo;
            m_logMgr.WriteEntry(sInfo, EventLogEntryType.Information);
        }

        public IPAddress ServerIP
        {
            get
            {
                return m_endPoint.Address;
            }
        }

        public IPAddress SharedDriveIP
        {
            get
            {
                return m_ipShareAddress;
            }
        }

        public void RemoveFromProcessing(IGAnswer answer)
        {
            int nAnswerId = answer.GetId();
            if ((nAnswerId != (int)IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_HEARTHBEAT) &&
                (nAnswerId != (int)IGAnswer.IGANSWER_ID.IGANSWER_FRAME_ACTIONINPROGRESS) &&
                (nAnswerId != IGSMAnswer.IGSMANSWER_GETSTATUS))
            {
                if ((nAnswerId != IGSMAnswer.IGSMANSWER_ERROR) &&
                    (nAnswerId != (int)IGAnswer.IGANSWER_ID.IGANSWER_FRAME_ACTIONFAILED) &&
                    (nAnswerId != (int)IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_ACTIONFAILED))
                    IGRequestProcessing.Remove(answer);
                else
                {
                    string sGuid = answer.GetAttributeValue(IGAnswer.IGANSWER_REQGUID);
                    if ((sGuid != null) && (sGuid != ""))
                        IGRequestProcessing.Remove(answer);
                }
            }
        }

        public virtual bool ProcessAnswer(IGAnswer answer)
        {
            RemoveFromProcessing(answer);
            return true;
        }

        public override bool Reset()
        {
            Terminate(IGSERVERMANAGER_AUTHORITY);
            return ConnectToAllServers();
        }

        public bool ConnectToAllServers()
        {
            lock (m_lockObject)
            {
                List<IGServer> lErrorServers = new List<IGServer>();
                foreach (IGServer igServer in m_lServers)
                {
                    if (!igServer.Initialize())
                        lErrorServers.Add(igServer);
                }
                foreach (IGServer igServer in lErrorServers)
                    m_lServers.Remove(igServer);
                bool bResult = true;
                List<IGServer> lCurrentServers = new List<IGServer>();
                lCurrentServers.AddRange(m_lServers);
                while (lCurrentServers.Count > 0)
                {
                    IGServer igServer = lCurrentServers[0];
                    if (!AddConnection(igServer))
                        bResult = false;
                    lCurrentServers.RemoveAt(0);
                }
                return bResult;
            }
        }

        public bool DisconnectAllServers(string sGuid)
        {
            lock (m_lockObject)
            {
                Terminate(sGuid);
                return true;
            }
        }

        public virtual bool ConnectServer(IGServer newServer)
        {
            lock (m_lockObject)
            {
                m_lServers.Add(newServer);
                try
                {
                    AddConnection(newServer);
                }
                catch (Exception exc)
                {
                    IGServerManager.Instance.AppendError(exc.ToString());
                    return false;
                }
                return true;
            }
        }

        public virtual bool DisconnectServer(IPEndPoint endPoint, bool bForce)
        {
            lock (m_lockObject)
            {
                IGServer serverFound = null;
                foreach (IGServer server in m_lServers)
                {
                    if (server.m_endPoint == endPoint || endPoint == null)
                    {
                        serverFound = server;
                        endPoint = server.m_endPoint;
                        break;
                    }
                }
                try
                {
                    RemoveConnection(endPoint);                    
                }
                catch (Exception exc)
                {
                    AppendError(exc.ToString());
                    m_lServers.Remove(serverFound);
                    return false;
                }
                m_lServers.Remove(serverFound);
                return true;
            }
        }

        public virtual IGServer GetServer(IPEndPoint endPoint)
        {
            lock (m_lockObject)
            {
                foreach (IGServer server in m_lServers)
                {
                    if (server.m_endPoint.Address.ToString() == endPoint.Address.ToString()
                        && server.m_endPoint.Port == endPoint.Port)
                        return server;
                }
            }
            return null;
        }

        public override string GetStatus()
        {
            lock (m_lockObject)
            {
                string serverStatus = m_lServers.Aggregate(string.Format("{{\"NbServers\":\"{0}\",\"List\":[ ", m_lServers.Count.ToString()),
                    (curStatus, server) => (curStatus + server.GetStatus() + ","), curStatus => (curStatus.Substring(0, curStatus.Length - 1) + "]}"));
                string connectionStatus = m_lConnections.Aggregate(string.Format("{{\"NbConnections\":\"{0}\",\"List\":[ ", m_lConnections.Count),
                    (curStatus, connection) => (curStatus + string.Format("{{\"Type\":\"{0}\",\"Server\":\"{1}p{2}\",\"User\":\"{3}\"}},",
                                                            connection.GetType().ToString() == "IGSMLib.IGConnectionLocal" ? "Local" : "Remote", 
                                                            connection.m_igServer.m_endPoint.Address.ToString(),
                                                            connection.m_igServer.m_endPoint.Port.ToString(), (connection.m_sUser == null ? "None" : 
                                                                                        (connection.m_sUser == connection.m_igServer.m_sIpEndPoint ? "None" : 
                                                                                        connection.m_sUser.Replace(':', 'p'))))), 
                                 curStatus => (curStatus.Substring(0, curStatus.Length - 1) + "]}"));
                string status = GetState().ToString();
                if (m_lServers.Count == 0)
                    status = IGSMStatus.IGState.IGSMSTATUS_NOTRESPONDING.ToString();
                return string.Format("\"ServerManager\":{{\"Type\":\"{0}\",\"Address\":\"{1}p{2}\",\"Status\":\"{3}\",\"Servers\":{4},\"Connections\":{5}}}",
                    IsLocalServer() ? "Local" : "Remote", m_endPoint.Address.ToString(), m_endPoint.Port.ToString(), status, serverStatus, connectionStatus);
            }
        }

        public virtual SynchronizedCollection<IGConnection> GetConnections()
        {
            return m_lConnections;
        }

        public bool Hearthbeat()
        {
            return Hearthbeat(null);
        }

        public virtual bool Hearthbeat(IPEndPoint serverEndPoint)
        {
            return false;
        }

        public bool AddConnection(IGServer igServer)
        {
            if (igServer.GetConnection() == null)
            {
                IGConnection newConnection = null;
                try
                {
                    newConnection = igServer.CreateConnection();
                    newConnection.Initialize();
                }
                catch (Exception exc)
                {
                    igServer.Terminate();
                    AppendError("- IGServerManager failed connecting to server " + igServer.m_sIpEndPoint + " exception: " + exc.ToString());
                    return false;
                }
                m_lConnections.Add(newConnection);
            }
            return true;
        }        

        public bool RemoveConnection(IPEndPoint endPoint)
        {
            IGConnection removeConnection = null;
            foreach (IGConnection connection in m_lConnections)
            {
                if (connection.m_igServer.m_endPoint == endPoint)
                {
                    removeConnection = connection;
                    connection.Terminate(IGSERVERMANAGER_AUTHORITY);                    
                    break;
                }
            }
            if (removeConnection == null)
                return false;
            m_lConnections.Remove(removeConnection);
            return true;
        }

        public virtual bool ResetConnection(IPEndPoint endPoint, bool bForce)
        {
            foreach (IGConnection connection in m_lConnections)
            {
                if (connection.m_igServer.m_endPoint == endPoint)
                {
                    connection.Reset(IGSERVERMANAGER_AUTHORITY);
                    return true;
                }
            }
            return false;
        }

        public bool ResetAllConnections()
        {
            foreach (IGConnection connection in m_lConnections)
                connection.Reset(IGSERVERMANAGER_AUTHORITY);
            return true;
        }

        public bool IsFree()
        {
            bool bIsFree = true;
            foreach (IGConnection connection in m_lConnections)
            {
                if (!connection.IsFree())
                    bIsFree = false;
            }
            return bIsFree;
        }

        public virtual void Terminate(string sGuid)
        {
            foreach (IGConnection connection in m_lConnections)
                connection.Terminate(sGuid);
            m_lConnections.Clear();
        }              
        
        protected virtual IGAnswer createAnswer(int nAnswerId, XmlDocument xmlDoc, XmlNode xmlNodeAnswer, IGConnection connection)
        {
            IGConnectionLocal userConnection = null;
            if (connection != null)
            {
                if (connection.IsLocal())
                    userConnection = (IGConnectionLocal)connection;
            }
            switch ((IGAnswer.IGANSWER_ID)nAnswerId)
            {
                case IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_CONNECTED:
                    return new IGAnswerConnected(userConnection, xmlDoc);
                case IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_DISCONNECTED:
                    return new IGAnswerDisconnected(userConnection, xmlDoc);
                case IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_HEARTHBEAT:
                    return new IGAnswerHearthbeat(userConnection, xmlDoc);
                case IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_ACTIONFAILED:
                case IGAnswer.IGANSWER_ID.IGANSWER_FRAME_ACTIONFAILED:
                    return new IGAnswerFailed(userConnection, xmlDoc);
                case IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_PONG:
                    return new IGAnswerPong(userConnection, xmlDoc);
                case IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_PROPERTYCHANGED:
                    return new IGAnswerPropertyChanged(userConnection, xmlDoc);
                case IGAnswer.IGANSWER_ID.IGANSWER_FRAME_ACTIONINPROGRESS:
                    return new IGAnswerProgress(userConnection, xmlDoc);
                case IGAnswer.IGANSWER_ID.IGANSWER_FRAME_ACTIONDONE:
                    return new IGAnswerFrameDone(userConnection, xmlDoc);
                case IGAnswer.IGANSWER_ID.IGANSWER_FRAME_CHANGED:
                    return new IGAnswerFrameChanged(userConnection, xmlDoc);
            }
            return new IGAnswerUnknown(xmlDoc);
        }

        public IGAnswer CreateAnswer(string sXml, IGConnection connection)
        {
            XmlDocument xmlDoc;
            XmlNode xmlNodeAnswer;
            int nAnswerId = IGAnswer.GetAnswerId(sXml, out xmlDoc, out xmlNodeAnswer);
            return createAnswer(nAnswerId, xmlDoc, xmlNodeAnswer, connection);
        }

        public IGAnswer CreateAnswer(IGRequest request)
        {
            IGAnswer newAnswer = CreateAnswer(request.GetResult(), null);
            newAnswer.SetRequest(request);
            return newAnswer;
        }

        private IGRequest createRequest(int nRequestId, XmlDocument xmlDoc, XmlNode xmlNodeRequest)
        {
            if (IGSMRequest.IsSMRequest(nRequestId))
            {
                switch ((IGSMRequest.IGSMREQUEST_ID)nRequestId)
                {
                    case IGSMRequest.IGSMREQUEST_ID.IGSMREQUEST_PING:
                        return new IGSMRequestPing(xmlDoc);
                    case IGSMRequest.IGSMREQUEST_ID.IGSMREQUEST_GETSTATUS:
                        return new IGSMRequestGetStatus(xmlDoc);
                    case IGSMRequest.IGSMREQUEST_ID.IGSMREQUEST_TERMINATE:
                        return new IGSMRequestTerminate(xmlDoc);                   
                    case IGSMRequest.IGSMREQUEST_ID.IGSMREQUEST_DESTROYACCOUNT:
                        return new IGSMRequestDestroyAccount(xmlDoc);
                    case IGSMRequest.IGSMREQUEST_ID.IGSMREQUEST_ADDIGAPPS:
                        return new IGSMRequestAddIGApps(xmlDoc);
                    case IGSMRequest.IGSMREQUEST_ID.IGSMREQUEST_UPLOADIMAGE:
                        return new IGSMRequestUpload(xmlDoc);
                    case IGSMRequest.IGSMREQUEST_ID.IGSMREQUEST_DOWNLOADIMAGE:
                        return new IGSMRequestDownload(xmlDoc);
                    case IGSMRequest.IGSMREQUEST_ID.IGSMREQUEST_DELETEIMAGE:
                        return new IGSMRequestDeleteImage(xmlDoc);
                }
            }
            else
            {
                switch (nRequestId)
                {
                    case IGRequest.IGREQUEST_WORKSPACE_PING:
                        return new IGRequestWorkspacePing(xmlDoc);
                    case IGRequest.IGREQUEST_WORKSPACE_CONNECT:
                        return new IGRequestWorkspaceConnect(xmlDoc);
                    case IGRequest.IGREQUEST_WORKSPACE_DISCONNECT:
                        return new IGRequestWorkspaceDisconnect(xmlDoc);
                    case IGRequest.IGREQUEST_WORKSPACE_NEWIMAGE:
                        return new IGRequestWorkspaceNew(xmlDoc);
                    case IGRequest.IGREQUEST_WORKSPACE_LOADIMAGE:
                        return new IGRequestWorkspaceLoad(xmlDoc);
                    case IGRequest.IGREQUEST_WORKSPACE_SHOW:
                        return new IGRequestWorkspaceShow(xmlDoc);
                    
                    case IGRequest.IGREQUEST_FRAME_SAVE:
                        return new IGRequestFrameSave(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_ADDLAYER:
                        return new IGRequestFrameAddLayer(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_REMOVELAYER:
                        return new IGRequestFrameRemoveLayer(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_MOVELAYER:
                        return new IGRequestFrameMoveLayer(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_MERGELAYER:
                        return new IGRequestFrameMergeLayer(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_FILTER:
                        return new IGRequestFrameFilterLayer(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_INDEX:
                        return new IGRequestFrameIndexLayer(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_CHANGEPROPERTY:
                        return new IGRequestChangeProperty(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_CLOSE:
                        return new IGRequestFrameClose(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_SELECT:
                        return new IGRequestFrameSelect(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_UPDATE:
                        return new IGRequestFrameUpdate(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_SETLAYERVISIBLE:
                        return new IGRequestFrameSetLayerVisible(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_GOTOHISTORY:
                        return new IGRequestFrameGotoHistory(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_DRAWLINES:
                        return new IGRequestFrameDrawLines(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_COPY:
                        return new IGRequestFrameCopy(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_CUT:
                        return new IGRequestFrameCut(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_PASTE:
                        return new IGRequestFramePaste(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_DELETE:
                        return new IGRequestFrameDelete(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_SMARTDELETE:
                        return new IGRequestFrameSmartDelete(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_PICKCOLOR:
                        return new IGRequestFramePickColor(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_MOVE:
                        return new IGRequestFrameMove(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_ROTATEANDRESIZE:
                        return new IGRequestFrameRotateAndResize(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_SELECTLAYER:
                        return new IGRequestFrameSelectLayer(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_RUBBER:
                        return new IGRequestFrameRubber(xmlDoc);
                    case IGRequest.IGREQUEST_FRAME_SAVE_BM:
                        return new IGRequestFrameSaveBeetleMorph(xmlDoc);
                }
            }
            return new IGRequestUnknown(xmlDoc);
        }

        public IGRequest CreateRequest(string sXml, AutoResetEvent stopWaitHandle)
        {
            XmlDocument xmlDoc = null;
            XmlNode xmlNodeRequest;
            int nRequestId = IGRequest.GetRequestId(sXml, ref xmlDoc, out xmlNodeRequest);
            IGRequest req = createRequest(nRequestId, xmlDoc, xmlNodeRequest);
            if (req != null)
                req.SetEventDone(stopWaitHandle);
            return req;
        }

        public IGRequest CreateRequest(XmlDocument xmlDoc)
        {
            XmlNode xmlNodeRequest;
            int nRequestId = IGRequest.GetRequestId(xmlDoc, out xmlNodeRequest);
            return createRequest(nRequestId, xmlDoc, xmlNodeRequest);
        }

        public void AddProcessedRequest(string guid, IGAnswer answer)
        {
            if (!m_processedRequests.ContainsKey(guid))
                m_processedRequests.Add(guid, new KeyValuePair<IGAnswer, DateTime>(answer, DateTime.Now));
            List<string> reqToRemove = new List<string>();
            foreach (var kvp in m_processedRequests)
            {
                if (kvp.Value.Value.Ticks < DateTime.Now.Ticks - (double)HC.HEARTHBEAT_REMOTE_SERVERTIMEOUT_NOTRESPONDING * 10000)
                    reqToRemove.Add(kvp.Key);
            }
            foreach (string key in reqToRemove)
                m_processedRequests.Remove(key);
        }

        public void KillAllProcesses(bool desktop)
        {
            foreach (Process clsProcess in Process.GetProcesses())
            {
                if (desktop && clsProcess.ProcessName.StartsWith("IGSMDesktop", StringComparison.CurrentCultureIgnoreCase))
                {
                    try
                    {
                        clsProcess.Kill();
                    }
                    catch (Exception ex)
                    {
                        AppendError(String.Format("Could not kill process {0}, exception {1}", clsProcess.ToString(), ex.ToString()));
                    }
                }
            }
            foreach (Process clsProcess in Process.GetProcesses())
            {
                if (clsProcess.ProcessName.StartsWith("IGServer", StringComparison.CurrentCultureIgnoreCase))
                {
                    try
                    {
                        clsProcess.Kill();
                    }
                    catch (Exception ex)
                    {
                        AppendError(String.Format("Could not kill process {0}, exception {1}", clsProcess.ToString(), ex.ToString()));
                    }
                }
            }
        }

        protected void addToFile(string contents)
        {
#if DEBUG
            lock (m_lockFile)
            {
                try
                {
                    UpdateLogPath();
                    //set up a filestream
                    FileStream fs = new FileStream(m_sLogPath, FileMode.OpenOrCreate, FileAccess.Write);
                    //set up a streamwriter for adding text
                    StreamWriter sw = new StreamWriter(fs);
                    //find the end of the underlying filestream
                    sw.BaseStream.Seek(0, SeekOrigin.End);
                    //add the text
                    sw.WriteLine(contents);
                    //add the text to the underlying filestream
                    sw.Flush();
                    //close the writer
                    sw.Close();
                }
                catch (Exception)
                {
                }
            }
#endif
        }
    }
}