using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Timers;
using System.IO;

namespace Robocopy
{
    class Program
    {
        private static string path_src = @"S:\Projets\TI_WebSite DEV\Imagenius";
        private static string path_dst = @"F:\Sauvegarde Shared\Projets\TI_WebSite DEV\Imagenius";
        private static int nStartHour   = 4;
        private static int nStartMinute = 0;

        private static System.Timers.Timer aTimer;

        static void Main(string[] args)
        {
            // Create a timer with a ten second interval.
            aTimer = new System.Timers.Timer(10000);

            // Hook up the Elapsed event for the timer.
            aTimer.Elapsed += new ElapsedEventHandler(OnTimedEvent);

            // Set the Interval to 2 seconds (2000 milliseconds).
            aTimer.Interval = 60000;
            aTimer.Enabled = true;

            Console.WriteLine("Press the Escape to exit the program.\nSarah, please do not close this application, or shutdown your computer.\nT'aime t'aime fort");
            ConsoleKeyInfo keyInfo = Console.ReadKey(true);
            while (keyInfo.Key != ConsoleKey.Escape)
                keyInfo = Console.ReadKey(true);
        }

        // Specify what you want to happen when the Elapsed event is 
        // raised.
        private static void OnTimedEvent(object source, ElapsedEventArgs e)
        {
            if (((e.SignalTime - DateTime.Today).Hours == nStartHour) &&
                ((e.SignalTime - DateTime.Today).Minutes == nStartMinute))
            {
                Console.WriteLine("Launching Imagenius copy at {0}", e.SignalTime);
                try
                {
                    // Determine whether the directory exists.
                    if (!Directory.Exists(path_src))
                    {
                        Console.WriteLine("Error: Source path could not be found");
                        return;
                    }

                    if (Directory.Exists(path_dst))
                    {
                        Console.WriteLine("Deleting {0}...", path_dst);
                        // Delete the target to ensure it is not there.
                        Directory.Delete(path_dst, true);                        
                    }
                    Console.WriteLine("Creating folder {0}", path_dst);
                    Directory.CreateDirectory(path_dst);

                    int nNbFilesCopied = 0;
                    recursiveCopy(path_src, path_dst, ref nNbFilesCopied);

                    // Count the files in the target directory.
                    Console.WriteLine("{0} files copied", nNbFilesCopied);
                    Console.WriteLine("Imagenius copy finished at {0}", DateTime.Now);
                    Console.WriteLine("Press the Escape to exit the program.\nSarah, please do not close this application, or shutdown your computer.\nT'aime t'aime fort");
                }
                catch (Exception exc)
                {
                    Console.WriteLine("The process failed: {0}", exc.ToString());
                }
                finally { }

            }
        }

        private static void recursiveCopy(string src, string dst, ref int nNbFilesCopied)
        {
            string[] paths = Directory.GetFileSystemEntries(src);
            foreach (string path in paths)
            {
                if (Directory.Exists(path))
                {
                    string folderDst = path.Replace(src, dst);
                    Console.WriteLine("Creating folder {0}", folderDst);
                    Directory.CreateDirectory(folderDst);
                    recursiveCopy(path, folderDst, ref nNbFilesCopied);
                }
                else
                {
                    string fileDst = path.Replace(src, dst);
                    Console.WriteLine("Copying {0}", path);
                    File.Copy(path, fileDst, true);
                    nNbFilesCopied++;
                }
            }
        }
    }
}
