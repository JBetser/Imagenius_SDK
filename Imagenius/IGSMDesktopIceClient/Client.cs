using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using IGServerController;
using System.Threading;

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
                    "ar=\"IMAGENIUS XML REQUEST\": add async request to buffer\n" +
                    "as: send all async requests from buffer and clear it\n" +
                    "db: display buffer contents\n" +
                    "rst: reset server\n" +
                    "s: status\n" +
                    "s: shutdown server\n" +
                    "x: exit\n" +
                    "?: help\n");
            }

            IGServerControllerIcePrx serverController = null;
            List<string> requests = new List<string>();
            int nb_req_wait = 0;

            void sendRequest(object obj)
            {
                Console.WriteLine(string.Format("{0} Answer: {1}", DateTime.Now.TimeOfDay, serverController.sendRequest((string)obj)));
                if (--nb_req_wait == 0)
                    Console.Write("\n==> ");
            }

            public override int run(string[] args)
            {
                if (args.Length > 0)
                {
                    Console.Error.WriteLine(appName() + ": too many arguments");
                    return 1;
                }

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
                            Console.WriteLine(string.Format("{0} Ping", DateTime.Now.TimeOfDay));
                            Console.WriteLine(string.Format("{0} Answer: {1}", DateTime.Now.TimeOfDay, serverController.ping()));
                        }
                        else if (line.Equals("s"))
                        {
                            Console.WriteLine(string.Format("{0} Status:\n{1}", DateTime.Now.TimeOfDay, serverController.getStatus()));
                        }
                        else if (line.Equals("shut"))
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
                            var tmp_req_wait = nb_req_wait;
                            nb_req_wait = 0;
                            sendRequest(line.Substring(3, line.Length - 4));
                            nb_req_wait = tmp_req_wait;
                        }
                        else if (line.StartsWith("ar=\"") && line.EndsWith("\""))
                        {
                            requests.Add(line.Substring(4, line.Length - 5));
                            Console.WriteLine(string.Format("{0} Request added to buffer", DateTime.Now.TimeOfDay));
                        }
                        else if (line.Equals("as"))
                        {
                            nb_req_wait += requests.Count;
                            while (requests.Count > 0){
                                var req = requests[0];
                                requests.RemoveAt(0);
                                Thread newThread = new Thread(sendRequest);
                                newThread.Start(req);
                            }
                        }
                        else if (line.Equals("db"))
                        {
                            foreach (var req in requests)
                                Console.WriteLine(string.Format("{0}", req));
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
