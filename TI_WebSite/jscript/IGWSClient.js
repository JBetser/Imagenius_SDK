/*****************************************************************************\

 Javascript "Imagenius Ajax/JSon Web Service Client" library
 
 @version: 1.0 - 2013.05.30
 @author: Jonathan Betser
 
\*****************************************************************************/
var waitingForReply = false;
var IGREQUEST_WORKSPACE_CONNECT = 1000;
var IGREQUEST_WORKSPACE_PING = 1001;
var IGREQUEST_WORKSPACE_DISCONNECT = 1002;
var IGREQUEST_WORKSPACE_NEWIMAGE = 1003;
var IGREQUEST_WORKSPACE_LOADIMAGE = 1004;
var IGREQUEST_WORKSPACE_TERMINATE = 1005;
var IGREQUEST_WORKSPACE_SHOW = 1006;
var IGREQUEST_WORKSPACE_DESTROYACCOUNT = 1007;

var IGREQUEST_FRAME_SAVE = 2000;
var IGREQUEST_FRAME_ADDLAYER = 2001;
var IGREQUEST_FRAME_REMOVELAYER = 2002;
var IGREQUEST_FRAME_MOVELAYER = 2003;
var IGREQUEST_FRAME_MERGELAYER = 2004;
var IGREQUEST_FRAME_FILTER = 2005;
var IGREQUEST_FRAME_CHANGEPROPERTY = 2006;
var IGREQUEST_FRAME_CLOSE = 2007;
var IGREQUEST_FRAME_UPDATE = 2008;
var IGREQUEST_FRAME_SELECT = 2009;
var IGREQUEST_FRAME_SETLAYERVISIBLE = 2010;
var IGREQUEST_FRAME_GOTOHISTORY = 2011;
var IGREQUEST_FRAME_DRAWLINES = 2012;
var IGREQUEST_FRAME_PICKCOLOR = 2013;
var IGREQUEST_FRAME_INDEX = 2014;
var IGREQUEST_FRAME_COPY = 2015;
var IGREQUEST_FRAME_CUT = 2016;
var IGREQUEST_FRAME_PASTE = 2017;
var IGREQUEST_FRAME_DELETE = 2018;
var IGREQUEST_FRAME_SMARTDELETE = 2019;
var IGREQUEST_FRAME_MOVE = 2020;
var IGREQUEST_FRAME_ROTATEANDRESIZE = 2021;
var IGREQUEST_FRAME_SELECTLAYER = 2022;
var IGREQUEST_FRAME_RUBBER = 2023;
var IGREQUEST_FRAME_SAVE_BM = 2024;

var IGSMANSWER_ACTIONDONE = 6004;
var IGSMANSWER_UPLOADED = 6005;
var IGSMANSWER_DELETED = 6006;
var IGSMANSWER_ERROR_NONE = 6665;
var IGSMANSWER_ERROR_TIMEOUT = 6719;

var UPLAYER_OFFSETX = 300;
var UPLAYER_OFFSETY = 200;
var WORKSPACE_OFFSETX = 80;
var WORKSPACE_OFFSETY = 120;
var DZ_OFFSETX = WORKSPACE_OFFSETX;
var DZ_OFFSETY = WORKSPACE_OFFSETY + 130;
var MANAGERS_WIDTH = 150;
var TOOLBOX_WIDTH = 60;

var ACCESS_ERROR = 'You are currently running a demo version. Please create an account for free or login with facebook to use that feature';
var GENERIC_ERROR = 'The connection has been lost. Please note that idle sessions get disconnected after 5 minutes, and only one connection per computer is allowed. Please reconnect.';
function IG_internalAPI(functionName, jsonData, successCallback, errorCallback) {
    if (jsonData == null)
        jsonData = {};
    var asmxName = null;
    switch (functionName) {
        case "OpenImage":
        case "DeleteImage":
        case "UploadImage":
        case "DownloadImage":
        case "SetCurrentView":
            asmxName = "IGWSAccount.asmx/";
            break;
        case "NewImage":
            asmxName = "IGWSWorkspace.asmx/";
            break;        
        case "CloseImage":
        case "UpdateImage":
        case "SaveImage":
        case "SaveImageBM":
        case "FilterImage":
        case "Copy":
        case "Cut":
        case "Paste":
        case "Delete":
        case "SmartDelete":
        case "SetViewport":
        case "LayerAdd":
        case "LayerMove":
        case "LayerMerge":
        case "GotoHistory":
        case "ChangeProperty":
            if (jsonData.FrameId == null)
                jsonData.FrameId = IG_internalGetCurrentFrameId();
            asmxName = "IGWSWorkspace.asmx/";
            break;
        case "DrawLines":
        case "Rubber":
        case "PickColor":
        case "LayerRemove":
        case "LayerSetVisible":
        case "ChangeSelection":
        case "MovePixels":
        case "RotateAndResize":
        case "SelectLayer":
            if (jsonData.FrameId == null)
                jsonData.FrameId = IG_internalGetCurrentFrameId();
            if (jsonData.LayerId == null)
                jsonData.LayerId = IG_internalGetCurrentWorkingLayer();
            asmxName = "IGWSWorkspace.asmx/";
            break;
        case "InitConnection":
        case "CheckConnection":
        case "ConnectDemo":
        case "ConnectUser":
        case "ConnectFbGuest":
        case "ConnectTwitterGuest":
		case "ConnectBitlUser":
		case "DisconnectUser":
		case "GetStatus":
            asmxName = "ImageniusPublicWS.asmx/";
            break;
        default:
            if (errorCallback)
                errorCallback("Error", "Unknown imagenius function name");
            return;
    }
    $.ajax({
        url: IG.urlDomain + asmxName + functionName,
        data: JSON.stringify(jsonData),
        dataType: 'json',
        type: 'POST',
        contentType: 'application/json; charset=utf-8',
        crossDomain: true,
        success: function (data) {
            var jsonData = jQuery.parseJSON(data.d);
            IG_ProcessAnswersCallback(jsonData, successCallback, errorCallback);
        },
        error: function (data, status, jqXHR) {
            if (errorCallback)
                errorCallback(status, jqXHR);
            else {
                $.ajax({
                    url: IG.urlDomain + "ImageniusPublicWS.asmx/DisconnectUser",
                    data: '{}',
                    dataType: 'json',
                    type: 'POST',
                    contentType: 'application/json; charset=utf-8',
                    crossDomain: true,
                    success: function (data) {
                        IG_internalReconnectUser(function (data) {
                            IG_internalAlertClient('Sorry, an error occured in your last request. Please check the arguments of your last request and try again. If this happens again please notify the problem below and we will fix it very shorty. Thanks! ' + status + ': ' + jqXHR, true);
                        }, function (data, status, jqXHR) {
                            IG_internalAlertClient(GENERIC_ERROR + ' ' + status + ': ' + jqXHR, true);
                        });
                    },
                    error: function (data, status, jqXHR) {
                        IG_internalAlertClient(GENERIC_ERROR + ' ' + status + ': ' + jqXHR, true);
                    }
                });
            }
        }
    });
}

function IG_publicAPI(functionName, jsonData, successCallback, errorCallback) {
    if (functionName == "DownloadImage" || functionName == "SaveImage" || functionName == "SaveImageCustom" ||
        functionName == "SaveAllImages" || functionName == "SaveImageBM" || functionName == "UploadImage" || functionName == "DeleteImage") {
        if (IGWS_WORKSPACE_DIV.getAttribute("UserName") == "Demo") {
            if (errorCallback)
                errorCallback("Access denied", ACCESS_ERROR);
            else
                IG_internalAlertClient(ACCESS_ERROR, true, "Access denied");
            return;
        }
    }
    if (errorCallback == null)
        errorCallback = function (status, jqXHR) {
            IG_internalAlertClient(jqXHR, true, status);};
    switch (functionName) {
        case "OpenImage":
            IG_internalAccountOpenImage(jsonData, successCallback, errorCallback);
            return;
        case "ConnectFbGuest":
            IG_internalConnectFbGuest(jsonData, successCallback, errorCallback);
            return;
		case "ConnectBitlUser":
            IG_internalBitlUser(jsonData, successCallback, errorCallback);
            return;
        case "ConnectTwitterGuest":
            IG_internalConnectTwGuest(jsonData, successCallback, errorCallback);
            return;
        case "CloseImage":
            if (jsonData == null)
                jsonData = {};
            if (jsonData.FrameId == "-1") {
                IG_internalCloseAllImages(successCallback, errorCallback);
                return;
            }
            break;
        case "FilterImage":
            IG_internalFilterImage(jsonData, successCallback, errorCallback);
            return;
        case "LayerMoveDown":
            IG_internalMoveLayerDown(jsonData, successCallback, errorCallback);
            return;
        case "LayerMoveUp":
            IG_internalMoveLayerUp(jsonData, successCallback, errorCallback);
            return;
        case "GotoHistory":
            IG_internalGotoHistory(jsonData, successCallback, errorCallback);
            return;
        case "SetCurrentView":
            IG_internalSetCurrentView(jsonData, successCallback, errorCallback);
            return;
        case "DownloadImage":            
            IG_internalAccountDownload(jsonData, successCallback, errorCallback);
            return;
        case "ChangeSelection":
            IG_internalChangeSelection(jsonData, successCallback, errorCallback);
            return;
        case 'NewImage':
            if (jsonData == null) {
                IG_internalAccountNew(successCallback, errorCallback);
                return;
            } break;
        case 'OpenImage':
            if (jsonData == null){
                IG_internalAccountOpen(successCallback, errorCallback);
                return;
            } break;
        case 'SaveImage':
            if (jsonData == null){
                IG_internalSaveImage(successCallback, errorCallback);
                return;
            } break;
        case 'SaveImageCustom':
            if (jsonData == null){
                IG_internalSaveImageCustom(successCallback, errorCallback);
                return;
            } break;
        case 'SaveAllImages':
            if (jsonData == null){
                IG_internalSaveAllImages(successCallback, errorCallback);
                return;
            } break;
        case 'CloseImage':
            if (jsonData == null){
                IG_internalCloseImage(successCallback, errorCallback);
                return;
            } break;
        case 'CloseAllImages':
            if (jsonData == null){
                IG_internalCloseAllImages(successCallback, errorCallback);
                return;
            } break;
        case 'ChangeProperty':
            if (jsonData == null){
                IG_internalChangeFrameProperties(successCallback, errorCallback);
                return;
            } break;
    }
    IG_internalAPI(functionName, jsonData, successCallback, errorCallback);
}

var IG_resizeBusy = false;
var IG_resizeTimeoutId = null;
var IG_internalOnResize_callback = null;

function IG_internalOnResize() {
    if (IG_resizeTimeoutId == null) {
        IG_resizeTimeoutId = setTimeout(function () { IG_internalOnResize(); }, 1000);
        return;
    }
    if (!IG_resizeBusy) {
        IG_resizeBusy = true;        
        var parentWidth = IGWS_WORKSPACE_DIV.parentNode ? IGWS_WORKSPACE_DIV.parentNode.offsetWidth : 800;
        var parentHeight = IGWS_WORKSPACE_DIV.parentNode ? IGWS_WORKSPACE_DIV.parentNode.offsetHeight : 600;
        var IG_divWorkspace = IGWS_WORKSPACE_DIV;
        if (IG_divWorkspace) {
            IG_internalResizeElement(IG_divWorkspace, WORKSPACE_OFFSETX, WORKSPACE_OFFSETY);
            IG_divWorkspace.style.width = Math.min(IG_divWorkspace.offsetWidth, parentWidth).toString() + "px";
            IG_divWorkspace.style.height = Math.min(IG_divWorkspace.offsetHeight, parentHeight).toString() + "px";
        }
        var IG_tableMain = IG_internalGetElementById("IG_tableMain");
        if (IG_tableMain) {
            IG_internalResizeElement(IG_tableMain, WORKSPACE_OFFSETX, WORKSPACE_OFFSETY);
            IG_tableMain.style.width = Math.min(IG_tableMain.offsetWidth, parentWidth).toString() + "px";
            IG_tableMain.style.height = Math.min(IG_tableMain.offsetHeight, parentHeight).toString() + "px";
        }
        var IG_divImageLibraryScroll = IG_internalGetElementById("IG_divImageLibraryScroll");
        if (IG_divImageLibraryScroll) {
            IG_internalResizeElement(IG_divImageLibraryScroll, WORKSPACE_OFFSETX, WORKSPACE_OFFSETY);
            IG_divImageLibraryScroll.style.width = parentWidth.toString() + "px";
            IG_divImageLibraryScroll.style.height = parentHeight.toString() + "px";
        }
        var IG_deepZoomPanel = IG_internalGetElementById("IG_deepZoomPanel");
        var IG_divManagers = IG_internalGetElementById("IG_divManagers");
        var IG_panelToolBox = IG_internalGetElementById("IG_panelToolBox");
        if (IG_deepZoomPanel && IG_divManagers && IG_panelToolBox) {
            IG_deepZoomPanel.style.left = 0;
            IG_deepZoomPanel.style.top = 0;
            IG_deepZoomPanel.style.width = (parentWidth - (IG_panelToolBox.style.display != "none" ? TOOLBOX_WIDTH : 0) - (IG_divManagers.style.display != "none" ? MANAGERS_WIDTH : 0)).toString() + "px";
            IG_deepZoomPanel.style.height = parentHeight.toString() + "px";
            IG_internalResizeElement(IG_divManagers, 0, DZ_OFFSETY);
            IG_divManagers.style.width = (MANAGERS_WIDTH - 20).toString() + "px";
            IG_panelToolBox.style.width = TOOLBOX_WIDTH.toString() + "px";
        }
        var divUpperLayer = IG_internalGetElementById("IG_divUpperlayer");
        if (divUpperLayer)
            IG_internalResizeElement(divUpperLayer, UPLAYER_OFFSETX, UPLAYER_OFFSETY);
        var IG_overCanvas = IG_internalGetElementById("IG_overCanvas");
        if (IG_overCanvas.parentNode.id == "IG_divUpperlayer") {
            IG_internalPopInUpperLayer();
            IG_internalResizeElement(IG_overCanvas, UPLAYER_OFFSETX + 30, UPLAYER_OFFSETY);
            var width = (IG_deepZoomPanel.offsetWidth - (IG_panelToolBox.style.display != "none" ? TOOLBOX_WIDTH : 0) - (IG_divManagers.style.display != "none" ? MANAGERS_WIDTH : 0)).toString();
            var height = IG_deepZoomPanel.offsetHeight.toString();
            IG_overCanvas.width = width;
            IG_overCanvas.height = height;
            //IG_overCanvas.clientWidth = width;
            //IG_overCanvas.clientHeight = height;
            IG_overCanvas.style.width = width + "px";
            IG_overCanvas.style.height = height + "px";
            IG_internalPopOutUpperLayer();
        }
        if (IGWS_WORKSPACE_DIV.FaceDescriptor != null)
            IGWS_WORKSPACE_DIV.FaceDescriptor.forceRefresh();
        IG_resizeTimeoutId = null;
        IG_resizeBusy = false;
        if (IG_internalOnResize_callback != null)
            IG_internalOnResize_callback();    
    }
}

function IG_internalConnectDemo(successCallback, errorCallback) {
    var jsonData = {};
    IG_internalAPI("ConnectDemo", jsonData,
       function (data) {
           var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
           divCurrentWorkspace.setAttribute("UserName", "Demo");
           divCurrentWorkspace.setAttribute("UserEmail", jsonData.UserLogin);
           IG_ProcessAnswersCallback(data);
           if (successCallback)
               successCallback(data);
       },
       function (status, jqXHR) {
           if (errorCallback)
               errorCallback(status, jqXHR);
           else
               IG_internalAlertClient(GENERIC_ERROR, true);
       }
    );
}

function IG_internalBitlUser(successCallback, errorCallback) {
    if (userEmail == null || userEmail == "") {
        IG_internalAlertClient('Please specify a valid user email', true);
        return;
    }
    if (userPwd == null || userPwd == "") {
        IG_internalAlertClient('Please specify the password', true);
        return;
    }
    IG_internalAPI("InitConnection", null,
        function (data) {
            IG_internalAPI("ConnectBitlUser", {}, successCallback, errorCallback);
        }, errorCallback
    );
}

function IG_internalReconnectUser(demo, successCallback, errorCallback) {
    var divWorkspace = IGWS_WORKSPACE_DIV;
    if (divWorkspace.getAttribute("UserName") == null || divWorkspace.getAttribute("UserEmail") == null) {
        if (errorCallback)
            errorCallback("Error", "User reconnection error");
        return;
    }
    if (divWorkspace.getAttribute("UserName") == "Demo") {
        IG_internalConnectDemo();
    }
    else {
        var jsonData = { 'UserName': divWorkspace.getAttribute("UserName"),
            'UserEmail': divWorkspace.getAttribute("UserEmail")
        };
        IG_internalConnectFbGuest(jsonData,
           function (data) {
               IG_ProcessAnswersCallback(data);
               if (successCallback)
                   successCallback(data);
           },
           function (status, jqXHR) {
               if (errorCallback)
                   errorCallback(status, jqXHR);
               else
                   IG_internalAlertClient(GENERIC_ERROR, true);
           }
        );
    }
}
 
function IG_internalOnUserConnected(data) {
     var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
     divCurrentWorkspace.setAttribute("currentPictureIndex", "0");
     divCurrentWorkspace.setAttribute("outputPath", data.UserOutputPath);
     var tOutputCmp = data.UserOutputPath.split(new RegExp("[/]+", "g"));
     divCurrentWorkspace.setAttribute("outputDownloadsPath", tOutputCmp[0] + "/Downloads/" + tOutputCmp[1] + "/");
     var reqGuid = data.RequestGuid;
          
     // Image Library
     var IG_divImageLibrary = IG_internalGetElementById("IG_divImageLibrary");
     IG_internalAddImagesToDiv(IG_divImageLibrary, data.MiniPictureList, true);

     if (data.PictureNames != null && data.PictureNames != "") {
         IG_internalShowWorkspace(data);
     }
     else {
         if (data.MiniPictureList.length > 0)
             IG_internalShowAccount();
         // Update Properties
         IG_internalUpdateProperties(data.WorkspaceProperties, data.FrameProperties);
     }
 }

 function IG_internalProcessWorkspaceAnswers(data, successCallback, errorCallback) {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    var error = false;
    if (data.AnswerId != IGANSWER_FRAME_ACTIONINPROGRESS)
        IG_internalSetProgress(100);
    switch (data.AnswerId) {
        case IGANSWER_WORKSPACE_CONNECTED:
            IG_internalOnUserConnected(data);            
            break;
        case IGANSWER_WORKSPACE_ACTIONFAILED:
        case IGANSWER_FRAME_ACTIONFAILED:
            var errMsg = null;
            if (data.RequestParams.RequestId == IGREQUEST_FRAME_FILTER)
                errMsg = "Aie aie aie, something went wrong.\nPlease make sure the faces are accurately placed before applying a filter.\nPress F5 to reload the page and try again.\nIf this happens again, please notify the problem below and we will fix it very shortly";
            else
                errMsg = "Aie aie aie, something went wrong.\nPlease check the arguments.\nPress F5 to reload the page and try again.\nIf this happens again, please notify the problem below and we will fix it very shortly";
            error = true;
            if (errorCallback)
                errorCallback("Error", errMsg);
            else
                IG_internalAlertClient(errMsg, true);
            return;
        case IGANSWER_FRAME_ACTIONINPROGRESS:
            IG_internalSetProgress(data.Progress);
            break;
        case IGANSWER_WORKSPACE_PROPERTYCHANGED:
            if (data.IsFrame)
                IG_internalUpdateAllFrameProperties(data.Properties);
            else
                IG_internalUpdateWorkspaceProperties(data.Properties);
            break;
        case IGANSWER_FRAME_CHANGED:
            IG_internalAddWorkspacePicture(data.PictureId, data.RequestGuid, data.FrameProperties, data.PictureName);
            IG_internalUpdateAllFrameLayersAndHistory(data.NbLayers, data.LayerVisibility, data.History);
            IG_internalUpdateWorkspaceProperties(data.WorkspaceProperties);
            IG_internalUpdateAllFrameProperties(data.FrameProperties);
            IG_internalSelectWorkspacePicture(data.PictureId + "/DZ" + data.RequestGuid, data.PictureName);
            if (data.RequestParams.RequestId == IGREQUEST_WORKSPACE_LOADIMAGE || data.RequestParams.RequestId == IGREQUEST_WORKSPACE_NEWIMAGE) {
                IG_internalSelectWorkspacePicture(data.PictureId + "/DZ" + data.RequestGuid, data.PictureName);
            }
            else if (data.RequestParams.RequestId == IGREQUEST_FRAME_ADDLAYER ||
                        data.RequestParams.RequestId == IGREQUEST_FRAME_MERGELAYER ||
                        data.RequestParams.RequestId == IGREQUEST_FRAME_REMOVELAYER) {
                IG_internalSelectWorkspacePicture(data.PictureId + "/DZ" + data.RequestGuid, data.PictureName);
                var idxLastLayer = Number(divCurrentWorkspace.getAttribute("currentPictureNbLayers")) - 1;
                IG_internalSelectLayer(idxLastLayer);
            }
            else if (data.RequestParams.RequestId == IGREQUEST_FRAME_GOTOHISTORY) {
                IG_internalSelectWorkspacePicture(data.PictureId + "/DZ" + data.RequestGuid, data.PictureName, false);
            }
            else {
                IG_internalSelectWorkspacePicture(data.PictureId + "/DZ" + data.RequestGuid, data.PictureName);
            }            
            if (divCurrentWorkspace.getAttribute("currentView") != "Workspace")
                IG_internalShowWorkspace(null);
            IG_internalOnResize();            
            if (IGWS_DEEPZOOM_DIV.Viewer) {
                if (IGWS_DEEPZOOM_DIV.Viewer.viewport)
                    IGWS_DEEPZOOM_DIV.Viewer.viewport.goHome();
            }
            break;
        case IGANSWER_FRAME_ACTIONDONE:
            if (data.RequestParams.RequestId == IGREQUEST_FRAME_SAVE) {
                var IG_divImageLibrary = IG_internalGetElementById("IG_divImageLibrary");
                addImageToDiv(IG_divImageLibrary, data.RequestParams.MiniPictureName, data.RequestParams.MiniPicturePath, data.RequestParams.MiniPictureWidth, data.RequestParams.MiniPictureHeight);
                IG_internalAlertClient("Your picture has been saved successfully");
            }
            else if (data.RequestParams.RequestId == IGREQUEST_FRAME_CLOSE) {
                IG_internalOnPictureClosed(data.RequestParams.NbLayers, data.RequestParams.LayerVisibility, data.History);
            }
            else if (data.RequestParams.RequestId == IGREQUEST_FRAME_COPY || data.RequestParams.RequestId == IGREQUEST_FRAME_CUT) {
                IG_internalUpdateAllFrameProperties(data.FrameProperties);
                IG_internalAlertClient("Your selection has been copied to the clipboard. You can now paste it as new layer on this current picture or another one.");
            }
            else {
                IG_internalUpdateAllFrameProperties(data.FrameProperties);
            }
            break;
        default:
            var errMsg = "Unknown answer: Answer Id " + data.AnswerId + " - Request Id " + data.RequestParams.RequestId;
            alert(errMsg);
            if (errorCallback)
                errorCallback("Error", GENERIC_ERROR);
            else
                IG_internalAlertClient(GENERIC_ERROR, true);
            return;
    }
    if (successCallback)
        successCallback(data);
}

function IG_internalProcessServerAnswers(data, successCallback, errorCallback) {
    if (data.AnswerId > IGSMANSWER_ERROR_NONE) {
        switch (data.AnswerId) {
            case IGSMANSWER_ERROR_TIMEOUT:                
                var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
                divCurrentWorkspace.style.display = "none";
                IG_internalReconnectUser();
                var errMsg = "Aie aie aie, something went wrong. Either there are too many faces in the photo (10 max), the image is too large or our servers are currently too busy. Please press F5 to refresh the page and try again with another picture. If this happens again, please notify the problem below and we will fix it very shortly.\nError code: " + data.AnswerId;
                if (fbGetAlbums)
                    fbGetAlbums();
                if (errorCallback)
                    errorCallback("Error", errMsg);
                else
                    IG_internalAlertClient(errMsg, true);
                return;
            default:
                IG_internalReconnectUser();
                var errMsg = "Aie aie aie, something went wrong. Please press F5 to refresh the page and restart. If this happens again, please notify the problem below and we will fix it very shortly.\nError code: " + data.AnswerId;
                if (fbGetAlbums)
                    fbGetAlbums();
                if (errorCallback)
                    errorCallback("Error", errMsg);
                else
                    IG_internalAlertClient(errMsg, true);
                return;
        }        
        return;
    }
    switch (data.AnswerId) {
        case IGSMANSWER_UPLOADED:
            IG_internalAccountUploadResponse(data);
            break;
        case IGSMANSWER_DELETED:
            IG_internalAccountDeleteResponse(data);
            break;
        case IGSMANSWER_ACTIONDONE:
            break;
        default:
            alert("Not yet implemented: AnswerId " + data.AnswerId);
            if (errorCallback)
                errorCallback("Error", GENERIC_ERROR);
            else
                IG_internalAlertClient(GENERIC_ERROR, true);
            return;
    }
    if (successCallback)
        successCallback(data);
}

function IG_ProcessAnswersCallback(result, successCallback, errorCallback) {
    waitingForReply = false;
    if (result) {
        if (result.Status == "DisconnectedBySSO")
            return;
        else {
            if ((result.Status == "Disconnected") || (result.Status == "Error")) {
                var errMsg = result.Message ? GENERIC_ERROR + ". Error description: " + result.Message : GENERIC_ERROR;
                if (errorCallback)
                    errorCallback("Error", errMsg);
                else {
                    IG_internalAlertClient(errMsg, true);
                }
                return;
            }
        }   
    }
    if (result.AnswerId) {
        if ((result.AnswerId >= 3000) &&
        (result.AnswerId < 5000)) {
            IG_internalProcessWorkspaceAnswers(result, successCallback, errorCallback);
        } else if ((result.AnswerId >= 6000) &&
                (result.AnswerId < 7000)) {
            IG_internalProcessServerAnswers(result, successCallback, errorCallback);
        }
        else {
            var errMsg = "Error reading answer: " + JSON.stringify(result);
            alert(errMsg);
            if (errorCallback)
                errorCallback("Error", GENERIC_ERROR);
            else
                IG_internalAlertClient(GENERIC_ERROR, true);
        }
    }
    else if (successCallback)
        successCallback(result);
}

function IG_internalShowAccount() {    
    var jsonData = { "CurrentView": "Account" };
    IG_publicAPI("SetCurrentView", jsonData);
}

function IG_internalConnectFbGuest(jsonData, successCallback, errorCallback) {
    if (jsonData == null)
        jsonData = {};
    if (jsonData.UserName == null) {
        IG_internalAlertClient('Please specify a valid user name', true);
        return;
    }
    if (jsonData.UserEmail == null) {
        IG_internalAlertClient('Please specify a valid user email or id', true);
        return;
    }
    IG_internalAPI("InitConnection", null,
        function (data) {
            var divWorkspace = IGWS_WORKSPACE_DIV;
            divWorkspace.setAttribute("UserName", jsonData.UserName);
            divWorkspace.setAttribute("UserEmail", jsonData.UserEmail);
            jsonData.Password = Sha256.hash(jsonData.UserName + data.Realm + data.PublicKey + jsonData.UserEmail.split('@')[0], false);
            IG_internalAPI("ConnectFbGuest", jsonData, successCallback, errorCallback);
        }, errorCallback
    );
    } 
    function IG_internalConnectTwGuest(jsonData, successCallback, errorCallback) {
        if (jsonData == null)
            jsonData = {};
        if (jsonData.UserName == null) {
            IG_internalAlertClient('Please specify a valid user name', true);
            return;
        }
        if (jsonData.UserEmail == null) {
            IG_internalAlertClient('Please specify a valid user email or id', true);
            return;
        }
        IG_internalAPI("InitConnection", null,
        function (data) {
            var divWorkspace = IGWS_WORKSPACE_DIV;
            divWorkspace.setAttribute("UserName", jsonData.UserName);
            divWorkspace.setAttribute("UserEmail", jsonData.UserEmail);
            jsonData.Password = Sha256.hash(jsonData.UserName + data.Realm + data.PublicKey + jsonData.UserEmail.split('@')[0], false);
            IG_internalAPI("ConnectTwitterGuest", jsonData, successCallback, errorCallback);
        }, errorCallback
    );
    }