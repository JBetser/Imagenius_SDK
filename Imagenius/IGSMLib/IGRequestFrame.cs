using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Web.SessionState;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Text.RegularExpressions;
using System.Runtime.Serialization;

namespace IGSMLib
{
    [DataContract]
    public class IGRequestFrame : IGRequest
    {
        public IGRequestFrame(string sUser, string sName, int nReqId, string sFrameId)
            : base(sName, IGREQUEST_FRAME, nReqId)
        {
            SetParameter(IGREQUEST_USERLOGIN, sUser);
            SetParameter(IGREQUEST_FRAMEID, sFrameId);
        }
        public IGRequestFrame(string sName, XmlDocument xmlDoc)
            : base(sName, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameLayer : IGRequestFrame
    {
        public IGRequestFrameLayer(string sUser, string sReqName, int nReqId, string sFrameId, string sLayerId)
            : base(sUser, sReqName, nReqId, sFrameId)
        {
            SetParameter(IGREQUEST_LAYERID, sLayerId);
        }
        public IGRequestFrameLayer(string sName, XmlDocument xmlDoc)
            : base(sName, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameSave : IGRequestFrame
    {
        [DataMember(Name = "MiniPictureName", Order = 0)]
        string miniPicName;
        [DataMember(Name = "MiniPicturePath", Order = 1)]
        string miniPicPath;
        [DataMember(Name = "MiniPictureWidth", Order = 2)]
        string miniPicWidth;
        [DataMember(Name = "MiniPictureHeight", Order = 3)]
        string miniPicHeight;

        public const string IGREQUESTFRAMESAVE_STRING = "Save image";
        private KeyValuePair<string, string> m_pairSize = new KeyValuePair<string,string>();
        public IGRequestFrameSave(string sUser, string sFrameId, string sPath)
            : base(sUser, IGREQUESTFRAMESAVE_STRING, IGREQUEST_FRAME_SAVE, sFrameId)
        {
            if ((Path.GetExtension(sPath).ToUpper() != ".JPG") &&
                (Path.GetExtension(sPath).ToUpper() != ".PNG") &&
                (Path.GetExtension(sPath).ToUpper() != ".BMP") &&
                (Path.GetExtension(sPath).ToUpper() != ".IG"))
                sPath += ".JPG";
            SetParameter(IGREQUEST_PATH, sPath);
        }
        public IGRequestFrameSave(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMESAVE_STRING, xmlDoc) { }
        public override bool ProcessAnswer(IGAnswer answer)
        {            
            string sLogin = answer.GetAttributeValue(IGREQUEST_USERLOGIN);
            string sFrameId = answer.GetParameterValue(IGREQUEST_FRAMEID);
            string sImageOutputPath = answer.GetParameterValue(IGREQUEST_PATH);
            sImageOutputPath = sImageOutputPath.Substring(0, sImageOutputPath.LastIndexOf('\\') + 1) + HC.PATH_LASTSAVEDPICTURE;
            if (!File.Exists(sImageOutputPath))
                return false;
            string sImagePath = GetParameterValue(IGREQUEST_PATH);
            string sImagePathJPEG = sImagePath.Remove(sImagePath.Length - Path.GetExtension(sImagePath).Length) + "$" + GetAttributeValue(IGREQUEST_GUID) + ".JPG";
            string sAccountMiniImagePath = HC.PATH_USERACCOUNT + sLogin + HC.PATH_USERMINI + HC.PATH_PREFIXMINI + sImagePath;
            Image imgInput = Image.FromFile(sImageOutputPath);
            float fRate = HC.MINIPICTURE_MAXSIZE / (float)Math.Max(imgInput.Size.Width, imgInput.Size.Height);
            Image imgMini = new Bitmap(imgInput, new Size((int)((float)imgInput.Size.Width * fRate), (int)((float)imgInput.Size.Height * fRate)));
            imgMini.Save(sAccountMiniImagePath, ImageFormat.Jpeg);
            imgMini.Save(HC.PATH_OUTPUT + sLogin + HC.PATH_OUTPUTMINI + HC.PATH_PREFIXMINI + sImagePathJPEG, ImageFormat.Jpeg);
            answer.SetParameter(IGREQUEST_SIZE, imgMini.Width.ToString() + "," + imgMini.Height.ToString());
            return true;
        }
        public override void GetParams()
        {
            base.GetParams();
            miniPicName = GetParameterValue(IGREQUEST_PATH);
            string sImagePathJPEG = miniPicName.Remove(miniPicName.Length - Path.GetExtension(miniPicName).Length) + "$" + GetAttributeValue(IGREQUEST_GUID) + ".JPG";
            miniPicPath = HC.PATH_USERACCOUNT + GetAttributeValue(IGREQUEST_USERLOGIN) + HC.PATH_USERMINI + HC.PATH_PREFIXMINI + miniPicName;
            miniPicWidth = m_pairSize.Key;
            miniPicHeight = m_pairSize.Value;
        }
        public override void GetParams(IGAnswer answer, HttpSessionState session)
        {
            if ((string)session[IGAnswer.IGANSWER_IMAGELIBRARY] == null)
                session[IGAnswer.IGANSWER_IMAGELIBRARY] = "";
            string sLogin = answer.GetAttributeValue(IGREQUEST_USERLOGIN);
            string sServerIP = answer.GetParameterValue(IGAnswer.IGANSWER_SERVERIP);
            string sImagePath = GetParameterValue(IGREQUEST_PATH);
            string sImagePathExt = Path.GetExtension(sImagePath);
            string sImagePathPrefix = sImagePath.Remove(sImagePath.Length - Path.GetExtension(sImagePath).Length) + "$";
            sImagePath = sImagePathPrefix + GetAttributeValue(IGREQUEST_GUID) + sImagePathExt;
            string sMiniOutputImagePath = HC.PATH_OUTPUTVIRTUAL + sServerIP + "/" + sLogin + HC.PATH_OUTPUTMINI + HC.PATH_PREFIXMINI + sImagePath;
            string sImageLibrary = (string)(string)session[IGAnswer.IGANSWER_IMAGELIBRARY];
            sImageLibrary = sImageLibrary.ToUpper();
            SetParameter(IGREQUEST_SIZE, answer.GetParameterValue(IGREQUEST_SIZE));
            List<string> lsListSize = new List<string>();
            splitParamToList(lsListSize, IGREQUEST_SIZE);
            m_pairSize = new KeyValuePair<string, string>(lsListSize[1], lsListSize[2]);
            if (sImageLibrary.Contains(sImagePathPrefix.ToUpper()))
            {
                int nIdxImPath = sImageLibrary.IndexOf(sImagePathPrefix.ToUpper());
                int nIdxNextDot = sImageLibrary.IndexOf(',', nIdxImPath);
                if (nIdxNextDot < nIdxImPath)
                    nIdxNextDot = sImageLibrary.Length;
                string sOldImagePath = HC.PATH_OUTPUTVIRTUAL + sServerIP + "/" + sLogin + HC.PATH_OUTPUTMINI + HC.PATH_PREFIXMINI + sImageLibrary.Substring(nIdxImPath, nIdxNextDot - nIdxImPath);
                sOldImagePath = sOldImagePath.ToUpper();
                SetParameter(IGAnswer.IGANSWER_IMAGELIBRARY, sImageLibrary);
                List<string> lsOutputPath = new List<string>();
                splitParamToList(lsOutputPath, IGAnswer.IGANSWER_IMAGELIBRARY);
                lsOutputPath[lsOutputPath.IndexOf(sOldImagePath)] = sMiniOutputImagePath;
                lsOutputPath.RemoveAt(0);
                session[IGAnswer.IGANSWER_IMAGELIBRARY] = createParamFromList(lsOutputPath);
            }
            else
            {
                if ((string)session[IGAnswer.IGANSWER_IMAGELIBRARY] != "")
                    session[IGAnswer.IGANSWER_IMAGELIBRARY] += ",";
                session[IGAnswer.IGANSWER_IMAGELIBRARY] += sMiniOutputImagePath;
                session[IGAnswer.IGANSWER_IMAGELIBRARY] += "," + lsListSize[1]; // width
                session[IGAnswer.IGANSWER_IMAGELIBRARY] += "," + lsListSize[2]; // height
            }
            SetParameter(IGREQUEST_USERLOGIN, sLogin);
            SetParameter(IGAnswer.IGANSWER_SERVERIP, sServerIP);
            SetParameter(IGREQUEST_PATH, sMiniOutputImagePath);            
        }
        public override bool NeedAccount()
        {
            return true;
        }
    }

    [DataContract]
    public class IGRequestFrameSaveBeetleMorph : IGRequestFrame
    {
        [DataMember(Name = "OutputPath", Order = 0)]
        string outPath;

        public const string IGREQUESTFRAMESAVEBM_STRING = "Save beetle morph image";
        public IGRequestFrameSaveBeetleMorph(string sUser, string sFrameId, string sPath)
            : base(sUser, IGREQUESTFRAMESAVEBM_STRING, IGREQUEST_FRAME_SAVE_BM, sFrameId)
        {
            if ((Path.GetExtension(sPath).ToUpper() != ".JPG") &&
                (Path.GetExtension(sPath).ToUpper() != ".PNG") &&
                (Path.GetExtension(sPath).ToUpper() != ".BMP") &&
                (Path.GetExtension(sPath).ToUpper() != ".IG"))
                sPath += ".JPG";
            SetParameter(IGREQUEST_PATH, HC.PATH_USERIMAGES_BEETLEMORPH + "/" + sPath);
        }
        public override bool ProcessAnswer(IGAnswer answer)
        {
            // no mini-picture for beetle morph pictures
            return true;
        }
        public override void GetParams(IGAnswer answer, HttpSessionState session)
        {
            return;
        }
        public override void GetParams()
        {
            base.GetParams();
            outPath = GetParameterValue(IGREQUEST_PATH);
        }
        public IGRequestFrameSaveBeetleMorph(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMESAVEBM_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameAddLayer : IGRequestFrame
    {
        public const string IGREQUESTFRAMEADDLAYER_STRING = "Add layer";
        public IGRequestFrameAddLayer(string sUser, string sFrameId)
            : base(sUser, IGREQUESTFRAMEADDLAYER_STRING, IGREQUEST_FRAME_ADDLAYER, sFrameId)
        {
        }
        public IGRequestFrameAddLayer(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMEADDLAYER_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameCopy : IGRequestFrame
    {
        public const string IGREQUESTCOPY_STRING = "Copy";
        public IGRequestFrameCopy(string sUserLogin, string sFrameId)
            : base(sUserLogin, IGREQUESTCOPY_STRING, IGREQUEST_FRAME_COPY, sFrameId) { }
        public IGRequestFrameCopy(XmlDocument xmlDoc)
            : base(IGREQUESTCOPY_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameCut : IGRequestFrame
    {
        public const string IGREQUESTCUT_STRING = "Cut";
        public IGRequestFrameCut(string sUserLogin, string sFrameId)
            : base(sUserLogin, IGREQUESTCUT_STRING, IGREQUEST_FRAME_CUT, sFrameId) { }
        public IGRequestFrameCut(XmlDocument xmlDoc)
            : base(IGREQUESTCUT_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFramePaste : IGRequestFrame
    {
        public const string IGREQUESTPASTE_STRING = "Paste";
        public IGRequestFramePaste(string sUserLogin, string sFrameId)
            : base(sUserLogin, IGREQUESTPASTE_STRING, IGREQUEST_FRAME_PASTE, sFrameId) { }
        public IGRequestFramePaste(XmlDocument xmlDoc)
            : base(IGREQUESTPASTE_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameDelete : IGRequestFrame
    {
        public const string IGREQUESTDELETE_STRING = "Delete";
        public IGRequestFrameDelete(string sUserLogin, string sFrameId)
            : base(sUserLogin, IGREQUESTDELETE_STRING, IGREQUEST_FRAME_DELETE, sFrameId) { }
        public IGRequestFrameDelete(XmlDocument xmlDoc)
            : base(IGREQUESTDELETE_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameSmartDelete : IGRequestFrame
    {
        public const string IGREQUESTSMARTDELETE_STRING = "SmartDelete";
        public IGRequestFrameSmartDelete(string sUserLogin, string sFrameId)
            : base(sUserLogin, IGREQUESTSMARTDELETE_STRING, IGREQUEST_FRAME_SMARTDELETE, sFrameId) { }
        public IGRequestFrameSmartDelete(XmlDocument xmlDoc)
            : base(IGREQUESTSMARTDELETE_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameRemoveLayer : IGRequestFrameLayer
    {
        public const string IGREQUESTFRAMEREMOVELAYER_STRING = "Remove layer";
        public IGRequestFrameRemoveLayer(string sUser, string sFrameId, string sLayerId)
            : base(sUser, IGREQUESTFRAMEREMOVELAYER_STRING, IGREQUEST_FRAME_REMOVELAYER, sFrameId, sLayerId)
        {
        }
        public IGRequestFrameRemoveLayer(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMEREMOVELAYER_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameMoveLayer : IGRequestFrameLayer
    {
        public const string IGREQUESTFRAMEMOVELAYER_STRING = "Move layer";
        public IGRequestFrameMoveLayer(string sUser, string sFrameId, string sLayerFrom, string sLayerTo)
            : base(sUser, IGREQUESTFRAMEMOVELAYER_STRING, IGREQUEST_FRAME_MOVELAYER, sFrameId, sLayerFrom)
        {
            SetParameter(IGREQUEST_LAYERIDTO, sLayerTo);
        }
        public IGRequestFrameMoveLayer(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMEMOVELAYER_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameMove : IGRequestFrameLayer
    {
        public const string IGREQUESTFRAMEMOVE_STRING = "Move Pixels";
        public IGRequestFrameMove(string sUser, string sFrameId, string sLayerId, string sVectorX, string sVectorY)
            : base(sUser, IGREQUESTFRAMEMOVE_STRING, IGREQUEST_FRAME_MOVE, sFrameId, sLayerId)
        {
            SetParameter(IGREQUEST_DIRECTIONX, sVectorX);
            SetParameter(IGREQUEST_DIRECTIONY, sVectorY);
        }
        public IGRequestFrameMove(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMEMOVE_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameRotateAndResize : IGRequestFrameLayer
    {
        public const string IGREQUESTFRAMEROTATEANDRESIZE_STRING = "RotateAndResize";
        public IGRequestFrameRotateAndResize(string sUser, string sFrameId, string sLayerId, string sAngle, string sRate)
            : base(sUser, IGREQUESTFRAMEROTATEANDRESIZE_STRING, IGREQUEST_FRAME_ROTATEANDRESIZE, sFrameId, sLayerId)
        {
            SetParameter(IGREQUEST_ANGLE, sAngle);
            SetParameter(IGREQUEST_RATE, sRate);
        }
        public IGRequestFrameRotateAndResize(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMEROTATEANDRESIZE_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameSelectLayer : IGRequestFrameLayer
    {
        public const string IGREQUESTFRAMESELECTLAYER_STRING = "SelectLayer";
        public IGRequestFrameSelectLayer(string sUser, string sFrameId, string sLayerId)
            : base(sUser, IGREQUESTFRAMESELECTLAYER_STRING, IGREQUEST_FRAME_SELECTLAYER, sFrameId, sLayerId){}
        public IGRequestFrameSelectLayer(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMESELECTLAYER_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameMergeLayer : IGRequestFrameLayer
    {
        public const string IGREQUESTFRAMEMERGELAYER_STRING = "Merge layers";
        public IGRequestFrameMergeLayer(string sUser, string sFrameId, string sLayersFrom, string sLayerTo)
            : base(sUser, IGREQUESTFRAMEMERGELAYER_STRING, IGREQUEST_FRAME_MERGELAYER, sFrameId, sLayerTo)
        {
            SetParameter(IGREQUEST_LAYERFROMIDS, sLayersFrom);
        }
        public IGRequestFrameMergeLayer(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMEMERGELAYER_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameFilterLayer : IGRequestFrameLayer
    {
        public const string IGREQUESTFRAMEFILTERLAYER_STRING = "Filter layer";
        public IGRequestFrameFilterLayer(string sUser, string sFrameId, string sLayerId, string sFilterId, bool original = false)
            : base(sUser, IGREQUESTFRAMEFILTERLAYER_STRING, IGREQUEST_FRAME_FILTER, sFrameId, sLayerId)
        {
            SetParameter(IGREQUEST_FILTERID, sFilterId);
            SetParameter(IGREQUEST_ORIGINAL, original ? "1" : "0");
        }
        public IGRequestFrameFilterLayer(string sUser, string sFrameId, string sLayerId, string sFilterId, Dictionary<string, string> dicParams, bool original = false)
            : base(sUser, IGREQUESTFRAMEFILTERLAYER_STRING, IGREQUEST_FRAME_FILTER, sFrameId, sLayerId)
        {
            SetParameter(IGREQUEST_FILTERID, sFilterId);
            SetParameter(IGREQUEST_ORIGINAL, original ? "1" : "0");
            if (dicParams != null)
            {
                foreach (var kvp in dicParams)
                    SetParameter(kvp.Key, kvp.Value);
            }
        }
        public IGRequestFrameFilterLayer(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMEFILTERLAYER_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameFilterLayerBrightness : IGRequestFrameFilterLayer
    {
        public const string IGIPFILTER_BRIGHTNESS_EFFECT = "10";
        public IGRequestFrameFilterLayerBrightness(string sUser, string sFrameId, string sLayerId, string sStrength)
            : base(sUser, sFrameId, sLayerId, IGIPFILTER_BRIGHTNESS_EFFECT, createParams(sStrength)){        
        }
        private static Dictionary<string, string> createParams(string sStrength)
        {
            Dictionary<string, string> dicParams = new Dictionary<string, string>();
            dicParams.Add(IGREQUEST_STRENGTH, sStrength);
            return dicParams;
        }
        public IGRequestFrameFilterLayerBrightness(XmlDocument xmlDoc)
            : base(xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameFilterLayerMorphing : IGRequestFrameFilterLayer
    {
        public const string IGIPFILTER_MORPHING_EFFECT = "12";
        public IGRequestFrameFilterLayerMorphing(string sUser, string sFrameId, string sLayerId, string sStrength, int nPosX, int nPosY, int nDirectionX, int nDirectionY)
            : base(sUser, sFrameId, sLayerId, IGIPFILTER_MORPHING_EFFECT, createParams(sStrength, nPosX, nPosY, nDirectionX, nDirectionY))
        {
        }
        private static Dictionary<string, string> createParams(string sStrength, int nPosX, int nPosY, int nDirectionX, int nDirectionY)
        {
            Dictionary<string, string> dicParams = new Dictionary<string, string>();
            dicParams.Add(IGREQUEST_STRENGTH, sStrength);
            dicParams.Add(IGREQUEST_POSX, nPosX.ToString());
            dicParams.Add(IGREQUEST_POSY, nPosY.ToString());
            dicParams.Add(IGREQUEST_DIRECTIONX, nDirectionX.ToString());
            dicParams.Add(IGREQUEST_DIRECTIONY, nDirectionY.ToString());
            return dicParams;
        }
        public IGRequestFrameFilterLayerMorphing(XmlDocument xmlDoc)
            : base(xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameFilterLayerFace : IGRequestFrameFilterLayer
    {
        public const string IGIPFILTER_FACE_EFFECT = "8";
        public IGRequestFrameFilterLayerFace(string sUser, string sFrameId, string sLayerId, string[] sFaceEffectParams)
            : base(sUser, sFrameId, sLayerId, IGIPFILTER_FACE_EFFECT, createParams(sFaceEffectParams))
        {
        }
        private static Dictionary<string, string> createParams(string[] sFaceEffectParams)
        {
            Dictionary<string, string> dicParams = new Dictionary<string, string>();
            int idxParam = 0;
            foreach (string param in sFaceEffectParams)
            {
                switch (idxParam++)
                {
                    case 0:
                        dicParams.Add(IGREQUEST_FACEEFFECTID, sFaceEffectParams[0]);
                        break;
                    case 1:
                        dicParams.Add(IGREQUEST_COLOR, sFaceEffectParams[1]);
                        break;
                    case 2:
                        dicParams.Add(IGREQUEST_COLOR2, sFaceEffectParams[2]);
                        break;
                    case 3:
                        dicParams.Add(IGREQUEST_PARAM, sFaceEffectParams[3]);
                        break;
                    case 4:
                        dicParams.Add(IGREQUEST_PARAM2, sFaceEffectParams[4]);
                        break;
                    case 5:
                        dicParams.Add(IGREQUEST_PARAM3, sFaceEffectParams[5]);
                        break;
                    case 6:
                        dicParams.Add(IGREQUEST_PATH, sFaceEffectParams[6]);
                        break;
                    case 7:
                        dicParams.Add(IGREQUEST_PATH2, sFaceEffectParams[7]);
                        break;
                }
            }
            return dicParams;
        }
        public IGRequestFrameFilterLayerFace(XmlDocument xmlDoc)
            : base(xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameIndexLayer : IGRequestFrameLayer
    {
        public const string IGREQUESTFRAMEINDEXLAYER_STRING = "Index layer";
        public IGRequestFrameIndexLayer(string sUser, string sFrameId, string sLayerId, string sIndexerId)
            : base(sUser, IGREQUESTFRAMEINDEXLAYER_STRING, IGREQUEST_FRAME_INDEX, sFrameId, sLayerId)
        {
            SetParameter(IGREQUEST_FILTERID, sIndexerId);
        }
        public IGRequestFrameIndexLayer(string sUser, string sFrameId, string sLayerId, string sIndexerId, string sMarkerType, string sMarkerType2,
                                                                                                           string sLstPts, string sLstPts2)
            : base(sUser, IGREQUESTFRAMEINDEXLAYER_STRING, IGREQUEST_FRAME_INDEX, sFrameId, sLayerId)
        {
            SetParameter(IGREQUEST_FILTERID, sIndexerId);
            SetParameter(IGREQUEST_MARKERTYPE, sMarkerType);
            SetParameter(IGREQUEST_MARKERTYPE2, sMarkerType2);
            SetParameter(IGREQUEST_POINTS, sLstPts);
            SetParameter(IGREQUEST_POINTS2, sLstPts2);
        }
        public IGRequestFrameIndexLayer(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMEINDEXLAYER_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestChangeProperty : IGRequestFrame
    {
        public const string IGREQUESTFRAMECHANGEPROPERTY_STRING = "Change property";
        public IGRequestChangeProperty(string sUser, string sFrameId, string sPropIds, string sPropVals)
            : base(sUser, IGREQUESTFRAMECHANGEPROPERTY_STRING, IGREQUEST_FRAME_CHANGEPROPERTY, sFrameId)
        {
            SetParameter(IGREQUEST_ISFRAME, sFrameId == "0" ? "0" : "1");
            SetParameter(IGREQUEST_PROPIDS, sPropIds);
            Regex RE = new Regex(@"[,]+");
            string[] tPropIds = RE.Split(sPropIds);
            string[] tPropVals = RE.Split(sPropVals);            
            if (tPropIds.Count() == tPropVals.Count())
            {
                for (int idxPropId = 0; idxPropId < tPropIds.Count(); idxPropId++)
                    SetParameter(tPropIds[idxPropId], tPropVals[idxPropId]);
            }            
        }
        public IGRequestChangeProperty(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMECHANGEPROPERTY_STRING, xmlDoc) { }
        private List<string> m_lPropIds = new List<string>();
        private List<string> m_lPropVals = new List<string>();
    }

    [DataContract]
    public class IGRequestFrameClose : IGRequestFrame
    {
        [DataMember(Name = "NbLayers", Order = 0)]
        string nbLayers;
        [DataMember(Name = "LayerVisibility", Order = 1)]
        string layerVisibility;

        public const string IGREQUESTFRAMECLOSE_STRING = "Close frame";
        public IGRequestFrameClose(string sUser, string sFrameId)
            : base(sUser, IGREQUESTFRAMECLOSE_STRING, IGREQUEST_FRAME_CLOSE, sFrameId)
        {
        }
        public IGRequestFrameClose(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMECLOSE_STRING, xmlDoc) { }
        public override bool ProcessAnswer(IGAnswer answer)
        {
            if (m_reqProc != null && UserConnection != null)
            {
                string sFrameId = m_reqProc.GetFrameId();
                UserConnection.RemoveImage(sFrameId);
            }
            return true;
        }
        public override void GetParams()
        {
            base.GetParams();
            nbLayers = GetParameterValue(IGAnswer.IGANSWER_ALLNBLAYERS);
            layerVisibility = GetParameterValue(IGAnswer.IGANSWER_ALLLAYERVISIBILITY);
        }
        public override void GetParams(IGAnswer answer, HttpSessionState session)
        {
            SetParameter(IGAnswer.IGANSWER_RESTOREDFRAMEIDS, (string)session[IGAnswer.IGANSWER_RESTOREDFRAMEIDS]);
            SetParameter(IGAnswer.IGANSWER_FRAMENAMES, (string)session[IGAnswer.IGANSWER_FRAMENAMES]);
            SetParameter(IGAnswer.IGANSWER_REQGUIDS, (string)session[IGAnswer.IGANSWER_REQGUIDS]);
            SetParameter(IGAnswer.IGANSWER_ALLNBLAYERS, (string)session[IGAnswer.IGANSWER_ALLNBLAYERS]);
            SetParameter(IGAnswer.IGANSWER_ALLLAYERVISIBILITY, (string)session[IGAnswer.IGANSWER_ALLLAYERVISIBILITY]);
            SetParameter(IGAnswer.IGANSWER_ALLSTEPIDS, (string)session[IGAnswer.IGANSWER_ALLSTEPIDS]);
            SetParameter(IGAnswer.IGANSWER_FRAMEPROPERTIES, (string)session[IGAnswer.IGANSWER_FRAMEPROPERTIES]);
            List<string> lsFrameIds = new List<string>();
            splitParamToList(lsFrameIds, IGAnswer.IGANSWER_RESTOREDFRAMEIDS);
            List<string> lsFrameNames = new List<string>();
            splitParamToList(lsFrameNames, IGAnswer.IGANSWER_FRAMENAMES);
            List<string> lsFrameGuids = new List<string>();
            splitParamToList(lsFrameGuids, IGAnswer.IGANSWER_REQGUIDS);
            List<string> lsNbLayers = new List<string>();
            splitParamToList(lsNbLayers, IGAnswer.IGANSWER_ALLNBLAYERS);
            List<string> lsLayerVisibility = new List<string>();
            splitParamToList(lsLayerVisibility, IGAnswer.IGANSWER_ALLLAYERVISIBILITY);
            List<string> lsStepIds = new List<string>();
            splitParamToList(lsStepIds, IGAnswer.IGANSWER_ALLSTEPIDS);
            List<string> lsFrameProperties = new List<string>();
            splitParamToList(lsFrameProperties, IGAnswer.IGANSWER_FRAMEPROPERTIES);
            string frameId = answer.GetParameterValue(IGREQUEST_FRAMEID);
            if (frameId == "-1")
            {
                session[IGAnswer.IGANSWER_RESTOREDFRAMEIDS] = "";
                session[IGAnswer.IGANSWER_FRAMENAMES] = "";
                session[IGAnswer.IGANSWER_REQGUIDS] = "";
                session[IGAnswer.IGANSWER_ALLNBLAYERS] = "";
                session[IGAnswer.IGANSWER_ALLLAYERVISIBILITY] = "";
                session[IGAnswer.IGANSWER_ALLSTEPIDS] = "";
                session[IGAnswer.IGANSWER_FRAMEPROPERTIES] = "";
                session[IGAnswer.IGANSWER_SELECTEDFRAMEID] = "";
                session[IGAnswer.IGANSWER_SELECTEDREQGUID] = "";                
            }
            else
            {
                int nIdxFrame = lsFrameIds.IndexOf(frameId);
                lsFrameIds.RemoveAt(nIdxFrame);
                lsFrameIds.RemoveAt(0);
                lsFrameNames.RemoveAt(nIdxFrame);
                lsFrameNames.RemoveAt(0);
                lsFrameGuids.RemoveAt(nIdxFrame);
                lsFrameGuids.RemoveAt(0);
                lsNbLayers.RemoveAt(nIdxFrame);
                lsNbLayers.RemoveAt(0);
                lsLayerVisibility.RemoveAt(nIdxFrame);
                lsLayerVisibility.RemoveAt(0);
                lsStepIds.RemoveAt(nIdxFrame);
                lsStepIds.RemoveAt(0);
                lsFrameProperties.RemoveAt(nIdxFrame);
                lsFrameProperties.RemoveAt(0);
                session[IGAnswer.IGANSWER_RESTOREDFRAMEIDS] = createParamFromList(lsFrameIds);
                session[IGAnswer.IGANSWER_FRAMENAMES] = createParamFromList(lsFrameNames);
                session[IGAnswer.IGANSWER_REQGUIDS] = createParamFromList(lsFrameGuids);
                session[IGAnswer.IGANSWER_ALLNBLAYERS] = createParamFromList(lsNbLayers);
                session[IGAnswer.IGANSWER_ALLLAYERVISIBILITY] = createParamFromList(lsLayerVisibility);
                session[IGAnswer.IGANSWER_ALLSTEPIDS] = createParamFromList(lsStepIds);
                session[IGAnswer.IGANSWER_FRAMEPROPERTIES] = createParamFromList(lsFrameProperties);
                if (lsFrameIds.Count > 0)
                {
                    session[IGAnswer.IGANSWER_SELECTEDFRAMEID] = lsFrameIds[lsFrameIds.Count - 1];
                    session[IGAnswer.IGANSWER_SELECTEDREQGUID] = lsFrameGuids[lsFrameGuids.Count - 1];
                }
                else
                {
                    session[IGAnswer.IGANSWER_SELECTEDFRAMEID] = "";
                    session[IGAnswer.IGANSWER_SELECTEDREQGUID] = "";
                }
            }
            answer.SetParameter(IGAnswer.IGANSWER_RESTOREDFRAMEIDS, (string)session[IGAnswer.IGANSWER_RESTOREDFRAMEIDS]);
            answer.SetParameter(IGAnswer.IGANSWER_FRAMENAMES, (string)session[IGAnswer.IGANSWER_FRAMENAMES]);
            answer.SetParameter(IGAnswer.IGANSWER_REQGUIDS, (string)session[IGAnswer.IGANSWER_REQGUIDS]);
            answer.SetParameter(IGAnswer.IGANSWER_ALLNBLAYERS, (string)session[IGAnswer.IGANSWER_ALLNBLAYERS]);
            answer.SetParameter(IGAnswer.IGANSWER_ALLLAYERVISIBILITY, (string)session[IGAnswer.IGANSWER_ALLLAYERVISIBILITY]);
            answer.SetParameter(IGAnswer.IGANSWER_ALLSTEPIDS, (string)session[IGAnswer.IGANSWER_ALLSTEPIDS]);
            answer.SetParameter(IGAnswer.IGANSWER_FRAMEPROPERTIES, (string)session[IGAnswer.IGANSWER_FRAMEPROPERTIES]);
            SetParameter(IGAnswer.IGANSWER_ALLNBLAYERS, (string)session[IGAnswer.IGANSWER_ALLNBLAYERS]);
            SetParameter(IGAnswer.IGANSWER_ALLLAYERVISIBILITY, (string)session[IGAnswer.IGANSWER_ALLLAYERVISIBILITY]);
            SetParameter(IGAnswer.IGANSWER_ALLSTEPIDS, (string)session[IGAnswer.IGANSWER_ALLSTEPIDS]);
        }
    }

    [DataContract]
    public class IGRequestFrameUpdate : IGRequestFrame
    {
        public const string IGREQUESTFRAMEUPDATE_STRING = "Update frame";
        public IGRequestFrameUpdate(string sUser, string sFrameId)
            : base(sUser, IGREQUESTFRAMEUPDATE_STRING, IGREQUEST_FRAME_UPDATE, sFrameId){ }
        public IGRequestFrameUpdate(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMEUPDATE_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameGotoHistory : IGRequestFrame
    {
        public const string IGREQUESTFRAMEGOTOHISTORY_STRING = "Goto history";
        public IGRequestFrameGotoHistory(string sUser, string sFrameId, string sHistoryStepId)
            : base(sUser, IGREQUESTFRAMEGOTOHISTORY_STRING, IGREQUEST_FRAME_GOTOHISTORY, sFrameId)
        {
            SetParameter(IGREQUEST_HISTORYID, sHistoryStepId);
        }
        public IGRequestFrameGotoHistory(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMEGOTOHISTORY_STRING, xmlDoc) { }
    }
    
    // frame selection coordinate range = [0;1000000]
    [DataContract]
    public class IGRequestFrameSelect : IGRequestFrameLayer
    {
        public const string IGREQUESTFRAMESELECT_STRING = "Select area";
        public IGRequestFrameSelect(string sUser, string sFrameId, string sLayerId, string sSelectionType, string sPts)
            : base(sUser, IGREQUESTFRAMESELECT_STRING, IGREQUEST_FRAME_SELECT, sFrameId, sLayerId)
        {
            SetParameter(IGREQUEST_SELECTIONTYPE, sSelectionType);
            if ((int.Parse(sSelectionType) & IGREQUEST_FRAME_SELECT_MAGIC) == IGREQUEST_FRAME_SELECT_MAGIC)
            {
                int nIdxTolerance = sPts.LastIndexOf(',');
                SetParameter(IGREQUEST_TOLERANCE, sPts.Substring(nIdxTolerance + 1));
                sPts = sPts.Substring(0, nIdxTolerance);
            }
            SetParameter(IGREQUEST_POINTS, sPts);
        }
        public IGRequestFrameSelect(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMESELECT_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameSetLayerVisible : IGRequestFrameLayer
    {
        public const string IGREQUESTFRAMESETLAYERVISIBLE_STRING = "Set layer visibility";
        public IGRequestFrameSetLayerVisible(string sUser, string sFrameId, string sLayerId, string sVisible)
            : base(sUser, IGREQUESTFRAMESETLAYERVISIBLE_STRING, IGREQUEST_FRAME_SETLAYERVISIBLE, sFrameId, sLayerId)
        {
            SetParameter(IGREQUEST_VISIBLE, sVisible);
        }
        public IGRequestFrameSetLayerVisible(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMESETLAYERVISIBLE_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameDrawLines : IGRequestFrameLayer
    {
        public const string IGREQUESTFRAMEDRAWLINES_STRING = "Draw Lines";
        public IGRequestFrameDrawLines(string sUser, string sFrameId, string sLayerId, string sCoords, string sTransparency, string sBrushSize)
            : base(sUser, IGREQUESTFRAMEDRAWLINES_STRING, IGREQUEST_FRAME_DRAWLINES, sFrameId, sLayerId)
        {
            SetParameter(IGREQUEST_POINTS, sCoords);
            SetParameter(IGREQUEST_TRANSPARENCY, sTransparency);
            SetParameter(IGREQUEST_BRUSHSIZE, sBrushSize);
        }
        public IGRequestFrameDrawLines(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMEDRAWLINES_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFrameRubber : IGRequestFrameLayer
    {
        public const string IGREQUESTFRAMERUBBER_STRING = "Rubber";
        public IGRequestFrameRubber(string sUser, string sFrameId, string sLayerId, string sCoords, string sTransparency, string sBrushSize)
            : base(sUser, IGREQUESTFRAMERUBBER_STRING, IGREQUEST_FRAME_RUBBER, sFrameId, sLayerId)
        {
            SetParameter(IGREQUEST_POINTS, sCoords);
            SetParameter(IGREQUEST_TRANSPARENCY, sTransparency);
            SetParameter(IGREQUEST_BRUSHSIZE, sBrushSize);
        }
        public IGRequestFrameRubber(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMERUBBER_STRING, xmlDoc) { }
    }

    [DataContract]
    public class IGRequestFramePickColor : IGRequestFrameLayer
    {
        public const string IGREQUESTFRAMEPICKCOLOR_STRING = "Pick Color";
        public IGRequestFramePickColor(string sUser, string sFrameId, string sLayerId, string sCoords)
            : base(sUser, IGREQUESTFRAMEPICKCOLOR_STRING, IGREQUEST_FRAME_PICKCOLOR, sFrameId, sLayerId)
        {
            SetParameter(IGREQUEST_POINTS, sCoords);
        }
        public IGRequestFramePickColor(XmlDocument xmlDoc)
            : base(IGREQUESTFRAMEPICKCOLOR_STRING, xmlDoc) { }
    }
}
