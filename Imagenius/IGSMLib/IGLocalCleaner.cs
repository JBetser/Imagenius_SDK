using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;
using System.Diagnostics;

namespace IGSMLib
{
    public class IGLocalCleaner
    {
        public static void ThreadProc()
        {
            EventLog logMgr = new EventLog("IGSMService", Environment.MachineName, "Session Manager");
            logMgr.WriteEntry("Local cleaner started", EventLogEntryType.Information);
            try
            {
                while (true)
                {
                    string[] paths = Directory.GetFileSystemEntries(HC.PATH_OUTPUT);
                    if (paths.Length == 0)
                    {
                        Thread.Sleep(1000);
                        continue;
                    }
                    foreach (string path in paths)
                    {
                        if (path.Contains("@@GARBAGE@@"))
                        {
                            try
                            {
                                Directory.Delete(path, true);
                            }
                            catch (Exception)
                            {
                                Thread.Sleep(100);
                                continue;
                            }
                        }
                        Thread.Sleep(100);
                    }
                }
            }
            catch (ThreadAbortException) { }
            catch (Exception exc)
            {
                logMgr.WriteEntry(exc.ToString(), EventLogEntryType.Error);
            }
            logMgr.WriteEntry("Local cleaner stopped", EventLogEntryType.Information);
        }
    }
}