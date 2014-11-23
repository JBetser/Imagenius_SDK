using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Timers;
using System.Threading;
using System.Security;
using System.Diagnostics;
using System.Configuration;

namespace IGSMLib
{
    public class IGServerLocal : IGServer
    {
        private const string m_sSynchroEventReady = "IGSMServerReady";

        private System.Diagnostics.Process m_process = null;
        private bool m_bReseting = false;
        string m_webServerIP;

        public IGServerLocal(string serverManagerIP, int nPort, string webServerIP)
            : base(serverManagerIP, nPort)
        {
            m_webServerIP = webServerIP;
        }

        unsafe public override bool Initialize()
        {
            if (m_process != null)
            {
                try
                {
                    if (!m_process.HasExited)
                        return true;
                }
                catch { }
            }
            string sCurPort = Convert.ToString(m_endPoint.Port);
            string sServerArgs = sCurPort;
            sServerArgs += " ";
            sServerArgs += m_endPoint.Address.ToString();
            sServerArgs += " ";
            sServerArgs += m_webServerIP;
            sServerArgs += " ";
            sServerArgs += IGServerManager.AppSettings["SDK_PATH"] + IGServerManager.AppSettings["DRIVE_OUTPUT"];
            sServerArgs += " ";
            sServerArgs += IGServerManager.AppSettings["DO_MOUNT"];
            EventWaitHandle hWaitReady = new EventWaitHandle(false, EventResetMode.ManualReset, m_sSynchroEventReady + sCurPort);
            m_process = new System.Diagnostics.Process();
            m_process.StartInfo.FileName = HC.PATH_IGSERVERFOLDER + "/" + HC.PATH_IGSERVEREXECUTABLE;
            m_process.StartInfo.Arguments = sServerArgs;
            m_process.StartInfo.CreateNoWindow = false;
            m_process.StartInfo.UseShellExecute = false;
            m_process.StartInfo.Verb = "Open";
            m_process.StartInfo.WorkingDirectory = HC.PATH_IGSERVERFOLDER;
            if (IGServerManager.AppSettings["DO_MOUNT"] == "Yes")
            {
                m_process.StartInfo.UserName = (string)IGServerManager.AppSettings["USER_NAME"];
                m_process.StartInfo.Domain = (string)IGServerManager.AppSettings["USER_DOMAIN"];
                char[] pwd = ((string)IGServerManager.AppSettings["USER_PASSWORD"]).ToCharArray();
                if (pwd.Length == 0)
                    throw new Exception("No password specified in the app.config");
                fixed (char* pChar = pwd)
                    m_process.StartInfo.Password = new System.Security.SecureString(pChar, pwd.Length);
            }
            m_process.Start();
            if (!hWaitReady.WaitOne(10000, true))
            {
                hWaitReady.Reset();
                return false;
            }
            hWaitReady.Reset();
            m_process.EnableRaisingEvents = true;
            m_process.Exited += new EventHandler(onProcessExited);
            return true;
        }

        public override IGConnection CreateConnection()
        {
            m_connection = new IGConnectionLocal(this);
            return m_connection;
        }

        public override void Disconnect()
        {
            IGServerManager.Instance.DisconnectServer(m_endPoint, true);
        }

        public override void Terminate()
        {
            if (m_process != null)
            {
                try
                {
                    if (m_process.HasExited)
                    {
                        if (m_bReseting)
                            Initialize();
                    }
                    else
                    {
                        m_process.Kill();
                    }
                }
                catch (Exception)
                {
                    if (m_bReseting)
                        Initialize();
                }
            }
            if (!m_bReseting)
                m_connection = null;
        }

        public override void ResetConnection(IGConnection connection)
        {
            m_bReseting = true;
            IGRequestProcessing.Cleanup(m_connection);
            m_connection = connection;
            Terminate();                
        }

        public override IGServer Clone()
        {
            IGServerLocal cloneApp = new IGServerLocal(m_endPoint.Address.ToString(), m_endPoint.Port, m_webServerIP);
            cloneApp.m_process = m_process;
            cloneApp.m_bReseting = m_bReseting;
            cloneApp.m_connection = m_connection;
            return cloneApp;
        }        

        private void onProcessExited(object o, EventArgs eventArgs)
        {
            if (m_connection == null)
                return;
            IGRequestProcessing.Cleanup(m_connection);
            if (m_bReseting)
            {
                m_bReseting = false;
                Initialize();
                m_connection.Initialize();
            }
            else
            {
                IGConnectionLocal resetConnection = (IGConnectionLocal)m_connection;
                if (resetConnection.IsUserConnected() ||
                    resetConnection.IsUserConnecting())
                {
                    IGAnswerDisconnected answerDisconnected = new IGAnswerDisconnected((IGConnectionLocal)m_connection);
                    answerDisconnected.Execute();
                }
                ResetConnection(m_connection);
                m_connection.Initialize();
            }            
        }
    }
}
