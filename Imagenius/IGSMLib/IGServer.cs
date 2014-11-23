using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Timers;
using System.Net;
using System.Net.Sockets;
using IGServerController;

namespace IGSMLib
{
    public abstract class IGServer
    {
        public readonly IPEndPoint m_endPoint = null;
        public readonly string m_sIpEndPoint = null;

        protected IGConnection m_connection = null;
        protected bool m_bDisconnected = false;
        private object m_lockSend = new object();
        
        public IGServer(string sIpAddress, int nPort)
        {
            m_sIpEndPoint = sIpAddress + ":" + nPort.ToString();
            if (sIpAddress != null)
                m_endPoint = new IPEndPoint(IPAddress.Parse(sIpAddress), nPort);
        }

        public IGServer(IPEndPoint endPoint)
        {
            m_endPoint = endPoint;        
            m_sIpEndPoint = m_endPoint.Address.ToString() + ":" + m_endPoint.Port.ToString();
        }

        public abstract IGServer Clone();

        public virtual bool Initialize()
        {
            return true;
        }

        public virtual IGConnection CreateConnection()
        {
            return null;
        }

        public virtual void Disconnect()
        {
        }

        public virtual IGSMStatus.IGState GetState()
        {
            return m_bDisconnected ? IGSMStatus.IGState.IGSMSTATUS_NOTRESPONDING : IGSMStatus.IGState.IGSMSTATUS_READY;
        }

        public virtual bool IsDisconnected()
        {
            return m_bDisconnected;
        }        

        public virtual IGConnection GetConnection()
        {
            return m_connection;
        }

        public virtual IGServerControllerIcePrx GetServerControllerClient()
        {
            return null;
        }

        public virtual void Terminate()
        {
            m_bDisconnected = true;
            m_connection = null;
        }

        public virtual void ResetConnection(IGConnection connection)
        {
            m_connection = connection;
        }

        public virtual void ResetState()
        {
        }

        public virtual bool Reset()
        {
            m_connection = null;
            m_bDisconnected = false;
            return true;
        }

        public virtual string GetStatus()
        {
            return string.Format("{{\"Type\":\"{0}\",\"Address\":\"{1}p{2}\",\"Status\":\"{3}\",{4}}}", 
                this.GetType().ToString() == "IGSMLib.IGServerLocal" ? "Local" : "Remote", 
                m_endPoint.Address.ToString(), m_endPoint.Port.ToString(),
                GetState().ToString(), m_connection == null ? "\"Connection\":\"None\"" : m_connection.GetStatus());
        }

        public virtual int GetNbAvailableConnections()
        {
            return 0;
        }

        public override string ToString()
        {
            return m_sIpEndPoint;
        }

        public bool SendRequest(IGRequest request)
        {
            if (request != null)
            {
                if (m_connection != null)
                    return m_connection.SendRequest(request);
            }
            IGServerManager.Instance.AppendError("- IGServer failed sending xml to server " + m_sIpEndPoint);
            return false;
        }    
    }
}
