/*****************************************************************************\

 Javascript "Imagenius Web Service Client" library
 Seadragon management
 
 @version: 1.0 - 2010.05.21
 @author: Jonathan Betser
\*****************************************************************************/
Seadragon.Config.imagePath = "images/DeepZoom/";

function IG_Seadragon() {
    this.initSeadragon = function () {
        // Start the viewer
        var viewer = new Seadragon.Viewer(IGWS_DEEPZOOM_DIV);
        viewer.addEventListener("animationfinish", function(viewer) {
            var center = viewer.viewport.getCenter();
            var zoom = viewer.viewport.getZoom();
            if (!center.x || !center.y || !zoom)
                return;

            // write viewport
            var viewportId = "divViewport" + IG_internalGetCurrentFrameId();
            var IG_divViewports = $("IG_divViewports");
            var divViewport = $(viewportId);
            if (divViewport) {
                var oldCenterX = divViewport.getAttribute("centerx");
                var oldCenterY = divViewport.getAttribute("centery");
                var oldZoom = divViewport.getAttribute("zoom");
                if ((oldCenterX != center.x) || (oldCenterY != center.y) ||
                (oldZoom != zoom)) {
                    var cloneDivViewports = IG_divViewports.cloneNode(true);
                    IG_internalCleanNode(IG_divViewports);
                    for (var idxPicture = 0; idxPicture < cloneDivViewports.children.length; idxPicture++) {
                        if (cloneDivViewports.children[idxPicture].getAttribute("id") == viewportId) {
                            var divNewViewport = document.createElement("div");
                            divNewViewport.setAttribute("id", viewportId);
                            divNewViewport.setAttribute("centerx", center.x);
                            divNewViewport.setAttribute("centery", center.y);
                            divNewViewport.setAttribute("zoom", zoom);
                            IG_divViewports.appendChild(divNewViewport);
                        }
                        else {
                            IG_divViewports.appendChild(cloneDivViewports.children[idxPicture].cloneNode(true));
                        }
                    }
                    IG_seadragon.sendViewport(center.x, center.y, zoom);
                }
            }
            else {
                var divNewViewport = document.createElement("div");
                divNewViewport.setAttribute("id", viewportId);
                divNewViewport.setAttribute("centerx", center.x);
                divNewViewport.setAttribute("centery", center.y);
                divNewViewport.setAttribute("zoom", zoom);
                IG_divViewports.appendChild(divNewViewport);
                IG_seadragon.sendViewport(center.x, center.y, zoom);
            }
        });

        viewer.addEventListener("open", function (viewer) {
            IG_seadragon.setViewport();
            var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
            divDeepZoomPanel.Started = true;
            IG_seadragon.dzOpen();
            IG_internalOnResize();
        });

        var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
        divDeepZoomPanel.Viewer = viewer;
        divDeepZoomPanel.Utils = Seadragon.Utils;
        divDeepZoomPanel.Started = false;

        // Remove full screen button
        var navControl = viewer.getNavControl();
        navControl.removeChild(navControl.lastChild);

        viewer.setVisible(false);
        divDeepZoomPanel.lastOpenDZ = IG.urlDomain + "App_Init/DZInit.xml";
        viewer.openDzi(divDeepZoomPanel.lastOpenDZ);
    }

    this.sendViewport = function(centerx, centery, zoom, frameId) {
        if (frameId == null)
            frameId = IG_internalGetCurrentFrameId();
        if (frameId) {
            var jsonData = { 'FrameId': frameId,
                'CenterX': centerx,
                'CenterY': centery,
                'Zoom': zoom
            };
            IG_internalAPI("SetViewport", jsonData);
        }
    }

    this.dzOpen = function() {
        var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
        var currentDZPath = divCurrentWorkspace.getAttribute("currentDZpath");
        if (currentDZPath != null) {
            var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
            if (divDeepZoomPanel.Started) {
                if (currentDZPath != divDeepZoomPanel.lastOpenDZ) {
                    divDeepZoomPanel.lastOpenDZ = currentDZPath;
                    divDeepZoomPanel.Viewer.setVisible(true);
                    divDeepZoomPanel.Viewer.openDzi(currentDZPath);
                    var canvas = IG_internalGetElementById("IG_overCanvas");
                    var ctx = canvas.getContext("2d");
                    ctx.strokeStyle = "rgba(0, 0, 200, 0.2)";
                    ctx.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
                    this.sendFrameViewport(IG_internalGetCurrentFrameId());                    
                }
            }
        }
    }

    this.sendFrameViewport = function(frameId) {
        var viewportId = "divViewport" + frameId;
        var picWidth = Number(IG_internalGetCurrentPictureProperty("Width"));
        var picHeight = Number(IG_internalGetCurrentPictureProperty("Height"));
        var ratioWidthHeight = Number(picWidth) / Number(picHeight);
        var centerX = 0.5;
        var centerY = 0.5;
        if (ratioWidthHeight > 1)
            centerY /= ratioWidthHeight;
        else
            centerY *= ratioWidthHeight;
        var divNewViewport = document.createElement("div");
        divNewViewport.setAttribute("id", viewportId);
        divNewViewport.setAttribute("centerx", centerX);
        divNewViewport.setAttribute("centery", centerY);
        divNewViewport.setAttribute("zoom", 1.0);
        var IG_divViewports = IG_internalGetElementById("IG_divViewports");
        if (IG_divViewports) {
            IG_divViewports.appendChild(divNewViewport);
            this.sendViewport(centerX, centerY, 1.0, frameId);
        }
    }

    this.setViewport = function() {
        var divViewport = IG_internalGetElementById("divViewport" + IG_internalGetCurrentFrameId());
        if (divViewport) {
            var centerx = divViewport.getAttribute("centerx");
            var centery = divViewport.getAttribute("centery");
            var zoom = divViewport.getAttribute("zoom");
            if (centerx != null && centery != null && zoom != null &&
           !isNaN(centerx) && !isNaN(centery) && !isNaN(zoom)) {
                var ptCenter = new Seadragon.Point(Number(centerx),
                                              Number(centery));
                var animationTime = Seadragon.Config.animationTime;
                Seadragon.Config.animationTime = 0;
                var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
                divDeepZoomPanel.Viewer.viewport.panTo(ptCenter);
                divDeepZoomPanel.Viewer.viewport.zoomTo(Number(zoom));
                Seadragon.Config.animationTime = animationTime;
            }
        }
        IG_internalSelectCurrentTool();
    }    
}

var IG_seadragon = null;

function IG_internalGetSeadragon() {
    if (IG_seadragon == null)
        IG_seadragon = new IG_Seadragon();
    return IG_seadragon;
}