using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Web.SessionState;
using System.Text.RegularExpressions;
using System.Runtime.Serialization;

namespace IGSMLib
{
    [DataContract]
    public class IGAnswerFrameChanged : IGAnswerFrameDone
    {
        [DataMember(Name = "PictureId", Order = 0)]
        string picId;
        [DataMember(Name = "WorkspaceProperties", Order = 1)]
        string workspaceProperties;
        [DataMember(Name = "PictureName", Order = 2)]
        string picName;
        [DataMember(Name = "NbLayers", Order = 3)]
        string nbLayers;
        [DataMember(Name = "LayerVisibility", Order = 4)]
        string layerVisibility;

        public IGAnswerFrameChanged(IGConnectionLocal connection, XmlDocument xmlDoc)
            : base(connection, xmlDoc, "frame changed")
        { 
        }

        protected override bool InternalProcess()
        {
            string sFrameId = GetParameterValue(IGRequest.IGREQUEST_FRAMEID);
            if (sFrameId == null)
                return false;
            if (m_reqProc != null)
            {
                if ((m_reqProc.GetReqId() == IGRequest.IGREQUEST_WORKSPACE_LOADIMAGE) ||
                    (m_reqProc.GetReqId() == IGRequest.IGREQUEST_WORKSPACE_NEWIMAGE))
                    m_connection.AddImage(sFrameId);
            }
            return true;
        }

        public override void GetParams()
        {
            base.GetParams();
            picId = GetParameterValue(IGRequest.IGREQUEST_FRAMEID);
            workspaceProperties = GetParameterValue(IGANSWER_WORKSPACEPROPERTIES);
            picName = GetParameterValue(IGANSWER_PICTURENAME);
            nbLayers = GetParameterValue(IGANSWER_ALLNBLAYERS);
            layerVisibility = GetParameterValue(IGANSWER_ALLLAYERVISIBILITY);
        }

        public override void GetParams(HttpSessionState session)
        {
            session[IGANSWER_REQGUID] = GetReqGuid();
            string sFrameId = GetParameterValue(IGRequest.IGREQUEST_FRAMEID);
            session[IGANSWER_SELECTEDFRAMEID] = sFrameId;
            session[IGANSWER_SELECTEDREQGUID] = GetReqGuid();
            bool bFrameExists = false;
            if ((session[IGANSWER_RESTOREDFRAMEIDS] == null) || ((string)session[IGANSWER_RESTOREDFRAMEIDS] == ""))
                session[IGANSWER_RESTOREDFRAMEIDS] = sFrameId;
            else if (((string)session[IGANSWER_RESTOREDFRAMEIDS]).Contains(sFrameId))
                bFrameExists = true;
            else
                session[IGANSWER_RESTOREDFRAMEIDS] += "," + sFrameId;
            session[IGANSWER_WORKSPACEPROPERTIES] = GetParameterValue(IGANSWER_WORKSPACEPROPERTIES);
            string sTestFramePath = GetParameterValue(IGANSWER_PICTURENAME);
            if (bFrameExists)
            {
                // Frame Manager
                // --> Restored frame ids
                SetParameter(IGANSWER_RESTOREDFRAMEIDS, (string)session[IGANSWER_RESTOREDFRAMEIDS]);
                List<string> lsFrameIds = new List<string>();
                splitParamToList(lsFrameIds, IGANSWER_RESTOREDFRAMEIDS);
                int nIdxFrame = lsFrameIds.IndexOf(sFrameId);

                // --> Restored req guids
                SetParameter(IGANSWER_REQGUIDS, (string)session[IGANSWER_REQGUIDS]);
                List<string> lsReqGuids = new List<string>();
                splitParamToList(lsReqGuids, IGANSWER_REQGUIDS);
                lsReqGuids[nIdxFrame] = (string)session[IGANSWER_REQGUID];
                lsReqGuids.RemoveAt(0);
                session[IGANSWER_REQGUIDS] = createParamFromList(lsReqGuids);
                session[IGANSWER_SELECTEDREQGUID] = (string)session[IGANSWER_REQGUID];

                // Layer Manager                
                // --> Number of layers
                SetParameter(IGANSWER_ALLNBLAYERS, (string)session[IGANSWER_ALLNBLAYERS]);
                List<string> lsNbLayers = new List<string>();
                splitParamToList(lsNbLayers, IGANSWER_ALLNBLAYERS);
                lsNbLayers[nIdxFrame] = GetParameterValue(IGANSWER_NBLAYERS);
                lsNbLayers.RemoveAt(0);
                session[IGANSWER_ALLNBLAYERS] = createParamFromList(lsNbLayers);

                // Frame properties
                string sCurFrameProperties = GetParameterValue(IGANSWER_FRAMEPROPERTIES);
                SetParameter(IGANSWER_FRAMEPROPERTIES, (string)session[IGANSWER_FRAMEPROPERTIES]);
                List<string> lsFrameProperties = new List<string>();
                splitParamToList(lsFrameProperties, IGANSWER_FRAMEPROPERTIES);
                lsFrameProperties[nIdxFrame] = sCurFrameProperties;
                lsFrameProperties.RemoveAt(0);
                session[IGANSWER_FRAMEPROPERTIES] = createParamFromList(lsFrameProperties);
                
                // --> Layer Visibility
                string sCurLayerVisibility = GetParameterValue(IGANSWER_LAYERVISIBILITY);
                if ((sCurLayerVisibility != null) && (sCurLayerVisibility != ""))
                {
                    SetParameter(IGANSWER_ALLLAYERVISIBILITY, (string)session[IGANSWER_ALLLAYERVISIBILITY]);
                    List<string> lsLayerVisibility = new List<string>();
                    splitParamToList(lsLayerVisibility, IGANSWER_ALLLAYERVISIBILITY);
                    lsLayerVisibility[nIdxFrame] = sCurLayerVisibility;
                    lsLayerVisibility.RemoveAt(0);
                    session[IGANSWER_ALLLAYERVISIBILITY] = createParamFromList(lsLayerVisibility);
                }

                string sCurStepIds = GetParameterValue(IGANSWER_STEPIDS);
                if ((sCurStepIds != null) && (sCurStepIds != ""))
                {
                    // step ids
                    SetParameter(IGANSWER_ALLSTEPIDS, (string)session[IGANSWER_ALLSTEPIDS]);
                    List<string> lsStepIds = new List<string>();
                    splitParamToList(lsStepIds, IGANSWER_ALLSTEPIDS);
                    lsStepIds[nIdxFrame] = sCurStepIds;
                    lsStepIds.RemoveAt(0);
                    session[IGANSWER_ALLSTEPIDS] = createParamFromList(lsStepIds);
                }
            }
            else
            {
                if ((session[IGANSWER_REQGUIDS] == null) || ((string)session[IGANSWER_REQGUIDS] == ""))
                    session[IGANSWER_REQGUIDS] = GetReqGuid();
                else
                    session[IGANSWER_REQGUIDS] += "," + GetReqGuid();
                SetParameter(IGANSWER_FRAMENAMES, (string)session[IGANSWER_FRAMENAMES]);
                List<string> lsFrameNames = new List<string>();
                splitParamToList(lsFrameNames, IGANSWER_FRAMENAMES);                

                string sFramePath = GetParameterValue(IGANSWER_PICTURENAME);
                string sFrameName = sFramePath.Substring(0, sFramePath.LastIndexOf('.'));
                string sFrameExt = sFramePath.Substring(sFramePath.LastIndexOf('.'));
                int nTestId = 1;
                while (lsFrameNames.Contains(sTestFramePath))
                    sTestFramePath = sFrameName + "(" + (++nTestId).ToString() + ")" + sFrameExt;
                string sLayerVisibility = GetParameterValue(IGANSWER_LAYERVISIBILITY);
                string sNbLayers = GetParameterValue(IGANSWER_NBLAYERS);
                if ((session[IGANSWER_FRAMENAMES] == null) || ((string)session[IGANSWER_FRAMENAMES] == ""))
                {
                    session[IGANSWER_FRAMENAMES] = sTestFramePath;
                    session[IGANSWER_ALLSTEPIDS] = "1";
                    session[IGANSWER_ALLNBLAYERS] = sNbLayers;
                    session[IGANSWER_ALLLAYERVISIBILITY] = sLayerVisibility;                    
                    session[IGANSWER_FRAMEPROPERTIES] = GetParameterValue(IGANSWER_FRAMEPROPERTIES);                    
                }
                else
                {
                    session[IGANSWER_FRAMENAMES] += "," + sTestFramePath;
                    session[IGANSWER_ALLSTEPIDS] += ",1";
                    session[IGANSWER_ALLNBLAYERS] += "," + sNbLayers;
                    session[IGANSWER_ALLLAYERVISIBILITY] += "," + sLayerVisibility;
                    session[IGANSWER_FRAMEPROPERTIES] += "," + GetParameterValue(IGANSWER_FRAMEPROPERTIES);
                }
            }
            SetParameter(IGANSWER_FRAMENAMES, (string)session[IGANSWER_FRAMENAMES]);
            SetParameter(IGANSWER_REQGUIDS, (string)session[IGANSWER_REQGUIDS]);
            SetParameter(IGANSWER_SELECTEDREQGUID, (string)session[IGANSWER_SELECTEDREQGUID]);
            SetParameter(IGANSWER_FRAMEPROPERTIES, (string)session[IGANSWER_FRAMEPROPERTIES]);
            SetParameter(IGANSWER_PICTURENAME, sTestFramePath);
            SetParameter(IGANSWER_ALLSTEPIDS, (string)session[IGANSWER_ALLSTEPIDS]);
            SetParameter(IGANSWER_ALLNBLAYERS, (string)session[IGANSWER_ALLNBLAYERS]);
            SetParameter(IGANSWER_ALLLAYERVISIBILITY, (string)session[IGANSWER_ALLLAYERVISIBILITY]);

            // Process the request specific session updates
            GetRequestParams(session);
        }

        public override void AddRequestParams(HttpSessionState session)
        {
        }
    }
}
