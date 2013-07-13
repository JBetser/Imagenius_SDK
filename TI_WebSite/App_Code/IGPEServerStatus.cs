using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Web;
using System.Web.SessionState;
using System.Net;
using System.Xml;
using System.Timers;
using System.Threading;
using IGSMLib;
using IGMadam;

namespace IGPE
{
    /// <summary>
    /// Summary description for IGPEServerStatus
    /// </summary>
    public class IGPEServerStatus
    {
        private static IGPEServerStatus mg_serverStatus = null;
        private System.Timers.Timer m_timer = null;
        private bool m_bIsProcessing = false;
        private object m_lockObject = new object();
        private IGSMStatusTreeView m_treeViewStatus = new IGSMStatusTreeView();
        private IGPEOutput m_output = new IGPEOutput();
        private IGConfigManagerRemote m_configMgr = null;

        private IGPEServerStatus()
        {
        }

        public static IGPEServerStatus GetInstance()
        {
            if (mg_serverStatus == null)
                mg_serverStatus = new IGPEServerStatus();
            return mg_serverStatus;
        }

        public bool Init()
        {
            m_configMgr = IGConfigManagerRemote.GetInstance();
            m_timer = new System.Timers.Timer(IGSMStatus.IGSMSTATUS_HEARTHBEAT_CHECKTIME);
            m_timer.Elapsed += new ElapsedEventHandler(timer_Elapsed);
            m_timer.Start();
            return true;
        }

        public void AddServerStatusToSession(string sUser, HttpSessionState userSession)
        {
            userSession[IGPEMultiplexing.SESSIONMEMBER_SERVERSTATUS] = m_treeViewStatus;
            if (userSession[DatabaseUserSecurityAuthority.IGMADAM_USERNAME] != null)
            {
                if ((string)userSession[DatabaseUserSecurityAuthority.IGMADAM_USERNAME] == sUser)
                    return;
            }
            string sAuthUser = sUser;
            if (sUser.Contains("Demo@"))
                sAuthUser = "Demo";
            userSession[DatabaseUserSecurityAuthority.IGMADAM_USERNAME] = sUser;
            userSession[DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE] = UserSecurityAuthority.FromConfig().GetUserPrivilege(sAuthUser);
            userSession[DatabaseUserSecurityAuthority.IGMADAM_USERSHORTNAME] = UserSecurityAuthority.FromConfig().GetUserShortName(sAuthUser);
            userSession[IGPEMultiplexing.SESSIONMEMBER_OUTPUT] = new IGPEOutput();
        }
        
        private void timer_Elapsed(object sender, ElapsedEventArgs e)
        {
            try
            {
                if (m_bIsProcessing)
                    return;
                m_bIsProcessing = true;
                lock (m_lockObject)
                {
                    // check that all server connections are UP
                    Hashtable hashServers = m_configMgr.GetHashtableServers();
                    IDictionaryEnumerator enumServers = hashServers.GetEnumerator();
                    Hashtable hashServerStates = new Hashtable();
                    while (enumServers.MoveNext())
                        hashServerStates.Add(enumServers.Value, (((IGServerRemote)enumServers.Value).GetState() == IGSMStatus.IGState.IGSMSTATUS_READY));
                    m_treeViewStatus.UpdateServerList(hashServerStates);
                }
            }
            catch (Exception exc)
            {
                IGServerManager.Instance.AppendError(exc.ToString());
                return;
            }
            m_bIsProcessing = false;
        }
    }
}