using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Text.RegularExpressions;
using System.Net.Sockets;
using System.Runtime.Serialization;

namespace IGSMLib
{
    [DataContract]
    public abstract class IGXmlMessage
    {
        protected XmlDocument m_xmlDoc = null;
        protected IGServerManager m_serverMgr = null;
        protected string m_sString = "";
        private string m_sDisplay = "";
        private bool m_bUpdateDisplay = true;
        
        public IGXmlMessage(XmlDocument xmlDoc, string sMessageString)
        {
            m_serverMgr = IGServerManager.Instance;
            m_xmlDoc = xmlDoc;
            m_sString = sMessageString;
        }
        
        public IGXmlMessage(string sXMLTemplate)
        {
            m_serverMgr = IGServerManager.Instance;
            m_xmlDoc = new XmlDocument();
            m_xmlDoc.LoadXml(sXMLTemplate);            
        }

        public virtual bool Silent
        {
            get
            {
                return false;
            }
        } 

        public abstract XmlNode GetXmlNode();
        public abstract string GetXml();

        private void updateDisplay()
        {
            if (m_bUpdateDisplay)
            {
                m_bUpdateDisplay = false;
                XmlNode xmlNode = GetXmlNode();
                if (xmlNode == null)
                {
                    m_sDisplay = "ERROR";
                    return;
                }
                m_sDisplay = "\nATTRIBUTES {";
                foreach (XmlAttribute att in xmlNode.Attributes)
                    m_sDisplay += " " + att.Name + "=\"" + att.Value + "\"";
                m_sDisplay += " }\nPARAMS {";
                XmlNode xmlParams = GetParameters();
                if (xmlParams != null)
                {
                    if (xmlParams.Attributes.Count > 0)
                    {
                        foreach (XmlAttribute att in xmlParams.Attributes)
                            m_sDisplay += " " + att.Name + "=\"" + att.Value + "\"";
                        m_sDisplay += " }";
                        return;
                    }
                }
                m_sDisplay += " EMPTY }";
            }
        }

        public void SetParameter(string sName, string sValue)
        {
            XmlNode paramNode = GetParameters();
            if (paramNode == null)
                return;
            XmlNode attNode = paramNode.Attributes.GetNamedItem(sName);
            if (attNode == null)
            {
                XmlAttribute newAtt = m_xmlDoc.CreateAttribute(sName);
                newAtt.Value = sValue;
                paramNode.Attributes.Append(newAtt);
            }
            else
            {
                attNode.Value = sValue;
            }
            m_bUpdateDisplay = true;
        }

        public XmlNode GetAttribute(string sAtt)
        {
            XmlNode xmlNode = GetXmlNode();
            if (xmlNode != null)
                return xmlNode.Attributes.GetNamedItem(sAtt);
            return null;
        }

        public string GetAttributeValue(string sAtt)
        {
            XmlNode xmlAtt = GetAttribute(sAtt);
            if (xmlAtt != null)
                return xmlAtt.Value;
            return null;
        }

        public void SetAttribute(string sAtt, string sValue)
        {
            XmlNode xmlNode = GetXmlNode();
            if (xmlNode != null)
            {
                XmlNode xmlAtt = xmlNode.Attributes.GetNamedItem(sAtt);
                if (xmlAtt == null)
                {
                    XmlAttribute newAtt = m_xmlDoc.CreateAttribute(sAtt);
                    newAtt.Value = sValue;
                    xmlNode.Attributes.Append(newAtt);
                }
                else
                {
                    xmlAtt.Value = sValue;
                }
            }
            m_bUpdateDisplay = true;
        }

        public XmlNode GetParameters()
        {
            XmlNode xmlNode = GetXmlNode();
            if (xmlNode != null)
                return xmlNode.FirstChild;
            return null;
        }

        public XmlNode GetParameter(string sParam)
        {
            XmlNode paramNode = GetParameters();
            if (paramNode != null)
                return paramNode.Attributes.GetNamedItem(sParam);
            return null;
        }

        public string GetParameterValue(string sParam)
        {
            XmlNode paramNode = GetParameter(sParam);
            if (paramNode != null)
                return paramNode.Value;
            return null;
        }

        public override string ToString()
        {
            updateDisplay();
            return m_sString + m_sDisplay;
        }

        public string GetTitle()
        {
            return m_sString;
        }

        public string GetDisplay()
        {
            updateDisplay();
            return m_sDisplay;
        }

        public void SetTitle(string Str)
        {
            m_sString = Str;
        }

        public void SetDisplay(string Str)
        {
            m_sDisplay = Str;
            m_bUpdateDisplay = false;
        }

        public bool Send(TcpClient tcpClient)
        {
            try
            {
                // create a char array to hold string...
                char[] ca = m_xmlDoc.InnerXml.ToCharArray();
                // create a byte arry to hold chars...
                byte[] ba = new byte[ca.Length];
                // loop thru chars and convert to byte...
                for (int i = 0; i < ca.Length; i++)
                {
                    // convert char to byte and place in byte array...
                    ba[i] = Convert.ToByte(ca[i]);
                }
                tcpClient.Client.Send(ba);
            }
            catch (Exception exc)
            {
                if (m_serverMgr != null)
                    m_serverMgr.AppendError(exc.ToString());
                return false;
            }
            return true;
        }

        protected string[] splitParam(string sParamId)
        {
            Regex RE = new Regex(@"[,]+");
            List<string> lsParams = new List<string>();
            string sParamValue = GetParameterValue(sParamId);
            if ((sParamValue == null) || (sParamValue == ""))
                return null;
            sParamValue = sParamValue.Replace('\\', '/');
            return RE.Split(sParamValue);
        }

        protected void splitParamToList(List<string> lsParams, string sParamId, bool bPrefixLength = true)
        {
            string[] tParams = splitParam(sParamId);
            if ((tParams != null) && (tParams.Length > 0))
            {
                if (bPrefixLength)
                    lsParams.Add(tParams.Length.ToString());
                foreach (string token in tParams)
                    lsParams.Add(token);
            }
            else
            {
                if (bPrefixLength)
                    lsParams.Add("0");
            }
        }

        protected void splitParamToList(List<KeyValuePair<int, int>> lsParams, string sParamId)
        {
            string[] tParams = splitParam(sParamId);
            if ((tParams != null) && (tParams.Length > 0))
            {
                for (int idxToken = 0; idxToken < tParams.Length; idxToken+=2)
                    lsParams.Add(new KeyValuePair<int, int>(int.Parse(tParams[idxToken]), int.Parse(tParams[idxToken+1])));
            }
        }

        protected string createParamFromList(List<string> lsParams)
        {
            return createParamFromList(lsParams, ',');
        }

        protected string createParamFromList(List<KeyValuePair<int, int>> lParams)
        {
            return createParamFromList(lParams, ',');
        }

        protected string createParamFromList(List<string> lsParams, char token)
        {
            string sRet = "";
            foreach (string sParam in lsParams)
            {
                if (sRet != "")
                    sRet += token.ToString();
                sRet += sParam;
            }
            return sRet;
        }

        protected string createParamFromList(List<KeyValuePair<int, int>> lParams, char token)
        {
            string sRet = "";
            foreach (KeyValuePair<int, int> param in lParams)
            {
                if (sRet != "")
                    sRet += token.ToString();
                sRet += param.Key.ToString();
                sRet += token.ToString() + param.Value.ToString();
            }
            return sRet;
        }
    }
}
