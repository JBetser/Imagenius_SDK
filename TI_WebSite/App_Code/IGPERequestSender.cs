using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Net;
using System.Net.Sockets;
using System.Xml;
using System.Text;
using IGSMLib;

namespace IGPE
{
    /// <summary>
    /// Summary description for IGPERequest
    /// </summary>
    public class IGPERequestSender
    {
        private string m_sServerName;
        private IGServer m_server;

        private IGPERequestSender(string sServerName)
        {
            m_sServerName = sServerName;
            IPEndPoint endPt = IGConfigManagerRemote.GetInstance().GetServerEndPoint(m_sServerName);
            m_server = IGConfigManagerRemote.GetInstance().GetServer(endPt.Address.ToString(), endPt.Port);
        }

        private IGPERequestSender(IPEndPoint endPoint)
        {
            m_server = IGConfigManagerRemote.GetInstance().GetServer(endPoint.Address.ToString(), endPoint.Port);
        }

        private bool execute(IGRequest request, bool async = false)
        {
            if (m_server == null)
                return false;
            // Send request to appropriate server  
            if (!m_server.SendRequest(request))
                return false;
            if (async)
                return true;
            return IGServerManager.Instance.ProcessAnswer(IGServerManager.Instance.CreateAnswer(request.GetResult(), m_server.GetConnection()));
        }

        public static bool SendRequest(string sServerName, IGRequest request)
        {
            IGPERequestSender newRequest = new IGPERequestSender(sServerName);
            return newRequest.execute(request);
        }

        public static bool SendRequest(IPEndPoint endPoint, IGRequest request)
        {
            IGPERequestSender newRequest = new IGPERequestSender(endPoint);
            return newRequest.execute(request);            
        }

        public static bool SendRequestAsync(IPEndPoint endPoint, IGRequest request)
        {
            IGPERequestSender newRequest = new IGPERequestSender(endPoint);
            return newRequest.execute(request, true);
        }
    }
}