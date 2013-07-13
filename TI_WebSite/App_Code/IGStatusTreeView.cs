using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Xml;
using System.Threading;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using IGSMLib;

namespace IGPE
{
    /// <summary>
    /// Summary description for IGSMStatusTreeView
    /// </summary>
    public class IGSMStatusTreeView
    {
        private const string IGSMSTATUS_TREEVIEWROOT = "WebServer Status";
        private TreeNodeCollection m_treeNodeCollectionStatus = new TreeNodeCollection();
        private object m_lockServerList = new object();

        public IGSMStatusTreeView()
        {
        }

        public IGSMStatusTreeView(TreeNodeCollection treeNodes)
        {
            m_treeNodeCollectionStatus = copyTreeNodes(treeNodes);
        }

        private TreeNodeCollection copyTreeNodes(TreeNodeCollection copyNodes)
        {
            TreeNodeCollection treeNodeCollectionStatus = new TreeNodeCollection();
            foreach (TreeNode treeNode in copyNodes)
                treeNodeCollectionStatus.Add(CopyTreeNode(treeNode));
            return treeNodeCollectionStatus;
        }

        public IGSMStatusTreeView GetCopy()
        {
            lock (m_lockServerList)
            {
                return new IGSMStatusTreeView(m_treeNodeCollectionStatus);
            }
        }
        
        public static TreeNode CopyTreeNode(TreeNode source)
        {
            TreeNode nodeDest = new TreeNode(source.Text, source.Value);
            foreach (TreeNode sourceChild in source.ChildNodes)
                nodeDest.ChildNodes.Add(CopyTreeNode(sourceChild));
            return nodeDest;
        }

        protected delegate int AddNodeCallback(TreeNode newNode);
        protected delegate void RemoveNodeCallback();
        protected delegate void UpdateNodeTextCallback(TreeNode newNode, string sText);

        protected TreeNode updateTreeNode(TreeNode nodeParent, string sNodeName)
        {
            return updateTreeNode(nodeParent, sNodeName, "", false, true);
        }

        protected TreeNode updateTreeNode(TreeNode nodeParent, string sNodeName, string sNodeValue)
        {
            return updateTreeNode(nodeParent, sNodeName, sNodeValue, false, false);
        }

        protected TreeNode updateTreeNode(TreeNode nodeParent, string sNodeName, string sNodeValue, bool bGetNode)
        {
            return updateTreeNode(nodeParent, sNodeName, sNodeValue, bGetNode, false);
        }

        protected TreeNode updateTreeNode(TreeNode nodeParent, string sNodeName, string sNodeValue, bool bGetNode, bool bNameOnly)
        {
            string sCurNodeName;
            string sCurNodeValue;
            TreeNode updateNode = null;
            string sUpdateNodeText;
            foreach (TreeNode treeNode in nodeParent.ChildNodes)
            {
                sCurNodeName = (bNameOnly) ? treeNode.Text : treeNode.Text.Substring(0, treeNode.Text.IndexOf(":"));
                if (sCurNodeName == sNodeName)
                {
                    if (sNodeValue == null)
                        return treeNode;
                    if (!bGetNode)
                    {
                        updateNode = treeNode;
                        sUpdateNodeText = sCurNodeName;
                        break;
                    }
                    // in get node mode, not necessary to update value if it is the same
                    sCurNodeValue = treeNode.Text.Substring(treeNode.Text.IndexOf(":") + 2);
                    if (sCurNodeValue == sNodeValue)
                        return treeNode;
                }
            }
            if (updateNode == null)
            {
                updateNode = new TreeNode(sNodeName + (bNameOnly ? "" : ": " + sNodeValue));
                nodeParent.ChildNodes.Add(updateNode);
            }
            else
            {
                updateNode.Text = sNodeName + (bNameOnly ? "" : ": " + sNodeValue);
            }
            return updateNode;
        }

        public void UpdateServerList(Hashtable serverStates)
        {
            lock (m_lockServerList)
            {
                // remove non-referenced servers
                List<TreeNode> lRemoveServerNodeList = new List<TreeNode>();
                foreach (TreeNode serverNode in m_treeNodeCollectionStatus)
                {
                    if (!serverStates.ContainsValue(serverNode.Value))
                        lRemoveServerNodeList.Add(serverNode);
                }
                foreach (TreeNode serverNode in lRemoveServerNodeList)
                {
                    m_treeNodeCollectionStatus.Remove(serverNode);
                }
                // add newly referenced servers
                IDictionaryEnumerator enumServers = serverStates.GetEnumerator();
                while (enumServers.MoveNext())
                {
                    string serverName = IGConfigManagerRemote.GetInstance().GetServerName(((IGServerRemote)enumServers.Key).m_sIpEndPoint);
                    bool bServerExists = false;
                    foreach (TreeNode serverNode in m_treeNodeCollectionStatus)
                    {
                        if (serverNode.Text == serverName)
                        {
                            bServerExists = true;
                            break;
                        }
                    }
                    if (!bServerExists)
                    {
                        m_treeNodeCollectionStatus.Add(new TreeNode(serverName));
                    }
                }
                // update server state
                foreach (TreeNode serverNode in m_treeNodeCollectionStatus)
                {
                    IGServer server = IGConfigManagerRemote.GetInstance().GetNamedServer(serverNode.Value);
                    updateTreeNode(serverNode, IGSMStatus.IGSMSTATUS_SERVERSTATE, (bool)serverStates[server] ? "Ready" : "Not responding");
                }
            }
        }

        public string GetServerList()
        {
            lock (m_lockServerList)
            {
                string sHtmlResult = "";
                // add newly referenced servers
                foreach (TreeNode serverNode in m_treeNodeCollectionStatus)
                {
                    sHtmlResult += serverNode.Value;
                    sHtmlResult += "?";
                }
                return sHtmlResult;
            }
        }

        public string GetServerStatus(string serverName)
        {
            lock (m_lockServerList)
            {
                string sHtmlResult = "";
                foreach (TreeNode serverNode in m_treeNodeCollectionStatus)
                {
                    if (serverNode.Value == serverName)
                    {
                        foreach (TreeNode treeNode in serverNode.ChildNodes)
                        {
                            sHtmlResult += treeNode.Value + "\n";
                            if (treeNode.Text == IGSMStatus.IGSMSTATUS_USERS + ": ")
                            {
                                foreach (TreeNode userNode in treeNode.ChildNodes)
                                {
                                    sHtmlResult += "   " + userNode.Value + "\n";
                                }
                            }
                        }
                        break;
                    }
                }
                return sHtmlResult;
            }
        }

        public void UpdateServer(XmlNode xmlNodeStatusServer)
        {
            lock (m_lockServerList)
            {
                if (xmlNodeStatusServer == null || xmlNodeStatusServer.Attributes == null)
                    return;
                TreeNode serverNode = null;
                XmlNode serverNodeIP = xmlNodeStatusServer.Attributes.GetNamedItem(IGSMStatus.IGSMSTATUS_COMPUTERIP);
                XmlNode serverNodePort = xmlNodeStatusServer.Attributes.GetNamedItem(IGSMStatus.IGSMSTATUS_COMPUTERPORT);
                if (serverNodeIP == null || serverNodePort == null)
                    return;
                // retrieve server node
                foreach (TreeNode enumServerNode in m_treeNodeCollectionStatus)
                {
                    IGServer server = IGConfigManagerRemote.GetInstance().GetNamedServer(enumServerNode.Value);
                    if (server.m_sIpEndPoint == (serverNodeIP.Value + ":" + serverNodePort.Value))
                    {
                        serverNode = enumServerNode;
                        break;
                    }
                }
                if (serverNode == null)
                    return;
                // update server main attributes
                foreach (XmlAttribute serverStatusAttribute in xmlNodeStatusServer.Attributes)
                    updateTreeNode(serverNode, serverStatusAttribute.Name, serverStatusAttribute.Value);
                if (xmlNodeStatusServer.FirstChild == null)
                    return;
                if (xmlNodeStatusServer.FirstChild.ChildNodes == null)
                    return;
                TreeNode usersTreeNode = updateTreeNode(serverNode, IGSMStatus.IGSMSTATUS_USERS, "", true, false);
                // remove disconnected users
                List<TreeNode> lDisconnectedUsers = new List<TreeNode>();
                foreach (TreeNode userNode in usersTreeNode.ChildNodes)
                {
                    bool bUserDisconnected = true;
                    foreach (XmlNode enumUserNode in xmlNodeStatusServer.FirstChild.ChildNodes)
                    {
                        if (enumUserNode.Value == userNode.Value)
                        {
                            bUserDisconnected = false;
                            break;
                        }
                    }
                    if (bUserDisconnected)
                        lDisconnectedUsers.Add(userNode);
                }
                foreach (TreeNode disconnectedUserNode in lDisconnectedUsers)
                    usersTreeNode.ChildNodes.Remove(disconnectedUserNode);
                // update user nodes
                foreach (XmlNode userNode in xmlNodeStatusServer.FirstChild.ChildNodes)
                {
                    if (userNode.Attributes == null)
                        continue;
                    TreeNode userTreeNode = updateTreeNode(usersTreeNode, userNode.Attributes.GetNamedItem(IGRequest.IGREQUEST_USERLOGIN).Value, "", true, true);
                    foreach (XmlAttribute userStatusAttribute in userNode.Attributes)
                    {
                        if (userStatusAttribute.Name != IGRequest.IGREQUEST_USERLOGIN)
                            updateTreeNode(userTreeNode, userStatusAttribute.Name, userStatusAttribute.Value);
                    }
                    XmlNodeList imageNodeList = userNode.SelectNodes("./Image");
                    if (imageNodeList == null)
                        continue;
                    foreach (XmlNode imageNode in imageNodeList)
                    {
                        if (imageNode.Attributes == null)
                            continue;
                        TreeNode imageTreeNode = updateTreeNode(userTreeNode, IGSMStatus.IGSMSTATUS_IMAGE, imageNode.Attributes.GetNamedItem(IGRequest.IGREQUEST_FRAMEID).Value, true);
                        foreach (XmlAttribute imageStatusAttribute in imageNode.Attributes)
                        {
                            if (imageStatusAttribute.Name != IGRequest.IGREQUEST_FRAMEID)
                                updateTreeNode(imageTreeNode, imageStatusAttribute.Name, imageStatusAttribute.Value);
                        }
                    }
                }
            }
        }

        public void RemoveUser(string sUser)
        {
            //TreeNode userTreeNode = updateTreeNode(m_nodeRoot, IGSMStatus.IGSMSTATUS_USER, sUser, true);
            //userTreeNode.Parent.ChildNodes.Remove(userTreeNode);
        }

        public void RemoveFrame(string sUser, string sFrameId)
        {
            //TreeNode userTreeNode = updateTreeNode(m_nodeRoot, IGSMStatus.IGSMSTATUS_USER, sUser, true);
            //TreeNode imageTreeNode = updateTreeNode(userTreeNode, IGSMStatus.IGSMSTATUS_IMAGE, sFrameId, true);
            //userTreeNode.ChildNodes.Remove(imageTreeNode);
        }

        public override string ToString()
        {
            return "IGSMStatusTreeView";
        }

        public string GetUserStatus(string sServer, string userName)
        {
            lock (m_lockServerList)
            {
                string sHtmlResult = "";
                foreach (TreeNode serverNode in m_treeNodeCollectionStatus)
                {
                    if (serverNode.Value == sServer)
                    {
                        foreach (TreeNode treeNode in serverNode.ChildNodes)
                        {
                            if (treeNode.Text == IGSMStatus.IGSMSTATUS_USERS + ": ")
                            {
                                foreach (TreeNode userNode in treeNode.ChildNodes)
                                {
                                    if (userNode.Value == userName)
                                    {
                                        foreach (TreeNode userNodeAttributes in userNode.ChildNodes)
                                        {
                                            sHtmlResult += userNodeAttributes.Value + "\n";
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
                return sHtmlResult;
            }
        }
    }
}