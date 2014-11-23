using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using IGServerController;

namespace IGSMLib
{
    public class IGServerRemote : IGServer
    {
        private IGSMStatus.IGState m_eState = IGSMStatus.IGState.IGSMSTATUS_READY;
        private long m_nHearthbeatTime = -1;
        private object m_lockObject = new object();
        private IGServerControllerIcePrx m_serverControllerClient = null;

        public IGServerRemote(IPEndPoint endPoint) : base(endPoint){            
        }

        public override IGServer Clone()
        {
            return new IGServerRemote(m_endPoint);
        }

        public override bool Initialize()
        {
            Ice.Communicator com = null;
            try
            {
                Ice.Properties prop = Ice.Util.createProperties();
                prop.setProperty("Ice.Default.Locator", "IGServerControllerIceGrid/Locator:default -t 5000 -h " + m_endPoint.Address.ToString() + " -p " + m_endPoint.Port.ToString());
                Ice.InitializationData initData = new Ice.InitializationData();
                initData.properties = prop;
                com = Ice.Util.initialize(initData);
                m_serverControllerClient = IGServerControllerIcePrxHelper.checkedCast(com.stringToProxy("serverController"));
            }
            catch (Ice.Exception)
            {
                IGServerManager.Instance.AppendError(string.Format("couldn't find a `::IGServerController::IGServerControllerIce' object for {0}", m_endPoint.Address.ToString()));
                return false;
            }
            finally
            {
                if (m_serverControllerClient == null)
                {
                    IGServerManager.Instance.AppendError(string.Format("An exception was thrown while attempting to connect to: {0}", m_endPoint.Address.ToString()));
                }
            }
            return true;
        }

        public override IGServerControllerIcePrx GetServerControllerClient()
        {
            if (m_serverControllerClient == null)
                Initialize();
            return m_serverControllerClient;
        }

        public override bool IsDisconnected()
        {
            return false;
        }

        public override IGConnection CreateConnection()
        {
            ResetState();
            if (m_connection == null)
                m_connection = new IGConnectionRemote(this);
            m_bDisconnected = false;
            return m_connection;
        }

        public override void Disconnect()
        {
        }

        public override IGSMStatus.IGState GetState()
        {
            lock (m_lockObject)
            {
                if (((DateTime.UtcNow.Ticks - m_nHearthbeatTime) / 10000) > HC.HEARTHBEAT_REMOTE_SERVERTIMEOUT_NOTRESPONDING)
                {
                    if (m_connection != null)
                    {
                        if (m_connection.Ping())
                        {
                            ResetState();
                            return m_eState;
                        }
                    }
                    m_eState = IGSMStatus.IGState.IGSMSTATUS_NOTRESPONDING;                   
                }
                else
                {
                    m_eState = IGSMStatus.IGState.IGSMSTATUS_READY;
                }
                return m_eState;
            }
        }

        public override void ResetState()
        {
            lock (m_lockObject)
            {
                m_nHearthbeatTime = DateTime.UtcNow.Ticks;
                m_eState = IGSMStatus.IGState.IGSMSTATUS_READY;
            }
        }

        public override int GetNbAvailableConnections()
        {
            try
            {
                if (m_serverControllerClient == null)
                    Initialize();
                if (m_serverControllerClient == null)
                    return 0;
                return m_serverControllerClient.getNbAvailableConnections();
            }
            catch{
                return 0;
            }
        }

        public override bool Reset()
        {
            m_serverControllerClient.reset();
            return true;
        }

        public void SetStateDown()
        {
            lock (m_lockObject)
            {
                m_nHearthbeatTime = -1;
                m_eState = IGSMStatus.IGState.IGSMSTATUS_NOTRESPONDING;
            }
        }

        public void Hearthbeat()
        {
            lock (m_lockObject)
            {
                m_nHearthbeatTime = DateTime.UtcNow.Ticks;
            }
        }
    }
}
