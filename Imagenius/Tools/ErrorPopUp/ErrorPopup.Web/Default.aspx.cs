using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace ErrorPopup.Web
{
    public partial class _Default : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
        }

        protected void Button_Enable_Click(object sender, EventArgs e)
        {
            this.ErrorWindow.ShowControl();
        }

        protected void Button_Dide_Click(object sender, EventArgs e)
        {
            this.ErrorWindow.HideControl();
        }
    }
}
