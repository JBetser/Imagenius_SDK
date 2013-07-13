using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Web;
using System.Web.SessionState;
using System.Net;
using System.Xml;
using System.Threading;
using System.Timers;
using IGMadam;
using IGSMLib;
using System.Configuration;
using System.Web.Security;

namespace IGPE
{
    /// <summary>
    /// Summary description for IGPEMultiplexing
    /// </summary>
    public class IGPEMultiplexing : IGServerManagerRemote
    {
        public const string SESSIONMEMBER_SERVERSTATUS = "ServerStatus";
        public const string SESSIONMEMBER_OUTPUT = "ServerStatusOutput";
        public const string SESSIONMEMBER_DISCONNECTING = "ServerDisconnecting";
        public const string SESSIONMEMBER_DISCONNECTED = "ServerDisconnected";
        public const string SESSIONMEMBER_CONNECTIONERROR = "ServerconnectionError";
        public const string SESSIONMEMBER_RELOADPAGE = "ReloadPage";
        public const string SESSIONMEMBER_SERVERIP = "ServerIP";
        public const string SESSIONMEMBER_HEARTHBEAT = "Hearthbeat";
        public const string SESSIONMEMBER_CONNECTRESULT = "ConnectResult";

        public const string IGPEMULTIPLEXING_ERROR_TITLE = "Imagenius Server Error";
        public const string IGPEMULTIPLEXING_ERROR_UNAVAILABLELOGIN = "Your account cannot be used for maintainability reasons. We apologize for the unconvenience.<br/>Please try again later. If the problem persists, please contact us via 'Help' -> 'Contact US'";
        public const string IGPEMULTIPLEXING_ERROR_SAFETY = "<br/>Try to reconnect. If the error persists, please contact our support.";
        public const string IGPEMULTIPLEXING_ERROR_CORRUPTEDREQUEST = "Imagenius server received a corrupted request." + IGPEMULTIPLEXING_ERROR_SAFETY;
        public const string IGPEMULTIPLEXING_ERROR_SENDREQUEST = "Imagenius failed to proceed a request." + IGPEMULTIPLEXING_ERROR_SAFETY;
        public const string IGPEMULTIPLEXING_ERROR_NEEDACCOUNT = "You are running a demo version. To perform this task please create an account for free.";
        public const string IGPEMULTIPLEXING_ERROR_SINGLESIGNON = "You have been disconnected because another session has been created with your account.<br/>If you suspect someone is using your account, then you should reset your password.";
        public const string IGPEMULTIPLEXING_ERROR_NULLSESSION = "Cannot open null session.";
        public const string IGPEMULTIPLEXING_ERROR_UNKNOWN = "An unexpected error occured. We apologize for the unconvenience.<br/>Please try to reconnect. If the problem persists, please contact us via 'Help' -> 'Contact US'";
        public const string IGPEMULTIPLEXING_ERROR_SERVERSTOOBUSY = "All servers are busy. Retry later.";
        public const string IGPEMULTIPLEXING_ERROR_DISCONNECTING = "Session is being disconnected";
        public const string IGPEMULTIPLEXING_ERROR_DISCONNECTED = "Session has been disconnected";
        public const string IGPEMULTIPLEXING_ERROR_INVALIDLOGIN = "Invalid login"; 
        
        private const int IGPEMULTIPLEXING_HEARTHBEAT_DISCONNECTUSERTIMEOUT = 60000;
        private InterThreadHashtable m_mapUserSessions = new InterThreadHashtable();
        private InterThreadHashtable m_mapAdminSessions = new InterThreadHashtable();

        public IGPEMultiplexing(Dictionary<string, string> appSettings)
            : base(appSettings)
        {
            m_timer.Start();
        }

        public bool AuthenticateMember(string user, string pwd)
        {
            bool bIsConnected = true;
            try
            {
                bIsConnected = (UserSecurityAuthority.FromConfig().Authenticate(user, pwd, null, "Forms") != null);
            }
            catch (PasswordFormatNotSupportedException)
            {
                bIsConnected = false;
            }
            return bIsConnected;
        }

        public bool ConnectMember(string user, string pwd, HttpSessionState userSession)
        {
            bool bIsConnected = true;
            bool bIsDisconnecting = (userSession[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTING] != null);
            if (bIsDisconnecting)
            {
                connectionError(userSession, IGPEMULTIPLEXING_ERROR_DISCONNECTING);
                return false;
            }
            else
            {
                if (user == null)
                {
                    connectionError(userSession, IGPEMultiplexing.IGPEMULTIPLEXING_ERROR_UNAVAILABLELOGIN);
                    return false;
                }
                bIsConnected = AuthenticateMember(user.StartsWith("Demo@") ? "Demo" : user, pwd);
            }
            bool isAlreadyConnected = false;
            if (bIsConnected)
            {
                isAlreadyConnected = openMemberSession(false, user, userSession);
                if (isAlreadyConnected)
                {
                    if (userSession[SESSIONMEMBER_CONNECTIONERROR] == null)
                        AppendInfo("Reconnecting user: " + user);
                    else
                    {
                        connectionError(userSession, (string)userSession[SESSIONMEMBER_CONNECTIONERROR]);
                        return false;
                    }
                }
                if ((string)userSession[DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE] == DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE_ADMIN)
                    return connectAdmin(user, userSession);
                if (!connectUser(user, userSession))
                {
                    connectionError(userSession, IGPEMULTIPLEXING_ERROR_SERVERSTOOBUSY);
                    return false;
                }
            }
            else
            {
                connectionError(userSession, IGPEMULTIPLEXING_ERROR_INVALIDLOGIN);
                return false;
            }
            userSession[DatabaseUserSecurityAuthority.IGMADAM_PASSWORD] = pwd;
            return true;
        }

        private void connectionError(HttpSessionState userSession, string error)
        {            
            AppendError(userSession, error);
            clearSession(userSession);
        }

        private bool connectUser(string user, HttpSessionState userSession)
        {            
            // if not already connected, connect to most available server
            if (userSession[SESSIONMEMBER_SERVERIP] == null)
            {
                IPEndPoint serverEndPoint = m_configMgr.GetMostAvailableServerEndPoint();
                if (serverEndPoint == null)
                {
                    m_mapUserSessions.Remove(user);
                    return false;
                }
                userSession[SESSIONMEMBER_SERVERIP] = serverEndPoint;
                IGRequest req = new IGRequestWorkspaceConnect(user);
                IGPERequestSender.SendRequest((IPEndPoint)userSession[SESSIONMEMBER_SERVERIP], req);
                IGAnswer answerConnected = CreateAnswer(req);
                if (userSession[SESSIONMEMBER_SERVERIP] != null)
                    answerConnected.SetParameter(IGAnswer.IGANSWER_SERVERIP, ((IPEndPoint)userSession[SESSIONMEMBER_SERVERIP]).Address.ToString());
                userSession[SESSIONMEMBER_CONNECTRESULT] = answerConnected.ToClientOutput();
            }
            else
            {
                IGPERequestSender.SendRequest((IPEndPoint)userSession[SESSIONMEMBER_SERVERIP], new IGRequestWorkspaceConnect(user));
                IGAnswerConnected answerConnected = new IGAnswerConnected(userSession);
                if (userSession[SESSIONMEMBER_SERVERIP] != null)
                    answerConnected.SetParameter(IGAnswer.IGANSWER_SERVERIP, ((IPEndPoint)userSession[SESSIONMEMBER_SERVERIP]).Address.ToString());
                userSession[SESSIONMEMBER_CONNECTRESULT] = answerConnected.ToClientOutput();
            }
            return true;
        }

        public override bool DisconnectUser(string sUser, IPEndPoint serverEndPoint)
        {
            if (m_mapUserSessions.ContainsKey(sUser))
            {
                HttpSessionState userSession = ((HttpSessionState)m_mapUserSessions.Get(sUser));                
                if (serverEndPoint != null)
                {
                    userSession.Clear();
                    userSession[DatabaseUserSecurityAuthority.IGMADAM_USERNAME] = sUser;
                    userSession[SESSIONMEMBER_RELOADPAGE] = true;
                    userSession[SESSIONMEMBER_DISCONNECTING] = true;
                    IGPERequestSender.SendRequest(serverEndPoint, new IGRequestWorkspaceDisconnect(sUser));
                }
                else {
                    clearSession(userSession);
                }
                return true;
            }
            if (m_mapAdminSessions.ContainsKey(sUser))
            {
                HttpSessionState session = (HttpSessionState)m_mapAdminSessions.Get(sUser);
                session.Clear();                      
                m_mapAdminSessions.Remove(sUser);
                if (m_mapAdminSessions.Length() == 0)
                {
                    Hashtable hashServers = m_configMgr.GetHashtableServers();
                    IDictionaryEnumerator enumServers = hashServers.GetEnumerator();
                    while (enumServers.MoveNext())
                    {
                        IGServerRemote server = (IGServerRemote)enumServers.Value;
                        if ((server == null) || (server.GetState() != IGSMStatus.IGState.IGSMSTATUS_READY))
                            continue;
                        IGPERequestSender.SendRequest(server.m_endPoint, new IGSMRequestGetStatus(IGSMStatus.IGSMStatusType.IGSMSTATUS_FULLSTOP));
                    }
                }
                return true;
            }
            return false;
        }

        public override bool DisconnectUser(string sUser, bool bSendRequest)
        {
            if (bSendRequest)
            {
                IPEndPoint endPoint = getUserServerIP(sUser);
                if (endPoint != null)
                {
                    return DisconnectUser(sUser, endPoint);
                }
            }
            else
            {
                HttpSessionState session = getUserSession(sUser);
                if (session != null)
                {
                    m_mapUserSessions.Remove(sUser);
                    if ((session[SESSIONMEMBER_DISCONNECTED] == null) &&
                        (session[SESSIONMEMBER_DISCONNECTING] == null))
                        AppendError(session, IGPEMULTIPLEXING_ERROR_UNKNOWN);
                    clearSession(session);                    
                }
                return true;
            }
            return false;
        }

        public bool IsConnected(string sUser)
        {
            if (m_mapUserSessions.ContainsKey(sUser))
                return true;
            bool bKeyFound = m_mapAdminSessions.ContainsKey(sUser);
            return bKeyFound;
        }

        private bool connectAdmin(string userAdmin, HttpSessionState userSession)
        {
            bool bSendStart = (m_mapAdminSessions.Length() == 1);
            if (bSendStart)
            {
                Hashtable hashServers = m_configMgr.GetHashtableServers();
                IDictionaryEnumerator enumServers = hashServers.GetEnumerator();
                while (enumServers.MoveNext())
                {
                    IGServerRemote server = (IGServerRemote)enumServers.Value;
                    if (server == null)
                        continue;
                    IGPERequestSender.SendRequest(server.m_endPoint, new IGSMRequestGetStatus(IGSMStatus.IGSMStatusType.IGSMSTATUS_FULLSTART));
                }
            }
            return true;
        }

        public override bool ProcessAnswer(IGAnswer answer)
        {
            // dispatch server messages
            List<object> adminSessionArray = m_mapAdminSessions.GetArrayValues();
            XmlNode xmlNodeRequestLogin = answer.GetAttribute(IGAnswer.IGANSWER_USERLOGIN);
            if ((xmlNodeRequestLogin == null) || (xmlNodeRequestLogin.Value == ""))
            {
                IGPEAnswer.OnProcessAnswer(adminSessionArray, null, answer);
                return base.ProcessAnswer(answer);  // server answer, leave
            }
            // dispatch user messages
            HttpSessionState userSession = (HttpSessionState)m_mapUserSessions.Get(xmlNodeRequestLogin.Value);
            if (userSession == null)
                return false;
            if (!IGPEAnswer.OnProcessAnswer(adminSessionArray, userSession, answer))
                AppendError("Server error: Failed processing answer " + answer.ToString());
            // end of user disconnection
            if (answer.GetId() == (int)IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_DISCONNECTED)
            {
                if (!DisconnectUser(xmlNodeRequestLogin.Value, false))
                    AppendError(userSession, IGPEMULTIPLEXING_ERROR_UNKNOWN);
            }
            return base.ProcessAnswer(answer);
        }
        
        public override void AppendError(string sError)
        {
            List<object> adminSessionArray = m_mapAdminSessions.GetArrayValues();
            foreach (HttpSessionState adminSession in adminSessionArray)
            {
                IGPEOutput output = (IGPEOutput)adminSession[SESSIONMEMBER_OUTPUT];
                if (output != null)
                    output.StackError(sError);
            }
            base.AppendError(sError);
        }

        /*
        public override void UpdateServer(IGAnswer answerSMGetStatus)
        {
            List<object> adminSessionArray = m_mapAdminSessions.GetArrayValues();
            foreach (HttpSessionState adminSession in adminSessionArray)
            {
                IGSMStatusTreeView serverStatusTreeView = (IGSMStatusTreeView)adminSession[SESSIONMEMBER_SERVERSTATUS];
                if (serverStatusTreeView != null)
                    serverStatusTreeView.UpdateServer(answerSMGetStatus.GetParameters());
            }
        }*/
                        
        public void Hearthbeat(HttpSessionState userSession)
        {
            userSession[SESSIONMEMBER_HEARTHBEAT] = DateTime.UtcNow.Ticks;
        }

        public bool ProcessUserCommand(HttpSessionState userSession, IGRequest request)
        {
            Hearthbeat(userSession);
            string sUser = (string)userSession[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
            HttpSessionState sessionMatch = getUserSession(sUser);
            if (sessionMatch == null)
            {
                AppendError(userSession, "The current user \"" + sUser + "\" session is corrupted." + IGPEMULTIPLEXING_ERROR_SAFETY);
                return false;
            }
            return sendUserRequest(sessionMatch, request);
        }

        public void AppendError(HttpSessionState userSession, string sError)
        {
            string sUser, sUserError;
            if (userSession != null)
            {
                sUser = (string)userSession[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
                HttpSessionState sessionMatch = getUserSession(sUser);
                sUserError = "User " + (sUser != null ? sUser : "unknown ") +
                                    (sUser != null ? " connected " : " not connected ") + sError;
                AppendError(sUserError);
                if (userSession[IGPEMultiplexing.SESSIONMEMBER_CONNECTIONERROR] == null)
                    userSession[SESSIONMEMBER_CONNECTIONERROR] = sError;                             
                if (sessionMatch != null)
                    sendUserRequest(sessionMatch, new IGRequestWorkspaceDisconnect(sUser));                
                clearSession(userSession);
            }
            else
            {
                sUserError = "No session " + sError;
                AppendError(sUserError);
            }            
        }

        private IPEndPoint getUserServerIP(string sUser)
        {
            HttpSessionState userSession = getUserSession(sUser);
            if (userSession != null)
                return (IPEndPoint)userSession[SESSIONMEMBER_SERVERIP];
            return null;
        }

        private HttpSessionState getUserSession(string sUser)
        {
            List<object> userSessionArray = m_mapUserSessions.GetArrayValues();
            if (userSessionArray.Count > 0)
            {
                foreach (HttpSessionState userSession in userSessionArray)
                {
                    if ((string)userSession[DatabaseUserSecurityAuthority.IGMADAM_USERNAME] == sUser)
                    {
                        return userSession;
                    }
                }
            }
            return null;
        }

        private void clearSession(HttpSessionState userSession)
        {
            string userName = (string)userSession[DatabaseUserSecurityAuthority.IGMADAM_USERNAME];
            userSession.Abandon();
            userSession.Clear();
            userSession[SESSIONMEMBER_DISCONNECTED] = true;
            userSession[SESSIONMEMBER_RELOADPAGE] = true;
            if (userName != null)
                AppendInfo("User " + userName + " has been disconnected");
            FormsAuthentication.SignOut();            
        }
        
        private bool removeUserConnection(IPEndPoint endPoint, bool bForce)
        {
            List<object> userSessionArray = m_mapUserSessions.GetArrayValues();
            if (userSessionArray.Count > 0)
            {
                bool bUserConnected = false;
                foreach (HttpSessionState userSession in userSessionArray)
                {
                    if ((IPEndPoint)userSession[SESSIONMEMBER_SERVERIP] == endPoint)
                    {
                        bUserConnected = true;
                        if (bForce)
                        {
                            string sKey = (string)m_mapUserSessions.GetKey(userSession);
                            DisconnectUser(sKey, false);
                        }
                    }
                }
                if (bUserConnected)
                {
                    if (bForce)
                        AppendError("Server Disconnection: forcing user disconnections");
                    else
                        AppendError("Server Disconnection Failed: some users are connected");
                    return !bForce;
                }
            }
            return false;
        }

        protected override void onTimedEvent(object source, ElapsedEventArgs e)
        {
            List<object> userSessionArray = m_mapUserSessions.GetArrayValues();
            foreach (HttpSessionState userSession in userSessionArray)
            {
                if (userSession[DatabaseUserSecurityAuthority.IGMADAM_USERNAME] != null)
                {
                    bool bDisconnect = false;
                    if (userSession[SESSIONMEMBER_HEARTHBEAT] != null)
                    {
                        if ((DateTime.UtcNow.Ticks - (long)userSession[SESSIONMEMBER_HEARTHBEAT]) / 10000 > IGPEMULTIPLEXING_HEARTHBEAT_DISCONNECTUSERTIMEOUT)
                            bDisconnect = true;
                    }
                    else
                    {
                        bDisconnect = true;
                    }
                    if (bDisconnect)
                    {
                        string sKey = (string)m_mapUserSessions.GetKey(userSession);
                        if (sKey != null)
                        {
                            DisconnectUser(sKey, (IPEndPoint)userSession[SESSIONMEMBER_SERVERIP]);
                            DisconnectUser(sKey, false);
                        }
                    }
                }
            }
            base.onTimedEvent(source, e);
        }

        private bool openMemberSession(bool bAdmin, string sUser, HttpSessionState userSession)
        {
            if (userSession != null)
            {
                userSession.Remove(SESSIONMEMBER_DISCONNECTED);
                userSession.Remove(SESSIONMEMBER_DISCONNECTING); 
                userSession.Remove(SESSIONMEMBER_CONNECTIONERROR);
            }
            InterThreadHashtable tableSessions = bAdmin ? m_mapAdminSessions : m_mapUserSessions;
            bool bAlreadyConnected = tableSessions.ContainsKey(sUser);
            if (bAlreadyConnected)
            {
                HttpSessionState connectedSession = null;
                try
                {
                    connectedSession = ((HttpSessionState)tableSessions.Get(sUser));
                }
                catch
                {
                    connectedSession = null;
                }
                if (userSession != null)
                {
                    try
                    {
                        if (connectedSession != null)
                        {
                            if (connectedSession.SessionID != userSession.SessionID) // disconnect the current session
                                AppendError(connectedSession, "User " + sUser + IGPEMULTIPLEXING_ERROR_SINGLESIGNON);
                        }
                    }
                    catch (Exception) { }
                    finally
                    {
                        tableSessions.Remove(sUser);
                        bAlreadyConnected = false;
                    }                    
                }
                else
                {
                    if (connectedSession != null)
                        AppendError(connectedSession, IGPEMULTIPLEXING_ERROR_NULLSESSION);
                    return true;
                }
            }
            tableSessions.Add(sUser, userSession);                       
            return bAlreadyConnected;
        }
        
        private bool sendUserRequest(HttpSessionState userSession, IGRequest request)
        {
            if ((request.GetType() != typeof(IGRequestWorkspaceDisconnect)) &&
                request.NeedAccount() && 
                (string)userSession[IGMadam.DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE] == IGMadam.DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE_DEMO)
            {
                AppendError(userSession, IGPEMULTIPLEXING_ERROR_NEEDACCOUNT);
                return false;
            }
            IPEndPoint endPoint = (IPEndPoint)userSession[SESSIONMEMBER_SERVERIP];
            if (endPoint == null)
                return false;            
            if (!IGPERequestSender.SendRequest(endPoint, request))
            {
                AppendError(userSession, IGPEMULTIPLEXING_ERROR_SENDREQUEST);
                return false;
            }
            return true;
        }
    }
}