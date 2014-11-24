using System;
using System.Web;
using System.Web.Services;
using System.Web.Services.Protocols;

[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
public class Service : System.Web.Services.WebService
{
    public Service()
    {

        //Uncomment the following line if using designed components
        //InitializeComponent();
    }

    public class Monte
    {
        public int InCircle = 0;
        public int Tries = 0;
    }

    [WebMethod]
    public Monte MonteLoop(int Tries, int Seed)
    {
        Monte Values = new Monte();
        Random Rnd = new Random(Seed);
        double x, y;
        Values.InCircle = 0;
        for (int i = 1; i < Tries; i++)
        {
            x = Rnd.NextDouble();
            y = Rnd.NextDouble();
            if (Math.Sqrt(x * x + y * y) <= 1) Values.InCircle++;
            //Optimization Note: Sqrt is not needed 1^2=1
        }
        Values.Tries = Tries;
        return (Values);
    }

}


