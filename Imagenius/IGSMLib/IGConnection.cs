using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Diagnostics;
using System.Threading;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace IGSMLib
{
    public abstract class IGConnection
    {
        public NetworkStream m_nwStream = null;
        public IGServer m_igServer = null;
        public string m_sUser = null;

        protected IGServerManager m_serverMgr = null;
        private byte[] m_answerBuf = new byte[IGServerManager.ANSWER_BUFSIZE];
        private string m_curAnswer = "";
        private bool m_bProcessingAnswers = false;
        private object m_lockProcessAnswers = new object();

        public IGConnection(IGServer igServer)
        {
            m_serverMgr = IGServerManager.Instance;
            m_igServer = igServer;
        }

        public abstract void Initialize();
        public abstract void Terminate(string sGuid);
        public abstract void Reset(string sGuid);
        public abstract bool IsLocal();

        public abstract bool SendRequest(IGRequest request);

        public string GetName()
        {
            return m_sUser;
        }   

        public void ProcessAnswers()
        {
            processAnswers(false);
        }        

        public virtual bool Ping()
        {
            return false;
        }

        public bool IsFree()
        {
            return (m_sUser == null);
        }

        protected void flush()
        {
            processAnswers(true);
        }

        public abstract string GetStatus();

        private void processAnswers(bool bFlush)
        {
            if (m_bProcessingAnswers)
                return;
            m_bProcessingAnswers = true;
            lock (m_lockProcessAnswers)
            {
                List<string> lAnswers = new List<string>();
                if (m_nwStream.DataAvailable)
                {
                    byte[] buf = new byte[0];
                    int nBytesRead;
                    do
                    {
                        nBytesRead = 0;
                        nBytesRead = m_nwStream.Read(m_answerBuf, 0, IGServerManager.PACKETDATA_SIZE);
                        List<byte> dataBuf = m_answerBuf.ToList();
                        dataBuf.RemoveRange(nBytesRead, dataBuf.Count - nBytesRead);
                        buf = buf.Concat(dataBuf).ToArray();
                    } while (nBytesRead == IGServerManager.PACKETDATA_SIZE);
                    nBytesRead = buf.Length;
                    if (bFlush)
                    {
                        m_bProcessingAnswers = false;
                        return;
                    }
                    if (nBytesRead > 0)
                    {
                        Encoding encoding = Encoding.ASCII;
                        char[] bufChars = new char[nBytesRead];
                        int nCharsUsed;
                        int nBytesUsed;
                        bool bCompleted;
                        encoding.GetDecoder().Convert(buf, 0, nBytesRead, bufChars, 0, nBytesRead, false, out nCharsUsed, out nBytesUsed, out bCompleted);
                        string sText = "";
                        foreach (char c in bufChars)
                        {
                            if (c == 0)
                                break;
                            sText += c;
                        }
                        int nOffset = 0;
                        int nNextOffset = 0;
                        while (nOffset != -1 && sText.Length > nOffset)
                        {
                            nNextOffset = sText.IndexOf("/Answer>", nOffset + 1);
                            if (nNextOffset == -1)
                            {
                                m_curAnswer += sText.Substring(nOffset);
                                break;
                            }
                            else
                            {
                                m_curAnswer += sText.Substring(nOffset, nNextOffset + 8 - nOffset);
                                lAnswers.Add(m_curAnswer);
                                m_curAnswer = "";
                                nOffset = nNextOffset + 8;
                            }                            
                        }
                        while (lAnswers.Count > 0)
                        {
                            IGAnswer answer = null;
                            try
                            {
                                answer = m_serverMgr.CreateAnswer(lAnswers[0], this);
                                bool bOk = (answer == null) ? false : answer.Execute();
                                if (bOk)
                                    m_serverMgr.AddProcessedRequest(answer.GetReqGuid(), answer);
                                else
                                    m_serverMgr.AppendError("- processAnswers failed processing answer from user \""
                                                                + ((m_sUser == null) ? "Unknown" : m_sUser) + "\" answer: \""
                                                                + ((answer == null) ? "Unknown" : answer.ToString()) + "\"");
                                lAnswers.RemoveAt(0);
                            }
                            catch (Exception exc)
                            {
                                lAnswers.RemoveAt(0);
                                m_serverMgr.AppendError("- processAnswers failed processing answer from user \""
                                                                + ((m_sUser == null) ? "Unknown" : m_sUser) + "\" answer: \""
                                                                + ((answer == null) ? "Unknown" : answer.ToString()) + "\" exception: \""
                                                                + exc.ToString() + "\".");
                            }
                        }
                    }
                }
            }
            m_bProcessingAnswers = false;
        }
    }
}
