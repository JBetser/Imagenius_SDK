/*****************************************************************************\

 Javascript "Imagenius Web Service Client" library
 
 @version: 1.0 - 2009.04.03
 @author: Jonathan Betser
 
\*****************************************************************************/
var waitingForReply = false;

function SetStatus(status)
{
    var divResult = IGWS_WORKSPACE_DIV;
	var labelResult = getElementInHierarchy ("LabelResult");
	if (labelResult)
	    divResult.removeChild (labelResult);
	labelResult = document.createElement ("label");
	labelResult.setAttribute ("id", "LabelResult");
	var labelResultText = document.createTextNode (status);
	divResult.appendChild (labelResultText);
}
function ViewResult (numImage)
{
    var divResult = getElementInHierarchy ("ViewResult");
	var imgResult = getElementInHierarchy ("Viewer");
	if (imgResult)
	    divResult.removeChild (imgResult);	    
	imgResult = document.createElement ("img");	
	imgResult.setAttribute ("id", "Viewer");
	var imgSrc = "TempResult/watershed/ImageResult/Image" + numImage.toString();
    imgSrc = imgSrc.concat (".png"); 
	imgResult.setAttribute ("src", imgSrc);
	divResult.appendChild (imgResult);
}
function Ping()
{
	var pl = new SOAPClientParameters();
	SOAPClient.invoke(urlWS, "Ping", pl, true, Ping_callBack);
}
function Ping_callBack(r)
{
    SetStatus (r);	
}
function SelectImage()
{
    var numImage = Number(document.frmIGClient.TextSelect.value);
    var newStatus = "Request SelectImage " + numImage.toString();
    SetStatus (newStatus);
	var pl = new SOAPClientParameters();
	pl.add("ImageId", numImage);
	SOAPClient.invoke(urlWS, "SelectImage", pl, true, SelectImage_callBack);
}
function SelectImage_callBack(numImage)
{	
    if (numImage < 0)
    {
        SetStatus ("Cannot find Image requested");
    }
    else
    {
        if (numImage == NaN)
        {
            SetStatus ("SelectImage request failed");                    
        }
        else
        {
            var newStatus = "Image ";  
            newStatus = newStatus.concat ("selected");
            ViewResult (numImage);
            SetStatus (newStatus); 
        } 
    }
}

function DisconnectUser() {
    var pl = new SOAPClientParameters();
    SOAPClient.invoke(urlPublicWS, "DisconnectUser", pl, true, DisconnectUser_callBack);
}

function DisconnectUser_callBack(result) {
}

function ConnectUser(userName) {
    var pl = new SOAPClientParameters();
    pl.add("UserName", userName);
    return SOAPClient.invoke(urlPublicWS, "ConnectUser", pl, true, ConnectUser_callBack);
}

function ConnectUser_callBack(result) {
}

function ConnectDemo(userName) {
    var pl = new SOAPClientParameters();
    return SOAPClient.invoke(urlPublicWS, "ConnectDemo", pl, true, ConnectDemo_callBack);
}

function ConnectDemo_callBack(result) {
}

function UpdateStatus() {
    if (waitingForReply)
        return;
    waitingForReply = true;
    var checkConnection = new IGCheckConnection();
    checkConnection.Check (UpdateStatus_callback);
}

function UpdateStatus_callback(bIsConnected) {
    if (bIsConnected)
    {
        GetServerList();
        DownloadOutput();
    }
    waitingForReply = false;
}

function GetServerList() {
    var pl = new SOAPClientParameters();
    SOAPClient.invoke(urlWS, "GetServerList", pl, true, GetServerList_callBack);
}

var g_updatingList = false;
var g_serverList = [];
var g_curServer = "";
var g_bUpdateUserStatus = false;
var g_refTimerUserStatus = null;

function GetServerList_callBack(result) {
    if (result && !g_updatingList) {
        g_updatingList = true;
        var selectServers = getElementInHierarchy("SelectServers");
        if (selectServers == null)
            return; // not in admin mode
        var serverList = document.getElementsByTagName("option");
        if (result == IGWS_RESULT_DISCONNECTED) {
            var textAreaServerStatus = getElementInHierarchy("TextAreaServerStatus");
            textAreaServerStatus.value = IGWS_RESULT_DISCONNECTED_MESSAGE;
            return;
        }
        var reg = new RegExp("[\?]+", "g");
        var tabServers = result.split(reg);
        var bNoChanges = (g_serverList.length > 0);
        if (bNoChanges) {
            for (var j = 0; j < g_serverList.length; j++) {
                for (var i = 0; i < tabServers.length; i++) {
                    if (g_serverList[j] != tabServers[i])
                        NoChanges = false;
                }
            }
        }
        if (bNoChanges) {
            selectServers = getElementInHierarchy("SelectServers");
            GetServerStatus(selectServers.options[selectServers.selectedIndex].text);
            g_updatingList = false;
            return;
        }
        var bRemoveServer = true;
        for (j = 0; j < serverList.length; j++) {
            bRemoveServer = true;
            for (i = 0; i < tabServers.length; i++) {
                if (serverList[j].firstChild == null)
                    continue;
                if (serverList[j].firstChild.value == tabServers[i])
                    bRemoveServer = false;
            }
            if (bRemoveServer) {
                selectServers.removeChild(serverList[j]);
                g_serverList.pop(tabServers[i]);
            }
        }
        serverList = document.getElementsByTagName("option");
        var bAddServer = true;
        for (i = 0; i < tabServers.length; i++) {
            bAddServer = true;
            for (j = 0; j < serverList.length; j++) {
                if (serverList[j].firstChild == null)
                    continue;
                if (serverList[j].firstChild.value == tabServers[i])
                    bAddServer = false;
            }
            if (bAddServer) {
                var newServer = document.createElement("option");
                newServer.appendChild(document.createTextNode(tabServers[i]));
                selectServers.appendChild(newServer);
                g_serverList.push(tabServers[i]);
            }
        }
        selectServers = getElementInHierarchy("SelectServers");
        GetServerStatus(selectServers.options[selectServers.selectedIndex].text);
        g_updatingList = false;
    }
}

function GetServerStatus(serverName) {
    var pl = new SOAPClientParameters();
    pl.add("ServerName", serverName);
    SOAPClient.invoke(urlWS, "GetServerStatus", pl, true, GetServerStatus_callBack);
}

function GetServerStatus_callBack(result) {
    var textAreaServerStatus = getElementInHierarchy("TextAreaServerStatus");
    textAreaServerStatus.value = result;    
    var reg = new RegExp("(?:ServerName: )(.+)", "g");
    g_curServer = reg.exec (result);
}

function DownloadOutput() {
    if (waitingForReply)
        return;
    waitingForReply = true;
    var pl = new SOAPClientParameters();
    var checkboxViewErrors = getElementInHierarchy("CheckboxViewErrors");
    var bChecked = (checkboxViewErrors == null) ? false : checkboxViewErrors.checked;
    pl.add("ViewErrorsOnly", bChecked);
    SOAPClient.invoke(urlWS, "DownloadOutput", pl, true, DownloadOutput_callBack);
}

function DownloadOutput_callBack(result) {
    waitingForReply = false;
    if ((result == null) || (result == ""))
        return;    
    showOutput (result);
}

function showOutput(output) {
    var textAreaOutput = getElementInHierarchy("TextAreaOutput");
    if (textAreaOutput)
    {
        if (output == IGWS_RESULT_DISCONNECTED) {
            textAreaOutput.value = IGWS_RESULT_DISCONNECTED_MESSAGE;
        }
        else {
            textAreaOutput.value = output + textAreaOutput.value;
        }
    }
    window.status = output;
}

function IGPEUpdate() {
    if ( waitingForReply)
        return;
    waitingForReply = true;
    var checkConnection = new IGCheckConnection();
    checkConnection.Check (IGPEUpdate_callback);
}

function IGPEUpdate_callback(bIsConnected) {
     waitingForReply = false;
}
    

function ProcessAnswers() {
    if ( waitingForReply)
        return;
    waitingForReply = true;
    var pl = new SOAPClientParameters();
    pl.add("ViewErrorsOnly", false);
    if (SOAPClient.invoke(urlWS, "DownloadOutput", pl, true, ProcessAnswers_callBack) == null) {
        var formIGPictureEditor = getElementInHierarchy("formIGPictureEditor");
        if (formIGPictureEditor)
            formIGPictureEditor.submit();
    }
    // ask for an update if no answer
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    if (divCurrentWorkspace == null)
        return;
    var nbTicksWithRequests = divCurrentWorkspace.getAttribute("nbTicksWithRequests");
    if (nbTicksWithRequests == null) {
        divCurrentWorkspace.setAttribute("nbTicksWithRequests", 0);
        return;
    }
    var IG_aNbRequests = getElementInHierarchy("IG_aNbRequests");
    var nbRequests = parseInt(IG_aNbRequests.innerHTML);
    if (isNaN(nbRequests))
        nbRequests = Number(IG_aNbRequests.innerHTML);
    if (nbRequests > 0)
        nbTicksWithRequests++;
    else
        nbTicksWithRequests = 0;
    if (nbTicksWithRequests >= 4) {
        divCurrentWorkspace.setAttribute("nbTicksWithRequests", 0);
        sendUserRequest();  // No answer after 6s, ask for a refresh 
    }
    else {
        divCurrentWorkspace.setAttribute("nbTicksWithRequests", nbTicksWithRequests);
    }
}

function ProcessAnswers_callBack(result) {
     waitingForReply = false;
    if ((result == null) || (result == "") || (result == "_") || (result == "Disconnected"))
        return;
    alert(result);
    var reg = new RegExp("[\n]+", "g");
    var tabAnswers = result.split(reg);
    for (var i = 0; i < tabAnswers.length; i++) {
        var curIdx = tabAnswers.length - 1 - i;
        if (tabAnswers[curIdx] == "" ||
            tabAnswers[curIdx] == "_")
            continue;
        var regAnswerId = new RegExp("[\?]", "g");
        var tabAnswerParams = tabAnswers[curIdx].split(regAnswerId);
        var answerId = Number(tabAnswerParams[0]);
        if ((answerId >= 3000) &&
            (answerId < 5000)) {
            ProcessWorkspaceAnswers(tabAnswerParams);
        } else if ( (answerId >= 6000) &&
                    (answerId < 7000)) {
                ProcessAccountAnswers(tabAnswerParams);
        }
        else {
            alert("Error reading answer: " + tabAnswers[i]);           
        }
    }
}

function GetUserStatus() {
    var inputUser = getElementInHierarchy("inputUser");
    if ((inputUser == null) || (g_curServer[1] == null))
        return;
    var pl = new SOAPClientParameters();
    pl.add("Server", g_curServer[1]);
    pl.add("User", inputUser.value);
    SOAPClient.invoke(urlWS, "GetUserStatus", pl, true, GetUserStatus_callback); 
}

function GetUserStatus_callback(status) {
    if ((status == null) || (status == "")) {
        status = "User not found";
        if (g_refTimerUserStatus)
        {
            clearInterval (g_refTimerUserStatus);
            g_refTimerUserStatus = null;
        }
        g_bUpdateUserStatus = false;        
    } else if (!g_bUpdateUserStatus) {
        if (g_refTimerUserStatus == null)
            g_refTimerUserStatus = setInterval("GetUserStatus()", 800);
    }
    var textAreaUserStatus = getElementInHierarchy("TextAreaUserStatus");
    textAreaUserStatus.value = status;
}

function SendCommand() {
    var inputCommandLine = getElementInHierarchy("textBoxConsole");
    if (inputCommandLine == null)
        return;
    var pl = new SOAPClientParameters();
    pl.add("CommandLine", inputCommandLine.value);
    SOAPClient.invoke(urlWS, "SendCommand", pl, true, SendCommand_callback); 
}

function SendCommand_callback(result) {
    if ((result == null) || (result == "")) {
        return;
    }
    showOutput (result);
}