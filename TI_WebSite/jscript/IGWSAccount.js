/*****************************************************************************\

 Javascript "Imagenius Account Managing Web Service" library
 
 @version: 1.0 - 2010.02.02
 @author: Jonathan Betser
\*****************************************************************************/
var IGSMANSWER_ACTIONDONE = 6005;
var IGSMANSWER_ERROR = 6666;
var IGSMREQUEST_UPLOAD = "SMUpload";
var IGSMREQUEST_DELETE = "SMDeleteImage";
var IGSMREQUEST_DOWNLOAD = "SMDownload";

var CLOSECOORD_LEFT = 147;
var CLOSECOORD_TOP = 3;
var CLOSECOORD_RIGHT = 160;
var CLOSECOORD_BOTTOM = 14;

var selectedSpan = null;

function IG_internalSelectSpan(span)
{
    var spanId = span.getAttribute("id");
    if (selectedSpan)
    {
        selectedSpan.setAttribute("onclick", "IG_internalOnImageSpanClick(this, event);");
        selectedSpan.setAttribute("style", "display: inline-block; background-image: url(images/frameSilver.png); margin: 5px; width: 168px; height: 181px; padding: 0px"); 
    }
    selectedSpan = span;
    selectedSpan.setAttribute("onclick", "IG_internalOnSelectedImageSpanClick(this, event);");
    selectedSpan.setAttribute ("style", "display: inline-block; background-image: url(images/frameBlack.png); margin: 5px; width: 168px; height: 181px; padding: 0px"); 
    var IG_divImageLibrary = IG_internalGetElementById ("IG_divImageLibrary");
    IG_divImageLibrary.setAttribute("title", selectedSpan.getAttribute("title"));
}

function IG_internalOnImageSpanClick(sender, event)
{
    IG_internalSelectSpan(sender);
}

function IG_internalAccountOpen() {
    var sTitleImageLibrary = IG_internalGetElementById("IG_divImageLibrary").getAttribute("title");
    var jsonData = { 'ImageName': sTitleImageLibrary, 'AutoRotate': true };
    IG_publicAPI("OpenImage", jsonData);
}

function IG_internalOnSelectedImageSpanClick(sender, event)
{
    IG_internalSelectSpan(sender);
    if (IG_internalIsInArea(CLOSECOORD_LEFT, CLOSECOORD_TOP, CLOSECOORD_RIGHT, CLOSECOORD_BOTTOM, event.offsetX, event.offsetY)) {
        var jsonData = { 'ImageName': sender.getAttribute("title") };
        IG_publicAPI("DeleteImage", jsonData);
    }
    else {
        IG_internalAccountOpen();   
    }
}

function IG_internalOnOpenImage(sender, event)
{
    IG_internalSelectSpan(sender);
    IG_internalAccountOpen();   
}

function IG_internalAccountDownload() {
    var currentPicName = IG_internalGetCurrentPictureName();
    if (currentPicName) {
        var jsonData = { 'ImageName': currentPicName };
        IG_publicAPI("DownloadImage", jsonData);
    }
}

function IG_internalAccountUploadResponse(data)
{
    var IG_divImageLibrary = IG_internalGetElementById("IG_divImageLibrary");
    IG_internalAddImagesToDiv(IG_divImageLibrary, data.RequestParams.MiniPictureList);
    if (data.RequestParams.MiniPictureList.length > 0) {
        var jsonData = { "ImageName" : data.RequestParams.MiniPictureList[data.RequestParams.MiniPictureList.length - 1].Name };
        IG_internalAccountOpenImage(jsonData);
    }
}

function IG_internalAccountDeleteResponse(data)
{
    var reg = new RegExp("[?]+", "g");
    var spanImage = IG_internalGetElementById("ImageSpan" + data.PictureName);
    var IG_divImageLibrary = IG_internalGetElementById ("IG_divImageLibrary");
    var divNewImageLibrary = document.createElement ("div");	
    for(var i=0; i<IG_divImageLibrary.children.length; i++)
    {
        if (IG_divImageLibrary.children[i].id != spanImage.id)
            divNewImageLibrary.appendChild(IG_divImageLibrary.children[i].cloneNode(true));
    }                    
    IG_internalCleanNode(IG_divImageLibrary);
    IG_internalCopyNode(IG_divImageLibrary, divNewImageLibrary);
}

function IG_internalAccountNew(successCallback, errorCallback)
{
    var modal = new IG_Modal();
    var tParams = {};
    var idxParam = 0;
    var reg = new RegExp("[,]", "g");
    var selRectOrig = IG_internalGetCurrentPictureProperty("SelectionRectOrig");
    var width = 1024;
    var height = 768;
    if (selRectOrig) {
        var tRcSel = selRectOrig.split(reg);
        if (tRcSel[1] == -1 || tRcSel[3] == -1) {
            tRcSel[0] = "0";
            tRcSel[1] = (Number(IG_internalGetCurrentPictureProperty("Width")) - 1).toString();
            tRcSel[2] = "0";
            tRcSel[3] = (Number(IG_internalGetCurrentPictureProperty("Height")) - 1).toString();
        }
        width = Math.abs(Number(tRcSel[1]) - Number(tRcSel[0]));
        height = Math.abs(Number(tRcSel[3]) - Number(tRcSel[2]));
    }
    tParams[idxParam++] = "Create a new picture";
    tParams[idxParam++] = function (result, tOptions) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "Width": tOptions[0],
                "Height": tOptions[1]
            };
            IG_internalAPI("NewImage", jsonData);
        }
    };
    tParams[idxParam++] = 2;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = width.toString();
    tParams[idxParam++] = "Width";
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = height.toString();
    tParams[idxParam++] = "Height";    
    modal.popOut(tParams);
}

function IG_internalAccountOpenImage(data, successCallback, errorCallback) {
    if (data == null)
        data = {};
    if (data.ImageName == null) {
        IG_internalAlertClient("Please specify an image name");
        return;
    }
    if (data.CloseAll == null)
        data.CloseAll = IGWS_WORKSPACE_DIV.getAttribute("MultiPictures") != "1";
    if (data.AutoRotate == null)
        data.AutoRotate = true;
    IG_internalSelectTool(IG_internalGetElementById("IGTOOL_FACEDESCRIPTOR"));
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    divCurrentWorkspace.style.display = "inherit";
    var divDeepZoomPanel = IG_internalGetElementById("IG_tableMain");
    divDeepZoomPanel.style.display = "inline-block";

    var ulWorkspacePictures = IG_internalGetWorkspaceMenuList();
    if (data.CloseAll) {
        IG_internalCleanNode(ulWorkspacePictures);
        IG_internalUpdateMenuList();
    }
    var isUnique = false;
    var desambigNb = 1;
    var desambigName = data.ImageName;
    var baseName = data.ImageName.substr(0, data.ImageName.lastIndexOf('.'));
    var ext = data.ImageName.substr(data.ImageName.lastIndexOf('.'));
    while (!isUnique) {
        isUnique = true;
        for (var idxPicture = 0; idxPicture < ulWorkspacePictures.children.length; idxPicture++) {
            if (ulWorkspacePictures.children[idxPicture].children[0].innerHTML == desambigName) {
                isUnique = false;
                desambigNb++;
                desambigName = baseName + "(" + desambigNb + ")" + ext;
                break;
            }
        }
    }
    if ((data.LoadAs == null) || (desambigName == data.ImageName))
        data.LoadAs = "";
    else
        data.LoadAs = desambigName;
    IG_internalAPI("OpenImage", data, successCallback, errorCallback);
}

function IG_internalAccountDownload(jsonData, successCallback, errorCallback) {
    if (jsonData == null)
        jsonData = {};
    IG_internalAPI("SaveImageBM", jsonData,
        function (data) {
            if (jsonData.ImageName != null)
                jsonData.ImageName = "BeetleMorph\\" + jsonData.ImageName;
            IG_internalAPI("DownloadImage", jsonData, successCallback, errorCallback);
        }, errorCallback
    );
}

function IG_internalShowAccount_callback() {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    divCurrentWorkspace.setAttribute("currentView", "Account");

    var IG_AccountMenu = IG_internalGetElementById("IG_AccountMenu");
    var IG_divCurrentMenu = IG_internalGetElementById("IG_divCurrentMenu");
    if (IG_divCurrentMenu) {
        IG_internalCleanNode(IG_divCurrentMenu);
        var currentMenu = IG_AccountMenu.cloneNode(true);
        currentMenu.id = "IG_currentMenu";
        IG_divCurrentMenu.appendChild(currentMenu);
    }
    divCurrentWorkspace.style.display = "none";
    IG.showImageLibrary();

    IGWS_DEEPZOOM_DIV.lastOpenDZ = "";
    IG_internalOnResize();
}

function IG_internalSetCurrentView(jsonData, successCallback, errorCallback) {
    if (jsonData == null)
        jsonData = {};
    IG_internalAPI("SetCurrentView", jsonData,
    function (data) {
        if (jsonData.CurrentView == "Workspace") {
            IG_internalShowWorkspace_callback();
        }
        else if (jsonData.CurrentView == "Account") {
            IG_internalShowAccount_callback();
        }
        if (successCallback)
            successCallback(data);
    }, errorCallback
    );
}