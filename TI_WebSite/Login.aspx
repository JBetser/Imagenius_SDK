<%@ Page Language="c#" %>
<script runat="server">  
    private void LoginButton_PageLoad(object sender, EventArgs e)
    {
        bool bIsConnected = (Session[IGMadam.DatabaseUserSecurityAuthority.IGMADAM_USERNAME] != null);
        if (bIsConnected)
            FormsAuthentication.RedirectFromLoginPage((string)Session[IGMadam.DatabaseUserSecurityAuthority.IGMADAM_USERNAME], false);
    }
        
    private void LoginButton_Click(object sender, EventArgs e)
    {
        if (!Page.IsValid)
            return;
        string userName = UsernameBox.Text.Trim();
        string password = PasswordBox.Text.Trim();
        if (IGMadam.UserSecurityAuthority.FromConfig().Authenticate(userName, password, null, "Forms") == null)
        {
            BadLoginLabel.Visible = true;
            return;
        }
        if (RememberMeBox.Checked)
        {
            HttpCookie cookie = Request.Cookies.Get("IGPictureEditorCookie");
            // Check if cookie exists in the current request.
            if (cookie != null)
                Request.Cookies.Remove("IGPictureEditorCookie");
            // Create cookie.
            cookie = new HttpCookie("IGPictureEditorCookie");
            // Set value of cookie to current user credentials.
            cookie.Values.Add(IGMadam.DatabaseUserSecurityAuthority.IGMADAM_USERNAME, userName);
            cookie.Values.Add(IGMadam.DatabaseUserSecurityAuthority.IGMADAM_PASSWORD, password);
            // Set cookie to expire in 10 hours.
            cookie.Expires = DateTime.Now.AddHours(10);
            // Insert the cookie in the current HttpResponse.
            Response.Cookies.Add(cookie);
        }
        Session[IGMadam.DatabaseUserSecurityAuthority.IGMADAM_USERNAME] = userName;
        //Session[IGMadam.DatabaseUserSecurityAuthority.IGMADAM_SERVERNAME] = IGMadam.UserSecurityAuthority.FromConfig().GetServerName(userName);
        Session[IGMadam.DatabaseUserSecurityAuthority.IGMADAM_USERPRIVILEGE] = IGMadam.UserSecurityAuthority.FromConfig().GetUserPrivilege(userName);
        Session[IGMadam.DatabaseUserSecurityAuthority.IGMADAM_USERSHORTNAME] = IGMadam.UserSecurityAuthority.FromConfig().GetUserShortName(userName);        
        FormsAuthentication.RedirectFromLoginPage(userName, RememberMeBox.Checked);
    }
</script>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
    <title>Login</title>
    <meta name="vs_targetSchema" content="http://schemas.microsoft.com/intellisense/ie5">
</head>
<body>
    <form id="Form1" method="post" runat="server" onload="LoginButton_PageLoad">
        <h1>
            Login</h1>
        <p>
            <strong>Welcome to Imagenius</strong>
        </p>
        <p>
            Please enter your username and password: 
        </p>
        <table cellspacing="0" cellpadding="5" border="0">
            <tr>
                <td>
                    Username:</td>
                <td>
                    <asp:TextBox ID="UsernameBox" runat="server" MaxLength="100" /></td>
                <td>
                    <asp:RequiredFieldValidator runat="server" ErrorMessage="Required" Display="Dynamic"
                        ControlToValidate="UsernameBox" /></td>
            </tr>
            <tr>
                <td>
                    Password:
                </td>
                <td>
                    <asp:TextBox ID="PasswordBox" runat="server" TextMode="Password" MaxLength="100" /></td>
                <td>
                    <asp:RequiredFieldValidator runat="server" ErrorMessage="Required" Display="Dynamic"
                        ControlToValidate="PasswordBox" /></td>
            </tr>
            <tr>
                <td>
                </td>
                <td>
                    <asp:CheckBox ID="RememberMeBox" runat="server" Text="Remember me" /></td>
                <td>
                </td>
            </tr>
            <tr>
                <td>
                </td>
                <td>
                    <asp:Button ID="LoginButton" runat="server" Text="Login" OnClick="LoginButton_Click" /></td>
                <td>
                </td>
            </tr>
        </table>
        <p>
            <asp:Label ID="BadLoginLabel" runat="server" Visible="False" ForeColor="Red">Invalid user name or password!</asp:Label></p>
    </form>
</body>
</html>
