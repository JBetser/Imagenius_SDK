using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Runtime.Serialization;

namespace IGSMLib
{
    [DataContract]
    public class IGRequestWorkspace : IGRequest
    {
        public IGRequestWorkspace(int nReqId, string sReqName, string sUserLogin)
            : base(sReqName, IGREQUEST_WORKSPACE, nReqId)
        {
            SetParameter(IGREQUEST_USERLOGIN, sUserLogin);
        }
        public IGRequestWorkspace(string sReqName, XmlDocument xmlDoc)
            : base(sReqName, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestWorkspaceConnect : IGRequestWorkspace
    {
        public const string IGREQUESTCONNECT_STRING = "Connect user";
        public IGRequestWorkspaceConnect(string sUserLogin)
            : base(IGREQUEST_WORKSPACE_CONNECT, IGREQUESTCONNECT_STRING, sUserLogin){}
        public IGRequestWorkspaceConnect(XmlDocument xmlDoc)
            : base(IGREQUESTCONNECT_STRING, xmlDoc) { }
        public override bool ProcessAnswer(IGAnswer answer)
        {
            if (UserConnection != null)
            {
                if (answer.GetId() == (int)IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_ACTIONFAILED)
                    UserConnection.OnUserDisconnected();
            }
            return true;
        }
    }

    [DataContract]
    public class IGRequestWorkspaceDisconnect : IGRequestWorkspace
    {
        public const string IGREQUESTDISCONNECT_STRING = "Disconnect user";
        public IGRequestWorkspaceDisconnect(string sUserLogin)
            : base(IGREQUEST_WORKSPACE_DISCONNECT, IGREQUESTDISCONNECT_STRING, sUserLogin) { }
        public IGRequestWorkspaceDisconnect(XmlDocument xmlDoc)
            : base(IGREQUESTDISCONNECT_STRING, xmlDoc) { }
        public override bool ProcessAnswer(IGAnswer answer)
        {
            if (UserConnection != null)
                UserConnection.OnUserDisconnected();
            return true;
        }
    }

    [DataContract]
    public class IGRequestWorkspacePing : IGRequestWorkspace
    {
        public const string IGREQUESTPING_STRING = "Ping";
        public IGRequestWorkspacePing(string sUserLogin)
            : base(IGREQUEST_WORKSPACE_PING, IGREQUESTPING_STRING, sUserLogin) { }
        public IGRequestWorkspacePing(XmlDocument xmlDoc)
            : base(IGREQUESTPING_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestWorkspaceLoad : IGRequestWorkspace
    {
        public const string IGREQUESTLOAD_STRING = "Load image";
        public IGRequestWorkspaceLoad(string sUserLogin, string sPath, string sLoadAs, string sAutoRotate)
            : base(IGREQUEST_WORKSPACE_LOADIMAGE, IGREQUESTLOAD_STRING, sUserLogin)
        {
            SetParameter(IGREQUEST_PATH, sPath);
            SetParameter(IGREQUEST_LOADAS, sLoadAs);
            SetParameter(IGREQUEST_AUTOROTATE, sAutoRotate);
        }
        public IGRequestWorkspaceLoad(XmlDocument xmlDoc)
            : base(IGREQUESTLOAD_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestWorkspaceNew : IGRequestWorkspace
    {
        public const string IGREQUESTNEW_STRING = "New image";
        public IGRequestWorkspaceNew(string sUserLogin, string sWidth, string sHeight, string sColorMode, string sBackgroundMode)
            : base(IGREQUEST_WORKSPACE_NEWIMAGE, IGREQUESTNEW_STRING, sUserLogin)
        {
            SetParameter(IGREQUEST_WIDTH, sWidth);
            SetParameter(IGREQUEST_HEIGHT, sHeight);
            SetParameter(IGREQUEST_COLORMODE, sColorMode);
            SetParameter(IGREQUEST_BACKGROUNDMODE, sBackgroundMode);
        }
        public IGRequestWorkspaceNew(XmlDocument xmlDoc)
            : base(IGREQUESTNEW_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestWorkspaceShow : IGRequestWorkspace
    {
        public const string IGREQUESTSHOW_STRING = "Show";
        public IGRequestWorkspaceShow(string sUser, bool bVisible)
            : base(IGREQUEST_WORKSPACE_SHOW, IGREQUESTSHOW_STRING, sUser)
        {
            SetParameter(IGREQUEST_VISIBLE, bVisible ? "1" : "0");
        }
        public IGRequestWorkspaceShow(XmlDocument xmlDoc)
            : base(IGREQUESTSHOW_STRING, xmlDoc) { }
    }    
}
