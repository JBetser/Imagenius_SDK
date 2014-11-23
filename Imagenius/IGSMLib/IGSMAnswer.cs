using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Net;
using System.Net.Sockets;
using System.Web.SessionState;
using System.Configuration;
using System.IO;
using System.Runtime.Serialization;

namespace IGSMLib
{
    [DataContract]
    public class IGSMAnswer : IGAnswer
    {
        public const string ANSWER_XMLTEMPLATE = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Answer Id=\"0\" ReqGuid=\"\" ReqName=\"\"><Params/></Answer>";
        public const string IGSMANSWER_ERRORCODE = "ErrorCode";
        public const string IGSMANSWER_ERRORMESSAGE = "ErrorMessage";
        
        public const int IGSMANSWER_PONG = 6000;
        public const int IGSMANSWER_GETSTATUS = 6001;
        public const int IGSMANSWER_TERMINATED = 6002;
        public const int IGSMANSWER_INITIALIZED = 6003;
        public const int IGSMANSWER_ACTIONDONE = 6004;
        public const int IGSMANSWER_UPLOADED = 6005;
        public const int IGSMANSWER_DELETED = 6006;

        // error status
        public const int IGSMANSWER_ERROR_NONE = 6665;
        public const int IGSMANSWER_ERROR = 6666;
        public const int IGSMANSWER_SUCCESS = IGSMANSWER_ERROR_NONE;

        // error codes
        public enum IGSMANSWER_ERROR_CODE {
            IGSMANSWER_ERROR_USERNOTFOUND = 6700,
            IGSMANSWER_ERROR_TOOMANYUSERS = 6701,
            IGSMANSWER_ERROR_WRONGUSERNAME = 6702,
            IGSMANSWER_ERROR_USERALREADYCONNECTED = 6703,
            IGSMANSWER_ERROR_REQUESTPROCESSING = 6704,
            IGSMANSWER_ERROR_USERCONNECTING = 6705,
            IGSMANSWER_ERROR_USERDISCONNECTING = 6706,
            IGSMANSWER_ERROR_SERVERBUSY = 6707,
            IGSMANSWER_ERROR_FRAMEBUSY = 6708,
            IGSMANSWER_ERROR_SERVERNOTFOUND = 6709,
            IGSMANSWER_ERROR_SENDREQUEST = 6710,
            IGSMANSWER_ERROR_CORRUPTEDREQUEST = 6711,
            IGSMANSWER_ERROR_SERVERDISCONNECTED = 6712,
            IGSMANSWER_ERROR_ANSWEREXECUTION = 6713,
            IGSMANSWER_ERROR_WRONGFILEFORMAT = 6714,
            IGSMANSWER_ERROR_FILEALREADYEXISTS = 6715,
            IGSMANSWER_ERROR_INVALIDFILENAME = 6716,
            IGSMANSWER_ERROR_FILEDONOTEXIST = 6717,
            IGSMANSWER_ERROR_NONETWORKCONNECTION = 6718,
            IGSMANSWER_ERROR_TIMEOUT = 6719
        }

        public IGSMAnswer(int nId, string sAnswerString)
            : base(ANSWER_XMLTEMPLATE, nId, sAnswerString)
        {
            if (IsSMError(nId))
                SetAttribute(IGSMANSWER_ERRORMESSAGE, ((IGSMANSWER_ERROR_CODE)nId).ToString() + " - " + sAnswerString);
        }

        public IGSMAnswer(XmlDocument xmlDoc, string sAnswerString)
            : base(xmlDoc, sAnswerString)
        {
        }

        public IGSMAnswer(string sXMLTemplate, string sAnswerString)
            : base(sXMLTemplate, sAnswerString)
        {
        }

        public static bool IsSMAnswer(int nAnswerId)
        {
            return ((nAnswerId >= IGSMANSWER_PONG) && 
                    (nAnswerId < 7000));
        }

        public static bool IsSMError(int nAnswerId)
        {
            return ((nAnswerId >= IGSMAnswer.IGSMANSWER_ERROR) &&
                    (nAnswerId < 7000));
        }

        public override string ToString()
        {
            string sIP = GetParameterValue(IGANSWER_SERVERIP);
            if (sIP == null)
                sIP = "No IP";
            return "<" + sIP + ">: " + base.ToString();
        }
    }

    [DataContract]
    public class IGSMAnswerPing : IGSMAnswer
    {
        public const string IGSMANSWERPING_STRING = "SMAnswer Pong";
        public IGSMAnswerPing() : base(IGSMANSWER_PONG, IGSMANSWERPING_STRING) { }
        public IGSMAnswerPing(XmlDocument xmlDoc)
            : base(xmlDoc, IGSMANSWERPING_STRING) { }
        public override string ToString() {
            m_sString = "pong";
            return base.ToString();
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
    public class IGSMAnswerTerminate : IGSMAnswer
    {
        public const string IGSMANSWERTERMINATE_STRING = "SMAnswer Terminated";
        public IGSMAnswerTerminate() : base(IGSMANSWER_TERMINATED, IGSMANSWERTERMINATE_STRING) {}
        public IGSMAnswerTerminate(XmlDocument xmlDoc)
            : base(xmlDoc, IGSMANSWERTERMINATE_STRING) { }
        public override string ToString(){
            m_sString = "terminated";
            return base.ToString();
        }
    }

    [DataContract]
    public class IGSMAnswerInitialized : IGSMAnswer
    {
        public const string IGSMANSWERINITIALIZE_STRING = "SMAnswer Initialized";
        public IGSMAnswerInitialized()
            : base(IGSMANSWER_INITIALIZED, IGSMANSWERINITIALIZE_STRING) {}
        public IGSMAnswerInitialized(XmlDocument xmlDoc)
            : base(xmlDoc, IGSMANSWERINITIALIZE_STRING) { }
        public override string ToString(){
            m_sString = "initialized";
            return base.ToString();
        }
        protected override bool InternalProcess() {
            return true;
        }
    }

    [DataContract]
    public class IGSMAnswerError : IGSMAnswer
    {
        [DataMember(Name = "RequestName", Order = 0)]
        string reqName;
        [DataMember(Name = "Reason", Order = 1)]
        string reason;
        public const string IGSMANSWERERROR_STRING = "SMAnswer Error";
        public const string IGSMANSWERERROR_CODEPARAM = "ErrorCode";
        public const string IGSMANSWERERROR_STRINGPARAM = "ErrorString";
        IGRequest m_request = null;       

        public IGSMAnswerError(IGRequest request, IGSMANSWER_ERROR_CODE errorCode)
            : base(IGSMANSWER_ERROR, IGSMANSWERERROR_STRING){
            string sGuid = (request == null ? "##Undefined##" : (request.GetGuid() == null ? "##Undefined##" : request.GetGuid()));
            string sName = (request == null ? "##Undefined##" : (request.GetTitle() == null ? "##Undefined##" : request.GetTitle()));
            string sUser = (request == null ? null : request.GetAttributeValue(IGRequest.IGREQUEST_USERLOGIN));
            SetAttribute(IGANSWER_REQGUID, sGuid);
            SetAttribute(IGANSWER_REQNAME, sName);
            if (sUser != null)
                SetAttribute(IGANSWER_USERLOGIN, sUser);
            SetParameter(IGSMANSWERERROR_CODEPARAM, ((int)errorCode).ToString());
            SetParameter(IGSMANSWERERROR_STRINGPARAM, errorCode.ToString());
        }
        public IGSMAnswerError(string sError) : base(IGSMANSWER_ERROR, IGSMANSWERERROR_STRING + ": " + sError) {
            SetParameter(IGSMANSWERERROR_STRINGPARAM, sError);
        }
        public IGSMAnswerError(XmlDocument xmlDoc)
            : base(xmlDoc, IGSMANSWERERROR_STRING) { }
        public string GetReason()
        {
            string sError = null;
            string sCode = GetParameterValue(IGSMANSWERERROR_CODEPARAM);
            int nCode = ((sCode == null) || (sCode == "")) ? -1 : Convert.ToInt32(sCode);
            switch ((IGSMANSWER_ERROR_CODE)nCode)
            {
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERNOTFOUND:
                    sError = "User cannot be found.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_TOOMANYUSERS:
                    sError = "Maximum number of users reached.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_WRONGUSERNAME:
                    sError = "User name invalid.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERALREADYCONNECTED:
                    sError = "User already connected.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_REQUESTPROCESSING:
                    sError = "Request processing failed.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERCONNECTING:
                    sError = "User is connecting to server.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERDISCONNECTING:
                    sError = "User is disconnecting from server.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_SERVERBUSY:
                    sError = "Server is busy.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_FRAMEBUSY:
                    sError = "Frame is busy.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_SERVERNOTFOUND:
                    sError = "Server cannot be found.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_SENDREQUEST:
                    sError = "Failed sending request.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_CORRUPTEDREQUEST:
                    sError = "Corrupted request.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_SERVERDISCONNECTED:
                    sError = "Connection closed by distant server.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_ANSWEREXECUTION:
                    sError = "Failed executing answer.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_WRONGFILEFORMAT:
                    sError = "The picture format is not supported.\nOnly JPG, BMP and PNG can be used.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_FILEALREADYEXISTS:
                    sError = "The file already exists.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_INVALIDFILENAME:
                    sError = "The file name contains a forbidden caracter.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_FILEDONOTEXIST:
                    sError = "The file does not exist.";
                    break;
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_NONETWORKCONNECTION:
                    sError = "No network connection available.";
                    break;
                default:
                    sError = "Unknown error";
                    break;
            }
            return sError;
        }
        public override string ToString(){
            string sError = GetParameterValue(IGSMANSWERERROR_STRINGPARAM);
            if ((sError != null) && (sError != ""))
                return base.ToString() + sError;
            sError = GetReason();
            string sReq = GetAttributeValue(IGAnswer.IGANSWER_REQNAME);
            if (sReq != null)
                sError += "\n";
            if (m_request != null)
                sError += "REQUEST { " + m_request.ToString() + "}\n";
            m_sString = sError;
            return base.ToString();
        }
        public override bool IsError()
        {
            return true;
        }
        protected override bool InternalProcess()
        {
            m_serverMgr.AppendError("Request processing failed: " + ToString());
            XmlNode xmlReqGuid = GetAttribute(IGANSWER_REQGUID);
            if (xmlReqGuid == null)
                return true;
            IGRequestProcessing reqProc = IGRequestProcessing.GetRequest(xmlReqGuid.Value);
            if (reqProc == null)
                return true;
            string sCode = GetParameterValue(IGSMANSWERERROR_CODEPARAM);
            int nCode = ((sCode == null) || (sCode == "")) ? -1 : Convert.ToInt32(sCode);
            switch ((IGSMANSWER_ERROR_CODE)nCode)
            {
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_USERNOTFOUND:
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_TOOMANYUSERS:
                case IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_WRONGUSERNAME:
                    string sUser = reqProc.GetUser();
                    if (sUser != null)
                        ((IGServerManagerRemote)m_serverMgr).DisconnectUser(sUser, false);
                    break;
            }
            return true;
        }
        public override void GetParams()
        {
            base.GetParams();
            reqName = GetAttributeValue(IGANSWER_REQNAME);
            reason = GetReason();
        }
    }

    [DataContract]    
    public class IGSMAnswerActionDone : IGSMAnswer
    {  
        public const string IGSMANSWERSUCCESS_STRING = "SMAnswer Success";

        public IGSMAnswerActionDone(IGSMRequest request) : base(IGSMANSWER_ACTIONDONE, IGSMANSWERSUCCESS_STRING) {
            init(request);            
        }
        public IGSMAnswerActionDone(IGSMRequest request, int code)
            : base(code, IGSMANSWERSUCCESS_STRING)
        {
            init(request);
        }
        public IGSMAnswerActionDone(XmlDocument xmlAnswerDoc)
            : base(xmlAnswerDoc, IGSMANSWERSUCCESS_STRING) {
        }
        private void init(IGRequest request)
        {
            SetAttribute(IGANSWER_REQGUID, request.GetGuid());
            SetAttribute(IGANSWER_REQNAME, request.GetTitle());
            string sUser = request.GetAttributeValue(IGRequest.IGREQUEST_USERLOGIN);
            if (sUser != null)
                SetAttribute(IGANSWER_USERLOGIN, sUser);
            XmlNode xmlReqParams = request.GetParameters();
            if (xmlReqParams != null)
            {
                if (xmlReqParams.Attributes.Count > 0)
                {
                    foreach (XmlAttribute att in xmlReqParams.Attributes)
                        SetParameter(att.Name, att.Value);
                }
            }
        }
        public override string ToString() {
            string sDone = "done";
            string sReq = GetAttributeValue(IGAnswer.IGANSWER_REQNAME);
            string sReqGuid = GetAttributeValue(IGAnswer.IGANSWER_REQGUID);
            if ((sReq != null) && (sReqGuid != null))
                sDone += " on request " + sReq + " GUID{ " + sReqGuid + " }";
            m_sString = sDone;
            return base.ToString();
        }
        public override void GetParams(HttpSessionState session)
        {
            IGRequestProcessing reqProc = IGRequestProcessing.GetRequest(GetAttributeValue(IGAnswer.IGANSWER_REQGUID));
            if (reqProc != null)
                reqProc.GetRequest().GetParams(this, session);
        }
    }

    [DataContract]
    public class IGSMAnswerUploadSucceeded : IGSMAnswerActionDone
    {
        [DataMember(Name = "PictureName", Order = 0)]
        string picName;

        public IGSMAnswerUploadSucceeded(IGSMRequest request, string fileName)
            : base(request, IGSMANSWER_UPLOADED)
        {
            SetParameter(IGANSWER_PICTURENAME, fileName);
        }
        public IGSMAnswerUploadSucceeded(XmlDocument xmlAnswerDoc)
            : base(xmlAnswerDoc) {
        }
        public override void GetParams()
        {
            base.GetParams();
            picName = GetParameterValue(IGANSWER_PICTURENAME);
        }
    }

    [DataContract]
    public class IGSMAnswerDeleted : IGSMAnswerActionDone
    {
        [DataMember(Name = "PictureName", Order = 0)]
        string picName;

        public IGSMAnswerDeleted(IGSMRequest request)
            : base(request, IGSMANSWER_DELETED)
        {
            SetParameter(IGANSWER_PICTURENAME, request.GetParameterValue(IGRequest.IGREQUEST_PATH));
        }
        public IGSMAnswerDeleted(XmlDocument xmlAnswerDoc)
            : base(xmlAnswerDoc)
        {
        }
        public override void GetParams()
        {
            base.GetParams();
            picName = GetParameterValue(IGANSWER_PICTURENAME);
        }
    }
}
