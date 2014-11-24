using System;
using System.Collections.Generic;
using System.Text;

namespace PiConsoleApp
{
    class Program
    {
        const int TriesPerWSCall = 10000000;
        const int NumWSCalls = 100;
        //TriesPerWSCall * NumWSCalls = Total Number of Points

        private static int ActiveWebServiceCounter = 0;
        public static long ttl_incirc, ttl_tries;

        class WSAsyncCall
        {
            public void DoneCallBack(Object source,
                WSReference.MonteLoopCompletedEventArgs e)
                //Create Web Reference named "WSReference" 
            {
                WSReference.Monte data = new WSReference.Monte();
                data = e.Result;
                ttl_incirc += data.InCircle;
                ttl_tries += data.Tries;
                Console.WriteLine(" Pi = {0:N9}  T = {1:N0}",
                    (double)ttl_incirc / (double)ttl_tries * 4.0, ttl_tries);
                ActiveWebServiceCounter--;
            }

            public void Start(int seed)
            {
                WSReference.Service ser = new
                    PiConsoleApp.WSReference.Service();
                ser.MonteLoopCompleted += new
                    WSReference.MonteLoopCompletedEventHandler(DoneCallBack);
                ActiveWebServiceCounter++;
                ser.MonteLoopAsync(TriesPerWSCall, seed);
            }
        }


        static void Main(string[] args)
        {
            DateTime startTime = DateTime.Now;
            Console.WriteLine("Starting Time:  " + startTime);

            WSAsyncCall[] WS = new WSAsyncCall[NumWSCalls + 1];
            Random seed = new Random();
            for (int i = 1; i <= NumWSCalls; i++)
            {
                WS[i] = new WSAsyncCall();
                WS[i].Start(seed.Next());
            }
            while (ActiveWebServiceCounter > 0) ;

            DateTime stopTime = DateTime.Now;
            Console.WriteLine("Finish Time:  " + stopTime);
            TimeSpan duration = stopTime - startTime;
            Console.WriteLine("Elapsed Time:  " + duration);
            Console.WriteLine();
            Console.WriteLine(" Pi = {0:N9}  T = {1:N0}",
                (double)ttl_incirc / (double)ttl_tries * 4.0, ttl_tries);

            Console.Read();
        }


    }
}
