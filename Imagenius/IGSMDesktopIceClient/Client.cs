using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using IGServerController;

namespace IGSMDesktopIceClient
{
    public class Client
    {
        public class App : Ice.Application
        {
            private void menu()
            {
                Console.WriteLine(
                    "usage:\n" +
                    "p: ping server\n" +
                    "r=\"IMAGENIUS XML REQUEST\": send specific request\n" +
                    "rst: reset server\n" +
                    "s: shutdown server\n" +
                    "x: exit\n" +
                    "?: help\n");
            }

            public override int run(string[] args)
            {
                if (args.Length > 0)
                {
                    Console.Error.WriteLine(appName() + ": too many arguments");
                    return 1;
                }

                IGServerControllerIcePrx serverController = null;
                try
                {
                    serverController = IGServerControllerIcePrxHelper.checkedCast(communicator().stringToProxy("serverController"));
                }
                catch (Ice.NotRegisteredException)
                {
                    IceGrid.QueryPrx query =
                        IceGrid.QueryPrxHelper.checkedCast(communicator().stringToProxy("IGServerControllerIceGrid/Query"));
                    serverController = IGServerControllerIcePrxHelper.checkedCast(query.findObjectByType("::IGServerController::IGServerControllerIce"));
                }
                if (serverController == null)
                {
                    Console.WriteLine("couldn't find a `::IGServerController::IGServerControllerIce' object");
                    return 1;
                }

                menu();

                string line = null;
                do
                {
                    try
                    {
                        Console.Write("==> ");
                        Console.Out.Flush();
                        line = Console.In.ReadLine();
                        if (line == null)
                        {
                            break;
                        }
                        if (line.Equals("p"))
                        {
                            Console.WriteLine(serverController.ping());
                        }
                        else if (line.Equals("s"))
                        {
                            serverController.shutdown();
                        }
                        else if (line.Equals("x"))
                        {
                            // Nothing to do
                        }
                        else if (line.Equals("?"))
                        {
                            menu();
                        }
                        else if (line.StartsWith("r=\"") && line.EndsWith("\""))
                        {
                            Console.WriteLine("Answer: \n" + serverController.sendRequest(line.Substring(3, line.Length - 4)));
                        }
                        else if (line.Equals("rst"))
                        {
                            serverController.reset();
                        }
                        else
                        {
                            Console.WriteLine("unknown command `" + line + "'");
                            menu();
                        }
                    }
                    catch (Ice.LocalException ex)
                    {
                        Console.WriteLine(ex);
                    }
                }
                while (!line.Equals("x"));

                return 0;
            }
        }

        public static int Main(string[] args)
        {
            App app = new App();
            return app.main(args, "config.client");
        }
    }
}
