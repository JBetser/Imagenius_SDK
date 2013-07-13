using System;
using System.Collections;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Xml.Linq;
using IGMadam;
using IGSMLib;
using IGPE;

namespace IGPEWeb
{
    public partial class Test : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            IGPEWebServer.OpenSession();
            if ((Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null) ||
                (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTING] != null))
            {
                signOut();
                return;
            }
            if (Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME] != null &&
                Session[DatabaseUserSecurityAuthority.IGMADAM_PASSWORD] != null)
            {
                login((string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME], 
                    (string)Session[DatabaseUserSecurityAuthority.IGMADAM_PASSWORD]);
            }
        }
                
        protected void login(string sUser, string sPwd)
        {
            if (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTED] != null ||
                (Session[IGPEMultiplexing.SESSIONMEMBER_DISCONNECTING] != null))
            {
                signOut();
                return;
            }
            if (!IGPEWebServer.ConnectMember(sUser, sPwd, Session)){
                signOut();
                return;
            }
        }
        
        private void signOut()
        {
            FormsAuthentication.SignOut();
        }
    }
}