using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using IGServerController;
using System.Xml;

namespace IGSMLib
{
    class IGConnectionRemote : IGConnection
    {
        public IGConnectionRemote(IGServer igServer)
            : base(igServer)
        {
        }

        public override void Initialize()
        {
            m_sUser = m_igServer.m_sIpEndPoint;       
        }

        public override void Terminate(string sGuid)
        {
            m_igServer.Terminate();
            SendRequest(new IGSMRequestTerminate(false));
        }

        public override void Reset(string sGuid)
        {
            Reset();
        }

        public void Reset()
        {
            m_igServer.Reset();
        }

        public override bool Ping()
        {
            IGRequest ping = new IGSMRequestPing();
            if (!SendRequest(ping))
                return false;
            return IGAnswer.GetAnswerId(ping.GetResult()) == IGSMAnswer.IGSMANSWER_PONG;
        }

        public override bool IsLocal()
        {
            return false;
        }

        public override string GetStatus()
        {
            string status = "";
            IGServerControllerIcePrx serverControllerClient = m_igServer.GetServerControllerClient();
            if (serverControllerClient == null)
            {
                m_serverMgr.AppendError("IGConnectionRemote couldn't send an Ice request");
                status = "\"Connection\":\"None\"";
            }
            else
            {
                status = serverControllerClient.getStatus();
            }
            return status;
        }

        public override bool SendRequest(IGRequest request)
        {
            IGRequestProcessing.Add(request, m_igServer);
            IGServerControllerIcePrx serverControllerClient = m_igServer.GetServerControllerClient();
            if (serverControllerClient == null){
                m_serverMgr.AppendError("IGConnectionRemote couldn't send an Ice request");
                return false;
            }
            request.SetResult(serverControllerClient.sendRequest(request.GetXml()));
            return m_serverMgr.CreateAnswer(request).Execute();
        }  
    }
}
