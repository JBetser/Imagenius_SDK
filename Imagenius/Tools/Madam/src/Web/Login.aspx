<%@ Page Language="c#" %>
<script runat="server">
    
    private void LoginButton_Click(object sender, EventArgs e)
    {
        if (!Page.IsValid)
            return;

        string userName = UsernameBox.Text.Trim();
        string password = PasswordBox.Text.Trim();

        if (Madam.UserSecurityAuthority.FromConfig().Authenticate(userName, password, Madam.PasswordFormat.ClearText, null, "Forms") == null)
        {
            BadLoginLabel.Visible = true;
            return;
        }

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
    <form id="Form1" method="post" runat="server">
        <h1>
            Login</h1>
        <p>
            This login page is configured to authenticate against the Forms authentication credential
            store (<code>&lt;credentials&gt;</code> in <code>Web.config</code>). If you change the user security authentication
            implementation used by MADAM and want to use the same credential store for Forms
            authentication, you'll need to update the code in this page to use the appropriate
            credential store.
        </p>
        <p>
            Try username/password: <strong>johndoe</strong>/<strong>secret</strong></p>
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
