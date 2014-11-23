using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Web.SessionState;
using IGMadam;
using System.Runtime.Serialization;
using System.IO;

namespace IGSMLib
{
    [DataContract]
    public class IGAnswerConnected : IGAnswerUser
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
        [DataMember(Name = "UserOutputPath", Order = 0)]
        string userOutput;
        [DataMember(Name = "MiniPictureList", Order = 1)]
        List<MiniPic> imLib = new List<MiniPic>();
        [DataMember(Name = "PictureIds", Order = 2)]
        string picIds;
        [DataMember(Name = "PictureNames", Order = 3)]
        string picNames;
        [DataMember(Name = "LastPictureRequestGuids", Order = 4)]
        string reqGuids;
        [DataMember(Name = "SelectedPictureId", Order = 5)]
        string selPicId;
        [DataMember(Name = "SelectedLastPictureRequestGuid", Order = 6)]
        string selReqGuid;
        [DataMember(Name = "CurrentView", Order = 7)]
        string currentView;
        [DataMember(Name = "PictureViewportList", Order = 8)]
        List<IGViewport> viewports = new List<IGViewport>();
        [DataMember(Name = "NbLayers", Order = 9)]
        string nbLayers;
        [DataMember(Name = "LayerVisibility", Order = 10)]
        string layerVisibility;
        [DataMember(Name = "History", Order = 11)]
        string history;
        [DataMember(Name = "WorkspaceProperties", Order = 12)]
        string workspaceProperties;
        [DataMember(Name = "FrameProperties", Order = 13)]
        string frameProperties;
        [DataMember(Name = "UserLogin", Order = 14)]
        string userLogin;

        private IGViewportMap m_currentViewportMap = null;

        public const string IGANSWERCONNECTED_STRING = "connected to server";

        public IGAnswerConnected(IGConnectionLocal connection, XmlDocument xmlDoc)
            : base(connection, xmlDoc, IGANSWERCONNECTED_STRING)
        {
        }

        // for reconnection
        public IGAnswerConnected(HttpSessionState session)
            : base((IGConnectionLocal)null, (int)IGANSWER_ID.IGANSWER_WORKSPACE_CONNECTED, IGANSWERCONNECTED_STRING)
        {
            SetAttribute(IGANSWER_USERLOGIN, (string)session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME]);
            SetAttribute(IGANSWER_REQGUID, (string)session[IGANSWER_REQGUID]);
            SetParameter(IGANSWER_USEROUTPUT, (string)session[IGANSWER_USEROUTPUT]);
            SetParameter(IGANSWER_IMAGELIBRARY, (string)session[IGANSWER_IMAGELIBRARY]);
            SetParameter(IGANSWER_RESTOREDFRAMEIDS, (string)session[IGANSWER_RESTOREDFRAMEIDS]);
            SetParameter(IGANSWER_SELECTEDFRAMEID, (string)session[IGANSWER_SELECTEDFRAMEID]);
            SetParameter(IGANSWER_FRAMENAMES, (string)session[IGANSWER_FRAMENAMES]);
            SetParameter(IGANSWER_REQGUIDS, (string)session[IGANSWER_REQGUIDS]);
            SetParameter(IGANSWER_SELECTEDREQGUID, (string)session[IGANSWER_SELECTEDREQGUID]);
            SetParameter(IGANSWER_ALLNBLAYERS, (string)session[IGANSWER_ALLNBLAYERS]);
            SetParameter(IGANSWER_ALLLAYERVISIBILITY, (string)session[IGANSWER_ALLLAYERVISIBILITY]);
            SetParameter(IGANSWER_ALLSTEPIDS, (string)session[IGANSWER_ALLSTEPIDS]);
            SetParameter(IGANSWER_WORKSPACEPROPERTIES, (string)session[IGANSWER_WORKSPACEPROPERTIES]);
            SetParameter(IGANSWER_FRAMEPROPERTIES, (string)session[IGANSWER_FRAMEPROPERTIES]);
            m_currentViewportMap = (IGViewportMap)session[IGANSWER_CURRENTVIEWPORTMAP];
        }

        protected override bool InternalProcess()
        {
            XmlNode xmlNodeRestoredFrames = GetParameter(IGANSWER_RESTOREDFRAMEIDS);
            if (xmlNodeRestoredFrames == null)
                m_connection.OnUserConnected("");
            else
                m_connection.OnUserConnected(xmlNodeRestoredFrames.Value);
            return true;
        }

        public override void GetParams()
        {
            base.GetParams();
            userOutput = GetParameterValue(IGANSWER_USEROUTPUT);
            string login = GetAttributeValue(IGRequest.IGREQUEST_USERLOGIN);
            string serverIP = GetParameterValue(IGANSWER_SERVERIP);
            //splitParamToList(lsParams, IGANSWER_IMAGELIBRARY);
            //splitParamToList(lsParams, IGANSWER_RESTOREDFRAMEIDS);
            //splitParamToList(lsParams, IGANSWER_FRAMENAMES);
            //splitParamToList(lsParams, IGANSWER_REQGUIDS);
            List<string> miniPics = new List<string>();
            splitParamToList(miniPics, IGANSWER_IMAGELIBRARY, false);
            for (int idxImageInputPath = 0; idxImageInputPath < miniPics.Count; idxImageInputPath++)
            {
                MiniPic miniPic = new MiniPic();
                string imageInputPath = miniPics[idxImageInputPath];
                int startIdx = imageInputPath.IndexOf(HC.PATH_OUTPUTMINI + HC.PATH_PREFIXMINI) + HC.PATH_OUTPUTMINI.Length + HC.PATH_PREFIXMINI.Length;
                string imageName = imageInputPath.Substring(startIdx, miniPics[idxImageInputPath].IndexOf('$') - startIdx);
                imageName += Path.GetExtension(imageInputPath);
                miniPic.Path = miniPics[idxImageInputPath];
                miniPic.Name = imageName;
                miniPic.Width = int.Parse(miniPics[++idxImageInputPath]); // width
                miniPic.Height = int.Parse(miniPics[++idxImageInputPath]); // height
                imLib.Add(miniPic);
            }
            picIds = GetParameterValue(IGANSWER_RESTOREDFRAMEIDS);
            if (picIds == null)
                picIds = "";
            picNames = GetParameterValue(IGANSWER_FRAMENAMES);
            if (picNames == null)
                picNames = "";
            reqGuids = GetParameterValue(IGANSWER_REQGUIDS);
            if (reqGuids == null)
                reqGuids = "";

            selPicId = GetParameterValue(IGANSWER_SELECTEDFRAMEID);
            selReqGuid = GetParameterValue(IGANSWER_SELECTEDREQGUID);
            currentView = GetParameterValue(IGANSWER_CURRENTVIEW);
            currentView = (currentView == null ? "Workspace" : currentView);
            if (m_currentViewportMap != null)
            {
                List<string> lsParamIds = new List<string>();
                splitParamToList(lsParamIds, IGANSWER_RESTOREDFRAMEIDS);
                for (int idxPicture = 1; idxPicture < lsParamIds.Count; idxPicture++)
                {
                    IGViewport viewport = m_currentViewportMap.GetViewport(lsParamIds[idxPicture]);
                    if (viewport == null)
                        break;
                    viewport.GetParams();
                    viewports.Add(viewport);
                }
            }
            nbLayers = GetParameterValue(IGANSWER_ALLNBLAYERS);
            layerVisibility = GetParameterValue(IGANSWER_ALLLAYERVISIBILITY);
            history = GetParameterValue(IGANSWER_ALLSTEPIDS);
            workspaceProperties = GetParameterValue(IGANSWER_WORKSPACEPROPERTIES);
            frameProperties = GetParameterValue(IGANSWER_FRAMEPROPERTIES);
            userLogin = GetAttributeValue(IGANSWER_USERLOGIN);
        }

        public override void GetParams(HttpSessionState session)
        {
            List<string> lsFrameIds = new List<string>();
            splitParamToList(lsFrameIds, IGANSWER_RESTOREDFRAMEIDS);
            int nNbPictures = lsFrameIds.Count - 1;
            if (nNbPictures > 0)
            {
                string sSelFrameId = GetParameterValue(IGANSWER_SELECTEDFRAMEID);
                if ((sSelFrameId == null) || (sSelFrameId == ""))
                    SetParameter(IGANSWER_SELECTEDFRAMEID, lsFrameIds[1]);
                string sFrameNames = GetParameterValue(IGANSWER_FRAMENAMES);
                string sFrameGuids = "";
                string sStepIds = "";
                if ((sFrameNames == null) || (sFrameNames == ""))
                {
                    sFrameNames = "";
                    int idxFrame = 1;
                    for (; idxFrame < nNbPictures; idxFrame++)
                    {
                        sFrameNames += "Recovered(" + idxFrame.ToString() + ").ig,";
                        sFrameGuids += GetReqGuid() + ",";
                        sStepIds += "1,";
                    }
                    sFrameNames += "Recovered(" + idxFrame.ToString() + ").ig";
                    sFrameGuids += GetReqGuid();
                    sStepIds += "1";
                    SetParameter(IGANSWER_FRAMENAMES, sFrameNames);
                    SetParameter(IGANSWER_REQGUIDS, sFrameGuids);
                    SetParameter(IGANSWER_SELECTEDREQGUID, GetReqGuid());
                    SetParameter(IGANSWER_ALLNBLAYERS, GetParameterValue(IGANSWER_ALLNBLAYERS));
                    SetParameter(IGANSWER_ALLSTEPIDS, sStepIds);                    
                }
                if (session[IGANSWER_ALLNBLAYERS] != null)
                    SetParameter(IGANSWER_ALLNBLAYERS, (string)session[IGANSWER_ALLNBLAYERS]);
                if (session[IGANSWER_ALLSTEPIDS] != null)
                    SetParameter(IGANSWER_ALLSTEPIDS, (string)session[IGANSWER_ALLSTEPIDS]);
                List<string> lsNbLayers = new List<string>();
                splitParamToList(lsNbLayers, IGANSWER_ALLNBLAYERS);
                lsNbLayers.RemoveAt(0);
                string sAllLayerVisibility = "";
                for (int idxFrame = 0; idxFrame < nNbPictures; idxFrame++)
                {
                    int nbLayers = int.Parse(lsNbLayers[idxFrame]);
                    for (int idxLayer = 0; idxLayer < nbLayers; idxLayer++)
                    {
                        sAllLayerVisibility += "1";
                        if (idxLayer < nbLayers - 1)
                            sAllLayerVisibility += "_";
                    }
                    if (idxFrame < nNbPictures - 1)
                        sAllLayerVisibility += ",";
                }
                SetParameter(IGANSWER_ALLLAYERVISIBILITY, sAllLayerVisibility);
            }
            if (session[IGANSWER_CURRENTVIEW] != null)
                SetParameter(IGANSWER_CURRENTVIEW, (string)session[IGANSWER_CURRENTVIEW]);
            if (session[IGANSWER_CURRENTVIEWPORTMAP] != null)
                m_currentViewportMap = (IGViewportMap)session[IGANSWER_CURRENTVIEWPORTMAP];
            else
            {
                m_currentViewportMap = new IGViewportMap();
                session[IGANSWER_CURRENTVIEWPORTMAP] = m_currentViewportMap;
            }
            string sImageLibrary = GetParameterValue(IGANSWER_IMAGELIBRARY);            
            sImageLibrary = sImageLibrary.Replace(@"\\", @"/");
            sImageLibrary = sImageLibrary.Replace(@"\", @"/");            
            session[IGANSWER_REQGUID] = GetReqGuid();
            session[IGANSWER_USEROUTPUT] = GetParameterValue(IGANSWER_USEROUTPUT);
            session[IGANSWER_IMAGELIBRARY] = sImageLibrary;
            session[IGANSWER_RESTOREDFRAMEIDS] = GetParameterValue(IGANSWER_RESTOREDFRAMEIDS);
            session[IGANSWER_SELECTEDFRAMEID] = GetParameterValue(IGANSWER_SELECTEDFRAMEID);
            session[IGANSWER_FRAMENAMES] = GetParameterValue(IGANSWER_FRAMENAMES);
            session[IGANSWER_REQGUIDS] = GetParameterValue(IGANSWER_REQGUIDS);
            session[IGANSWER_SELECTEDREQGUID] = GetParameterValue(IGANSWER_SELECTEDREQGUID);
            session[IGANSWER_ALLNBLAYERS] = GetParameterValue(IGANSWER_ALLNBLAYERS);
            session[IGANSWER_ALLLAYERVISIBILITY] = GetParameterValue(IGANSWER_ALLLAYERVISIBILITY);
            session[IGANSWER_ALLSTEPIDS] = GetParameterValue(IGANSWER_ALLSTEPIDS);
            // Workspace properties
            session[IGANSWER_WORKSPACEPROPERTIES] = GetParameterValue(IGANSWER_WORKSPACEPROPERTIES);
            // Frame properties
            session[IGANSWER_FRAMEPROPERTIES] = GetParameterValue(IGANSWER_FRAMEPROPERTIES);
        }
    }
}
