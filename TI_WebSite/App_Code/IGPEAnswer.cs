using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Web;
using System.Web.SessionState;
using System.Xml;
using IGSMLib;
using IGMadam;

namespace IGPE
{
    /// <summary>
    /// Summary description for IGPEAnswer
    /// </summary>
    public static class IGPEAnswer
    {
        public static bool OnProcessAnswer(List<object> adminSessionArray, HttpSessionState userSession, IGAnswer answer)
        {
            int nAnswerId = answer.GetId();
            XmlNode xmlNodeRequestGuid = answer.GetAttribute(IGAnswer.IGANSWER_REQGUID);
            IGPEOutput output = null;
            if (adminSessionArray != null)
            {
                foreach (HttpSessionState adminSession in adminSessionArray)
                {
                    //  stack output for admins
                    if ((nAnswerId != IGSMAnswer.IGSMANSWER_GETSTATUS) &&
                        (nAnswerId != (int)IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_HEARTHBEAT))
                    {
                        output = (IGPEOutput)adminSession[IGPEMultiplexing.SESSIONMEMBER_OUTPUT];
                        if (output != null)
                            output.Stack(true, answer);
                    }
                }
            }

            // process server answer
            if (nAnswerId >= IGSMAnswer.IGSMANSWER_ERROR)
                stackError(adminSessionArray, answer.ToString());

            if (userSession == null)
                return false;

            // process user answer
            if (nAnswerId != (int)IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_DISCONNECTED)
            {
                // update user session
                answer.GetParams(userSession);

                // stack output for current user
                output = (IGPEOutput)userSession[IGPEMultiplexing.SESSIONMEMBER_OUTPUT];
                if (output != null)
                    output.Stack(false, answer);
            }
            return true;
        }

        private static void stackError(List<object> adminSessionArray, string errorReason)
        {
            foreach (HttpSessionState adminSession in adminSessionArray)
            {
                IGPEOutput userOutput = (IGPEOutput)adminSession[IGPEMultiplexing.SESSIONMEMBER_OUTPUT];
                userOutput.StackError("Server error: " + errorReason);
            }
        }
    }
}