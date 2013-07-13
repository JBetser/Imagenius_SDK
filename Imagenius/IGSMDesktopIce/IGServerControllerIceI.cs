using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using IGServerController;
using IGSMLib;
using System.Threading;

namespace IGSMDesktopIce
{
    class IGServerControllerIceI : IGServerControllerIceDisp_
    {
        string serverName;
        IGServerManagerLocal serverManager;

        public IGServerControllerIceI(IGServerManagerLocal srvMgr, string name)
        {
            serverName = name;
            serverManager = srvMgr;
        }

        public override string sendRequest(string reqXML, Ice.Current current)
        {
            if (serverManager.GetNbConnections() == 0)
                serverManager.Reset();
            AutoResetEvent stopWaitHandle = new AutoResetEvent(false);
            IGRequest curReq = serverManager.ProcessRequest(reqXML, stopWaitHandle);
            if (curReq.GetId() == IGRequest.IGREQUEST_WORKSPACE_DISCONNECT)
            {
                IGAnswerDisconnected answer = new IGAnswerDisconnected(null);
                answer.SetAttribute(IGRequest.IGREQUEST_USERLOGIN, curReq.GetAttributeValue(IGRequest.IGREQUEST_USERLOGIN));
                curReq.SetResult(answer.GetXml());
            }
            else if (!IGSMAnswer.IsSMError(curReq.GetId()))
            {
                if (!stopWaitHandle.WaitOne(HC.REQUEST_PROCESSING_TIMEOUT)) // wait for request processed event (10s timeout)
                {
                    IGAnswer error = new IGSMAnswer((int)IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_TIMEOUT, "Following request did not respond in time: " + curReq.ToString());
                    error.SetReqGuid(curReq.GetGuid());
                    Console.WriteLine("sending error: " + error.GetXml());
                    if (curReq.UserConnection != null)
                        curReq.UserConnection.Reset(IGServerManager.IGSERVERMANAGER_AUTHORITY);                    
                    return error.GetXml();
                }
            }
            return curReq.GetResult();
        }

        public override string ping(Ice.Current current)
        {
            string pong = "pong";
            Console.WriteLine(pong);
            return pong;
        }

        public override void reset(Ice.Current current)
        {
            Console.WriteLine("reset");
            serverManager.Reset();
        }

        public override void shutdown(Ice.Current current)
        {
            Console.WriteLine(serverName + " is shut down");
            serverManager.Terminate(IGServerManager.IGSERVERMANAGER_AUTHORITY);
            current.adapter.getCommunicator().shutdown();
        }

        public override int getNbAvailableConnections(Ice.Current current)
        {
            return serverManager.GetNbAvailableConnections();
        }

        public override int getNbConnections(Ice.Current current)
        {
            return serverManager.GetNbConnections();
        }
    }
}
