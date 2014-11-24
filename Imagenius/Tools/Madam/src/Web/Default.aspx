<%@ Page Language="C#" %>
<%@ Assembly Name="Madam" %>
<script runat="server">
    private void Page_Load() { DataBind(); }
    private void Signout_Click(object sender, EventArgs e)
    {
        FormsAuthentication.SignOut();
    }
</script>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
    <title>MADAM Demo</title>
</head>
<body>
    <form id="Logout" method="post" runat="server">
    <asp:Button ID="Signout" Text="Signout" OnClick="Signout_Click" Visible=true runat=server/>
    </form>
    <h1>
        <acronym title="Mixed Authentication and Disposition ASP.NET Module">MADAM</acronym> Demo Home</h1>
    <p>
        <asp:Label runat="server" Visible="<%# Request.IsAuthenticated %>">
            Welcome <strong><%# Server.HtmlEncode(User.Identity.Name) %></strong>!
            You are signed in using <strong><%# Server.HtmlEncode(User.Identity.AuthenticationType) %></strong>.</asp:Label></p>
    <p>
        This demo is factory-configured configured to use Basic and Forms authentication with
        <code><% = Server.HtmlEncode(Madam.UserSecurityAuthority.FromConfig().GetType().Name) %></code>
        as the current user security authority implementation. The various credentials stores
        provided with this demo have each a valid username/password combination that is
        <strong>johndoe</strong>/<strong>secret</strong> and which you can use to sign in.</p>
    <p>
        To change the credentials store or authentication scheme used, edit <code>Web.config</code>. For
        your convenience, additional examples have been included as comments in the <code>Web.config</code> 
        that illustrate how to use <code>DatabaseUserSecurityAuthority</code> as well as 
        <code>MembershipUserSecurityAuthority</code> (the latter under ASP.NET 2.0 only).</p>
    <p>
        <strong>IMPORTANT!</strong> Before proceeding with demonstrations, please make sure that this 
        web stie is not protected by Windows Authentication.</p>
    <hr>
    <h2>
        Protecting a Resource Based on Output</h2>
    <p>
        The <a href="http://msdn.microsoft.com/asp.net/default.aspx?pull=/library/en-us/dnaspp/html/elmah.asp">ELMAH</a> 
        (Error Logging Modules and Handlers) tool automatically records unhandled
        exceptions to an error log that can be reviewed through an HTML front-end or from
        an RSS feed. In either case, the resource should be protected so anonymous visitors
        cannot inspect the error details. For the RSS feeds, it's likely that the client
        consuming the feed will be a program, like a news aggregator. For the RSS feed, then,
        it's important that it be protected using a standard HTTP authentication scheme.</p>
    <p>
        This first example shows how to protect one resource (<code>elmah.aspx</code>) using
        Forms authentication, and another (<code>elmah.aspx/rss</code>) with Basic authentication
        against the same credentials store. Note that if you authenticate over one scheme first, 
        then you can view the resource protected over another scheme without needing to re-authenticate.
    </p>
    <ul>
        <li>
            <asp:HyperLink runat="server" NavigateUrl="~/elmah.aspx">Error Log Page (secured by Forms)</asp:HyperLink></li>
        <li>
            <asp:HyperLink runat="server" NavigateUrl="~/elmah.aspx/rss">Error Log RSS Feed (secured by Basic/Digest)</asp:HyperLink></li>
    </ul>
    <hr>
    <h2>
        Only Allowing Authenticated Users to Download Files</h2>
    <p>
        This second example shows how to protect files, allowing only authenticated users
        to download them. This example assumes you place your protected downloads in the
        <code>Downloads</code> folder and configure IIS to deny HTTP read access to that folder.
        Then, all downloads must be accessed using <code>Download.ashx?<i>filename</i></code>,
        which streams the binary contents of the specified file down to the user (assuming
        they're authorized). <code>Web.config</code> is configured to grant authorization to download
        files to authenticated users only.
    </p>
    <p>
        This approach uses Basic authentication because you may want to let download agents,
        like <a href="http://www.microsoft.com/windowsserver2003/techinfo/overview/bits.mspx">BITS</a> 
        or <a href="http://www.gnu.org/software/wget/"><code>wget</code></a>, to be able
        to access and download files via <code>Download.ashx</code>.
    </p>
    <ul>
        <li>
            <asp:HyperLink runat="server" NavigateUrl="~/Download.ashx?Secret.zip" ID="DownloadLink">Download Secret (members only; secured by Basic/Digest)</asp:HyperLink></li>
    </ul>
    <hr />
    <p>This web site is running with ASP.NET <% = Server.HtmlEncode(Environment.Version.ToString(3)) %>.</p>
</body>
</html>
