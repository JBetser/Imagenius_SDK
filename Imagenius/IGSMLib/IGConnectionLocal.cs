using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;

namespace IGSMLib
{
    public class IGConnectionLocal : IGConnection
    {
        public TcpClient m_TCPclient = null;
        private IGSMStatusUser m_status = null;
        private List<string> m_lsImages = null;
        private bool m_bUserConnected = false;
        private bool m_bUserConnecting = false;
        private bool m_bUserDisconnecting = false;
        
        public IGConnectionLocal(IGServer igServer) : base(igServer)
        {
            m_status = new IGSMStatusUser();
            m_lsImages = new List<string>();
        }

        public override void Initialize()
        {
            OnUserDisconnected();
            m_TCPclient = new TcpClient(m_igServer.m_endPoint.Address.ToString(), m_igServer.m_endPoint.Port);
            m_nwStream = m_TCPclient.GetStream();
            flush();
        }

        public override bool IsLocal()
        {
            return true;
        }

        public override void Reset(string sGuid)
        {
            m_igServer.ResetConnection(this);
        }

        public override void Terminate(string sGuid)
        {
            m_igServer.Terminate();
        }

        public override bool SendRequest(IGRequest request)
        {
            IGRequestProcessing.Add(request, m_igServer);
            // Send request to appropriate server application
            return request.Send(m_TCPclient);
        }

        public int ConnectUser(string sUser, string sGuid)
        {
            if (m_bUserConnected)
                return (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERALREADYCONNECTED;
            m_sUser = sUser;
            m_bUserConnected = true;
            m_bUserConnecting = true;
            m_status.m_nIdleTime = 0;
            m_status.m_nStartIdleTime = DateTime.UtcNow.Ticks;
            m_status.m_nSessionLength = 0;
            m_status.m_nStartTime = DateTime.UtcNow.Ticks;
            m_status.m_sUser = m_sUser;
            m_status.m_eStatus = IGSMStatus.IGState.IGSMSTATUS_WORKING;
            m_status.Start();
            return IGSMAnswer.IGSMANSWER_ERROR_NONE;
        }

        public int DisconnectUser(string sGuid)
        {
            m_status.m_eStatus = IGSMStatus.IGState.IGSMSTATUS_WORKING;
            m_bUserDisconnecting = true;
            lock (m_lsImages)
            {
                m_lsImages.Clear();
            }
            if (!m_bUserConnected)
                return (int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERNOTFOUND;
            return IGSMAnswer.IGSMANSWER_ERROR_NONE;
        }

        public override string GetStatus()
        {
            GetUserStatus();
            return string.Format("\"Connection\":{{\"Type\":\"Local\",\"User\":\"{0}\",\"Status\":\"{1}\",\"SessionTime\":\"{2}s\",\"IdleTime\":\"{3}s\",\"Images\":\"{4}\"}}", 
                m_sUser == null ? "None" : m_sUser, m_status.m_eStatus.ToString(), (m_status.m_nSessionLength / 1000).ToString(), 
                (m_status.m_nIdleTime / 1000).ToString(), m_status.m_lsImages.Count);
        }

        public IGSMStatusUser GetUserStatus()
        {
            if (m_sUser == null)
                return null;
            m_status.m_sUser = m_sUser;
            m_status.m_nSessionLength = (DateTime.UtcNow.Ticks - m_status.m_nStartTime) / 10000;
            if (m_status.m_eStatus == IGSMStatusUser.IGState.IGSMSTATUS_READY)
            {
                if (IGRequestProcessing.IsBusy(m_sUser))
                {
                    m_status.m_eStatus = IGSMStatusUser.IGState.IGSMSTATUS_WORKING;
                    m_status.m_nIdleTime = 0;
                    m_status.m_nStartIdleTime = DateTime.UtcNow.Ticks;
                }
                else
                {
                    m_status.m_nIdleTime = (DateTime.UtcNow.Ticks - m_status.m_nStartIdleTime) / 10000;
                }
            }
            else
            {
                m_status.m_nIdleTime = 0;
                m_status.m_nStartIdleTime = DateTime.UtcNow.Ticks;
            }
            List<string> lsImagesCopy = null;
            lock (m_lsImages)
            {
                lsImagesCopy = m_lsImages.ToList();
            }
            lock (m_status.m_lsImages)
            {
                m_status.m_lsImages.Clear();
                m_status.m_lsImages.AddRange(lsImagesCopy);
            }
            return m_status;
        }

        public void AddImage(string sImage)
        {
            lock (m_lsImages)
            {
                m_lsImages.Add(sImage);
            }
        }

        public void RemoveImage(string sImage)
        {
            lock (m_lsImages)
            {
                if (sImage == "-1")
                    m_lsImages.Clear();
                else
                    m_lsImages.Remove(sImage);
            }
            m_status.RemoveFrame(m_sUser, sImage);
        }

        public void OnUserDisconnected()
        {
            m_status.Stop();
            m_status.RemoveUser();
            lock (m_lsImages)
            {
                m_lsImages.Clear();
            }
            m_bUserConnecting = false;
            m_bUserDisconnecting = false;
            m_bUserConnected = false;
            m_sUser = null;
            m_status.m_eStatus = IGSMStatus.IGState.IGSMSTATUS_READY;
        }

        public void OnUserConnected(string sRestoredFrameIds)
        {
            if (sRestoredFrameIds.Length > 0)
            {
                int nNextFrame = -1;
                do
                {
                    nNextFrame = sRestoredFrameIds.IndexOf(',');
                    lock (m_lsImages)
                    {
                        if (nNextFrame >= 0)
                            m_lsImages.Add(sRestoredFrameIds.Substring(0, nNextFrame));
                        else
                            m_lsImages.Add(sRestoredFrameIds);
                    }
                    sRestoredFrameIds = sRestoredFrameIds.Substring(nNextFrame + 1);
                }
                while (nNextFrame != -1);
            }
            m_bUserConnecting = false;
            m_status.m_eStatus = IGSMStatus.IGState.IGSMSTATUS_READY;
        }

        public string[] GetImagePaths()
        {
            lock (m_lsImages)
            {
                if (m_lsImages.Count == 0)
                    return null;
                string[] imagePaths = new string[m_lsImages.Count];
                m_lsImages.CopyTo(imagePaths, 0);
                return imagePaths;
            }
        }

        public void Hearthbeat()
        {
            m_serverMgr.Hearthbeat();
            m_status.m_nHearthbeatTime = DateTime.UtcNow.Ticks;
        }

        public void RefreshStatus()
        {
            m_status.m_nStartIdleTime = DateTime.UtcNow.Ticks;
        }

        public bool IsUserConnected()
        {
            return m_bUserConnected;
        }

        public bool IsUserConnecting()
        {
            return m_bUserConnecting;
        }

        public bool IsUserDisconnecting()
        {
            return m_bUserDisconnecting;
        }        
    }
}
