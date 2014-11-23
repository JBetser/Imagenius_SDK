using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Xml;
using System.Timers;
using System.Threading;
using System.Text.RegularExpressions;
using IGSMLib;

namespace IGServerTester
{
    public partial class IGServerTester : Form
    {
        private XmlDocument m_xmlDoc;
        private int m_nNbWorkspaceReqs = 0;
        private int m_nNbFrameReqs = 0;
        private int m_nNbServerReqs = 0;

        public IGServerTester()
        {
            InitializeComponent();
            comboRequests.Items.Add("WORKSPACE_CONNECT");
            comboRequests.Items.Add("WORKSPACE_PING");
            comboRequests.Items.Add("WORKSPACE_DISCONNECT");
            comboRequests.Items.Add("WORKSPACE_NEW");
            comboRequests.Items.Add("WORKSPACE_LOAD");
            comboRequests.Items.Add("WORKSPACE_TERMINATE");            
            comboRequests.Items.Add("WORKSPACE_SHOW");
            comboRequests.Items.Add("WORKSPACE_DESTROYACCOUNT");
            m_nNbWorkspaceReqs = comboRequests.Items.Count;            
            comboRequests.Items.Add("FRAME_SAVE");
            comboRequests.Items.Add("FRAME_ADDLAYER");
            comboRequests.Items.Add("FRAME_REMOVELAYER");
            comboRequests.Items.Add("FRAME_MOVELAYER");
            comboRequests.Items.Add("FRAME_MERGELAYER");
            comboRequests.Items.Add("FRAME_FILTER");
            comboRequests.Items.Add("FRAME_CHANGEPROPERTY");
            comboRequests.Items.Add("FRAME_CLOSE");
            comboRequests.Items.Add("FRAME_UPDATE");
            comboRequests.Items.Add("FRAME_SELECT");
            comboRequests.Items.Add("FRAME_SETLAYERVISIBLE");
            comboRequests.Items.Add("FRAME_GOTOHISTORY");
            comboRequests.Items.Add("FRAME_DRAWLINES");
            comboRequests.Items.Add("FRAME_PICKCOLOR");
            comboRequests.Items.Add("FRAME_INDEX");
            comboRequests.Items.Add("FRAME_COPY");
            comboRequests.Items.Add("FRAME_CUT");
            comboRequests.Items.Add("FRAME_PASTE");
            comboRequests.Items.Add("FRAME_DELETE");
            comboRequests.Items.Add("FRAME_SMARTDELETE");
            comboRequests.Items.Add("FRAME_MOVE");
            comboRequests.Items.Add("FRAME_ROTATEANDRESIZE");
            comboRequests.Items.Add("FRAME_SELECTLAYER");
            comboRequests.Items.Add("FRAME_RUBBER");
            m_nNbFrameReqs = comboRequests.Items.Count - m_nNbWorkspaceReqs;  
            comboRequests.Items.Add("SERVER_PING");
            comboRequests.Items.Add("SERVER_GETSTATUS");
            comboRequests.Items.Add("SERVER_TERMINATE");
            comboRequests.Items.Add("SERVER_INITIALIZE");
            comboRequests.Items.Add("SERVER_DESTROYACCOUNT");
            comboRequests.Items.Add("SERVER_ADDIGAPPS");
            comboRequests.Items.Add("SERVER_UPLOADIMAGE");
            comboRequests.Items.Add("SERVER_DOWNLOADIMAGE");
            comboRequests.Items.Add("SERVER_DELETEIMAGE");
            m_nNbServerReqs = comboRequests.Items.Count - m_nNbWorkspaceReqs - m_nNbFrameReqs;  
            comboRequests.SelectedIndex = 0;
            m_xmlDoc = new XmlDocument();
        }

        private void buttonConnect_Click(object sender, EventArgs e)
        {
            if (m_TCPclient != null)
                m_TCPclient.Close();
            if (m_TCPlistener != null)
                m_TCPlistener.Stop();
            try
            {
                IPEndPoint endpoint = new IPEndPoint(0, Convert.ToInt32(textTesterPort.Text));
                m_TCPlistener = new TcpListener(endpoint);
                m_TCPclient = new TcpClient(textServerIP.Text, Convert.ToInt32(textServerPort.Text));
            }
            catch (Exception exc)
            {
                MessageBox.Show("Connection failed.\nException raised: " + exc.ToString(), "Server error");
                Text = "IG Server Tester - Disconnected";
                return;
            }
            Text = "IG Server Tester - Listening Port " + textTesterPort.Text;
            m_nwStream = m_TCPclient.GetStream();
            m_timer = new System.Timers.Timer(100);            

            // Hook up the Elapsed event for the timer.
            m_timer.Elapsed += new ElapsedEventHandler(OnTimedEvent);

            m_timer.Start();
        }

        private byte[] m_buf = new byte[65536];
        private char[] m_bufChars = new char[65536];

        private void OnTimedEvent(object source, ElapsedEventArgs e)
        {
            lock (m_nwStream)
            {
                if (m_nwStream.DataAvailable)
                {
                    m_buf[m_nwStream.Read(m_buf, 0, 65500)] = 0;

                    Encoding encoding = Encoding.ASCII;
                    int nCharsUsed;
                    int nBytesUsed;
                    bool bCompleted;
                    encoding.GetDecoder().Convert(m_buf, 0, m_buf.Length, m_bufChars, 0, m_bufChars.Length, false, out nCharsUsed, out nBytesUsed, out bCompleted);
                    string sText = "";
                    foreach (char c in m_bufChars)
                    {
                        if (c == 0)
                            break;
                        sText += c;
                    }
                    sText += "\n";
                    FilterText(sText);
                }
            }
        }

        delegate void ClearTextCallback();
        delegate void SetTextCallback(string text);

        private void FilterText(string sText)
        {
            List<string> lAnswers = new List<string>();
            int nOffset = 0;
            int nNextOffset = 0;
            while (nOffset != -1)
            {
                nNextOffset = sText.IndexOf("<?xml", nOffset + 1);
                lAnswers.Add((nNextOffset == -1) ? sText.Substring(nOffset) : sText.Substring(nOffset, nNextOffset - nOffset));
                nOffset = nNextOffset;
            }
            while (lAnswers.Count > 0)
            {
                m_xmlDoc.LoadXml(lAnswers[0]);
                if ((checkBoxStatus.Checked || m_xmlDoc.SelectSingleNode("Answer[@Id = \"6001\"]") == null) &&
                    (checkBoxHearthbeat.Checked || (m_xmlDoc.SelectSingleNode("Answer[@Id = \"3004\"]") == null) &&
                                                    (m_xmlDoc.SelectSingleNode("Answer[@Id = \"6004\"]") == null) ) &&
                    (checkBoxProgress.Checked || m_xmlDoc.SelectSingleNode("Answer[@Id = \"4003\"]") == null))
                    AppendText(lAnswers[0]);
                XmlNode frameChanged = m_xmlDoc.SelectSingleNode("Answer[@Id = \"4000\"]");
                if (frameChanged != null)
                    ChangeText(textParam1, frameChanged.FirstChild.Attributes.GetNamedItem("FrameId").Value.ToString());
                lAnswers.RemoveAt(0);
            }
        }

        private void ChangeText(TextBox textBox, string text)
        {
            if (textBox.InvokeRequired)
            {
                ClearTextCallback cl = new ClearTextCallback(textBox.Clear);
                Invoke(cl);
                SetTextCallback ap = new SetTextCallback(textBox.AppendText);
                Invoke(ap, new object[] { text });
            }
            else
            {
                textBox.Text = text;
            }
        }

        private void AppendText(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (richTextBoxAnswer.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(richTextBoxAnswer.AppendText);
                Invoke(d, new object[] { text });
            }
            else
            {
                richTextBoxAnswer.AppendText (text);
            }
        }


        private TcpClient m_TCPclient = null;
        private TcpListener m_TCPlistener = null;
        private NetworkStream m_nwStream = null;
        private System.Timers.Timer m_timer = null;

        private void buttonPing_Click(object sender, EventArgs e)
        {
            IGSMRequestPing pingRequest = new IGSMRequestPing();
            pingRequest.Send(m_TCPclient);
        }
        
        private void buttonAdd_Click(object sender, EventArgs e)
        {
            try
            {
                IGRequest reqAdd = null;
                if (comboRequests.Text.StartsWith("WORKSPACE"))
                {
                    switch (comboRequests.SelectedIndex + 1000)
                    {
                        case IGRequest.IGREQUEST_WORKSPACE_CONNECT:
                            reqAdd = new IGRequestWorkspaceConnect(textBoxUser.Text);
                            break;
                        case IGRequest.IGREQUEST_WORKSPACE_PING:
                            reqAdd = new IGRequestWorkspacePing(textBoxUser.Text);
                            break;
                        case IGRequest.IGREQUEST_WORKSPACE_DISCONNECT:
                            reqAdd = new IGRequestWorkspaceDisconnect(textBoxUser.Text);
                            break;
                        case IGRequest.IGREQUEST_WORKSPACE_NEWIMAGE:
                            reqAdd = new IGRequestWorkspaceNew(textBoxUser.Text, textParam1.Text, textParam2.Text, textParam3.Text, textParam4.Text);
                            break;
                        case IGRequest.IGREQUEST_WORKSPACE_LOADIMAGE:
                            reqAdd = new IGRequestWorkspaceLoad(textBoxUser.Text, textParam1.Text, (textParam2.Text != "" && textParam2.Text != "0") ? textParam2.Text : "", (textParam3.Text != "" && textParam3.Text != "0") ? textParam3.Text : "");
                            break;
                        case IGRequest.IGREQUEST_WORKSPACE_SHOW:
                            reqAdd = new IGRequestWorkspaceShow(textBoxUser.Text, int.Parse(textParam1.Text) == 1);
                            break;
                    }
                }
                if (comboRequests.Text.StartsWith("FRAME"))
                {
                    switch (comboRequests.SelectedIndex - m_nNbWorkspaceReqs + 2000)
                    {
                        case IGRequest.IGREQUEST_FRAME_SAVE:
                            reqAdd = new IGRequestFrameSave(textBoxUser.Text, textParam1.Text, textParam2.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_ADDLAYER:
                            reqAdd = new IGRequestFrameAddLayer(textBoxUser.Text, textParam1.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_REMOVELAYER:
                            reqAdd = new IGRequestFrameRemoveLayer(textBoxUser.Text, textParam1.Text, textParam2.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_MOVELAYER:
                            reqAdd = new IGRequestFrameMoveLayer(textBoxUser.Text, textParam1.Text, textParam2.Text, textParam3.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_MERGELAYER:
                            reqAdd = new IGRequestFrameMergeLayer(textBoxUser.Text, textParam1.Text, textParam2.Text, textParam3.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_FILTER:
                            Dictionary<string, string> dicParams = null;
                            if (textParam4.Text.Contains('_'))
                            {
                                dicParams = textParam4.Text.Split(',').ToDictionary(str => str.Split('_')[0], str => str.Split('_')[1]);
                                if (dicParams.ContainsKey("PosX"))
                                    dicParams["PosX"] = fromPctToIGCoords(dicParams["PosX"]);
                                if (dicParams.ContainsKey("PosY"))
                                    dicParams["PosY"] = fromPctToIGCoords(dicParams["PosY"]);
                                if (dicParams.ContainsKey("DirectionX"))
                                    dicParams["DirectionX"] = fromPctToIGCoords(dicParams["DirectionX"]);
                                if (dicParams.ContainsKey("DirectionY"))
                                    dicParams["DirectionY"] = fromPctToIGCoords(dicParams["DirectionY"]);
                            }
                            reqAdd = new IGRequestFrameFilterLayer(textBoxUser.Text, textParam1.Text, textParam2.Text, textParam3.Text, dicParams);
                            break;
                        case IGRequest.IGREQUEST_FRAME_CHANGEPROPERTY:
                            reqAdd = new IGRequestChangeProperty(textBoxUser.Text, textParam1.Text, textParam2.Text, textParam3.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_CLOSE:
                            reqAdd = new IGRequestFrameClose(textBoxUser.Text, textParam1.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_UPDATE:
                            reqAdd = new IGRequestFrameUpdate(textBoxUser.Text, textParam1.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_SELECT:
                            reqAdd = new IGRequestFrameSelect(textBoxUser.Text, textParam1.Text, textParam2.Text, textParam3.Text, fromPctToIGCoords(textParam4.Text, false));
                            break;
                        case IGRequest.IGREQUEST_FRAME_SETLAYERVISIBLE:
                            reqAdd = new IGRequestFrameSetLayerVisible(textBoxUser.Text, textParam1.Text, textParam2.Text, textParam3.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_GOTOHISTORY:
                            reqAdd = new IGRequestFrameGotoHistory(textBoxUser.Text, textParam1.Text, textParam2.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_DRAWLINES:
                            reqAdd = new IGRequestFrameDrawLines(textBoxUser.Text, textParam1.Text, textParam2.Text, fromPctToIGCoords(textParam3.Text), textParam4.Text, "10");
                            break;
                        case IGRequest.IGREQUEST_FRAME_PICKCOLOR:
                            reqAdd = new IGRequestFramePickColor(textBoxUser.Text, textParam1.Text, textParam2.Text, fromPctToIGCoords(textParam3.Text));
                            break;
                        case IGRequest.IGREQUEST_FRAME_INDEX:
                            Regex RE = new Regex(@"[,]+");
                            string[] tParams2 = RE.Split(textParam2.Text);
                            if (tParams2.Length == 2)
                                reqAdd = new IGRequestFrameIndexLayer(textBoxUser.Text, textParam1.Text, tParams2[0], tParams2[1]);
                            else
                                reqAdd = new IGRequestFrameIndexLayer(textBoxUser.Text, textParam1.Text, tParams2[0], tParams2[1], tParams2[2], tParams2[3],
                                                                        fromPctToIGCoords(textParam3.Text), fromPctToIGCoords(textParam4.Text));
                            break;
                        case IGRequest.IGREQUEST_FRAME_COPY:
                            reqAdd = new IGRequestFrameCopy(textBoxUser.Text, textParam1.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_CUT:
                            reqAdd = new IGRequestFrameCut(textBoxUser.Text, textParam1.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_PASTE:
                            reqAdd = new IGRequestFramePaste(textBoxUser.Text, textParam1.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_DELETE:
                            reqAdd = new IGRequestFrameDelete(textBoxUser.Text, textParam1.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_SMARTDELETE:
                            reqAdd = new IGRequestFrameSmartDelete(textBoxUser.Text, textParam1.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_MOVE:
                            reqAdd = new IGRequestFrameMove(textBoxUser.Text, textParam1.Text, textParam2.Text, fromPctToIGCoords(textParam3.Text), fromPctToIGCoords(textParam4.Text));
                            break;
                        case IGRequest.IGREQUEST_FRAME_ROTATEANDRESIZE:
                            reqAdd = new IGRequestFrameRotateAndResize(textBoxUser.Text, textParam1.Text, textParam2.Text, textParam3.Text, fromPctToIGCoords(textParam4.Text));
                            break;
                        case IGRequest.IGREQUEST_FRAME_SELECTLAYER:
                            reqAdd = new IGRequestFrameSelectLayer(textBoxUser.Text, textParam1.Text, textParam2.Text);
                            break;
                        case IGRequest.IGREQUEST_FRAME_RUBBER:
                            reqAdd = new IGRequestFrameRubber(textBoxUser.Text, textParam1.Text, textParam2.Text, fromPctToIGCoords(textParam3.Text), textParam4.Text, "10");
                            break;
                    }
                }
                if (comboRequests.Text.StartsWith("SERVER"))
                {
                    switch ((IGSMRequest.IGSMREQUEST_ID)(comboRequests.SelectedIndex - m_nNbWorkspaceReqs - m_nNbFrameReqs + 5000))
                    {
                        case IGSMRequest.IGSMREQUEST_ID.IGSMREQUEST_PING:
                            reqAdd = new IGSMRequestPing();
                            break;
                        case IGSMRequest.IGSMREQUEST_ID.IGSMREQUEST_GETSTATUS:
                            reqAdd = new IGSMRequestGetStatus((IGSMStatus.IGSMStatusType)int.Parse(textParam1.Text));
                            break;
                        case IGSMRequest.IGSMREQUEST_ID.IGSMREQUEST_TERMINATE:
                            reqAdd = new IGSMRequestTerminate(int.Parse(textParam1.Text) == 1);
                            break;
                        case IGSMRequest.IGSMREQUEST_ID.IGSMREQUEST_DESTROYACCOUNT:
                            reqAdd = new IGSMRequestDestroyAccount(textBoxUser.Text);
                            break;

                    }
                }
                if (reqAdd == null)
                {
                    MessageBox.Show("Request " + comboRequests.SelectedText + " is not implemented");
                    return;
                }
                reqAdd.SetDisplay(reqAdd.GetDisplay().Replace('\n', ' '));
                listRequestBuffer.Items.Add(reqAdd);
            }
            catch (Exception exc)
            {
                MessageBox.Show(exc.ToString(), "Wrong request format");
            }
        }

        private void buttonSend_Click(object sender, EventArgs e)
        {
            foreach (IGRequest request in listRequestBuffer.Items)
            {
                request.Send(m_TCPclient);
            }
            listRequestBuffer.Items.Clear();
        }

        private void buttonClear_Click(object sender, EventArgs e)
        {
            listRequestBuffer.Items.Clear();
        }

        private void buttonRemove_Click(object sender, EventArgs e)
        {
            foreach (IGRequest request in listRequestBuffer.Items)
            {
                if (listRequestBuffer.GetSelected(listRequestBuffer.Items.IndexOf(request)))
                    listRequestBuffer.Items.Remove(request);
            }
        }

        private void textBoxUser_TextChanged(object sender, EventArgs e)
        {
        }

        private void IGServerTester_Load(object sender, EventArgs e)
        {
        }

        private void buttonClearBuffer_Click(object sender, EventArgs e)
        {
            richTextBoxAnswer.Clear();
        }

        private string fromPctToIGCoords(string sPctCoords, bool bConvertLast = true)
        {
            // convert percentage to imagenius coordinates (*10000)
            Regex RE = new Regex(@"[,]+");
            string[] tCoordinates = RE.Split(sPctCoords);
            string sConvertedCoords = "";
            for (int idxCoord = 0; idxCoord < tCoordinates.Length; idxCoord++)
            {
                if (bConvertLast || (idxCoord < tCoordinates.Length - 1))
                    tCoordinates[idxCoord] = (int.Parse(tCoordinates[idxCoord]) * 10000).ToString();
                sConvertedCoords += tCoordinates[idxCoord];
                if (idxCoord < tCoordinates.Length - (bConvertLast ? 0 : 1))
                    sConvertedCoords += ",";
            }
            if (sConvertedCoords != "" && sConvertedCoords[sConvertedCoords.Length - 1] == ',')
                sConvertedCoords = sConvertedCoords.Substring(0, sConvertedCoords.Length - 1);
            return sConvertedCoords;
        }
    }
}
