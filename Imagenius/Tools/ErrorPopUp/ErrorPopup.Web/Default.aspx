<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Default.aspx.cs" Inherits="ErrorPopup.Web._Default" %>

<%@ Register assembly="AjaxControlToolkit" namespace="AjaxControlToolkit" tagprefix="cc1" %>


<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">


<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title></title>
    <link href="Style.css" rel="stylesheet" type="text/css" />
</head>
<body>
    <form id="form1" runat="server">
    <div>
    <asp:ScriptManager runat="server" ID="scriptManager" EnablePartialRendering="true"></asp:ScriptManager>
    <nw:ErrorPopUp ID="ErrorWindow" runat="server" Message="Put Error Message here" Show="true"  ></nw:ErrorPopUp>
    
        <asp:Button ID="Button_Enable" runat="server" Text="Enable" 
            onclick="Button_Enable_Click" />
        &nbsp;
        <asp:Button ID="Button_Dide" runat="server" Text="Hide" 
            onclick="Button_Dide_Click" style="width: 40px" />
        <br />
    
    </div>
    </form>
</body>
</html>
