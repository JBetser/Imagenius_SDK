using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Threading;
using System.Xml;
using IGSMLib;

namespace IGPE
{    
    /// <summary>
    /// Stacks the IGPE answers in a list
    /// </summary>
    public class IGPEOutput
    {
        private List<string> m_lOutput = new List<string>();
        private const int IGPEOUTPUT_MAXSTACKITEMS = 100;
        private object m_lockStack = new object();

        public IGPEOutput()
        {
        }

        public void Stack(bool bFullDisplay, IGAnswer answer)
        {
            lock (m_lockStack)
            {
                string sAnswer = "";
                if (bFullDisplay)
                {
                    int nAnswerId = answer.GetId();
                    if ((nAnswerId == IGSMAnswer.IGSMANSWER_ERROR) ||
                        (nAnswerId == (int)IGAnswer.IGANSWER_ID.IGANSWER_FRAME_ACTIONFAILED) ||
                        (nAnswerId == (int)IGAnswer.IGANSWER_ID.IGANSWER_WORKSPACE_ACTIONFAILED))
                        sAnswer = "#";  // error markup
                }
                sAnswer += (bFullDisplay ? answer.ToString() : answer.ToClientOutput());
                m_lOutput.Insert(0, sAnswer);
                while (m_lOutput.Count > IGPEOUTPUT_MAXSTACKITEMS)
                    m_lOutput.RemoveAt(m_lOutput.Count - 1);
            }
        }

        public void StackError(string sError)
        {
            lock (m_lockStack)
            {
                m_lOutput.Insert(0, "#" + sError);
                while (m_lOutput.Count > IGPEOUTPUT_MAXSTACKITEMS)
                    m_lOutput.RemoveAt(m_lOutput.Count - 1);
            }
        }

        public string Download(bool bViewErrorsOnly)
        {
            string sOutput;
            lock (m_lockStack)
            {
                sOutput = m_lOutput.Count > 0 ? "" : "_";
                foreach (string sAnswer in m_lOutput)
                {
                    if (!bViewErrorsOnly || sAnswer.StartsWith("#"))
                        sOutput += sAnswer + "\n";
                }
                m_lOutput.Clear();
            }
            return sOutput;
        }        
    }
}