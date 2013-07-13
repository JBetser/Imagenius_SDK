<%@ Page Language="C#" AutoEventWireup="true" CodeFile="ContactUs.aspx.cs" Inherits="IGPEWeb.ContactUs" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
    <div>
        <a style = "height: 25px" >Please type your message below:</a><br />
        <br />
        <a style="vertical-align: top">Subject:&nbsp;</a><asp:TextBox ID="Subject" runat="server" Height="20px" Width="200px"  ></asp:TextBox>
        <br />
        <br />
        <a style="vertical-align: top">Your email:&nbsp;</a><asp:TextBox ID="Email" runat="server" Height="20px" Width="200px"  ></asp:TextBox>
        <br />
        <br />
    
        <asp:TextBox ID="EditorContactUs" runat="server" Height="110px" Width="400px" 
            TextMode="MultiLine"></asp:TextBox>
    
        <br />
        <asp:Button ID="ButtonSend" runat="server" Text="Send" OnClick="ButtonSend_Click" />
    
    <asp:Label ID="SuccessLabel" runat="server" visible="False" 
            style="vertical-align: top; color: green" Width="273px">&nbsp;&nbsp;&nbsp; Your message has been successfully sent.</asp:Label></div>
    </form>
</body>
</html>
