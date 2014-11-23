/*****************************************************************************\

Javascript "Imagenius Web Service API" library
 
@version: 1.0 - 2013.02.25
@author: Jonathan Betser
\*****************************************************************************/
var IGIPFILTER_FACEEFFECT_HAPPY = 200;
var IGIPFILTER_FACEEFFECT_ANGER = 201;
var IGIPFILTER_FACEEFFECT_SADNESS = 202;
var IGIPFILTER_FACEEFFECT_UGLY = 203;
var IGIPFILTER_FACEEFFECT_SICK = 204;
var IGIPFILTER_FACEEFFECT_HOOLIGAN = 205;
var IGIPFILTER_FACEEFFECT_SURPRISED = 206;
var IGIPFILTER_FACEEFFECT_RANDOM = 207;

var IGIPFILTER_FACEEFFECT_REDEYE = 300;
var IGIPFILTER_FACEEFFECT_SOFTEN = 301;
var IGIPFILTER_FACEEFFECT_COLORIZE = 302;
var IGIPFILTER_FACEEFFECT_GLINT = 303;
var IGIPFILTER_FACEEFFECT_MATIFY = 304;
var IGIPFILTER_FACEEFFECT_COLORIZEEYES = 305;
var IGIPFILTER_FACEEFFECT_SHARPEN = 306;
var IGIPFILTER_FACEEFFECT_CARTOON = 307;

function Imagenius() {
    this.resizeCallback = null;
    this.id = Imagenius.Instances.length;
    Imagenius.Instances[this.id] = this;
    this.urlAppli = (typeof IG_urlAppli == 'undefined' ? IG_WEBSERVICE_URL : IG_urlAppli);
    this.urlDomain = (typeof IG_urlDomain == 'undefined' ? IG_WEBSERVICE_URL : IG_urlDomain);
    this.urlWS = this.urlDomain + "ImageniusWS.asmx";
    this.urlPublicWS = this.urlDomain + "ImageniusPublicWS.asmx";
    this.urlWSAccount = this.urlDomain + "IGWSAccount.asmx";
    this.urlWSWorkspace = this.urlDomain + "IGWSWorkspace.asmx";
    this.urlWSAuthenticate = this.urlDomain + "IGWSAuthenticate.asmx";
    this.init = function (divName, options) {
        IGWS_WORKSPACE_DIV = IG_internalGetElementById(divName);
        var hideWorkspace = (options == null ? false : options["Workspace"] == "hidden");
        if (hideWorkspace)
            IGWS_WORKSPACE_DIV.style.display = "none";
        IGWS_WORKSPACE_DIV.style.maxHeight = "600px";
        var multiPictures = (options == null ? false : options["MultiPictures"]);
        if (multiPictures)
            IGWS_WORKSPACE_DIV.setAttribute("MultiPictures", "1");
        var saveHistory = (options == null ? false : options["SaveHistory"]);
        if (saveHistory)
            IGWS_WORKSPACE_DIV.setAttribute("SaveHistory", "1");
        var DZDragging = (options == null ? false : options["DZDragging"]);
        if (DZDragging)
            IGWS_WORKSPACE_DIV.setAttribute("DZDragging", "1");
        IGWS_DEEPZOOM_DIV = document.createElement("div");
        IGWS_DEEPZOOM_DIV.id = "IG_deepZoomPanel";
        var hidePicturePanel = (options == null ? false : options["PicturePanel"] == "hidden");
        if (hidePicturePanel)
            IGWS_DEEPZOOM_DIV.style.display = "none";
        IGWS_DEEPZOOM_DIV.style.cssFloat = "left";
        IGWS_DEEPZOOM_DIV.style.maxHeight = "580px";
        var backgroundImage = (options == null ? null : options["BackgroundImage"]);
        if (backgroundImage != null)
            IGWS_DEEPZOOM_DIV.style.backgroundImage = "url('" + backgroundImage + "')";

        // create container for deep zoom, toolbox, layer and history managers
        var IG_tableMain = document.createElement("table");
        IG_tableMain.id = "IG_tableMain";
        IG_tableMain.style.maxHeight = "580px";
        // create tool options 
        var trOptions = document.createElement("tr");
        var thOptions = document.createElement("th");
        thOptions.id = "IG_thToolOptions";
        var IG_divIGTOOL_OPTIONS = document.createElement("div");
        IG_divIGTOOL_OPTIONS.id = "IG_divIGTOOL_OPTIONS";
        var hideToolOptions = (options == null ? true : options["ToolOptions"] != "visible");
        if (hideToolOptions)
            IG_divIGTOOL_OPTIONS.style.display = "none";
        IG_divIGTOOL_OPTIONS.style.fontSize = "medium";
        thOptions.appendChild(IG_divIGTOOL_OPTIONS);
        trOptions.appendChild(document.createElement("th"));
        trOptions.appendChild(thOptions);
        trOptions.appendChild(document.createElement("th"));
        IG_tableMain.appendChild(trOptions);

        // create tool box panel
        var trPanels = document.createElement("tr");
        var thToolBox = document.createElement("th");
        thToolBox.id = "IG_thToolBox";
        var createToolBox = (options == null ? true : options["ToolBox"] != "external");
        if (createToolBox) {
            var IG_panelToolBox = document.createElement("div");
            IG_panelToolBox.id = "IG_panelToolBox";
            var hideToolBox = (options == null ? true : options["ToolBox"] != "visible");
            if (hideToolBox)
                IG_panelToolBox.style.display = "none";
            IG_panelToolBox.style.cssFloat = "left";
            var IG_tableToolBox = document.createElement("table");
            IG_tableToolBox.id = "IG_tableToolBox";
            IG_tableToolBox.style = "width: 100%; margin-left: 0px; margin-top: 10px; padding: 0px; overflow: hidden";
            IG_panelToolBox.appendChild(IG_tableToolBox);
            thToolBox.appendChild(IG_panelToolBox);
        }
        trPanels.appendChild(thToolBox);

        // create deep zoom panel
        var thDZ = document.createElement("th");
        thDZ.id = "IG_thDeepZoomPanel";
        thDZ.appendChild(IGWS_DEEPZOOM_DIV);
        trPanels.appendChild(thDZ);

        // create workspace menu
        var createWorkspaceMenu = (options == null ? true : options["WorkspaceMenu"] != "external");
        if (createWorkspaceMenu) {
            var IG_divCurrentMenu = document.createElement("div");
            IG_divCurrentMenu.id = "IG_divCurrentMenu";
            var hideWorkspaceMenu = (options == null ? true : options["WorkspaceMenu"] != "visible");
            if (hideWorkspaceMenu)
                IG_divCurrentMenu.style.display = "none";
            IGWS_WORKSPACE_DIV.appendChild(IG_divCurrentMenu);

            var IG_AccountMenu = document.createElement("div");
            IG_AccountMenu.id = "IG_AccountMenu";
            IG_AccountMenu.style.display = "none";
            IGWS_WORKSPACE_DIV.appendChild(IG_AccountMenu);

            var IG_divWorkspaceMenu = document.createElement("div");
            IG_divWorkspaceMenu.id = "IG_divWorkspaceMenu";
            IG_divWorkspaceMenu.style.display = "none";
            var IG_WorkspaceMenu = document.createElement("ul");
            IG_WorkspaceMenu.id = "IG_WorkspaceMenu";
            var IG_WorkspaceMenuItem = document.createElement("li");
            IG_WorkspaceMenuItem.id = "IG_WorkspaceMenuItem";
            var WorkspaceText = document.createElement("a");
            WorkspaceText.appendChild(document.createTextNode("Workspace"));
            IG_WorkspaceMenuItem.appendChild(WorkspaceText);
            IG_WorkspaceMenu.appendChild(IG_WorkspaceMenuItem);
            var liPictures = document.createElement("li");
            var PicturesText = document.createElement("a");
            PicturesText.appendChild(document.createTextNode("Pictures"));
            liPictures.appendChild(PicturesText);
            var IG_WorkspaceMenuEnumPictures = document.createElement("ul");
            IG_WorkspaceMenuEnumPictures.id = "IG_WorkspaceMenuEnumPictures";
            liPictures.appendChild(IG_WorkspaceMenuEnumPictures);
            IG_WorkspaceMenu.appendChild(liPictures);
            IG_divWorkspaceMenu.appendChild(IG_WorkspaceMenu);
            IGWS_WORKSPACE_DIV.appendChild(IG_divWorkspaceMenu);
        }

        // create image library
        var createImageLibrary = (options == null ? true : options["ImageLibrary"] != "external");
        if (createImageLibrary) {
            var IG_divImageLibraryScroll = document.createElement("div");
            IG_divImageLibraryScroll.id = "IG_divImageLibraryScroll";
            var hideImageLibrary = (options == null ? true : options["ImageLibrary"] != "visible");
            if (hideImageLibrary) {
                IGWS_WORKSPACE_DIV.setAttribute("HideImageLibrary", "1");
                IG_divImageLibraryScroll.style.display = "none";
            }
            IG_divImageLibraryScroll.style.maxHeight = "580px";
            IG_divImageLibraryScroll.style.overflowX = "hidden";
            IG_divImageLibraryScroll.style.overflowY = "auto";
            var IG_divImageLibrary = document.createElement("div");
            IG_divImageLibrary.id = "IG_divImageLibrary";
            IG_divImageLibraryScroll.appendChild(IG_divImageLibrary);
            IGWS_WORKSPACE_DIV.appendChild(IG_divImageLibraryScroll);
        }

        // create viewport container
        var createViewports = (options == null ? true : options["Viewports"] != "external");
        if (createViewports) {
            var IG_divViewports = document.createElement("div");
            IG_divViewports.id = "IG_divViewports";
            var hideViewports = (options == null ? true : options["Viewports"] != "visible");
            if (hideViewports)
                IG_divViewports.style.display = "none";
            IGWS_WORKSPACE_DIV.appendChild(IG_divViewports);
        }

        // create request queue
        var createRequests = (options == null ? true : options["Requests"] != "external");
        if (createRequests) {
            var IG_divRequests = document.createElement("div");
            IG_divRequests.id = "IG_divRequests";
            var hideRequests = (options == null ? true : options["Requests"] != "visible");
            if (hideRequests)
                IG_divRequests.style.display = "none";
            var IG_aNbRequests = document.createElement("a");
            IG_aNbRequests.id = "IG_aNbRequests";
            IG_aNbRequests.appendChild(document.createTextNode("0"));
            IG_divRequests.appendChild(IG_aNbRequests);
            var IG_listRequests = document.createElement("ul");
            IG_listRequests.id = "IG_listRequests";
            IG_divRequests.appendChild(IG_listRequests);
            IGWS_WORKSPACE_DIV.appendChild(IG_divRequests);
        }

        // create texture, history and layer managers
        var thManagers = document.createElement("th");
        thManagers.id = "IG_thManagers";
        var createManagers = (options == null ? true : (options["HistoryManager"] != "external" || options["LayerManager"] != "external"));
        if (createManagers) {
            var IG_divManagers = document.createElement("div");
            IG_divManagers.id = "IG_divManagers";
            var hideManagers = (options == null ? true : (options["HistoryManager"] == "hidden" && options["LayerManager"] == "hidden"));
            if (hideManagers)
                IG_divManagers.style.display = "none";
            IG_divManagers.style.cssFloat = "right";
            IG_divManagers.style.maxHeight = "550px";
            IG_divManagers.style.fontSize = "small";

            var createTextureManager = (options == null ? true : options["TextureManager"] != "external");
            if (createTextureManager) {
                var IG_ulTextureManager = document.createElement("ul");
                IG_ulTextureManager.id = "IG_ulTextureManager";
                var hideTextureManager = (options == null ? true : options["TextureManager"] != "visible");
                if (hideTextureManager)
                    IG_ulTextureManager.style.display = "none";
                else
                    IG_ulTextureManager.style.display = "inherit";
                var IG_mouseX = document.createElement("a");
                IG_mouseX.id = "IG_mouseX";
                IG_mouseX.style = "vertical-align: top; padding-left: 0px;";
                var IG_mouseY = document.createElement("a");
                IG_mouseY.id = "IG_mouseY";
                IG_mouseY.style = "vertical-align: top; padding-left: 0px;";
                var IG_lblTexture1 = document.createElement("a");
                IG_lblTexture1.id = "IG_lblTexture1";
                IG_lblTexture1.appendChild(document.createTextNode("Color1:"));
                var IG_Texture1 = document.createElement("input");
                IG_Texture1.id = "IG_Texture1";
                IG_Texture1.className = "color {pickerClosable:true}";
                IG_Texture1.style = "width: 18px; font-size: small";
                IG_Texture1.setAttribute("onchange", "IG_internalChangeColor(this)");
                var IG_SwapColors = document.createElement("input");
                IG_SwapColors.title = "Swap colors";
                IG_SwapColors.type = "image"; IG_SwapColors.src = "images/SwapColors.png";
                IG_SwapColors.style = "vertical-align: top; margin-left: 5px; margin-right: 5px";
                IG_SwapColors.setAttribute("onclick", "IG_internalSwapColors()");
                var IG_lblTexture2 = document.createElement("a");
                IG_lblTexture2.id = "IG_lblTexture2";
                IG_lblTexture2.appendChild(document.createTextNode("Color2:"));
                var IG_Texture2 = document.createElement("input");
                IG_Texture2.id = "IG_Texture2";
                IG_Texture2.className = "color {pickerClosable:true}";
                IG_Texture2.style = "width: 18px; font-size: small";
                IG_Texture2.setAttribute("onchange", "IG_internalChangeColor(this)");
                IG_ulTextureManager.appendChild(IG_mouseX);
                IG_ulTextureManager.appendChild(IG_mouseY);
                IG_ulTextureManager.appendChild(document.createElement("br"));
                IG_ulTextureManager.appendChild(IG_lblTexture1);
                IG_ulTextureManager.appendChild(IG_Texture1);
                IG_ulTextureManager.appendChild(document.createElement("br"));
                IG_ulTextureManager.appendChild(IG_SwapColors);
                IG_ulTextureManager.appendChild(document.createElement("br"));
                IG_ulTextureManager.appendChild(IG_lblTexture2);
                IG_ulTextureManager.appendChild(IG_Texture2);
                IG_divManagers.appendChild(IG_ulTextureManager);
            }
            var createHistoryManager = (options == null ? true : options["HistoryManager"] != "external");
            if (createHistoryManager) {
                var IG_ulHistoryManager = document.createElement("ul");
                IG_ulHistoryManager.id = "IG_ulHistoryManager";
                var hideHistoryManager = (options == null ? true : options["HistoryManager"] != "visible");
                if (hideHistoryManager)
                    IG_ulHistoryManager.style.display = "none";
                else
                    IG_ulHistoryManager.style.display = "inherit";
                IG_ulHistoryManager.style.margin = "0px";
                IG_ulHistoryManager.style.padding = "0px";
                IG_ulHistoryManager.style.overflowY = "auto";
                IG_ulHistoryManager.style.overflowX = "hidden";
                IG_ulHistoryManager.style.height = "40%";
                IG_ulHistoryManager.style.width = "150px";
                IG_divManagers.appendChild(IG_ulHistoryManager);
            }
            var createLayerManager = (options == null ? true : options["LayerManager"] != "external");
            if (createLayerManager) {
                var IG_ulLayerManager = document.createElement("ul");
                IG_ulLayerManager.id = "IG_ulLayerManager";
                IG_ulLayerManager.style.margin = "0px";
                IG_ulLayerManager.style.padding = "0px";
                IG_ulLayerManager.style.overflowY = "auto";
                IG_ulLayerManager.style.overflowX = "hidden";
                IG_ulLayerManager.style.height = "60%";
                IG_ulLayerManager.style.width = "150px";

                var inputAddLayer = document.createElement("input");
                inputAddLayer.title = "Add current layer";
                inputAddLayer.type = "image";
                inputAddLayer.src = "images/AddLayer.png";
                inputAddLayer.style = "vertical-align: middle; margin-left: 5px; margin-top: 5px; padding: 10px";
                inputAddLayer.onclick = function () { IG_internalAddLayer(); };
                var inputMoveUpLayer = document.createElement("input");
                inputMoveUpLayer.title = "Move current layer up";
                inputMoveUpLayer.type = "image";
                inputMoveUpLayer.src = "images/Up.png";
                inputMoveUpLayer.style = "vertical-align: middle; margin-left: 5px; margin-top: 5px; padding: 10px";
                inputMoveUpLayer.onclick = function () { IG_internalMoveLayerUp(); };
                var inputMoveDownLayer = document.createElement("input");
                inputMoveDownLayer.title = "Move current layer down";
                inputMoveDownLayer.type = "image";
                inputMoveDownLayer.src = "images/Down.png";
                inputMoveDownLayer.style = "vertical-align: middle; margin-left: 5px; margin-top: 5px; padding: 10px";
                inputMoveDownLayer.onclick = function () { IG_internalMoveLayerDown(); };
                var inputMergeLayer = document.createElement("input");
                inputMergeLayer.title = "Merge current layers";
                inputMergeLayer.type = "image";
                inputMergeLayer.src = "images/MergeLayers.png";
                inputMergeLayer.style = "vertical-align: middle; margin-left: 5px; margin-top: 5px; padding: 10px";
                inputMergeLayer.onclick = function () { IG_internalMergeLayers(); };
                var inputRemoveLayer = document.createElement("input");
                inputRemoveLayer.title = "Remove current layer";
                inputRemoveLayer.type = "image";
                inputRemoveLayer.src = "images/RemoveLayer.png";
                inputRemoveLayer.style = "vertical-align: middle; margin-left: 5px; margin-top: 5px; padding: 10px";
                inputRemoveLayer.onclick = function () { IG_internalRemoveLayer(); };
                var IG_divLayerManager = document.createElement("div");
                var hideLayerManager = (options == null ? true : options["LayerManager"] != "visible");
                if (hideLayerManager)
                    IG_divLayerManager.style.display = "none";
                else
                    IG_divLayerManager.style.display = "inherit";
                IG_divLayerManager.appendChild(inputAddLayer);
                IG_divLayerManager.appendChild(inputMoveUpLayer);
                IG_divLayerManager.appendChild(inputMoveDownLayer);
                IG_divLayerManager.appendChild(inputMergeLayer);
                IG_divLayerManager.appendChild(inputRemoveLayer);
                IG_divLayerManager.appendChild(IG_ulLayerManager);

                IG_divManagers.appendChild(IG_divLayerManager);
            }
            thManagers.appendChild(IG_divManagers);
        }
        trPanels.appendChild(thManagers);
        IG_tableMain.appendChild(trPanels);
        IGWS_WORKSPACE_DIV.appendChild(IG_tableMain);

        // create overlay deepzoom canvas
        var createOverCanvas = (options == null ? true : options["OverCanvas"] != "external");
        if (createOverCanvas) {
            var IG_divOverCanvasBackup = document.createElement("div");
            IG_divOverCanvasBackup.id = "IG_divOverCanvasBackup";
            IG_divOverCanvasBackup.style.display = "none";
            var IG_overCanvas = document.createElement("canvas");
            IG_overCanvas.id = "IG_overCanvas";
            var hideOverCanvas = (options == null ? false : options["OverCanvas"] == "hidden");
            if (hideOverCanvas)
                IG_overCanvas.style.display = "none";
            IG_divOverCanvasBackup.appendChild(IG_overCanvas);
            IGWS_WORKSPACE_DIV.appendChild(IG_divOverCanvasBackup);
        }

        // initialize seadragon deep zoom
        IG_internalGetSeadragon().initSeadragon();
        if (!DZDragging)
            IGWS_DEEPZOOM_DIV.Viewer.setMouseNavEnabled(false);

        // create the tool box
        IG_internalCreateToolBox();

        window.setInterval('Imagenius.Instances[' + this.id + '].updateStatus()', 10000);
        IG_internalAddEvent('resize', IG_internalOnResize);
    }

    this.updateStatus = function () {
        IG_internalAPI("CheckConnection");
    }

    this.initFB = function (divName, options) {
        this.init(divName, options);
    }

    this.initDemo = function (divName, options, successCallback, errorCallback) {
        this.init(divName, options);
        IG_internalConnectDemo(successCallback, errorCallback);
    }

    this.connectDemo = function (successCallback, errorCallback) {
        IG_internalConnectDemo(successCallback, errorCallback);
    }

    this.connectBitlUser = function (userEmail, userPwd, successCallback, errorCallback) {
        IG_internalBitlUser(userEmail, userPwd, successCallback, errorCallback);
    }

    this.api = function (functionName, jsonData, successCallback, errorCallback) {
        IG_publicAPI(functionName, jsonData, successCallback, errorCallback);
    }

    this.showImageLibrary = function (show) {
        if (IGWS_WORKSPACE_DIV.getAttribute("HideImageLibrary") != "1") {
            var IG_divImageLibraryScroll = IG_internalGetElementById("IG_divImageLibraryScroll");
            IG_divImageLibraryScroll.style.display = show ? "inherit" : (typeof show === 'undefined' ? "inherit" : "none");
        }
        IG_internalOnResize();
    }

    this.showAccount = function () {
        IG_internalShowAccount();
    }

    this.showWorkspace = function () {
        IG_internalShowWorkspace(null);
    }

    this.showPopupMessage = function (msg, error, title) {
        IG_internalAlertClient(msg, error, title);
    }

    this.getCurrentView = function () {
        return IGWS_WORKSPACE_DIV.getAttribute("currentView");
    }

    this.getNbPictures = function () {
        return IG_internalGetWorkspaceMenuList().children.length;
    }

    this.openImage = function (image, successCallback, errorCallback) {
        this.api("OpenImage", { 'ImageName': image, 'AutoRotate': true }, successCallback, errorCallback);        
    }

    this.filterImage = function (filterId, successCallback, errorCallback) {
        this.api("FilterImage", { "FilterId": filterId }, successCallback, errorCallback);       
    }

    this.filterFace = function (filterId, successCallback, errorCallback) {
        switch (filterId) {
            case IGIPFILTER_FACEEFFECT_HAPPY:
                IG_FaceHappy(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_ANGER:
                IG_FaceAnger(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_SADNESS:
                IG_FaceSadness(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_UGLY:
                IG_FaceUgly(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_REDEYE:
                IG_FaceRemoveRedEyes(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_SICK:
                IG_FaceSick(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_HOOLIGAN:
                IG_FaceHooligan(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_SURPRISED:
                IG_FaceSurprised(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_RANDOM:
                IG_FaceRandom(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_REDEYE:
                IG_FaceRemoveRedEyes(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_COLORIZEEYES:
                IG_FaceColorizeEyes(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_SOFTEN:
                IG_FaceSoften(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_SHARPEN:
                IG_internalFaceSharpen(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_CARTOON:
                IG_FaceCartoon(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_COLORIZE:
                IG_FaceColorizeSkin(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_GLINT:
                IG_FaceColorizeSkinFun(successCallback, errorCallback);
                break;
            case IGIPFILTER_FACEEFFECT_MATIFY:
                IG_FaceMatifySkin(successCallback, errorCallback);
                break;
        }
        IG_internalSelectTool();
    }

    this.addEvent = function (evt, func, mode) {
        if (evt == 'resize')
            IG_internalOnResize_callback = func;
        else
            IG_internalAddEvent(evt, func, mode);
    }

    this.resizeElement = function (element, posX, posY, bIframe, bUpdate, fYCoeff) {
        IG_internalResizeElement(element, posX, posY, bIframe, bUpdate, fYCoeff);
    }

    this.resize = function () {
        IG_internalOnResize();
    }

    this.selectTool = function (toolId) {
        IG_internalSelectTool(IG_internalGetElementById(toolId));
    }

    this.contactUs = function () {
        IG_internalContactUs();
    }

    this.aboutUs = function () {
        IG_internalAbout();
    }

    this.signUp = function () {
        IG_internalSignUp();
    }

    this.getFaceDescriptor = function () {
        return IGWS_WORKSPACE_DIV.FaceDescriptor;
    }

    if (typeof IG_Init !== 'undefined')
        IG_Init();
}

Imagenius.Instances = new Array();

var IG = new Imagenius();
