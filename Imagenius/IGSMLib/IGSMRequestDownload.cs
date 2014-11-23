using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.IO;
using System.Web.SessionState;
using System.Runtime.Serialization;

namespace IGSMLib
{
    [DataContract]
    public class IGSMRequestDownload : IGSMRequestUser
    {
        [DataMember(Name = "PathList", Order = 0)]
        private List<string> m_lsOutputPath = new List<string>();

        public const string IGSMREQUESTDOWNLOAD_STRING = "SMDownload";

        private List<string> m_lsInputImageName = new List<string>();        

        public IGSMRequestDownload(string sUser, string sListPaths)
            : base(IGSMREQUESTDOWNLOAD_STRING, (int)IGSMREQUEST_ID.IGSMREQUEST_DOWNLOADIMAGE, sUser) 
        {
            SetParameter(IGSMREQUEST_PARAM_LISTPATH, sListPaths);
            splitParamToList(m_lsInputImageName, IGSMREQUEST_PARAM_LISTPATH, false);
        }

        public IGSMRequestDownload(XmlDocument xmlDoc)
            : base(IGSMREQUESTDOWNLOAD_STRING, xmlDoc) 
        {
            splitParamToList(m_lsInputImageName, IGSMREQUEST_PARAM_LISTPATH, false);
        }

        public override IGAnswer CreateAnswer()
        {
            IGSMAnswer.IGSMANSWER_ERROR_CODE nErrorCode = IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_REQUESTPROCESSING;
            try
            {
                string login = GetAttributeValue(IGREQUEST_USERLOGIN);
                string reqGuid = GetAttributeValue(IGREQUEST_GUID);                
                foreach (string imageName in m_lsInputImageName)
                {
                    string inputPath = HC.PATH_USERACCOUNT + login + HC.PATH_USERIMAGES + imageName;
                    string outputFolder = HC.PATH_OUTPUT + HC.PATH_OUTPUTDOWNLOADS + login + "/" + reqGuid;
                    string outputImageName = imageName.Replace(HC.PATH_USERIMAGES_BEETLEMORPH + "/", "");
                    string outputPath = outputFolder + "/" + outputImageName;
                    if (!File.Exists(inputPath))
                    {
                        nErrorCode = IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_FILEDONOTEXIST;
                        throw new IOException();
                    }
                    Directory.CreateDirectory(outputFolder);
                    File.Copy(inputPath, outputPath);
                }
            }
            catch (Exception exc)
            {
                IGServerManager.Instance.AppendError(exc.ToString());
                return new IGSMAnswerError(this, nErrorCode);
            }
            SetParameter(IGAnswer.IGANSWER_SERVERIP, m_serverMgr.ServerIP.ToString());
            return new IGSMAnswerActionDone(this);
        }

        public override bool ProcessAnswer(IGAnswer answer)
        {
            SetParameter(IGAnswer.IGANSWER_SERVERIP, answer.GetParameterValue(IGAnswer.IGANSWER_SERVERIP));
            return true;
        }

        public override void GetParams(IGAnswer answer, HttpSessionState session)
        {
            string sLogin = GetAttributeValue(IGREQUEST_USERLOGIN);
            string sServerIP = answer.GetParameterValue(IGAnswer.IGANSWER_SERVERIP);
            string sReqGuid = GetAttributeValue(IGREQUEST_GUID);
            foreach (string sImageName in m_lsInputImageName)
                m_lsOutputPath.Add(HC.PATH_OUTPUTVIRTUAL + sServerIP + "/" + HC.PATH_OUTPUTDOWNLOADS + sLogin + "/" + sReqGuid + "/" + sImageName.Replace(HC.PATH_USERIMAGES_BEETLEMORPH + "/", ""));
            session[IGSMREQUEST_PARAM_LISTPATH] = createParamFromList(m_lsOutputPath);
            session[IGAnswer.IGANSWER_RELOADPAGE] = true;
        }
    }
}
