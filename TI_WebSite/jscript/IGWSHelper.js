/*****************************************************************************\

Javascript "BeetleMorph Web Service Client" library helpers
 
@version: 1.0 - 2013.03.15
@author: Jonathan Betser
\*****************************************************************************/
var IGWS_RESULT_OK = "OK";
var IGWS_RESULT_ERROR = "Error";
var IGWS_RESULT_DISCONNECTED = "Disconnected";
var IGWS_RESULT_DISCONNECTED_MESSAGE = "You are not connected to server.";
var IGWS_WORKSPACE_DIV = null;
var IGWS_DEEPZOOM_DIV = null;
var IGBM_APPID = "480781025322684";
var IGBM_CHANNEL = '//bitlsoft.com/beetlemorph/channel.html'
var IG_PIXEL_SELECTOR_PRECISION = 1000000;
var IG_WEBSERVICE_URL = "http://localhost:8080/WebDebug/";

function IG_internalContactUs(subject) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Contact us";
    tParams[idxParam++] = null;
    tParams[idxParam++] = null;
    tParams[idxParam++] = null;
    tParams[idxParam++] = 1;
    tParams[idxParam++] = IGMODALTYPE_IFRAME;
    tParams[idxParam++] = 500;
    tParams[idxParam++] = 400;
    tParams[idxParam++] = IG.urlAppli + "ContactUs.aspx" + (subject ? "?Subject=" + subject : "");
    modal.popOut(tParams);
}

function IG_internalSignUp(subject) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = { 'OK': 'Connect' }
    tParams[idxParam++] = "Login to BITL";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = {}
            IG_publicAPI("ConnectBitlUser", jsonData, successCallback, errorCallback);
        }
    };
    tParams[idxParam++] = null;
    tParams[idxParam++] = null;
    tParams[idxParam++] = 1;
    tParams[idxParam++] = IGMODALTYPE_IFRAME;
    tParams[idxParam++] = 500;
    tParams[idxParam++] = 450;
    tParams[idxParam++] = IG.urlAppli + "CreateAccount.aspx";
    modal.popOut(tParams);
}

function IG_internalAbout() {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "About Imagenius";
    tParams[idxParam++] = null;
    tParams[idxParam++] = null;
    tParams[idxParam++] = null;
    tParams[idxParam++] = 1;
    tParams[idxParam++] = IGMODALTYPE_MESSAGE;
    tParams[idxParam++] = "This service is provided by the B.I.T.L. company.\nPlease give us a feedback in 'Help' -> 'Contact Us'.\nWe will do our best to improve the service :)";
    modal.popOut(tParams);
}

function IG_internalAlertClient(message, isError, title) {
    if (!message || (message.indexOf("Internal") == 0)) {
        alert(message);
        return; // avoid showing off ugly error messages
    }
    var IG_Modal_id = IG_internalGetElementById("IG_Modal");
    if (IG_Modal_id == null) {
        var modal = new IG_Modal();
        var idxParam = 0;
        var tParams = {};
        tParams[idxParam++] = (title != null ? title : (isError ? "Please reconnect" : "Information"));
        tParams[idxParam++] = function (result, tOptions) {
            if (result == IGMODALRETURN_REPORTPB)
                IG_internalContactUs("I have found a bug");
            else if (result == IGMODALRETURN_RECONNECT)
                location.reload(false);
        };
        tParams[idxParam++] = null;
        tParams[idxParam++] = null;
        tParams[idxParam++] = 1;
        tParams[idxParam++] = isError ? IGMODALTYPE_ERROR : IGMODALTYPE_MESSAGE;
        tParams[idxParam++] = message;
        modal.popOut(tParams);
    }
}

function IG_internalGetUrlDomainOutput() {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    return IG.urlDomain + divCurrentWorkspace.getAttribute("outputPath");
}

function IG_internalGetUrlDownloadsOutput() {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    return IG.urlAppli + divCurrentWorkspace.getAttribute("outputDownloadsPath");
}

function IG_internalGetElementInDOM(div, id) {
    for (var idxElem = 0; idxElem < div.children.length; idxElem++) {
        if (div.children[idxElem].getAttribute("id") == id)
            return div.children[idxElem];
        var recElem = IG_internalGetElementInDOM(div.children[idxElem], id);
        if (recElem)
            return recElem;
    }
    return null;
}

function IG_internalGetElementById(id) {
    return window.document.getElementById(id);
}

function IG_internalCleanNode(node) {
    if (node) {
        while (node.childNodes.length >= 1) {
            node.removeChild(node.firstChild);
        }
    }
}

function IG_internalCopyNode(nodeDest, nodeSrcParent) {
    if (nodeDest && nodeSrcParent) {
        for (var i = 0; i < nodeSrcParent.children.length; i++) {
            nodeDest.appendChild(nodeSrcParent.children[i].cloneNode(true));
        }
    }
}

function IG_internalAddEvent(event_, func_, mode_) {
    if (window.addEventListener)
        window.addEventListener(event_, func_, mode_ ? mode_ : false);
    else
        window.attachEvent('on' + event_, func_);
}

function IG_internalIsInArea(l, t, r, b, x, y) {
    return ((x >= l) && (x <= r) && (y >= t) && (y <= b));
}

function IG_internalResizeElement(element, posX, posY, bIframe, bUpdate, fYCoeff) {
    if (typeof element === 'undefined')
        return;
    if (typeof fYCoeff === 'undefined')
        fYCoeff = 1.0;
    posX = posX ? posX : 0;
    posY = posY ? posY : 0;
    var offsets = $('#' + element.parentNode.getAttribute("id")).offset();
    if (offsets) {
        posX += offsets.left;
        posY += offsets.top;
    }
    var curWidth, curHeight;
    if (bIframe) {
        //iframe mode
        if (element.document) {  // IE8
            curWidth = element.document.body.clientWidth;
            curHeight = element.document.body.clientHeight;
        }
        else {
            curWidth = element.ownerDocument.body.clientWidth;
            curHeight = element.ownerDocument.body.clientHeight;
        }
    } else if (typeof (window.innerWidth) == 'number') {
        //Non-IE
        curWidth = window.innerWidth;
        curHeight = window.innerHeight;
    } else if (document.documentElement && (document.documentElement.clientWidth || document.documentElement.clientHeight)) {
        //IE 6+ in 'standards compliant mode'
        curWidth = document.documentElement.clientWidth;
        curHeight = document.documentElement.clientHeight;
    } else if (document.body && (document.body.clientWidth || document.body.clientHeight)) {
        //IE 4 compatible
        curWidth = document.body.clientWidth;
        curHeight = document.body.clientHeight;
    } else {
        IG_internalAlertClient("Your browser is not supported", true);
        return;
    }
    var reg = new RegExp("(%|px)+", "g");
    // iFrame management
    var attNameWidth, attNameHeight, attNameWidth2, attNameHeight2;
    var elemWidth, elemHeight;
    var newWidth, newHeight;
    if (element.getAttribute("width")) {
        attNameWidth = "width";
        attNameHeight = "height";
        attNameWidth2 = "clientWidth";
        attNameHeight2 = "clientHeight";
        elemWidth = Number((element.getAttribute(attNameWidth).split(reg))[0]);
        elemHeight = Number((element.getAttribute(attNameHeight).split(reg))[0]);
        if (isNaN(element.getAttribute(attNameWidth)) ||
            isNaN(element.getAttribute(attNameHeight))) {
            newWidth = (curWidth - posX).toString();
            newHeight = (Math.round((curHeight - posY) * fYCoeff)).toString();
        }
        else {
            newWidth = curWidth - posX;
            newHeight = Math.round((curHeight - posY) * fYCoeff);
        }
    } else if (element.getAttribute("clientWidth")) {
        // div management
        attNameWidth = "clientWidth";
        attNameHeight = "clientHeight";
        attNameWidth2 = "width";
        attNameHeight2 = "height";
        elemWidth = Number((element.getAttribute(attNameWidth).toString().split(reg))[0]);
        elemHeight = Number((element.getAttribute(attNameHeight).toString().split(reg))[0]);
        newWidth = curWidth - posX;
        newHeight = Math.round((curHeight - posY) * fYCoeff);
    } else {
        // div management
        attNameWidth = "clientWidth";
        attNameHeight = "clientHeight";
        attNameWidth2 = "width";
        attNameHeight2 = "height";
        elemWidth = Number(element.getAttribute(attNameWidth));
        elemHeight = Number(element.getAttribute(attNameHeight));
        newWidth = curWidth - posX;
        newHeight = Math.round((curHeight - posY) * fYCoeff);
    }
    if ((Math.abs(elemWidth - (curWidth - posX)) > 20) ||
        (Math.abs(elemHeight - (curHeight - posY)) > 20) || bUpdate) {
        element.setAttribute(attNameWidth, newWidth);
        element.setAttribute(attNameHeight, newHeight);
        if (attNameWidth2 && attNameHeight2) {
            element.setAttribute(attNameWidth2, newWidth);
            element.setAttribute(attNameHeight2, newHeight);
        }
        element.style.width = newWidth;
        element.style.height = newHeight;
    }
}

function IG_internalGetCurrentPictureName() {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    return divCurrentWorkspace.getAttribute("currentPicName");
}

function IG_internalGetCurrentConcatFrameIdReqGuid() {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    if (!divCurrentWorkspace)
        return null;
    var currentDZPath = divCurrentWorkspace.getAttribute("currentDZpath");
    if (!currentDZPath)
        return null;
    return currentDZPath.substring(IG_internalGetUrlDomainOutput().length, currentDZPath.lastIndexOf("."));
}

function IG_internalGetCurrentFrameId() {
    var curConcatFrameIdReqGuid = IG_internalGetCurrentConcatFrameIdReqGuid();
    if (curConcatFrameIdReqGuid == null)
        return null;
    var reg = new RegExp("(/DZ)+", "g");
    var tFrameIdReqGuid = curConcatFrameIdReqGuid.split(reg);
    return tFrameIdReqGuid[0];
}

function IG_internalGetCurrentWorkingLayer() {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    return parseInt(divCurrentWorkspace.getAttribute("currentWorkingLayer"));
}

function IG_internalCleanRequestQueue() {
    var IG_aNbRequests = IG_internalGetElementById("IG_aNbRequests");
    if (IG_aNbRequests) {
        var nbRequests = parseInt(IG_aNbRequests.innerHTML);
        if (isNaN(nbRequests))
            nbRequests = Number(IG_aNbRequests.innerHTML);
        if (nbRequests > 0) {
            var IG_listRequests = IG_internalGetElementById("IG_listRequests");
            var reg = new RegExp("[,]+", "g");
            var tReqDef = {};
            for (var idxRequest = 0; idxRequest < nbRequests; idxRequest++) {
                tReqDef = IG_listRequests.children[idxRequest].innerHTML.split(reg);
                if (tReqDef[1] == "UpdateImage") {
                    IG_listRequests.removeChild(IG_listRequests.children[idxRequest]);
                    IG_internalCleanNode(IG_aNbRequests);
                    IG_aNbRequests.appendChild(document.createTextNode((nbRequests - 1).toString()));
                }
            }
            var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
            divCurrentWorkspace.setAttribute("nbTicksWithRequests", 0);
        }
    }
}

function IG_internalSetProgress(progress) {
    var imgProgress = IG_internalGetElementById("imgProgess");
    if (imgProgress) {
        var nProgress = Number(progress);
        var nRoundProgress = Math.floor(nProgress / 5);
        nRoundProgress = nRoundProgress * 5;
        if (nProgress < 100) {
            imgProgress.src = "images/Progress/WebProgress" + nRoundProgress + "pct.png";
            imgProgress.alt = "Progress: " + nProgress + "%";
        }
        else {
            imgProgress.src = "images/Progress/WebProgressHidden.png";
            imgProgress.alt = "Idle";
        }
    }
}

function IG_internalIsImageInLibrary(divPictures, imageName) {
    for (var idxSpan = 0; idxSpan < divPictures.children.length; idxSpan++) {
        if (divPictures.children[idxSpan].getAttribute("title") == imageName)
            return true;
    }
    return false;
}

function IG_internalAddImagesToDiv(divPictures, miniPicList, clean) {
    if (miniPicList.length > 0) {
        var bRefresh = false;
        var cloneDivPictures = divPictures.cloneNode(true);
        if (clean)
            IG_internalCleanNode(divPictures);
        for (var idxImg = 0; idxImg < miniPicList.length; idxImg++) {
            var reg = new RegExp("(?:/MINI/MINI|/Mini/Mini|/mini/mini)", "g");
            var tSplitPath = miniPicList[idxImg].Path.split(reg);
            var nWidth = miniPicList[idxImg].Width;
            var nHeight = miniPicList[idxImg].Height;
            var imageNameServer = tSplitPath[1];
            if (imageNameServer == null)
                return;
            var imageNameExt = imageNameServer.substring(imageNameServer.lastIndexOf("."), imageNameServer.length);
            var imageNameJPEG = imageNameServer.substring(0, imageNameServer.lastIndexOf(".")) + ".JPG";
            var miniImagePath = tSplitPath[0] + "/Mini/Mini" + imageNameJPEG;

            // avoid adding several times the same mini picture
            var bFound = false;
            for (var idxSpan = 0; idxSpan < divPictures.children.length; idxSpan++) {
                if (divPictures.children[idxSpan].getAttribute("title") == miniPicList[idxImg].Name) {
                    cloneDivPictures.children[idxSpan].children[0].setAttribute("src", miniImagePath);
                    bFound = true;
                    bRefresh = true;
                    break;
                }
            }
            if (bFound)
                continue;

            var newSpan = document.createElement("span");
            newSpan.setAttribute("style", "display: inline-block; background-image: url(images/frameSilver.png); margin: 5px; width: 168px; height: 181px; padding: 0px; vertical-align: top");
            //newSpan.setAttribute ("align", "center");
            var spanId = "ImageSpan" + miniPicList[idxImg].Name;
            newSpan.setAttribute("id", spanId);

            newSpan.setAttribute("title", miniPicList[idxImg].Name);
            newSpan.setAttribute("onclick", "IG_internalOnImageSpanClick(this, event);");

            var newImg = document.createElement("img");
            newImg.setAttribute("id", "ImageLibraryPicture" + idxImg.toString());
            newImg.setAttribute("src", miniImagePath);
            var offsetX = 2 + (160 - nWidth) / 2;
            var offsetY = 18 + (160 - nHeight) / 2;
            newImg.setAttribute("style", "padding: 0px; margin-left: " + offsetX.toString() + "px; margin-top: " + offsetY.toString() + "px");

            newSpan.appendChild(newImg);
            divPictures.appendChild(newSpan);
        }
        if (bRefresh) {
            // refresh the image library
            IG_internalCleanNode(divPictures);
            for (var idxPic = 0; idxPic < cloneDivPictures.children.length; idxPic++)
                divPictures.appendChild(cloneDivPictures.children[idxPic].cloneNode(true));
        }
    }
}

function IG_internalAddImageToDiv(divPictures, pictureName, picturePath, miniWidth, miniHeight) {
    var miniPicList = [ { 'Path': picturePath,
        'Name': pictureName,
        'Width': miniWidth,
        'Height': miniHeight } ];
    IG_internalAddImagesToDiv(divPictures, miniPicList);
}

function Point(x, y) {
    var self = this;
    this.x = typeof (x) == "number" ? x : 0;
    this.y = typeof (y) == "number" ? y : 0;
    this.plus = function (point) {
        return new Point(self.x + point.x, self.y + point.y);
    };
    this.minus = function (point) {
        return new Point(self.x - point.x, self.y - point.y);
    };
    this.times = function (factor) {
        return new Point(self.x * factor, self.y * factor);
    };
    this.divide = function (factor) {
        return new Point(self.x / factor, self.y / factor);
    };
    this.scale = function (point) {
        return new Point(self.x * point.x, self.y * point.y);
    };
    this.negate = function () {
        return new Point(-self.x, -self.y);
    };
    this.distanceTo = function (point) {
        return Math.sqrt(Math.pow(self.x - point.x, 2) +
               Math.pow(self.y - point.y, 2));
    };
    this.norm = function () {
        return Math.sqrt(Math.pow(self.x, 2) + Math.pow(self.y, 2));
    };
    this.rotate = function (angle) {
        var pxTmpX = 0.0; var pxTmpY = 0.0;
        pxTmpX = self.x * Math.cos(angle) + self.y * Math.sin(angle);
        pxTmpY = -self.x * Math.sin(angle) + self.y * Math.cos(angle);
        return new Point(pxTmpX, pxTmpY);
    };
    this.apply = function (func) {
        return new Point(func(self.x), func(self.y));
    };
    this.equals = function (point) {
        return (point instanceof Point) && (self.x === point.x) && (self.y === point.y);
    };
    this.toString = function () {
        return "(" + self.x + "," + self.y + ")";
    };
}

var IG_tAlphaNum = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'];

function IG_internalFromColorNumberToAN(numCol) {
    var col = parseInt(numCol);
    var col_tr = Math.floor(col / (256 * 256 * 256));
    var color_B = Math.floor((col - col_tr * 256 * 256 * 256) / (256 * 256));
    var color_G = Math.floor((col - col_tr * 256 * 256 * 256 - color_B * 256 * 256) / 256);
    var color_R = Math.floor(col - col_tr * 256 * 256 * 256 - color_B * 256 * 256 - color_G * 256);
    var colAN = "";
    var color_RH = Math.floor(color_R / 16); var color_RL = color_R - 16 * color_RH;
    var color_GH = Math.floor(color_G / 16); var color_GL = color_G - 16 * color_GH;
    var color_BH = Math.floor(color_B / 16); var color_BL = color_B - 16 * color_BH;
    colAN = IG_tAlphaNum[color_BH] + IG_tAlphaNum[color_BL] + IG_tAlphaNum[color_GH] + IG_tAlphaNum[color_GL] + IG_tAlphaNum[color_RH] + IG_tAlphaNum[color_RL];
    return colAN;
}

function IG_internalFromColorANToNumber(numCol) {
    var pow = 1;
    var number = 0;
    for (var idxChar = numCol.length - 1; idxChar >= 0; idxChar--) {
        var idxAN = 0;
        for (; idxAN < 16; idxAN++) {
            if (IG_tAlphaNum[idxAN] == numCol[idxChar])
                break;
        }
        number += idxAN * pow;
        pow *= 16;
    }
    var col_tr = Math.floor(number / (256 * 256 * 256));
    var color_R = Math.floor((number - col_tr * 256 * 256 * 256) / (256 * 256));
    var color_G = Math.floor((number - col_tr * 256 * 256 * 256 - color_R * 256 * 256) / 256);
    var color_B = Math.floor(number - col_tr * 256 * 256 * 256 - color_R * 256 * 256 - color_G * 256);
    return col_tr * 256 * 256 * 256 + color_B * 256 * 256 + color_G * 256 + color_R;
}

function IG_internalFromColorNumberToRGB(numCol) {
    var col = parseInt(numCol);
    var col_tr = Math.floor(col / (256 * 256 * 256));
    var color_R = Math.floor((col - col_tr * 256 * 256 * 256) / (256 * 256));
    var color_G = Math.floor((col - col_tr * 256 * 256 * 256 - color_R * 256 * 256) / 256);
    var color_B = Math.floor(col - col_tr * 256 * 256 * 256 - color_R * 256 * 256 - color_G * 256);
    return "rgba(" + color_R.toString() + "," + color_G.toString() + "," + color_B.toString() + "," + col_tr.toString() + ")";
}

function IG_internalGetCurrentPoint(curEvent) {
    var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
    if (divDeepZoomPanel.Viewer == null)
        return null;
    if (!divDeepZoomPanel.Viewer.isOpen())
        return null;
    var pixel = divDeepZoomPanel.Utils.getMousePosition(curEvent).minus
        (divDeepZoomPanel.Utils.getElementPosition(divDeepZoomPanel.Viewer.elmt));
    var point = divDeepZoomPanel.Viewer.viewport.pointFromPixel(pixel);
    point.x = Math.round(point.x * IG_PIXEL_SELECTOR_PRECISION);
    point.y = Math.round(point.y * IG_PIXEL_SELECTOR_PRECISION);
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    var picWidth = Number(IG_internalGetCurrentPictureProperty("Width"));
    var picHeight = Number(IG_internalGetCurrentPictureProperty("Height"));
    var ratioWidthHeight = picWidth / picHeight;
    if (ratioWidthHeight < 1) {
        point.x = Math.round(Number(point.x) * ratioWidthHeight);
        point.y = Math.round(Number(point.y) * ratioWidthHeight);
    }
    return point;
}

function IG_internalGetCurrentPictureProperty(propName) {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    return divCurrentWorkspace.getAttribute("currentPictureProperty_" + propName);
}

function IG_internalSetRectangleProp(divCurrentWorkspace, propName, tRcSel) {
    // convert coordinates    
    var tRcSelDZ = tRcSel.slice(0);
    var tRcSelOrig = tRcSel.slice(0);
    if (tRcSel[1] != "-1" && tRcSel[3] != "-1") {
        var picWidth = Number(IG_internalGetCurrentPictureProperty("Width"));
        var picHeight = Number(IG_internalGetCurrentPictureProperty("Height"));
        var ratioWidthHeight = Number(picWidth) / Number(picHeight);
        tRcSelDZ[0] /= Number(picWidth);
        tRcSelDZ[1] /= Number(picWidth);
        tRcSelDZ[2] /= Number(picHeight);
        tRcSelDZ[3] /= Number(picHeight);
        tRcSelDZ[2] = 1 - tRcSelDZ[2];
        tRcSelDZ[3] = 1 - tRcSelDZ[3];
        tRcSelDZ[2] /= ratioWidthHeight;
        tRcSelDZ[3] /= ratioWidthHeight;
        if (ratioWidthHeight < 1) {
            var sizeRatio = IG_PIXEL_SELECTOR_PRECISION * ratioWidthHeight;
            tRcSel[0] = tRcSelDZ[0] * sizeRatio;
            tRcSel[1] = tRcSelDZ[1] * sizeRatio;
            tRcSel[2] = tRcSelDZ[2] * sizeRatio;
            tRcSel[3] = tRcSelDZ[3] * sizeRatio;
        }
        else {
            var sizeRatio = IG_PIXEL_SELECTOR_PRECISION / picWidth;
            tRcSel[2] = picHeight - tRcSel[2];
            tRcSel[3] = picHeight - tRcSel[3];
            tRcSel[0] *= sizeRatio;
            tRcSel[1] *= sizeRatio;
            tRcSel[2] *= sizeRatio;
            tRcSel[3] *= sizeRatio;
        }
    }
    var propValue = tRcSelDZ[0].toString() + "," + tRcSelDZ[1].toString() + "," + tRcSelDZ[2].toString() + "," + tRcSelDZ[3].toString();
    divCurrentWorkspace.setAttribute("currentPictureProperty_" + propName + "DZ", propValue);
    propValue = tRcSel[0].toString() + "," + tRcSel[1].toString() + "," + tRcSel[2].toString() + "," + tRcSel[3].toString();
    divCurrentWorkspace.setAttribute("currentPictureProperty_" + propName, propValue);
}

function IG_internalSetRectanglePropDZ(divCurrentWorkspace, propName, tRcSelDZ) {
    // convert coordinates    
    var tRcSel = tRcSelDZ.slice(0);
    if (tRcSel[1] != "-1" && tRcSel[3] != "-1") {
        var picWidth = Number(IG_internalGetCurrentPictureProperty("Width"));
        var picHeight = Number(IG_internalGetCurrentPictureProperty("Height"));
        var ratioWidthHeight = Number(picWidth) / Number(picHeight);
        tRcSelDZ[0] /= IG_PIXEL_SELECTOR_PRECISION;
        tRcSelDZ[1] /= IG_PIXEL_SELECTOR_PRECISION;
        tRcSelDZ[2] /= IG_PIXEL_SELECTOR_PRECISION;
        tRcSelDZ[3] /= IG_PIXEL_SELECTOR_PRECISION;
        tRcSel[0] *= picWidth;
        tRcSel[1] *= picWidth;
        tRcSel[2] *= picHeight;
        tRcSel[3] *= picHeight;
        if (ratioWidthHeight < 1) {
            tRcSelDZ[0] /= ratioWidthHeight;
            tRcSelDZ[1] /= ratioWidthHeight;
            tRcSelDZ[2] /= ratioWidthHeight;
            tRcSelDZ[3] /= ratioWidthHeight;
        }
        else {
            tRcSel[0] *= ratioWidthHeight;
            tRcSel[1] *= ratioWidthHeight;
            tRcSel[2] *= ratioWidthHeight;
            tRcSel[3] *= ratioWidthHeight;
        }       
    }
    var propValue = tRcSelDZ[0].toString() + "," + tRcSelDZ[1].toString() + "," + tRcSelDZ[2].toString() + "," + tRcSelDZ[3].toString();
    divCurrentWorkspace.setAttribute("currentPictureProperty_" + propName + "DZ", propValue);
    propValue = tRcSel[0].toString() + "," + tRcSel[1].toString() + "," + tRcSel[2].toString() + "," + tRcSel[3].toString();
    divCurrentWorkspace.setAttribute("currentPictureProperty_" + propName, propValue);
}

function IG_internalSetCurrentPictureProperty(propName, propValue) {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    if (propName == "SelectionRect") {
        var reg = new RegExp("[#]", "g");
        var tRcSel = propValue.split(reg);
        IG_internalSetRectangleProp(divCurrentWorkspace, propName, tRcSel);
    }
    else if (propName == "FaceDescriptor") {
        divCurrentWorkspace.setAttribute("currentPictureProperty_NbFaces", 0);
        var reg = new RegExp("[#]", "g");
        var isDefFace = false;
        var picWidth = Number(IG_internalGetCurrentPictureProperty("Width"));
        var picHeight = Number(IG_internalGetCurrentPictureProperty("Height"));
        var ratioWidthHeight = Number(picWidth) / Number(picHeight);
        if (propValue == "NoValue") {
            isDefFace = true;
            var width = Math.min(Math.max(picWidth,picHeight), 639);
            var height = width;
            if (ratioWidthHeight > 1)
                height /= ratioWidthHeight;
            else
                width *= ratioWidthHeight;
            propValue = width + "&" + height + "&" + IGFACEDESCRIPTOR_DEFAULTFACE;
        }
        var tFaces = propValue.split(reg);
        reg = new RegExp("[&]", "g");
        for (var idxFace = 0; idxFace < tFaces.length; idxFace++) {
            var tFaceDescr = tFaces[idxFace].split(reg);
            // read scale global header and rotation face header
            if (idxFace == 0) {
                divCurrentWorkspace.setAttribute("currentPictureProperty_ScaledWidth", tFaceDescr[0]);
                divCurrentWorkspace.setAttribute("currentPictureProperty_ScaledHeight", tFaceDescr[1]);
                divCurrentWorkspace.setAttribute("currentPictureProperty_Face" + idxFace + "_Rot", tFaceDescr[2]);
            }
            else
                divCurrentWorkspace.setAttribute("currentPictureProperty_Face" + idxFace + "_Rot", tFaceDescr[0]);
            var startSliceIndex = (idxFace == 0 ? 3 : 1);
            var endSliceIndex = startSliceIndex + 4;
            var tRcFace = tFaceDescr.slice(startSliceIndex, endSliceIndex);
            startSliceIndex = endSliceIndex; endSliceIndex = startSliceIndex + 4;
            var curFace = "Face" + idxFace;
            IG_internalSetRectanglePropDZ(divCurrentWorkspace, curFace + "_TLTR", tRcFace);
            tRcFace = tFaceDescr.slice(startSliceIndex, endSliceIndex);
            startSliceIndex = endSliceIndex; endSliceIndex = startSliceIndex + 4;
            IG_internalSetRectanglePropDZ(divCurrentWorkspace, curFace + "_BLBR", tRcFace);

            var tRcEyeLeft = tFaceDescr.slice(startSliceIndex, endSliceIndex);
            startSliceIndex = endSliceIndex; endSliceIndex = startSliceIndex + 4;
            IG_internalSetRectanglePropDZ(divCurrentWorkspace, curFace + "_EyeLeft_TLTR", tRcEyeLeft);
            tRcEyeLeft = tFaceDescr.slice(startSliceIndex, endSliceIndex);
            startSliceIndex = endSliceIndex; endSliceIndex = startSliceIndex + 4;
            IG_internalSetRectanglePropDZ(divCurrentWorkspace, curFace + "_EyeLeft_BLBR", tRcEyeLeft);

            var tRcEyeRight = tFaceDescr.slice(startSliceIndex, endSliceIndex);
            startSliceIndex = endSliceIndex; endSliceIndex = startSliceIndex + 4;
            IG_internalSetRectanglePropDZ(divCurrentWorkspace, curFace + "_EyeRight_TLTR", tRcEyeRight);
            tRcEyeRight = tFaceDescr.slice(startSliceIndex, endSliceIndex);
            startSliceIndex = endSliceIndex; endSliceIndex = startSliceIndex + 4;
            IG_internalSetRectanglePropDZ(divCurrentWorkspace, curFace + "_EyeRight_BLBR", tRcEyeRight);

            var tRcMouth = tFaceDescr.slice(startSliceIndex, endSliceIndex);
            startSliceIndex = endSliceIndex; endSliceIndex = startSliceIndex + 4;
            IG_internalSetRectanglePropDZ(divCurrentWorkspace, curFace + "_Mouth_TLTR", tRcMouth);
            tRcMouth = tFaceDescr.slice(startSliceIndex, endSliceIndex);
            startSliceIndex = endSliceIndex; endSliceIndex = startSliceIndex + 4;
            IG_internalSetRectanglePropDZ(divCurrentWorkspace, curFace + "_Mouth_BLBR", tRcMouth);

            var tRcNoze = tFaceDescr.slice(startSliceIndex, endSliceIndex);
            startSliceIndex = endSliceIndex; endSliceIndex = startSliceIndex + 4;
            IG_internalSetRectanglePropDZ(divCurrentWorkspace, curFace + "_Noze_TLTR", tRcNoze);
            tRcNoze = tFaceDescr.slice(startSliceIndex, endSliceIndex);
            startSliceIndex = endSliceIndex; endSliceIndex = startSliceIndex + 4;
            IG_internalSetRectanglePropDZ(divCurrentWorkspace, curFace + "_Noze_BLBR", tRcNoze);
        }
        divCurrentWorkspace.setAttribute("currentPictureProperty_NbFaces", tFaces.length);
        // update current picture face descriptor
        divCurrentWorkspace.FaceDescriptor = new IG_FaceDescriptor();
        if (isDefFace) {            
            if (ratioWidthHeight > 1)
                divCurrentWorkspace.FaceDescriptor.scale(1.0, 1.0 / ratioWidthHeight);
            else
                divCurrentWorkspace.FaceDescriptor.scale(ratioWidthHeight, 1.0);
        }
    }
    else
        divCurrentWorkspace.setAttribute("currentPictureProperty_" + propName, propValue);
}

function IG_internalGetSquareDescriptor(tPt) {
    var tConvPt = {};
    tConvPt[0] = new Point(tPt[0].x, tPt[0].y);
    tConvPt[1] = new Point(tPt[1].x, tPt[1].y);
    tConvPt[2] = new Point(tPt[2].x, tPt[2].y);
    tConvPt[3] = new Point(tPt[3].x, tPt[3].y);
    var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
    var picWidth = Number(IG_internalGetCurrentPictureProperty("Width"));
    var picHeight = Number(IG_internalGetCurrentPictureProperty("Height"));
    var ratioWidthHeight = Number(picWidth) / Number(picHeight);
    if (tConvPt[0].x != -1 && tConvPt[0].y != -1) {
        tConvPt[0] = tConvPt[0].times(IG_PIXEL_SELECTOR_PRECISION);
        tConvPt[1] = tConvPt[1].times(IG_PIXEL_SELECTOR_PRECISION);
        tConvPt[2] = tConvPt[2].times(IG_PIXEL_SELECTOR_PRECISION);
        tConvPt[3] = tConvPt[3].times(IG_PIXEL_SELECTOR_PRECISION);
        if (ratioWidthHeight < 1) {
            tConvPt[0] = tConvPt[0].times(ratioWidthHeight);
            tConvPt[1] = tConvPt[1].times(ratioWidthHeight);
            tConvPt[2] = tConvPt[2].times(ratioWidthHeight);
            tConvPt[3] = tConvPt[3].times(ratioWidthHeight);
        }
    }
    descriptor = "";
    descriptor += Math.round(tConvPt[0].x) + "&";
    descriptor += Math.round(tConvPt[1].x) + "&";
    descriptor += Math.round(tConvPt[0].y) + "&";
    descriptor += Math.round(tConvPt[1].y) + "&";
    descriptor += Math.round(tConvPt[2].x) + "&";
    descriptor += Math.round(tConvPt[3].x) + "&";
    descriptor += Math.round(tConvPt[2].y) + "&";
    descriptor += Math.round(tConvPt[3].y);
    return descriptor;
}

function IG_internalGetFaceDescriptor() {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    var faceDesciptor = divCurrentWorkspace.FaceDescriptor;
    if (faceDesciptor == null)
        return "";
    var descriptor = divCurrentWorkspace.getAttribute("currentPictureProperty_ScaledWidth");
    if (!descriptor)
        return "";
    descriptor = ",FaceDescriptor_" + descriptor + "&" + divCurrentWorkspace.getAttribute("currentPictureProperty_ScaledHeight") + "&";
    for (var idxFace = 0; idxFace < faceDesciptor.nbFaces; idxFace++) {
        var curFaceProp = "currentPictureProperty_Face" + idxFace + "_";
        descriptor += faceDesciptor.tFaces[idxFace].rotation + "&";
        descriptor += IG_internalGetSquareDescriptor(faceDesciptor.tFaces[idxFace].tFacePt) + "&";
        descriptor += IG_internalGetSquareDescriptor(faceDesciptor.tFaces[idxFace].tEyeLeftPt) + "&";
        descriptor += IG_internalGetSquareDescriptor(faceDesciptor.tFaces[idxFace].tEyeRightPt) + "&";
        descriptor += IG_internalGetSquareDescriptor(faceDesciptor.tFaces[idxFace].tMouthPt) + "&";
        descriptor += IG_internalGetSquareDescriptor(faceDesciptor.tFaces[idxFace].tNozePt);
        if (idxFace < faceDesciptor.nbFaces - 1)
            descriptor += "#";
    }
    return descriptor;
}

function IG_internalUpdateMenuList() {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    if (divCurrentWorkspace.getAttribute("currentView") == "Workspace") {
        var IG_WorkspaceMenu = IG_internalGetElementById("IG_WorkspaceMenu");
        var IG_divCurrentMenu = IG_internalGetElementById("IG_divCurrentMenu");
        IG_internalCleanNode(IG_divCurrentMenu);
        var currentMenu = IG_WorkspaceMenu.cloneNode(true);
        currentMenu.id = "IG_currentMenu";
        IG_divCurrentMenu.appendChild(currentMenu);
    }
}

function IG_internalGetWorkspaceMenuList() {
    var IG_WorkspaceMenu = IG_internalGetElementById("IG_WorkspaceMenu");
    return IG_WorkspaceMenu.children[1].children[1];
}

function IG_internalAreIntersecting(v1x1, v1y1, v1x2, v1y2,
    v2x1, v2y1, v2x2, v2y2) {
    var d1 = 0.0; var d2 = 0.0;
    var a1 = 0.0; var a2 = 0.0; var b1 = 0.0; var b2 = 0.0; var c1 = 0.0; var c2 = 0.0;

    // Convert vector 1 to a line (line 1) of infinite length.
    // We want the line in linear equation standard form: A*x + B*y + C = 0
    // See: http://en.wikipedia.org/wiki/Linear_equation
    a1 = v1y2 - v1y1;
    b1 = v1x1 - v1x2;
    c1 = (v1x2 * v1y1) - (v1x1 * v1y2);

    // Every point (x,y), that solves the equation above, is on the line,
    // every point that does not solve it, is either above or below the line.
    // We insert (x1,y1) and (x2,y2) of vector 2 into the equation above.
    d1 = (a1 * v2x1) + (b1 * v2y1) + c1;
    d2 = (a1 * v2x2) + (b1 * v2y2) + c1;

    // If d1 and d2 both have the same sign, they are both on the same side of
    // our line 1 and in that case no intersection is possible. Careful, 0 is
    // a special case, that's why we don't test ">=" and "<=", but "<" and ">".
    if (d1 > 0 && d2 > 0) return false;
    if (d1 < 0 && d2 < 0) return false;

    // We repeat everything above for vector 2.
    // We start by calculating line 2 in linear equation standard form.
    a2 = v2y2 - v2y1;
    b2 = v2x1 - v2x2;
    c2 = (v2x2 * v2y1) - (v2x1 * v2y2);

    // Calulate d1 and d2 again, this time using points of vector 1
    d1 = (a2 * v1x1) + (b2 * v1y1) + c2;
    d2 = (a2 * v1x2) + (b2 * v1y2) + c2;

    // Again, if both have the same sign (and neither one is 0),
    // no intersection is possible.
    if (d1 > 0 && d2 > 0) return false;
    if (d1 < 0 && d2 < 0) return false;

    // If we get here, only three possibilities are left. Either the two
    // vectors intersect in exactly one point or they are collinear
    // (they both lie both on the same infinite line), in which case they
    // may intersect in an infinite number of points or not at all.
    if ((a1 * b2) - (a2 * b1) == 0.0) return false;

    // If they are not collinear, they must intersect in exactly one point.
    return true;
}

function IG_internalDrawWideEllipse(ctx, pt1, pt2, pt3, pt4, fill) {
    ctx.beginPath();
    var pt12m = pt1.plus(pt2).divide(2.0);
    var pt23m = pt2.plus(pt3).divide(2.0);
    var pt34m = pt3.plus(pt4).divide(2.0);
    var pt41m = pt4.plus(pt1).divide(2.0);
    ctx.moveTo(pt12m.x, pt12m.y);
    ctx.bezierCurveTo(pt2.x, pt2.y, (pt2.x + pt23m.x) / 2, (pt2.y + pt23m.y) / 2, pt23m.x, pt23m.y);
    ctx.bezierCurveTo((pt3.x + pt23m.x) / 2, (pt3.y + pt23m.y) / 2, pt3.x, pt3.y, pt34m.x, pt34m.y);
    ctx.bezierCurveTo(pt4.x, pt4.y, (pt4.x + pt41m.x) / 2, (pt4.y + pt41m.y) / 2, pt41m.x, pt41m.y);
    ctx.bezierCurveTo((pt1.x + pt41m.x) / 2, (pt1.y + pt41m.y) / 2, pt1.x, pt1.y, pt12m.x, pt12m.y);
    ctx.closePath();
    if (fill)
        ctx.fill();
    else
        ctx.stroke();
}

function IG_internalDrawEllipse(ctx, pt1, pt2, pt3, pt4, fill) {
    ctx.beginPath();
    var pt23m = pt2.plus(pt3).divide(2.0);
    var pt41m = pt4.plus(pt1).divide(2.0);
    ctx.moveTo(pt41m.x, pt41m.y);
    ctx.bezierCurveTo(pt1.x, pt1.y, pt2.x, pt2.y, pt23m.x, pt23m.y);
    ctx.bezierCurveTo(pt3.x, pt3.y, pt4.x, pt4.y, pt41m.x, pt41m.y);
    ctx.closePath();
    if (fill)
        ctx.fill();
    else
        ctx.stroke();
}

function IG_internalOffsetSquare(tPx, vec) {
    tPx[0].x += vec.x;
    tPx[0].y += vec.y;
    tPx[1].x += vec.x;
    tPx[1].y += vec.y;
    tPx[2].x += vec.x;
    tPx[2].y += vec.y;
    tPx[3].x += vec.x;
    tPx[3].y += vec.y;
}

function IG_internalSquareCopy(square) {
    var sqCopy = {};
    sqCopy[0] = new Point(square[0].x, square[0].y);
    sqCopy[1] = new Point(square[1].x, square[1].y);
    sqCopy[2] = new Point(square[2].x, square[2].y);
    sqCopy[3] = new Point(square[3].x, square[3].y);
    return sqCopy;
}

function IG_internalGetSquareCenter(tPx) {
    return new Point((tPx[0].x + tPx[3].x) / 2, (tPx[0].y + tPx[3].y) / 2);
}

function IG_internalSetSquareCenter(tPx, curPx) {
    var centerVector = new Point(curPx.x - (tPx[0].x + tPx[3].x) / 2, curPx.y - (tPx[0].y + tPx[3].y) / 2);
    tPx[0].x += centerVector.x;
    tPx[0].y += centerVector.y;
    tPx[1].x += centerVector.x;
    tPx[1].y += centerVector.y;
    tPx[2].x += centerVector.x;
    tPx[2].y += centerVector.y;
    tPx[3].x += centerVector.x;
    tPx[3].y += centerVector.y;
}

function IG_internalGetFileName(url) {
    //this removes the anchor at the end, if there is one
    url = url.substring(0, (url.indexOf("#") == -1) ? url.length : url.indexOf("#"));
    //this removes the query after the file name, if there is one
    url = url.substring(0, (url.indexOf("?") == -1) ? url.length : url.indexOf("?"));
    //this removes everything before the last slash in the path
    url = url.substring(url.lastIndexOf("/") + 1, url.length);
    //return
    return url;
}

