using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Timers;
using System.Diagnostics;
using System.Xml;

namespace IGSMLib
{
    public class IGSMStatus : IGSMAnswer
    {
        public enum IGSMStatusType
        {
            IGSMSTATUS_FULLSTOP = 0,
            IGSMSTATUS_FULLSTART = 1,
            IGSMSTATUS_FULLANSWER = 2, 
            IGSMSTATUS_USER = 3,
            IGSMSTATUS_ISUSERCONNECTED = 4,
            IGSMSTATUS_AVAILABILITY = 5,
            IGSMSTATUS_UNDEFINED = 6
        };

        public enum IGState
        {
            IGSMSTATUS_READY = 0,
            IGSMSTATUS_WORKING = 1,
            IGSMSTATUS_NOTRESPONDING = 2,
            IGSMSTATUS_DISCONNECTING = 2,
            IGSMSTATUS_UNKNOWN = 4
        };

        public const int IGSMSTATUS_HEARTHBEAT_CHECKTIME = 1400;
                
        public delegate void OnUpdate(XmlNode xmlNodeStatusRoot);
        protected static OnUpdate m_delegateOnUpdate = null;
        public static void SetUpdateDelegate(OnUpdate delegateOnUpdate)
        {
            m_delegateOnUpdate = delegateOnUpdate;
        }

        protected static IGSMFullStatus s_runningStatus = null;

        public const string IGSMSTATUS_ANSWER = "SMAnswer Status";
        public const string IGSMSTATUS_SERVERSTATE = "State";
        public const string IGSMSTATUS_ATTRIBUTE_SERVERNAME = "ServerName";
        public const string IGSMSTATUS_STATUSTYPE = "StatusType";
        public const string IGSMSTATUS_STATUSID = "StatusId";
        public const string IGSMSTATUS_PROGRESS = "Progress";
        public const string IGSMSTATUS_COMPUTERNAME = "ServerName";
        public const string IGSMSTATUS_COMPUTERIP = "ServerIP";
        public const string IGSMSTATUS_COMPUTERPORT = "ServerPort";
        public const string IGSMSTATUS_MAXCONNECTIONS = "MaxConnections";
        public const string IGSMSTATUS_NBCONNECTIONS = "NbConnections";
        public const string IGSMSTATUS_SESSIONLENGTH = "SessionLength";
        public const string IGSMSTATUS_IDLETIME = "IdleTime";
        public const string IGSMSTATUS_STATUS = "Status";
        public const string IGSMSTATUS_IMAGE = "Image";
        public const string IGSMSTATUS_USERS = "Users";
        public const string IGSMSTATUS_USER = "User";
        public const string IGSMSTATUS_LASTREQ = "LastRequest";
        
        public IGSMStatus(IGSMStatusType eType)
            : base(IGSMAnswer.IGSMANSWER_GETSTATUS, IGSMSTATUS_ANSWER)
        {
            // do not use IGServerManagerLocal.LocalInstance !!!
            SetParameter(IGSMSTATUS_STATUSTYPE, ((int)eType).ToString());
        }

        public IGSMStatus(XmlDocument xmlDoc)
            : base(xmlDoc, IGSMSTATUS_ANSWER)
        {
            // do not use IGServerManagerLocal.LocalInstance !!!            
        }        

        protected override bool InternalProcess()
        {
            return true;
        }

        public virtual void Update()
        {
        }
    }

    public class IGSMFullStatus : IGSMStatus
    {        
        protected System.Timers.Timer m_timer;
        private SynchronizedCollection<IGSMStatusUser> m_lUsers;
        private XmlNode m_xmlNodeUsers;
        private bool m_bIsUpdating; 

        private const int IGSMSTATUS_REFRESHTIME = 250;

        public IGSMFullStatus()
            : base(IGSMStatusType.IGSMSTATUS_FULLANSWER)
        {
            m_bIsUpdating = false;
            m_timer = new System.Timers.Timer();
            m_timer.Elapsed += new ElapsedEventHandler(OnTimedEvent);
            m_timer.Interval = IGSMSTATUS_REFRESHTIME;
            m_lUsers = new SynchronizedCollection<IGSMStatusUser>();
            if (m_xmlNodeAnswer.FirstChild != null)
                m_xmlNodeUsers = m_xmlNodeAnswer.FirstChild.AppendChild(m_xmlDoc.CreateNode(XmlNodeType.Element, IGSMSTATUS_USERS, null));
            SetParameter(IGSMSTATUS_COMPUTERNAME, Environment.MachineName);
            SetParameter(IGSMSTATUS_COMPUTERIP, IGServerManagerLocal.LocalInstance.ServerIP.ToString());
            SetParameter(IGSMSTATUS_COMPUTERPORT, IGServerManagerLocal.LocalInstance.ServerPort.ToString());
            Update();
        }

        public override void Update()
        {
            SetParameter(IGSMSTATUS_MAXCONNECTIONS, IGServerManagerLocal.LocalInstance.GetNbConnections().ToString());
            SetParameter(IGSMSTATUS_NBCONNECTIONS, IGServerManagerLocal.LocalInstance.GetNbUserConnections().ToString());
            m_lUsers.Clear();
            m_xmlNodeUsers.RemoveAll();
            foreach (IGConnectionLocal connection in IGServerManagerLocal.LocalInstance.GetConnections())
            {
                IGSMStatusUser userStatus = connection.GetUserStatus();
                userStatus.Stop();
                userStatus.OnTimedEvent(null, null);
                if (connection.m_sUser != null)
                {
                    IGSMStatusUser statusUser = new IGSMStatusUser(connection);
                    XmlNode userNodeAnswer = statusUser.GetAnswer();
                    XmlNode userNodeStatus = userNodeAnswer.SelectSingleNode("./Params");
                    XmlNode userNode = m_xmlDoc.CreateNode(XmlNodeType.Element, IGSMSTATUS_USER, null);
                    foreach (XmlAttribute att in userNodeStatus.Attributes)
                    {
                        XmlAttribute newAtt = m_xmlDoc.CreateAttribute(att.Name);
                        newAtt.Value = att.Value;
                        userNode.Attributes.Append(newAtt);
                    }
                    XmlNode xmlSessionLength = userNode.Attributes.GetNamedItem(IGSMSTATUS_SESSIONLENGTH);
                    if (xmlSessionLength != null)
                    {
                        DateTime sessionLength = new DateTime(Convert.ToInt64(xmlSessionLength.Value) * 10000);
                        XmlNode xmlIdleTime = userNode.Attributes.GetNamedItem(IGSMSTATUS_IDLETIME);
                        if (xmlIdleTime != null)
                        {
                            DateTime idleTime = new DateTime(Convert.ToInt64(xmlIdleTime.Value) * 10000);
                            string sSessionLength = (sessionLength.Hour > 0) ? sessionLength.Hour + " : " : "";
                            sSessionLength += (sessionLength.Minute > 0) ? sessionLength.Minute + "' " : "";
                            sSessionLength += sessionLength.Second + "''";
                            string sIdleTime = (idleTime.Hour > 0) ? idleTime.Hour + " : " : "";
                            sIdleTime += (idleTime.Minute > 0) ? idleTime.Minute + "' " : "";
                            sIdleTime += idleTime.Second + "''";
                            int nStatus = Convert.ToInt32(userNode.Attributes.GetNamedItem(IGSMSTATUS_STATUS).Value);
                            string sStatus = (nStatus == (int)IGSMStatus.IGState.IGSMSTATUS_READY) ? "Ready" :
                                            (nStatus == (int)IGSMStatus.IGState.IGSMSTATUS_WORKING) ? "Working..." :
                                            (nStatus == (int)IGSMStatus.IGState.IGSMSTATUS_NOTRESPONDING) ? "Not Responding" : 
                                            (nStatus == (int)IGSMStatus.IGState.IGSMSTATUS_DISCONNECTING) ? "Disconnecting" : "Unknown";
                            XmlAttribute newUserAtt = m_xmlDoc.CreateAttribute(IGSMSTATUS_SESSIONLENGTH);
                            newUserAtt.Value = sSessionLength;
                            userNode.Attributes.Append(newUserAtt);
                            newUserAtt = m_xmlDoc.CreateAttribute(IGSMSTATUS_IDLETIME);
                            newUserAtt.Value = sIdleTime;
                            userNode.Attributes.Append(newUserAtt);
                            newUserAtt = m_xmlDoc.CreateAttribute(IGSMSTATUS_STATUS);
                            newUserAtt.Value = sStatus;
                            userNode.Attributes.Append(newUserAtt);
                            newUserAtt = m_xmlDoc.CreateAttribute(IGSMSTATUS_STATUSID);
                            newUserAtt.Value = nStatus.ToString();
                            userNode.Attributes.Append(newUserAtt);
                            foreach (XmlNode childNode in userNodeAnswer.SelectNodes("./Image"))
                            {
                                XmlNode imageNode = m_xmlDoc.CreateNode(XmlNodeType.Element, IGSMSTATUS_IMAGE, null);
                                foreach (XmlAttribute att in childNode.Attributes)
                                {
                                    XmlAttribute newAtt = m_xmlDoc.CreateAttribute(att.Name);
                                    newAtt.Value = att.Value;
                                    imageNode.Attributes.Append(newAtt);
                                }
                                int nImageStatus = Convert.ToInt32(imageNode.Attributes.GetNamedItem(IGSMSTATUS_STATUS).Value);
                                string sImageStatus = (nImageStatus == (int)IGSMStatusUser.IGState.IGSMSTATUS_READY) ? "Ready" :
                                                        (nImageStatus == (int)IGSMStatusUser.IGState.IGSMSTATUS_WORKING) ? "Working..." : "Not Responding";
                                XmlAttribute newImageAtt = m_xmlDoc.CreateAttribute(IGSMSTATUS_STATUS);
                                newImageAtt.Value = sImageStatus;
                                imageNode.Attributes.Append(newImageAtt);
                                userNode.AppendChild(imageNode);
                            }
                            m_xmlNodeUsers.AppendChild(userNode);
                            m_lUsers.Add(statusUser);
                        }
                    }
                    userStatus.Start();
                }
            }
            if (m_delegateOnUpdate != null)
                m_delegateOnUpdate(m_xmlDoc.SelectSingleNode("*/Params"));
        }

        public void Start()
        {
            m_timer.Enabled = true;
        }

        public void Stop()
        {
            m_timer.Enabled = false;
        }

        public void OnTimedEvent(object source, ElapsedEventArgs e)
        {
            if (!m_bIsUpdating)
            {
                m_bIsUpdating = true;
                try
                {
                    Update();
                }
                catch (Exception)
                {
                    m_timer.Enabled = false;
                    m_bIsUpdating = false;
                    return;
                }
                m_bIsUpdating = false;
            }
        }         
    }

    public class IGSMFullStatusStart : IGSMStatus
    {
        public IGSMFullStatusStart()
            : base(IGSMStatusType.IGSMSTATUS_FULLSTART)
        {
            if (s_runningStatus == null)
                s_runningStatus = new IGSMFullStatus();
            s_runningStatus.Start();
        }
    }

    public class IGSMFullStatusStop : IGSMStatus
    {
        public IGSMFullStatusStop()
            : base(IGSMStatusType.IGSMSTATUS_FULLSTOP)
        {
            if (s_runningStatus != null)
                s_runningStatus.Stop();
        }
    }

    public class IGSMStatusUserConnected : IGSMStatus
    {
        public string m_sUser;

        protected const string IGSMANSWER_USERCONNECTED = "UserConnected";

        public IGSMStatusUserConnected(string sUser)
            : base(IGSMStatusType.IGSMSTATUS_ISUSERCONNECTED)
        {
            m_sUser = sUser;
            SetParameter(IGRequest.IGREQUEST_USERLOGIN, sUser);
            SetParameter(IGSMANSWER_USERCONNECTED, IGServerManagerLocal.LocalInstance.IsUserConnected(sUser) ? "1" : "0");
        }
    }

    public class IGSMStatusUser : IGSMStatusUserConnected
    {        
        public long m_nSessionLength = -1;
        public long m_nIdleTime = -1;
        public long m_nStartTime = -1;
        public long m_nStartIdleTime = -1;
        public long m_nHearthbeatTime = -1;
        public IGState m_eStatus = IGState.IGSMSTATUS_NOTRESPONDING;
        public List<string> m_lsImages;
        public System.Timers.Timer m_timer;
        public int m_nLastReqId = -1;

        public IGSMStatusUser()
            : base(null)
        {
            m_nSessionLength = 0;
            m_nIdleTime = 0;
            m_nStartTime = DateTime.UtcNow.Ticks;
            m_nStartIdleTime = DateTime.UtcNow.Ticks;
            m_nHearthbeatTime = DateTime.UtcNow.Ticks;
            m_eStatus = IGState.IGSMSTATUS_READY;
            m_lsImages = new List<string>();
            m_timer = new System.Timers.Timer();
            m_timer.Elapsed += new ElapsedEventHandler(OnTimedEvent);
            m_timer.Interval = IGSMSTATUS_HEARTHBEAT_CHECKTIME;            
        }

        public IGSMStatusUser(string sUser) : base(sUser)
        {
            IGConnection userConnection = null;
            IGServerManagerLocal.LocalInstance.GetConnection(sUser, out userConnection);
            if(userConnection == null)
                m_eStatus = IGState.IGSMSTATUS_READY;
            else
                Init((IGConnectionLocal)userConnection);
        }

        public IGSMStatusUser(IGConnectionLocal userConnection)
            : base(userConnection.GetName())
        {
            Init(userConnection);
        }

        public void Init(IGConnectionLocal userConnection)
        {
            IGSMStatusUser userStatus = userConnection.GetUserStatus();
            if (userStatus != null)
            {
                SetParameter(IGSMSTATUS_SESSIONLENGTH, userStatus.m_nSessionLength.ToString());
                SetParameter(IGSMSTATUS_IDLETIME, userStatus.m_nIdleTime.ToString());
                SetParameter(IGSMSTATUS_STATUS, ((int)userStatus.m_eStatus).ToString());
                IGRequestProcessing imProcUser = IGRequestProcessing.GetUserRequest(userStatus.m_sUser);
                SetParameter(IGSMSTATUS_PROGRESS, ((imProcUser == null) || (imProcUser.GetProgress() < 0)) ? "-" : imProcUser.GetProgress().ToString() + "%");
                SetParameter(IGSMSTATUS_LASTREQ, (userStatus.m_nLastReqId > 0) ? userStatus.m_nLastReqId.ToString() : "None");
                List<string> lsImages = new List<string>();
                lock (userStatus.m_lsImages)
                {
                    lsImages.AddRange(userStatus.m_lsImages);
                }
                foreach (string sImage in lsImages)
                {
                    XmlNode imageNode = m_xmlDoc.CreateNode(XmlNodeType.Element, IGSMSTATUS_IMAGE, null);
                    XmlAttribute newAttImageId = m_xmlDoc.CreateAttribute(IGRequest.IGREQUEST_FRAMEID);
                    newAttImageId.Value = sImage;
                    imageNode.Attributes.Append(newAttImageId);
                    IGRequestProcessing imProc = IGRequestProcessing.GetImageRequest(sImage);
                    XmlAttribute newAttProgress = m_xmlDoc.CreateAttribute(IGSMSTATUS_PROGRESS);
                    newAttProgress.Value = ((imProc == null) || (imProc.GetProgress() < 0)) ? "-" : imProc.GetProgress().ToString() + "%";
                    imageNode.Attributes.Append(newAttProgress);
                    XmlAttribute newAttStatus = m_xmlDoc.CreateAttribute(IGSMSTATUS_STATUS);
                    int nStatus = (imProc == null) ? (int)IGState.IGSMSTATUS_READY : (int)imProc.GetStatus();
                    newAttStatus.Value = nStatus.ToString();
                    imageNode.Attributes.Append(newAttStatus);/*
                    XmlAttribute newAttLastReq = m_xmlDoc.CreateAttribute(IGSMSTATUS_LASTREQ);
                    newAttLastReq.Value = (userStatus.m_nLastReqId > 0) ? userStatus.m_nLastReqId.ToString() : "None";
                    imageNode.Attributes.Append(newAttLastReq);*/
                    m_xmlNodeAnswer.AppendChild(imageNode);
                }
            }
        }

        public delegate void OnUserRemoved(string sUser);
        private static OnUserRemoved m_delegateOnUserRemoved = null;
        public delegate void OnFrameRemoved(string sUser, string sFrameId);
        private static OnFrameRemoved m_delegateOnFrameRemoved = null;
        public static void SetUserRemovedDelegate(OnUserRemoved delegateOnUserRemoved)
        {
            m_delegateOnUserRemoved = delegateOnUserRemoved;
        }
        public static void SetFrameRemovedDelegate(OnFrameRemoved delegateOnFrameRemoved)
        {
            m_delegateOnFrameRemoved = delegateOnFrameRemoved;
        }

        public void RemoveUser()
        {
            Stop();
            if (m_delegateOnUserRemoved != null)
                m_delegateOnUserRemoved(m_sUser);
        }

        public void RemoveFrame(string sUser, string sFrameId)
        {
            Stop();
            if (m_delegateOnFrameRemoved != null)
                m_delegateOnFrameRemoved(sUser, sFrameId);
            Start();
        }
        
        public XmlNode GetAnswer()
        {
            return m_xmlNodeAnswer;
        }

        public void OnTimedEvent(object source, ElapsedEventArgs e)
        {
            if(((DateTime.UtcNow.Ticks - m_nHearthbeatTime) / 10000) > HC.HEARTHBEAT_LOCAL_TIMEOUT_NOTRESPONDING)
            {
                if (((DateTime.UtcNow.Ticks - m_nHearthbeatTime) / 10000) > HC.HEARTHBEAT_LOCAL_TIMEOUT_DISCONNECTING)
                {
                    m_timer.Enabled = false;
                    if ((m_sUser != null) &&
                        (m_eStatus != IGState.IGSMSTATUS_DISCONNECTING) &&
                        m_serverMgr.IsLocalServer())
                    {
                        m_eStatus = IGState.IGSMSTATUS_DISCONNECTING;
                        m_serverMgr.AppendError("Hearthbeat TIMEOUT, disconnecting user " + m_sUser);
                        IGConnection connection = null;
                        IGServerManagerLocal.LocalInstance.GetConnection(m_sUser, out connection, true);
                        IGConnectionLocal userConnection = (IGConnectionLocal)connection;
                        if (userConnection != null)
                            userConnection.Reset(null);
                    }
                    m_eStatus = IGState.IGSMSTATUS_DISCONNECTING;                    
                }
                else
                    m_eStatus = IGState.IGSMSTATUS_NOTRESPONDING;
            }
            else
            {
                if (IGRequestProcessing.IsBusy(m_sUser))
                    m_eStatus = IGState.IGSMSTATUS_WORKING;
                else
                    m_eStatus = IGState.IGSMSTATUS_READY;
            }
        }

        public void Start()
        {
            m_timer.Enabled = true;
        }

        public void Stop()
        {
            m_timer.Enabled = false;
        }
    }

    public class IGSMStatusAvailability : IGSMStatus
    {
        public IGSMStatusAvailability()
            : base(IGSMStatusType.IGSMSTATUS_AVAILABILITY)
        {
            SetParameter(IGSMSTATUS_MAXCONNECTIONS, IGServerManagerLocal.LocalInstance.GetNbConnections().ToString());
            SetParameter(IGSMSTATUS_NBCONNECTIONS, IGServerManagerLocal.LocalInstance.GetNbUserConnections().ToString());            
        }
    }
}
