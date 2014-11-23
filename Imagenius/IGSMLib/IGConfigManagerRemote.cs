using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;
using System.Xml;
using System.Net;

namespace IGSMLib
{
    public class IGConfigManagerRemote
    {
        private static IGConfigManagerRemote mg_configMgr = null;
        private InterThreadHashtable m_mapServers = null;
        private string m_sSubNetwork = null;

        private IGConfigManagerRemote()
        {
        }

        public static IGConfigManagerRemote GetInstance()
        {
            if (mg_configMgr == null)
            {
                mg_configMgr = new IGConfigManagerRemote();
                if (!mg_configMgr.Init())
                    mg_configMgr = null;
            }
            return mg_configMgr;
        }

        public bool Init()
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.Load(AppDomain.CurrentDomain.BaseDirectory + "\\App_Config\\IGPEServerConfig.xml");
            XmlNode xmlNodeConfig = xmlDoc.SelectSingleNode("./ServerConfig");                    
            XmlNodeList xmlListServers = xmlNodeConfig.SelectNodes("*/Server");
            if (xmlListServers.Count > 0)
            {
                m_mapServers = new InterThreadHashtable();
                foreach (XmlNode xmlServer in xmlListServers)
                {
                    string sServerName = xmlServer.Attributes.GetNamedItem("name").Value;
                    int nPort = int.Parse(xmlServer.Attributes.GetNamedItem("port").Value);    
                    IPEndPoint serverIP = new IPEndPoint(IPAddress.Parse(xmlServer.FirstChild.Value), nPort);
                    m_mapServers.Add(sServerName, new IGServerRemote(serverIP));
                    string sServerIP = serverIP.Address.ToString();
                    if (m_sSubNetwork != null)
                    {
                        if (sServerIP.Substring(0, sServerIP.LastIndexOf('.')) != m_sSubNetwork)
                            return false;
                    }
                    else
                        m_sSubNetwork = sServerIP.Substring(0, sServerIP.LastIndexOf('.'));
                }
            }
            return true;
        }

        public string GetSubNetwork()
        {
            return m_sSubNetwork;
        }

        public IPEndPoint GetServerEndPoint(string sServerName)
        {
            Hashtable hashServers = m_mapServers.GetHashtable();
            IDictionaryEnumerator enumServers = hashServers.GetEnumerator();
            while (enumServers.MoveNext())
            {
                if (String.Compare(enumServers.Key.ToString(), sServerName, true) == 0)
                    return ((IGServer)enumServers.Value).m_endPoint;
            }
            return null;
        }

        public IPEndPoint GetMostAvailableServerEndPoint()
        {
            Hashtable hashServers = m_mapServers.GetHashtable();
            IDictionaryEnumerator enumServers = hashServers.GetEnumerator();
            int nScoreAvailability = 0;
            IGServer mostAvailableServer = null;
            while (enumServers.MoveNext())
            {
                if (((IGServer)enumServers.Value).GetNbAvailableConnections() > nScoreAvailability)
                {
                    mostAvailableServer = (IGServer)enumServers.Value;
                    nScoreAvailability = ((IGServer)enumServers.Value).GetNbAvailableConnections();
                }
            }
            if (mostAvailableServer != null)
                return mostAvailableServer.m_endPoint;
            return null;
        }

        public string GetServerName(string sServerEndPoint)
        {
            Hashtable hashServers = m_mapServers.GetHashtable();
            IDictionaryEnumerator enumServers = hashServers.GetEnumerator();
            while (enumServers.MoveNext())
            {
                if (enumServers.Value.ToString() == sServerEndPoint)
                    return enumServers.Key.ToString();
            }
            return null;
        }

        public List<IPEndPoint> GetListServerEndPoints()
        {
            Hashtable hashServers = m_mapServers.GetHashtable();
            List<IPEndPoint> lServers = new List<IPEndPoint>();
            IDictionaryEnumerator enumServers = hashServers.GetEnumerator();
            while (enumServers.MoveNext())
            {
                lServers.Add(((IGServer)enumServers.Value).m_endPoint);
            }
            return lServers;
        }

        public List<IGServer> GetListServers()
        {
            Hashtable hashServers = m_mapServers.GetHashtable();
            List<IGServer> lServers = new List<IGServer>();
            IDictionaryEnumerator enumServers = hashServers.GetEnumerator();
            while (enumServers.MoveNext())
            {
                lServers.Add((IGServer)enumServers.Value);
            }
            return lServers;
        }

        public Hashtable GetHashtableServers()
        {
            return m_mapServers.GetHashtable();
        }

        public bool AddServer(string sName, IPAddress serverIP, int nPort)
        {
            try
            {
                m_mapServers.Add(sName, new IGServerRemote(new IPEndPoint(serverIP, nPort)));
            }
            catch (Exception)
            {
                return false;
            }
            return true;
        }

        public bool RemoveServer(string sName)
        {
            if (m_mapServers.Get(sName) == null)
                return false;
            m_mapServers.Remove(sName);
            return true;
        }

        public IGServer GetNamedServer(string sName)
        {
            IPEndPoint endPoint = GetServerEndPoint(sName);
            if (endPoint == null)
                return null;
            return GetServer(endPoint.Address.ToString(), endPoint.Port);
        }

        public IGServer GetServer(string sServerIP, int nPort)
        {
            Hashtable hashServers = m_mapServers.GetHashtable();
            IDictionaryEnumerator enumServers = hashServers.GetEnumerator();
            while (enumServers.MoveNext())
            {
                if (enumServers.Value.ToString() == sServerIP + ":" + nPort.ToString())
                    return ((IGServer)enumServers.Value);
            }
            return null;
        }
    }
}
