using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Net.Mail;
using System.Net;
using System.Security;
using IGPE;
using IGMadam;
using IGSMLib;

namespace IGPEWeb
{
    public partial class ContactUs : System.Web.UI.Page
    {
        private bool sendMessage(string userEmail, string userName, string subject, string message)
        {
            string subj = "A message from " + userEmail + " - " + (subject == "" ? "no subject" : subject);
            string body = message + "<br/><br/>" + userName;
            return IGPEWebServer.SendEmail(Session, "contactus@imageniuspaint.com", userName, subj, body);
        }
        
        protected void Page_Load(object sender, EventArgs e)
        {
        }

        protected void ButtonSend_Click(object sender, EventArgs e)
        {
            if (sendMessage(Email.Text != "" ? Email.Text : (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERNAME], (string)Session[DatabaseUserSecurityAuthority.IGMADAM_USERSHORTNAME], Subject.Text, EditorContactUs.Text))
            {
                SuccessLabel.Visible = true;
                Subject.Text = "";
                Email.Text = "";
                EditorContactUs.Text = "";
            }
        }
    }
}