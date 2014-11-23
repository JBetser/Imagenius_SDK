using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using System.Drawing.Imaging;
using System.Timers;
using System.IO;
using System.Web.SessionState;
using System.Runtime.Serialization;

namespace IGSMLib
{
    [DataContract]
    public class IGSMRequestUpload : IGSMRequestUser
    {
        [DataContract]
        public struct MiniPic
        {
            [DataMember(Name = "Path", Order = 0)]
            public string Path;
            [DataMember(Name = "Name", Order = 1)]
            public string Name;
            [DataMember(Name = "Width", Order = 2)]
            public int Width;
            [DataMember(Name = "Height", Order = 3)]
            public int Height; 
        }

        [DataMember(Name = "MiniPictureList", Order = 0)]
        List<MiniPic> miniPics = new List<MiniPic>();

        public const string IGSMREQUESTUPLOAD_STRING = "SMUpload";
        public Timer m_timerDeletor = new Timer(20000);

        private List<string> m_lsInputPath = new List<string>();
        private List<KeyValuePair<int, int>> m_lsImageSize = new List<KeyValuePair<int, int>>();

        public IGSMRequestUpload(string sUser, string sListPaths)
            : base(IGSMREQUESTUPLOAD_STRING, (int)IGSMREQUEST_ID.IGSMREQUEST_UPLOADIMAGE, sUser)
        {
            SetParameter(IGSMREQUEST_PARAM_LISTPATH, sListPaths);
            splitParamToList(m_lsInputPath, IGSMREQUEST_PARAM_LISTPATH, false);
        }

        public IGSMRequestUpload(XmlDocument xmlDoc)
            : base(IGSMREQUESTUPLOAD_STRING, xmlDoc)
        {
            splitParamToList(m_lsInputPath, IGSMREQUEST_PARAM_LISTPATH, false);
            m_timerDeletor.Elapsed += new ElapsedEventHandler(onDeleteInputFiles);
        }

        void onDeleteInputFiles(object sender, ElapsedEventArgs e)
        {
            m_timerDeletor.Stop();
            try
            {
                foreach (string sInputPath in m_lsInputPath)
                    File.Delete(sInputPath);
            }
            catch (Exception exc)
            {
                IGServerManager.Instance.AppendError(exc.ToString());
            }
        }

        public override IGAnswer CreateAnswer()
        {
            IGSMAnswer.IGSMANSWER_ERROR_CODE nErrorCode = IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_REQUESTPROCESSING;
            string firstImageName = "";
            try
            {
                string sLogin = GetAttributeValue(IGREQUEST_USERLOGIN);
                foreach (string sImageInputPath in m_lsInputPath)
                {
                    if ((Path.GetExtension(sImageInputPath).ToUpper() != ".JPG") &&
                        (Path.GetExtension(sImageInputPath).ToUpper() != ".PNG") &&
                        (Path.GetExtension(sImageInputPath).ToUpper() != ".BMP"))
                    {
                        nErrorCode = IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_WRONGFILEFORMAT;
                        throw new FormatException();
                    }
                    else if (sImageInputPath.Contains(","))
                    {
                        nErrorCode = IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_INVALIDFILENAME;
                        throw new FormatException();
                    }
                    string sImageName = sImageInputPath.Substring(sImageInputPath.IndexOf('$') + 1);
                    string sImageNameJPEG = sImageName.Remove(sImageName.Length - Path.GetExtension(sImageName).Length) + "$" + GetAttributeValue(IGREQUEST_GUID) + ".JPG";
                    Image imgInput = Image.FromFile(sImageInputPath);
                    float fRate = HC.MINIPICTURE_MAXSIZE / (float)Math.Max(imgInput.Size.Width, imgInput.Size.Height);
                    Image imgMini = new Bitmap(imgInput, new Size((int)((float)imgInput.Size.Width * fRate), (int)((float)imgInput.Size.Height * fRate)));
                    imgMini.Save(HC.PATH_USERACCOUNT + sLogin + HC.PATH_USERMINI + HC.PATH_PREFIXMINI + sImageName, ImageFormat.Jpeg);
                    imgMini.Save(HC.PATH_OUTPUT + sLogin + HC.PATH_OUTPUTMINI + HC.PATH_PREFIXMINI + sImageNameJPEG, ImageFormat.Jpeg);
                    string sDestPath = HC.PATH_USERACCOUNT + sLogin + HC.PATH_USERIMAGES + sImageName;
                    if (firstImageName == "")
                        firstImageName = sImageName;
                    m_lsImageSize.Add(new KeyValuePair<int, int>(imgMini.Width, imgMini.Height));
                    if (File.Exists(sDestPath))
                    {
                        nErrorCode = IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_FILEALREADYEXISTS;
                        throw new IOException();
                    }                    
                    File.Copy(sImageInputPath, sDestPath);                    
                }
                m_timerDeletor.Start();
            }
            catch (Exception exc)
            {
                IGServerManager.Instance.AppendError(exc.ToString());
                m_timerDeletor.Start();
                if (nErrorCode != IGSMAnswer.IGSMANSWER_ERROR_CODE.IGSMANSWER_ERROR_FILEALREADYEXISTS)
                    return new IGSMAnswerError(this, nErrorCode);
            }
            SetParameter(IGAnswer.IGANSWER_SERVERIP, m_serverMgr.ServerIP.ToString());
            SetParameter(IGSMREQUEST_PARAM_LISTSIZE, createParamFromList(m_lsImageSize));
            return new IGSMAnswerUploadSucceeded(this, firstImageName);
        }

        public override bool ProcessAnswer(IGAnswer answer)
        {
            SetParameter(IGAnswer.IGANSWER_SERVERIP, answer.GetParameterValue(IGAnswer.IGANSWER_SERVERIP));
            return true;
        }

        public override void GetParams()
        {
            base.GetParams();
            string login = GetAttributeValue(IGREQUEST_USERLOGIN);
            string serverIP = GetParameterValue(IGAnswer.IGANSWER_SERVERIP);
            for (int idxImageInputPath = 0; idxImageInputPath < m_lsInputPath.Count; idxImageInputPath++)
            {
                MiniPic miniPic = new MiniPic();
                string imageInputPath = m_lsInputPath[idxImageInputPath];
                string imageName = imageInputPath.Substring(m_lsInputPath[idxImageInputPath].IndexOf('$') + 1);
                string imageNameJPEG = imageName.Remove(imageName.Length - Path.GetExtension(imageName).Length) + "$" + GetAttributeValue(IGREQUEST_GUID) + ".JPG";
                miniPic.Path = HC.PATH_OUTPUTVIRTUAL + serverIP + "/" + login + HC.PATH_OUTPUTMINI + HC.PATH_PREFIXMINI + imageNameJPEG;
                miniPic.Name = imageName;
                miniPic.Width = m_lsImageSize[idxImageInputPath].Key; // width
                miniPic.Height = m_lsImageSize[idxImageInputPath].Value; // height
                miniPics.Add(miniPic);
            }
        }

        public override void GetParams(IGAnswer answer, HttpSessionState session)
        {
            if ((string)session[IGAnswer.IGANSWER_IMAGELIBRARY] == null)
                session[IGAnswer.IGANSWER_IMAGELIBRARY] = "";
            string sLogin = GetAttributeValue(IGREQUEST_USERLOGIN);
            string sServerIP = GetParameterValue(IGAnswer.IGANSWER_SERVERIP);
            SetParameter(IGSMREQUEST_PARAM_LISTSIZE, answer.GetParameterValue(IGSMREQUEST_PARAM_LISTSIZE));
            splitParamToList(m_lsImageSize, IGSMREQUEST_PARAM_LISTSIZE);
            for (int idxImageInputPath = 0; idxImageInputPath < m_lsInputPath.Count; idxImageInputPath++)
            {
                string sImageInputPath = m_lsInputPath[idxImageInputPath];
                if ((string)session[IGAnswer.IGANSWER_IMAGELIBRARY] != "")
                    session[IGAnswer.IGANSWER_IMAGELIBRARY] += ",";
                string sImageName = sImageInputPath.Substring(sImageInputPath.IndexOf('$') + 1);
                string sImageNameJPEG = sImageName.Remove(sImageName.Length - Path.GetExtension(sImageName).Length) + "$" + GetAttributeValue(IGREQUEST_GUID) + ".JPG";
                session[IGAnswer.IGANSWER_IMAGELIBRARY] += HC.PATH_OUTPUTVIRTUAL + sServerIP + "/" + sLogin + HC.PATH_OUTPUTMINI + HC.PATH_PREFIXMINI + sImageNameJPEG;
                session[IGAnswer.IGANSWER_IMAGELIBRARY] += "," + m_lsImageSize[idxImageInputPath].Key.ToString(); // width
                session[IGAnswer.IGANSWER_IMAGELIBRARY] += "," + m_lsImageSize[idxImageInputPath].Value.ToString(); // height
            }
        }
        public override bool NeedAccount()
        {
            return true;
        }
    }
}
