using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Net;
using System.Net.Sockets;
using System.Text.RegularExpressions;
using System.IO;
using System.Web.SessionState;
using System.Runtime.Serialization;

namespace IGSMLib
{
    [DataContract]
    public class IGSMRequest : IGRequest
    {
        public enum IGSMREQUEST_ID {    IGSMREQUEST_PING = 5000,
				                        IGSMREQUEST_GETSTATUS = 5001,
                                        IGSMREQUEST_TERMINATE = 5002,
                                        //IGSMREQUEST_INITIALIZE = 5003, deprecated
                                        IGSMREQUEST_DESTROYACCOUNT = 5004,
                                        IGSMREQUEST_ADDIGAPPS = 5005,
                                        IGSMREQUEST_UPLOADIMAGE = 5006,
                                        IGSMREQUEST_DOWNLOADIMAGE = 5007,
                                        IGSMREQUEST_DELETEIMAGE = 5008 };

        public const string IGSMREQUEST_PARAM_NBIGAPPS = "NbIgApps";
        public const string IGSMREQUEST_PARAM_RESET = "Reset";
        public const string IGSMREQUEST_PARAM_LISTPATH = "ListPath";
        public const string IGSMREQUEST_PARAM_LISTSIZE = "ListSize";

        public IGSMRequest(string sName, int nReqId)
            : base(sName, IGREQUEST_SERVER, nReqId)
        {
        }

        public IGSMRequest(string sReqString, XmlDocument xmlDoc)
            : base(sReqString, xmlDoc)
        {
        }

        public static bool IsSMRequest(int nRequestId)
        {
            return ((nRequestId >= (int)IGSMREQUEST_ID.IGSMREQUEST_PING) &&
                    (nRequestId < (int)IGSMAnswer.IGSMANSWER_PONG));
        } 

        public virtual IGAnswer CreateAnswer()
        {
            return new IGSMAnswerActionDone(this);
        }
    }

    [DataContract]
    public class IGSMRequestPing : IGSMRequest
    {
        public const string IGSMREQUESTPING_STRING = "SMPing";
        public IGSMRequestPing() :
            base(IGSMREQUESTPING_STRING, (int)IGSMREQUEST_ID.IGSMREQUEST_PING) { }
        public IGSMRequestPing(XmlDocument xmlDoc)
            : base(IGSMREQUESTPING_STRING, xmlDoc) { }
        public override IGAnswer CreateAnswer()
        {
            return new IGSMAnswerPing();
        }
        public override bool Silent
        {
            get
            {
                return true;
            }
        } 
    }

    [DataContract]
    public class IGSMRequestGetStatus : IGSMRequest
    {
        public const string IGSMREQUESTGETSTATUS_STRING = "SMGetStatus";
        private IGSMStatus.IGSMStatusType m_eType = IGSMStatus.IGSMStatusType.IGSMSTATUS_UNDEFINED;
        private string m_sUser = null;
        public IGSMRequestGetStatus(IGSMStatus.IGSMStatusType eType)
            : base(IGSMREQUESTGETSTATUS_STRING, (int)IGSMREQUEST_ID.IGSMREQUEST_GETSTATUS)
        {
            SetParameter(IGSMStatus.IGSMSTATUS_STATUSTYPE, ((int)eType).ToString());
            m_eType = eType;
            if ((m_eType == IGSMStatus.IGSMStatusType.IGSMSTATUS_USER)
                || (m_eType == IGSMStatus.IGSMStatusType.IGSMSTATUS_ISUSERCONNECTED))
                m_eType = IGSMStatus.IGSMStatusType.IGSMSTATUS_UNDEFINED;  // error       
        }
        public IGSMRequestGetStatus(XmlDocument xmlDoc)
            : base(IGSMREQUESTGETSTATUS_STRING, xmlDoc) {
            m_eType = (IGSMStatus.IGSMStatusType)Convert.ToInt32(GetParameterValue(IGSMStatus.IGSMSTATUS_STATUSTYPE));
        }

        public override IGAnswer CreateAnswer()
        {
            IGSMAnswer status = null;
            switch (m_eType)
            {
                case IGSMStatus.IGSMStatusType.IGSMSTATUS_FULLSTART:                    
                    status = new IGSMFullStatusStart();
                    break;
                case IGSMStatus.IGSMStatusType.IGSMSTATUS_FULLSTOP:
                    status = new IGSMFullStatusStop();
                    break;
                case IGSMStatus.IGSMStatusType.IGSMSTATUS_USER:
                    status = new IGSMStatusUser(m_sUser);
                    break;
                case IGSMStatus.IGSMStatusType.IGSMSTATUS_ISUSERCONNECTED:
                    status = new IGSMStatusUserConnected(m_sUser);
                    break;
                case IGSMStatus.IGSMStatusType.IGSMSTATUS_AVAILABILITY:
                    status = new IGSMStatusAvailability();
                    break;
                default:
                    status = new IGSMAnswerActionDone(this);
                    break;
            }
            return status;
        }   
    }

    [DataContract]
    public class IGSMRequestTerminate : IGSMRequest
    {
        public const string IGSMREQUESTTERMINATE_STRING = "SMTerminate";
        public IGSMRequestTerminate(bool bReset) :
            base(IGSMREQUESTTERMINATE_STRING, (int)IGSMREQUEST_ID.IGSMREQUEST_TERMINATE) {
                m_bReset = bReset;
                SetParameter(IGSMREQUEST_PARAM_RESET, m_bReset ? "true" : "false");
        }
        public IGSMRequestTerminate(XmlDocument xmlDoc)
            : base(IGSMREQUESTTERMINATE_STRING, xmlDoc) {
            m_bReset = (GetParameterValue(IGSMREQUEST_PARAM_RESET).ToLower() == "true");
        }
        public override IGAnswer CreateAnswer()
        {
            if (m_bReset)
            {
                m_serverMgr.ResetAllConnections();
                return new IGSMAnswerInitialized();
            }
            m_serverMgr.DisconnectAllServers(m_sGuid);            
            return new IGSMAnswerTerminate();
        }
        private bool m_bReset;
    }

    [DataContract]
    public class IGSMRequestAddIGApps : IGSMRequest
    {
        public const string IGSMREQUESTADDIGAPPS_STRING = "SMRequestAddIGApps";
        public IGSMRequestAddIGApps(ushort nNbApps) : 
            base(IGSMREQUESTADDIGAPPS_STRING, (int)IGSMREQUEST_ID.IGSMREQUEST_ADDIGAPPS) {
            m_nNbApps = nNbApps;
            SetParameter(IGSMREQUEST_PARAM_NBIGAPPS, m_nNbApps.ToString());
        }
        public IGSMRequestAddIGApps(XmlDocument xmlDoc)
            : base(IGSMREQUESTADDIGAPPS_STRING, xmlDoc) {
            m_nNbApps = Convert.ToUInt16(GetParameterValue(IGSMREQUEST_PARAM_NBIGAPPS));
        }
        public override IGAnswer CreateAnswer()
        {
            while (m_nNbApps-- > 0)
                IGServerManagerLocal.LocalInstance.AddServer();
            return base.CreateAnswer();
        }
        private ushort m_nNbApps;
    }

    [DataContract]
    public class IGSMRequestUser : IGSMRequest
    {
        protected TcpClient m_TCPOutput = null;
        protected TcpClient[] m_tTCPOutput = null;
        public IGSMRequestUser(string sName, int nReqId, string sUserTarget)
            : base(sName, nReqId)
        {
            if (m_serverMgr.IsLocalServer())
            {
                if (sUserTarget.Equals("*"))
                {
                    // get all user connections
                    m_tTCPOutput = new TcpClient[((IGServerManagerLocal)m_serverMgr).GetNbUserConnections()];
                    int nIdxTcpOutput = 0;
                    foreach (IGConnectionLocal connection in ((IGServerManagerLocal)m_serverMgr).GetConnections())
                        if (connection.IsUserConnected())
                            m_tTCPOutput[nIdxTcpOutput++] = connection.m_TCPclient;
                }
                else
                {
                    IGConnection connection = null;
                    int nResult = ((IGServerManagerLocal)m_serverMgr).GetConnection(sUserTarget, out connection);
                    if (nResult == IGSMAnswer.IGSMANSWER_ERROR_NONE)
                        m_TCPOutput = ((IGConnectionLocal)connection).m_TCPclient;
                }
            }
            else
            {
                SetAttribute(IGREQUEST_USERLOGIN, sUserTarget);
            }
        }

        public IGSMRequestUser(string sName, int nReqId, IGConnectionLocal connection)
            : base(sName, nReqId)
        {
            if (connection != null)
                m_TCPOutput = connection.m_TCPclient;
        }

        public IGSMRequestUser(string sReqString, XmlDocument xmlDoc)
            : base(sReqString, xmlDoc) { }

        public override IGAnswer CreateAnswer()
        {
            bool bRet = false;
            if (m_TCPOutput != null)
                bRet = Send(m_TCPOutput);
            else if (m_tTCPOutput != null)
            {
                bRet = true;
                // send request to all user connections
                foreach (TcpClient tcpOuput in m_tTCPOutput)
                    bRet = (bRet && Send(tcpOuput));
            }
            if (bRet)
                return null;
            return new IGSMAnswerError(this, IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_REQUESTPROCESSING);
        }
    }

    [DataContract]
    public class IGSMRequestDestroyAccount : IGSMRequestUser
    {
        public const string IGSMREQUESTDESTROYACCOUNT_STRING = "SMDestroyAccount";
        public IGSMRequestDestroyAccount(string sUser)
            : base(IGSMREQUESTDESTROYACCOUNT_STRING, (int)IGSMREQUEST_ID.IGSMREQUEST_DESTROYACCOUNT, sUser) { }
        public IGSMRequestDestroyAccount(XmlDocument xmlDoc)
            : base(IGSMREQUESTDESTROYACCOUNT_STRING, xmlDoc) { }
        public override bool NeedAccount()
        {
            return true;
        }
    }

    [DataContract]
    public class IGSMRequestDeleteImage : IGSMRequestUser
    {
        private List<string> m_lsImageNames = new List<string>();
        public const string IGSMDELETEIMAGE_STRING = "SMDeleteImage";
        public IGSMRequestDeleteImage(string sUser, string sImageName)
            : base(IGSMDELETEIMAGE_STRING, (int)IGSMREQUEST_ID.IGSMREQUEST_DELETEIMAGE, sUser)
        {
            SetParameter(IGREQUEST_PATH, sImageName);
            splitParamToList(m_lsImageNames, IGREQUEST_PATH, false);
        }
        public IGSMRequestDeleteImage(XmlDocument xmlDoc)
            : base(IGSMDELETEIMAGE_STRING, xmlDoc) {
            splitParamToList(m_lsImageNames, IGREQUEST_PATH, false);
        }

        public override IGAnswer CreateAnswer()
        {
            try
            {
                string sFolder = HC.PATH_USERACCOUNT + GetAttributeValue(IGREQUEST_USERLOGIN) + "/";
                foreach (string sImageName in m_lsImageNames)
                {
                    File.Delete(sFolder + HC.PATH_USERIMAGES + sImageName);
                    File.Delete(sFolder + HC.PATH_USERMINI + HC.PATH_PREFIXMINI + sImageName);
                }
            }
            catch (Exception exc)
            {
                IGServerManager.Instance.AppendError(exc.ToString());
                return new IGSMAnswerError(this, IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_REQUESTPROCESSING);
            }
            return new IGSMAnswerDeleted(this);
        }

        public override void GetParams(IGAnswer answer, HttpSessionState session)
        {
            List<string> lsImageVirtualPath = new List<string>();
            List<string> lsRemoveImageVirtualPath = new List<string>();
            SetParameter(IGAnswer.IGANSWER_IMAGELIBRARY, (string)session[IGAnswer.IGANSWER_IMAGELIBRARY]);
            splitParamToList(lsImageVirtualPath, IGAnswer.IGANSWER_IMAGELIBRARY, false);
            for (int idxVirtualPath = 0; idxVirtualPath < lsImageVirtualPath.Count; idxVirtualPath += 3)
            {
                string sVirtualPath = lsImageVirtualPath[idxVirtualPath];
                string sMiniPrefix = HC.PATH_OUTPUTMINI + HC.PATH_PREFIXMINI;
                string sImageInputPath = sVirtualPath.Substring(sVirtualPath.IndexOf(sMiniPrefix) + sMiniPrefix.Length);
                string sImageName = sImageInputPath.Substring(0, sImageInputPath.IndexOf('$')) + Path.GetExtension(sImageInputPath);
                if (m_lsImageNames.Contains(sImageName))
                    lsRemoveImageVirtualPath.Add(sVirtualPath);
            }
            foreach (string sVirtualPath in lsRemoveImageVirtualPath)
            {
                int idxVirtualPath = lsImageVirtualPath.IndexOf(sVirtualPath);
                lsImageVirtualPath.RemoveAt(idxVirtualPath);
                lsImageVirtualPath.RemoveAt(idxVirtualPath);    // width
                lsImageVirtualPath.RemoveAt(idxVirtualPath);    // height
            }
            session[IGAnswer.IGANSWER_IMAGELIBRARY] = createParamFromList(lsImageVirtualPath);
            session[IGAnswer.IGANSWER_RELOADPAGE] = true;
        }
        public override bool NeedAccount()
        {
            return true;
        }
    }
}
