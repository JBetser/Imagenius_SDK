// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

using System;
using System.Reflection;
using IGSMLib;
using System.Collections.Generic;
using System.Diagnostics;
using System.Configuration;
using IGSMDesktopIce;
using System.Threading;

public class Server
{
    public class App : Ice.Application
    {
        IGServerManagerLocal m_serverMgr = null;
        public static string s_sServerManagerPort;

        private EventLog m_logMgr = null;

        public override int run(string[] args)
        {
            if (args.Length != 0)
                throw new ApplicationException("starting: too many arguments in application call.");

            if (!EventLog.SourceExists("IGSMDesktop"))
                EventLog.CreateEventSource(new EventSourceCreationData("IGSMDesktop", "IGSMService"));
            m_logMgr = new EventLog("IGSMService", Environment.MachineName, "IGSMDesktop");
            m_logMgr.WriteEntry("Server manager started", EventLogEntryType.Information);

            try
            {
                s_sServerManagerPort = IGSMDesktopIce.Properties.Settings.Default.SERVERMGR_PORT_INPUT;

                Dictionary<string, string> dicSettings = new Dictionary<string, string>();
                foreach (SettingsPropertyValue prop in IGSMDesktopIce.Properties.Settings.Default.PropertyValues)
                    dicSettings.Add(prop.Name, (string)prop.PropertyValue);
                m_serverMgr = (IGServerManagerLocal)IGServerManager.CreateInstanceLocal(dicSettings, Convert.ToInt32(s_sServerManagerPort));
                string sIPShare = IGSMDesktopIce.Properties.Settings.Default.SERVERMGR_IPSHARE;
                int nFirstPort = Convert.ToInt32(IGSMDesktopIce.Properties.Settings.Default.SERVERMGR_PORT_INPUT) + 1;
                int nNbServers = 1; // start the application with one server
                List<IGServer> lServerPorts = new List<IGServer>();
                for (int idxPort = 0; idxPort < nNbServers; idxPort++)
                    lServerPorts.Add(new IGServerLocal(IGSMDesktopIce.Properties.Settings.Default.IP_LOCAL, nFirstPort + idxPort, IGSMDesktopIce.Properties.Settings.Default.SERVERMGR_IPWEBSERVER));
                m_serverMgr.ErrorEvent += new IGServerManager.ErrorHandler(OnError);
                m_serverMgr.Initialize(IGSMDesktopIce.Properties.Settings.Default.SERVERMGR_IPWEBSERVER, sIPShare, lServerPorts);
                
                Ice.ObjectAdapter adapter = communicator().createObjectAdapter("IGServerControllerIce");
                Ice.Properties properties = communicator().getProperties();
                Ice.Identity id = communicator().stringToIdentity(properties.getProperty("Identity"));
                adapter.add(new IGServerControllerIceI(m_serverMgr, properties.getProperty("Ice.ProgramName")), id);
                adapter.activate();

                m_logMgr.WriteEntry("Server manager initialized", EventLogEntryType.Information);
                communicator().waitForShutdown();
            }
            catch (Exception exc)
            {
                m_logMgr.WriteEntry(exc.ToString(), EventLogEntryType.Error);
            }  

            return 0;
        }

        void OnError(object sender, string error)
        {
            m_logMgr.WriteEntry(error, EventLogEntryType.Error);
        }
    }    

    static public int Main(string[] args)
    {
        App app = new App();
        return app.main(args);
    }
}
