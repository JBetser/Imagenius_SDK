<%@ Page Language="c#" AutoEventWireup="true" %>
<script runat="server">

    private void Page_Load()
    {
        WithRssFlag.Visible = Request.PathInfo.EndsWith("rss");
        WithoutRssFlag.Visible = !WithRssFlag.Visible;
    }

</script>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
    <title>ELMAH Error Log Summary</title>
    <meta name="vs_targetSchema" content="http://schemas.microsoft.com/intellisense/ie5">
</head>
<body>
    <form id="Form1" method="post" runat="server">
        <h1>
            ELMAH Error Log</h1>
        <asp:Panel runat="server" ID="WithRssFlag">
            <p>
            If this site was setup using <a href="http://msdn.microsoft.com/asp.net/default.aspx?pull=/library/en-us/dnaspp/html/elmah.asp">ELMAH</a> 
            (Error Logging Modules and Handlers) you'd be looking at an RSS feed
            with the 10 most recent error log summaries. Since this information would likely
            be accessed by an RSS feed reader (such as a news aggregator) you'd want to protect 
            the RSS-enabled feature using a standard HTTP authentication scheme, like Basic.</p>
            <p><img src="http://msdn.microsoft.com/library/en-us/dnaspp/html/elmah_fig03.gif" alt="Error log RSS feed" width="450" height="338" /></p>
        </asp:Panel>
        <asp:Panel runat="server" ID="WithoutRssFlag">
            <p>
            If this site was setup using <a href="http://msdn.microsoft.com/asp.net/default.aspx?pull=/library/en-us/dnaspp/html/elmah.asp">ELMAH</a> 
            (Error Logging Modules and Handlers) you'd be looking at an HTML summary
            report of the errors that occurred on this website. The HTML summary report is intended
            for humans to read using an interactive user agent such as web browser and therefore can 
            be protected by Forms authentication.</p>
            <p><img src="http://msdn.microsoft.com/library/en-us/dnaspp/html/elmah_fig01.gif" alt="Error log HTML view" width="450" height="338" /></p>
        </asp:Panel>
    </form>
</body>
</html>
