/*****************************************************************************\

 Javascript "Imagenius Workspace Managing Web Service" library
 
 @version: 1.0 - 2010.02.02
 @author: Jonathan Betser
\*****************************************************************************/
var IGANSWER_WORKSPACE_CONNECTED = 3000;
var IGANSWER_WORKSPACE_DISCONNECTED = 3001;
var IGANSWER_WORKSPACE_ACTIONFAILED	= 3002;
var IGANSWER_WORKSPACE_PONG = 3003;
var IGANSWER_WORKSPACE_HEARTHBEAT = 3004;
var IGANSWER_WORKSPACE_PROPERTYCHANGED = 3005;
var IGANSWER_FRAME_CHANGED = 4000;
var IGANSWER_FRAME_ACTIONDONE = 4001;
var IGANSWER_FRAME_ACTIONFAILED	= 4002;
var IGANSWER_FRAME_ACTIONINPROGRESS = 4003;

var IGFACEDESCRIPTOR_MAXNBFACES = 10;
var IGFACEDESCRIPTOR_DEFAULTFACE = "0&275430&719875&795076&795076&275430&719875&192572&192572&292645&458529&493041&493041&292645&458529&383495&383495&538341&704225&493041&493041&538341&704225&383495&383495&408451&594679&740303&740303&408451&594679&629191&629191&427230&569640&615107&615107&427230&569640&529035&529035";
var IGFACEDESCRIPTOR_BUTTON_NONE = 0;
var IGFACEDESCRIPTOR_BUTTON_DELETE = 1;
var IGFACEDESCRIPTOR_BUTTON_DUPLICATE = 2;
var IGFACEDESCRIPTOR_BUTTON_ZOOMIN = 3;
var IGFACEDESCRIPTOR_BUTTON_ZOOMOUT = 4;

var IG_tHISTORY = [{'id' : 'IGFRAMEHISTORY_STEP_UNDEFINED', 'picture' : 'images/StepStart.png', 'text' : 'Undefined'},
                {'id' : 'IGFRAMEHISTORY_STEP_START', 'picture' : 'images/StepStart.png', 'text' : 'Original'},
                {'id' : 'IGFRAMEHISTORY_STEP_SELECTION', 'picture' : 'images/StepChangeSelection.png', 'text' : 'Selection'},
                {'id' : 'IGFRAMEHISTORY_STEP_RESIZE', 'picture' : 'images/StepStart.png', 'text' : 'Resize'},
                {'id' : 'IGFRAMEHISTORY_STEP_LAYERCHANGE', 'picture' : 'images/StepFilter.png', 'text' : 'Filter'},
                {'id' : 'IGFRAMEHISTORY_STEP_LAYERADD', 'picture' : 'images/AddLayer.png', 'text' : 'Add layer'},
                {'id' : 'IGFRAMEHISTORY_STEP_LAYERMOVE', 'picture' : 'images/StepMoveLayer.png', 'text' : 'Move layer'},
                {'id' : 'IGFRAMEHISTORY_STEP_LAYERREMOVE', 'picture' : 'images/RemoveLayer.png', 'text' : 'Remove layer'},
                {'id' : 'IGFRAMEHISTORY_STEP_LAYERMERGE', 'picture': 'images/MergeLayers.png', 'text': 'Merge layers' },
                {'id' : 'IGFRAMEHISTORY_STEP_MOVE', 'picture': 'images/mover.png', 'text': 'Move layer' },
                {'id' : 'IGFRAMEHISTORY_STEP_MOVEPIXELS', 'picture': 'images/mover.png', 'text': 'Move pixels' },
                { 'id': 'IGFRAMEHISTORY_STEP_ROTATEANDRESIZE', 'picture': 'images/rotateAndResize.png', 'text': 'Homothety'}];

var IG_tLASSO = [];
var IG_tLASSO_Visual = [];

function IG_Face(IG_deepZoomPanel, tFacePts, tFaceEyeLeftPt, tFaceEyeRightPt, tFaceMouthPt, tFaceNozePt, faceColor, faceRot) {
    this.divDeepZoomPanel = IG_deepZoomPanel;
    this.tFacePt = IG_internalSquareCopy(tFacePts);
    this.tEyeLeftPt = IG_internalSquareCopy(tFaceEyeLeftPt);
    this.tEyeRightPt = IG_internalSquareCopy(tFaceEyeRightPt);
    this.tMouthPt = IG_internalSquareCopy(tFaceMouthPt);
    this.tNozePt = IG_internalSquareCopy(tFaceNozePt);
    this.color = faceColor;
    this.rotation = parseInt(faceRot);
    this.isOverFace = false;
    this.isOverEyeLeft = false;
    this.isOverEyeRight = false;
    this.isOverMouth = false;
    this.isOverNoze = false;
    this.isRotating = false;
    this.isResizing = false;
    this.isDragging = false;
    this.tFacePx = {};
    this.tEyeLeftPx = {};
    this.tEyeRightPx = {};
    this.tMouthPx = {};
    this.tNozePx = {};
    this.tStartDragFaceCenterPx = null;
    this.isPxUpdated = false;
    this.minFaceSize = 50;
    this.buttonRadiusSize = 10;    
    this.startFacePxCtr;
    this.startFace;
    this.buttonDeletePxCtr;
    this.buttonRotatePxCtr;
    this.buttonResizePxCtr;
    this.buttonDuplicatePxCtr;
    this.buttonZoomInPxCtr;
    this.buttonZoomOutPxCtr;

    this.Clone = function () {
        var cloneFace = new IG_Face(this.divDeepZoomPanel, this.tFacePt, this.tEyeLeftPt, this.tEyeRightPt, this.tMouthPt, this.tNozePt, this.color, this.rotation);
        cloneFace.updatePx();
        return cloneFace;
    }

    this.updatePx = function () {
        if (this.divDeepZoomPanel == null)
            return;
        if (this.divDeepZoomPanel.Viewer == null)
            return;
        if (this.divDeepZoomPanel.Viewer.viewport == null)
            return;
        this.tFacePx[0] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tFacePt[0]);
        this.tFacePx[1] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tFacePt[1]);
        this.tFacePx[2] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tFacePt[2]);
        this.tFacePx[3] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tFacePt[3]);
        this.tEyeLeftPx[0] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tEyeLeftPt[0]);
        this.tEyeLeftPx[1] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tEyeLeftPt[1]);
        this.tEyeLeftPx[2] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tEyeLeftPt[2]);
        this.tEyeLeftPx[3] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tEyeLeftPt[3]);
        this.tEyeRightPx[0] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tEyeRightPt[0]);
        this.tEyeRightPx[1] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tEyeRightPt[1]);
        this.tEyeRightPx[2] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tEyeRightPt[2]);
        this.tEyeRightPx[3] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tEyeRightPt[3]);
        this.tMouthPx[0] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tMouthPt[0]);
        this.tMouthPx[1] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tMouthPt[1]);
        this.tMouthPx[2] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tMouthPt[2]);
        this.tMouthPx[3] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tMouthPt[3]);
        this.tNozePx[0] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tNozePt[0]);
        this.tNozePx[1] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tNozePt[1]);
        this.tNozePx[2] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tNozePt[2]);
        this.tNozePx[3] = this.divDeepZoomPanel.Viewer.viewport.pixelFromPoint(this.tNozePt[3]);
        var faceCtr = new Point((this.tFacePx[0].x + this.tFacePx[3].x) / 2, (this.tFacePx[2].y + this.tFacePx[1].y) / 2);
        var vecDirDelete = this.tFacePx[3].minus(faceCtr);
        var vecDirRotate = this.tFacePx[2].minus(faceCtr);
        var vecDirResize = this.tFacePx[1].minus(faceCtr);
        var vecDirDuplicate = this.tFacePx[0].minus(faceCtr);
        var faceRightCtr = new Point((this.tFacePx[1].x + this.tFacePx[3].x) / 2, (this.tFacePx[1].y + this.tFacePx[3].y) / 2);
        var vecDirRightCtr = faceRightCtr.minus(faceCtr);
        var vecDirRightCtrToTop = this.tFacePx[3].minus(faceRightCtr);
        var sizeDiag = vecDirDelete.norm() - this.buttonRadiusSize;
        var sizeSide = vecDirRightCtr.norm() - this.buttonRadiusSize;
        vecDirDelete = vecDirDelete.times(sizeDiag / vecDirDelete.norm());
        vecDirRotate = vecDirRotate.times(sizeDiag / vecDirRotate.norm());
        vecDirResize = vecDirResize.times(sizeDiag / vecDirResize.norm());
        vecDirRightCtr = vecDirRightCtr.times(sizeSide / vecDirRightCtr.norm());
        vecDirRightCtrToTop = vecDirRightCtrToTop.times(10.0 / vecDirRightCtrToTop.norm());
        this.buttonDeletePxCtr = faceCtr.plus(vecDirDelete);
        this.buttonRotatePxCtr = faceCtr.plus(vecDirRotate);
        this.buttonResizePxCtr = faceCtr.plus(vecDirResize);
        this.buttonDuplicatePxCtr = faceCtr.plus(vecDirDuplicate);
        this.buttonZoomInPxCtr = faceCtr.plus(vecDirRightCtr).plus(vecDirRightCtrToTop);
        this.buttonZoomOutPxCtr = faceCtr.plus(vecDirRightCtr).minus(vecDirRightCtrToTop);
        this.isPxUpdated = true;
    }
    this.updatePt = function () {
        this.tFacePt[0] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tFacePx[0]);
        this.tFacePt[1] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tFacePx[1]);
        this.tFacePt[2] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tFacePx[2]);
        this.tFacePt[3] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tFacePx[3]);
        this.tEyeLeftPt[0] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tEyeLeftPx[0]);
        this.tEyeLeftPt[1] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tEyeLeftPx[1]);
        this.tEyeLeftPt[2] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tEyeLeftPx[2]);
        this.tEyeLeftPt[3] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tEyeLeftPx[3]);
        this.tEyeRightPt[0] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tEyeRightPx[0]);
        this.tEyeRightPt[1] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tEyeRightPx[1]);
        this.tEyeRightPt[2] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tEyeRightPx[2]);
        this.tEyeRightPt[3] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tEyeRightPx[3]);
        this.tMouthPt[0] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tMouthPx[0]);
        this.tMouthPt[1] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tMouthPx[1]);
        this.tMouthPt[2] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tMouthPx[2]);
        this.tMouthPt[3] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tMouthPx[3]);
        this.tNozePt[0] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tNozePx[0]);
        this.tNozePt[1] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tNozePx[1]);
        this.tNozePt[2] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tNozePx[2]);
        this.tNozePt[3] = this.divDeepZoomPanel.Viewer.viewport.pointFromPixel(this.tNozePx[3]);
    }
    this.isPxOver = function (tPx, curPx) {
        var intersections = 0;
        if (IG_internalAreIntersecting(tPx[0].x, tPx[0].y, tPx[1].x, tPx[1].y, 0, 0, curPx.x, curPx.y))
            intersections++;
        if (IG_internalAreIntersecting(tPx[1].x, tPx[1].y, tPx[3].x, tPx[3].y, 0, 0, curPx.x, curPx.y))
            intersections++;
        if (IG_internalAreIntersecting(tPx[3].x, tPx[3].y, tPx[2].x, tPx[2].y, 0, 0, curPx.x, curPx.y))
            intersections++;
        if (IG_internalAreIntersecting(tPx[2].x, tPx[2].y, tPx[0].x, tPx[0].y, 0, 0, curPx.x, curPx.y))
            intersections++;
        return ((intersections & 1) == 1);
    }
    this.isPxOverFace = function (curPx) {
        return this.isPxOver(this.tFacePx, curPx);
    }
    this.isPxOverEyeLeft = function (curPx) {
        return this.isPxOver(this.tEyeLeftPx, curPx);
    }
    this.isPxOverEyeRight = function (curPx) {
        return this.isPxOver(this.tEyeRightPx, curPx);
    }
    this.isPxOverMouth = function (curPx) {
        return this.isPxOver(this.tMouthPx, curPx);
    }
    this.isPxOverNoze = function (curPx) {
        return this.isPxOver(this.tNozePx, curPx);
    }
    this.isVisible = function () {
        var dzSize = this.divDeepZoomPanel.Utils.getElementSize(this.divDeepZoomPanel.Viewer.elmt);
        if (this.tFacePx[0].x < 0 || this.tFacePx[1].x < 0 || this.tFacePx[2].x < 0 || this.tFacePx[3].x < 0 ||
            this.tFacePx[0].y < 0 || this.tFacePx[1].y < 0 || this.tFacePx[2].y < 0 || this.tFacePx[3].y < 0 ||
            this.tFacePx[0].x > dzSize.x || this.tFacePx[1].x > dzSize.x || this.tFacePx[2].x > dzSize.x || this.tFacePx[3].x > dzSize.x ||
            this.tFacePx[0].y > dzSize.y || this.tFacePx[1].y > dzSize.y || this.tFacePx[2].y > dzSize.y || this.tFacePx[3].y > dzSize.y)
            return false;
        return true;
    }
    this.rotate = function (startVec, endVec) {
        startVec = startVec.divide(startVec.norm());
        endVec = endVec.divide(endVec.norm());
        var scalProd = endVec.x * startVec.x + endVec.y * startVec.y;
        var vectProd = endVec.x * startVec.y - endVec.y * startVec.x;
        var angle = Math.acos(scalProd);
        if (vectProd < 0)
            angle *= -1;
        if (!this.isRotating) {
            this.isRotating = true;
            this.startFace = this.Clone();
            this.startFacePxCtr = new Point((this.tFacePx[0].x + this.tFacePx[3].x) / 2, (this.tFacePx[2].y + this.tFacePx[1].y) / 2);
        }
        this.rotation = this.startFace.rotation - Math.round(angle * 180.0 / Math.PI);
        this.tFacePx[0] = this.startFacePxCtr.plus(this.startFace.tFacePx[0].minus(this.startFacePxCtr).rotate(angle));
        this.tFacePx[1] = this.startFacePxCtr.plus(this.startFace.tFacePx[1].minus(this.startFacePxCtr).rotate(angle));
        this.tFacePx[2] = this.startFacePxCtr.plus(this.startFace.tFacePx[2].minus(this.startFacePxCtr).rotate(angle));
        this.tFacePx[3] = this.startFacePxCtr.plus(this.startFace.tFacePx[3].minus(this.startFacePxCtr).rotate(angle));
        this.tEyeLeftPx[0] = this.startFacePxCtr.plus(this.startFace.tEyeLeftPx[0].minus(this.startFacePxCtr).rotate(angle));
        this.tEyeLeftPx[1] = this.startFacePxCtr.plus(this.startFace.tEyeLeftPx[1].minus(this.startFacePxCtr).rotate(angle));
        this.tEyeLeftPx[2] = this.startFacePxCtr.plus(this.startFace.tEyeLeftPx[2].minus(this.startFacePxCtr).rotate(angle));
        this.tEyeLeftPx[3] = this.startFacePxCtr.plus(this.startFace.tEyeLeftPx[3].minus(this.startFacePxCtr).rotate(angle));
        this.tEyeRightPx[0] = this.startFacePxCtr.plus(this.startFace.tEyeRightPx[0].minus(this.startFacePxCtr).rotate(angle));
        this.tEyeRightPx[1] = this.startFacePxCtr.plus(this.startFace.tEyeRightPx[1].minus(this.startFacePxCtr).rotate(angle));
        this.tEyeRightPx[2] = this.startFacePxCtr.plus(this.startFace.tEyeRightPx[2].minus(this.startFacePxCtr).rotate(angle));
        this.tEyeRightPx[3] = this.startFacePxCtr.plus(this.startFace.tEyeRightPx[3].minus(this.startFacePxCtr).rotate(angle));
        this.tMouthPx[0] = this.startFacePxCtr.plus(this.startFace.tMouthPx[0].minus(this.startFacePxCtr).rotate(angle));
        this.tMouthPx[1] = this.startFacePxCtr.plus(this.startFace.tMouthPx[1].minus(this.startFacePxCtr).rotate(angle));
        this.tMouthPx[2] = this.startFacePxCtr.plus(this.startFace.tMouthPx[2].minus(this.startFacePxCtr).rotate(angle));
        this.tMouthPx[3] = this.startFacePxCtr.plus(this.startFace.tMouthPx[3].minus(this.startFacePxCtr).rotate(angle));
        this.tNozePx[0] = this.startFacePxCtr.plus(this.startFace.tNozePx[0].minus(this.startFacePxCtr).rotate(angle));
        this.tNozePx[1] = this.startFacePxCtr.plus(this.startFace.tNozePx[1].minus(this.startFacePxCtr).rotate(angle));
        this.tNozePx[2] = this.startFacePxCtr.plus(this.startFace.tNozePx[2].minus(this.startFacePxCtr).rotate(angle));
        this.tNozePx[3] = this.startFacePxCtr.plus(this.startFace.tNozePx[3].minus(this.startFacePxCtr).rotate(angle));
        this.buttonDeletePxCtr = this.startFacePxCtr.plus(this.startFace.buttonDeletePxCtr.minus(this.startFacePxCtr).rotate(angle));
        this.buttonRotatePxCtr = this.startFacePxCtr.plus(this.startFace.buttonRotatePxCtr.minus(this.startFacePxCtr).rotate(angle));
        this.buttonResizePxCtr = this.startFacePxCtr.plus(this.startFace.buttonResizePxCtr.minus(this.startFacePxCtr).rotate(angle));
        this.buttonDuplicatePxCtr = this.startFacePxCtr.plus(this.startFace.buttonDuplicatePxCtr.minus(this.startFacePxCtr).rotate(angle));
        this.buttonZoomInPxCtr = this.startFacePxCtr.plus(this.startFace.buttonZoomInPxCtr.minus(this.startFacePxCtr).rotate(angle));
        this.buttonZoomOutPxCtr = this.startFacePxCtr.plus(this.startFace.buttonZoomOutPxCtr.minus(this.startFacePxCtr).rotate(angle));
    }
    this.resize = function (vec) {
        if (!this.isResizing) {
            this.isResizing = true;
            this.startFace = this.Clone();
            this.startFacePxCtr = new Point((this.tFacePx[0].x + this.tFacePx[3].x) / 2, (this.tFacePx[2].y + this.tFacePx[1].y) / 2);
        }
        var vecDiagFace = this.tFacePx[1].minus(this.tFacePx[2]);
        var cos = (vecDiagFace.x * vec.x + vecDiagFace.y * vec.y) / vecDiagFace.norm();
        if (vecDiagFace.norm() + cos < this.minFaceSize)
            return;
        var ratio = (vecDiagFace.norm() + cos) / vecDiagFace.norm();
        this.tFacePx[0] = this.startFacePxCtr.plus(this.startFace.tFacePx[0].minus(this.startFacePxCtr).times(ratio));
        this.tFacePx[1] = this.startFacePxCtr.plus(this.startFace.tFacePx[1].minus(this.startFacePxCtr).times(ratio));
        this.tFacePx[2] = this.startFacePxCtr.plus(this.startFace.tFacePx[2].minus(this.startFacePxCtr).times(ratio));
        this.tFacePx[3] = this.startFacePxCtr.plus(this.startFace.tFacePx[3].minus(this.startFacePxCtr).times(ratio));
        this.tEyeLeftPx[0] = this.startFacePxCtr.plus(this.startFace.tEyeLeftPx[0].minus(this.startFacePxCtr).times(ratio));
        this.tEyeLeftPx[1] = this.startFacePxCtr.plus(this.startFace.tEyeLeftPx[1].minus(this.startFacePxCtr).times(ratio));
        this.tEyeLeftPx[2] = this.startFacePxCtr.plus(this.startFace.tEyeLeftPx[2].minus(this.startFacePxCtr).times(ratio));
        this.tEyeLeftPx[3] = this.startFacePxCtr.plus(this.startFace.tEyeLeftPx[3].minus(this.startFacePxCtr).times(ratio));
        this.tEyeRightPx[0] = this.startFacePxCtr.plus(this.startFace.tEyeRightPx[0].minus(this.startFacePxCtr).times(ratio));
        this.tEyeRightPx[1] = this.startFacePxCtr.plus(this.startFace.tEyeRightPx[1].minus(this.startFacePxCtr).times(ratio));
        this.tEyeRightPx[2] = this.startFacePxCtr.plus(this.startFace.tEyeRightPx[2].minus(this.startFacePxCtr).times(ratio));
        this.tEyeRightPx[3] = this.startFacePxCtr.plus(this.startFace.tEyeRightPx[3].minus(this.startFacePxCtr).times(ratio));
        this.tMouthPx[0] = this.startFacePxCtr.plus(this.startFace.tMouthPx[0].minus(this.startFacePxCtr).times(ratio));
        this.tMouthPx[1] = this.startFacePxCtr.plus(this.startFace.tMouthPx[1].minus(this.startFacePxCtr).times(ratio));
        this.tMouthPx[2] = this.startFacePxCtr.plus(this.startFace.tMouthPx[2].minus(this.startFacePxCtr).times(ratio));
        this.tMouthPx[3] = this.startFacePxCtr.plus(this.startFace.tMouthPx[3].minus(this.startFacePxCtr).times(ratio));
        this.tNozePx[0] = this.startFacePxCtr.plus(this.startFace.tNozePx[0].minus(this.startFacePxCtr).times(ratio));
        this.tNozePx[1] = this.startFacePxCtr.plus(this.startFace.tNozePx[1].minus(this.startFacePxCtr).times(ratio));
        this.tNozePx[2] = this.startFacePxCtr.plus(this.startFace.tNozePx[2].minus(this.startFacePxCtr).times(ratio));
        this.tNozePx[3] = this.startFacePxCtr.plus(this.startFace.tNozePx[3].minus(this.startFacePxCtr).times(ratio));
        this.buttonDeletePxCtr = this.startFacePxCtr.plus(this.startFace.buttonDeletePxCtr.minus(this.startFacePxCtr).times(ratio));
        this.buttonRotatePxCtr = this.startFacePxCtr.plus(this.startFace.buttonRotatePxCtr.minus(this.startFacePxCtr).times(ratio));
        this.buttonResizePxCtr = this.startFacePxCtr.plus(this.startFace.buttonResizePxCtr.minus(this.startFacePxCtr).times(ratio));
        this.buttonDuplicatePxCtr = this.startFacePxCtr.plus(this.startFace.buttonDuplicatePxCtr.minus(this.startFacePxCtr).times(ratio));
        this.buttonZoomInPxCtr = this.startFacePxCtr.plus(this.startFace.buttonZoomInPxCtr.minus(this.startFacePxCtr).times(ratio));
        this.buttonZoomOutPxCtr = this.startFacePxCtr.plus(this.startFace.buttonZoomOutPxCtr.minus(this.startFacePxCtr).times(ratio));
    }
    this.scale = function (scaleX, scaleY) {
        var sclVec = new Point(scaleX, scaleY);
        var ctrFace = this.tFacePt[3].plus(this.tFacePt[0]).times(0.5);
        var offsetVec = ctrFace.scale(sclVec).minus(ctrFace);
        this.tFacePt[0] = this.tFacePt[0].plus(offsetVec);
        this.tFacePt[1] = this.tFacePt[1].plus(offsetVec);
        this.tFacePt[2] = this.tFacePt[2].plus(offsetVec);
        this.tFacePt[3] = this.tFacePt[3].plus(offsetVec);
        this.tEyeLeftPt[0] = this.tEyeLeftPt[0].plus(offsetVec);
        this.tEyeLeftPt[1] = this.tEyeLeftPt[1].plus(offsetVec);
        this.tEyeLeftPt[2] = this.tEyeLeftPt[2].plus(offsetVec);
        this.tEyeLeftPt[3] = this.tEyeLeftPt[3].plus(offsetVec);
        this.tEyeRightPt[0] = this.tEyeRightPt[0].plus(offsetVec);
        this.tEyeRightPt[1] = this.tEyeRightPt[1].plus(offsetVec);
        this.tEyeRightPt[2] = this.tEyeRightPt[2].plus(offsetVec);
        this.tEyeRightPt[3] = this.tEyeRightPt[3].plus(offsetVec);
        this.tMouthPt[0] = this.tMouthPt[0].plus(offsetVec);
        this.tMouthPt[1] = this.tMouthPt[1].plus(offsetVec);
        this.tMouthPt[2] = this.tMouthPt[2].plus(offsetVec);
        this.tMouthPt[3] = this.tMouthPt[3].plus(offsetVec);
        this.tNozePt[0] = this.tNozePt[0].plus(offsetVec);
        this.tNozePt[1] = this.tNozePt[1].plus(offsetVec);
        this.tNozePt[2] = this.tNozePt[2].plus(offsetVec);
        this.tNozePt[3] = this.tNozePt[3].plus(offsetVec);
        this.updatePx();
    }
    this.drag = function (vec) {
        if (!this.isDragging)
            this.isDragging = true;
        var centerEyeLeftPx = IG_internalGetSquareCenter(this.tEyeLeftPx);
        var centerEyeRightPx = IG_internalGetSquareCenter(this.tEyeRightPx);
        var centerMouthPx = IG_internalGetSquareCenter(this.tMouthPx);
        var centerFacePx = IG_internalGetSquareCenter(this.tFacePx);
        IG_internalSetSquareCenter(this.tFacePx, this.tStartDragFaceCenterPx);
        IG_internalOffsetSquare(this.tFacePx, new Point(vec.x, vec.y));

        var newCenterFacePx = IG_internalGetSquareCenter(this.tFacePx);
        IG_internalSetSquareCenter(this.tEyeLeftPx, newCenterFacePx.plus(centerEyeLeftPx).minus(centerFacePx));
        IG_internalSetSquareCenter(this.tEyeRightPx, newCenterFacePx.plus(centerEyeRightPx).minus(centerFacePx));
        IG_internalSetSquareCenter(this.tMouthPx, newCenterFacePx.plus(centerMouthPx).minus(centerFacePx));
    }
    this.mouseOver = function (divCurrentWorkspace, divDeepZoomPanel, ctx, curPx, event) {
        if (divDeepZoomPanel) {
            if (divDeepZoomPanel.Viewer) {
                this.updatePx();
                var isCurOver = this.isPxOverEyeLeft(curPx);
                var change = false;
                if (isCurOver != this.isOverEyeLeft) {
                    this.isOverEyeLeft = isCurOver;
                    change = true;
                    if (this.isOverEyeLeft) {
                        this.isOverEyeRight = false;
                        this.isOverMouth = false;
                        this.isOverNoze = false;
                        this.isOverFace = false;
                        return true;
                    }
                }
                isCurOver = this.isPxOverEyeRight(curPx);
                if (isCurOver != this.isOverEyeRight) {
                    this.isOverEyeRight = isCurOver;
                    change = true;
                    if (this.isOverEyeRight) {
                        this.isOverEyeLeft = false;
                        this.isOverMouth = false;
                        this.isOverNoze = false;
                        this.isOverFace = false;
                        return true;
                    }
                }
                isCurOver = this.isPxOverMouth(curPx);
                if (isCurOver != this.isOverMouth) {
                    this.isOverMouth = isCurOver;
                    change = true;
                    if (this.isOverMouth) {
                        this.isOverEyeLeft = false;
                        this.isOverEyeRight = false;
                        this.isOverNoze = false;
                        this.isOverFace = false;
                        return true;
                    }
                }
                isCurOver = this.isPxOverNoze(curPx);
                if (isCurOver != this.isOverNoze) {
                    this.isOverNoze = isCurOver;
                    change = true;
                    if (this.isOverNoze) {
                        this.isOverEyeLeft = false;
                        this.isOverEyeRight = false;
                        this.isOverMouth = false;
                        this.isOverFace = false;
                        return true;
                    }
                }
                if (this.isOverEyeLeft || this.isOverEyeRight || this.isOverMouth || this.isOverNoze)
                    return true;
                isCurOver = this.isPxOverFace(curPx);
                if (isCurOver != this.isOverFace) {
                    this.isOverFace = isCurOver;
                    if (this.isOverFace) {
                        this.isOverEyeLeft = false;
                        this.isOverEyeRight = false;
                        this.isOverMouth = false;
                        this.isOverNoze = false;
                    }
                    return true;
                }
                return change || this.isOverFace;
            }
        }
        return false;
    }
    this.mouseDrag = function (divCurrentWorkspace, divDeepZoomPanel, ctx, startPx, curPx, event) {
        if (!this.isRotating && !this.isResizing && !this.isDragging && !this.isVisible())
            return false;
        var tPx = {};
        if (this.isOverFace)
            tPx = this.tFacePx;
        else if (this.isOverEyeLeft)
            tPx = this.tEyeLeftPx;
        else if (this.isOverEyeRight)
            tPx = this.tEyeRightPx;
        else if (this.isOverMouth)
            tPx = this.tMouthPx;
        var doUpdatePt = (this.isOverEyeLeft || this.isOverEyeRight || this.isOverMouth || this.isOverNoze || this.isOverFace);
        if (doUpdatePt) {
            if (this.tStartDragFaceCenterPx == null)
                this.tStartDragFaceCenterPx = IG_internalGetSquareCenter(this.tFacePx);
            var doOffset = false;
            var vecFaceLeftX = this.tFacePx[2].x - this.tFacePx[0].x;
            var vecFaceLeftY = this.tFacePx[2].y - this.tFacePx[0].y;
            var vecFaceTopX = this.tFacePx[1].x - this.tFacePx[0].x;
            var vecFaceTopY = this.tFacePx[1].y - this.tFacePx[0].y;
            var vecTestBottomX = curPx.x - this.tFacePx[0].x;
            var vecTestBottomY = curPx.y - this.tFacePx[0].y;
            var vecTestTopX = curPx.x - this.tFacePx[2].x;
            var vecTestTopY = curPx.y - this.tFacePx[2].y;
            var vecTestLeftX = curPx.x - this.tFacePx[0].x;
            var vecTestLeftY = curPx.y - this.tFacePx[0].y;
            var vecTestRightX = curPx.x - this.tFacePx[1].x;
            var vecTestRightY = curPx.y - this.tFacePx[1].y;
            if ((vecFaceLeftX * vecTestTopX + vecFaceLeftY * vecTestTopY < 0)
                && (vecFaceLeftX * vecTestBottomX + vecFaceLeftY * vecTestBottomY > 0)
                && (vecFaceTopX * vecTestLeftX + vecFaceTopY * vecTestLeftY > 0)
                && (vecFaceTopX * vecTestRightX + vecFaceTopY * vecTestRightY < 0))
                doOffset = true;
            if (this.isOverFace) {
                if (((Math.max(Math.abs(this.tFacePx[1].x - this.tFacePx[0].x), Math.abs(this.tFacePx[2].x - this.tFacePx[0].x)) > this.minFaceSize) &&
                    (this.buttonRotatePxCtr.minus(curPx).norm() < this.buttonRadiusSize) && !this.isResizing && !this.isDragging) || this.isRotating) { // rotate face
                    var pxCenter = new Point((this.tFacePx[0].x + this.tFacePx[3].x) / 2, (this.tFacePx[2].y + this.tFacePx[1].y) / 2);
                    var startVec = new Point(startPx.x - pxCenter.x, startPx.y - pxCenter.y);
                    var endVec = new Point(curPx.x - pxCenter.x, curPx.y - pxCenter.y);
                    this.rotate(startVec, endVec);
                }
                else if (((Math.max(Math.abs(this.tFacePx[1].x - this.tFacePx[0].x), Math.abs(this.tFacePx[2].x - this.tFacePx[0].x)) > this.minFaceSize) &&
                        (this.buttonResizePxCtr.minus(curPx).norm() < this.buttonRadiusSize) && !this.isRotating && !this.isDragging) || this.isResizing) { // resize face
                    var vec = new Point(curPx.x - this.buttonResizePxCtr.x, curPx.y - this.buttonResizePxCtr.y);
                    this.resize(vec);
                }
                else if (((Math.max(Math.abs(this.tFacePx[1].x - this.tFacePx[0].x), Math.abs(this.tFacePx[2].x - this.tFacePx[0].x)) > this.minFaceSize) &&
                        !this.isRotating && !this.isResizing) || this.isDragging) { // drag face
                    var vec = new Point(curPx.x - startPx.x, curPx.y - startPx.y);
                    this.drag(vec);                    
                }
            }
            else if (doOffset) {
                IG_internalSetSquareCenter(tPx, startPx);
                IG_internalOffsetSquare(tPx, new Point(curPx.x - startPx.x, curPx.y - startPx.y));
            }
            this.updatePt();
            ctx.fillStyle = this.color;
            if (this.isOverFace) {
                IG_internalDrawWideEllipse(ctx, this.tFacePx[0], this.tFacePx[1], this.tFacePx[3], this.tFacePx[2], true);
                IG_internalDrawEllipse(ctx, this.tEyeLeftPx[0], this.tEyeLeftPx[1], this.tEyeLeftPx[3], this.tEyeLeftPx[2], true);
                IG_internalDrawEllipse(ctx, this.tEyeRightPx[0], this.tEyeRightPx[1], this.tEyeRightPx[3], this.tEyeRightPx[2], true);
                IG_internalDrawEllipse(ctx, this.tMouthPx[0], this.tMouthPx[1], this.tMouthPx[3], this.tMouthPx[2], true);
            }
            else {
                IG_internalDrawEllipse(ctx, tPx[0], tPx[1], tPx[3], tPx[2], true);
                var pxCtr = new Point((tPx[0].x + tPx[3].x) / 2, (tPx[0].y + tPx[3].y) / 2);    
                ctx.beginPath();
                ctx.arc(pxCtr.x, pxCtr.y, 2, 0, 2 * Math.PI, false);
                ctx.fillStyle = 'white';
                ctx.fill();
            }
        }
        return doUpdatePt;
    }
    this.mouseUp = function (divCurrentWorkspace, startPx, curPx, event) {
        this.tStartDragFaceCenterPx = null;
        this.isRotating = false;
        this.isResizing = false;
        this.isDragging = false;
    }
    this.mouseClick = function (divCurrentWorkspace, curPx, event) {
        this.isRotating = false;
        this.isResizing = false;
        this.isDragging = false;
        if (this.isOverFace && this.isVisible() &&
            (Math.max(Math.abs(this.tFacePx[1].x - this.tFacePx[0].x), Math.abs(this.tFacePx[2].x - this.tFacePx[0].x)) > this.minFaceSize)) {
            if (this.buttonDeletePxCtr.minus(curPx).norm() < this.buttonRadiusSize)
                return IGFACEDESCRIPTOR_BUTTON_DELETE;      // delete face
            else if (this.buttonDuplicatePxCtr.minus(curPx).norm() < this.buttonRadiusSize)
                return IGFACEDESCRIPTOR_BUTTON_DUPLICATE;   // duplicate face
            else if (this.buttonZoomInPxCtr.minus(curPx).norm() < this.buttonRadiusSize)
                return IGFACEDESCRIPTOR_BUTTON_ZOOMIN;   // zoom in face
            else if (this.buttonZoomOutPxCtr.minus(curPx).norm() < this.buttonRadiusSize)
                return IGFACEDESCRIPTOR_BUTTON_ZOOMOUT;   // zoom out face
        }
        return IGFACEDESCRIPTOR_BUTTON_NONE;
    }

    this.paint = function (divCurrentWorkspace, divDeepZoomPanel, ctx, event) {
        if (divDeepZoomPanel) {
            if (divDeepZoomPanel.Viewer) {
                if (!this.isPxUpdated)
                    this.updatePx();
                if (!this.isRotating && !this.isResizing && !this.isDragging && !this.isVisible())
                    return false;
                ctx.fillStyle = this.color;
                IG_internalDrawWideEllipse(ctx, this.tFacePx[0], this.tFacePx[1], this.tFacePx[3], this.tFacePx[2], true);
                if (this.tEyeLeftPx[0] != -1)
                    IG_internalDrawEllipse(ctx, this.tEyeLeftPx[0], this.tEyeLeftPx[1], this.tEyeLeftPx[3], this.tEyeLeftPx[2], true);
                if (this.tEyeRightPx[0] != -1)
                    IG_internalDrawEllipse(ctx, this.tEyeRightPx[0], this.tEyeRightPx[1], this.tEyeRightPx[3], this.tEyeRightPx[2], true);
                if (this.tMouthPx[0] != -1)
                    IG_internalDrawEllipse(ctx, this.tMouthPx[0], this.tMouthPx[1], this.tMouthPx[3], this.tMouthPx[2], true);
                ctx.strokeStyle = "rgba(255, 255, 255, 1.0)";
                ctx.lineWidth = 2;
                if (this.isOverFace || this.isOverEyeLeft || this.isOverEyeRight || this.isOverMouth) {
                    IG_internalDrawWideEllipse(ctx, this.tFacePx[0], this.tFacePx[1], this.tFacePx[3], this.tFacePx[2]);
                    if (Math.max(Math.abs(this.tFacePx[1].x - this.tFacePx[0].x), Math.abs(this.tFacePx[2].x - this.tFacePx[0].x)) > this.minFaceSize) {
                        // delete button
                        ctx.beginPath();
                        ctx.arc(this.buttonDeletePxCtr.x, this.buttonDeletePxCtr.y, this.buttonRadiusSize, 0, 2 * Math.PI, false);
                        ctx.fillStyle = this.color;
                        ctx.fill();
                        ctx.fillStyle = 'white';
                        ctx.beginPath();
                        ctx.arc(this.buttonDeletePxCtr.x, this.buttonDeletePxCtr.y, this.buttonRadiusSize, 0, 2 * Math.PI, false);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(this.buttonDeletePxCtr.x - 4, this.buttonDeletePxCtr.y + 4);
                        ctx.lineTo(this.buttonDeletePxCtr.x + 4, this.buttonDeletePxCtr.y - 4);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(this.buttonDeletePxCtr.x - 4, this.buttonDeletePxCtr.y - 4);
                        ctx.lineTo(this.buttonDeletePxCtr.x + 4, this.buttonDeletePxCtr.y + 4);
                        ctx.stroke();

                        // rotate button
                        ctx.beginPath();
                        ctx.arc(this.buttonRotatePxCtr.x, this.buttonRotatePxCtr.y, this.buttonRadiusSize, 0, 2 * Math.PI, false);
                        ctx.fillStyle = this.color;
                        ctx.fill();
                        ctx.fillStyle = 'white';
                        ctx.beginPath();
                        ctx.arc(this.buttonRotatePxCtr.x, this.buttonRotatePxCtr.y, this.buttonRadiusSize, 0, 2 * Math.PI, false);
                        ctx.stroke();
                        ctx.beginPath();
                        var rotateIconSize = 10;
                        var iconPxCtr = new Point(this.buttonRotatePxCtr.x + rotateIconSize / 2 + 2, this.buttonRotatePxCtr.y + rotateIconSize / 2 + 2);
                        ctx.arc(iconPxCtr.x, iconPxCtr.y, rotateIconSize, Math.PI, 1.5 * Math.PI, false);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(iconPxCtr.x - rotateIconSize / 2, iconPxCtr.y - rotateIconSize - 3);
                        ctx.lineTo(iconPxCtr.x, iconPxCtr.y - rotateIconSize);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(iconPxCtr.x - rotateIconSize / 2 + 1, iconPxCtr.y - rotateIconSize + 5);
                        ctx.lineTo(iconPxCtr.x, iconPxCtr.y - rotateIconSize);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(iconPxCtr.x - rotateIconSize - 3, iconPxCtr.y - rotateIconSize / 2);
                        ctx.lineTo(iconPxCtr.x - rotateIconSize, iconPxCtr.y);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(iconPxCtr.x - rotateIconSize + 5, iconPxCtr.y - rotateIconSize / 2 + 1);
                        ctx.lineTo(iconPxCtr.x - rotateIconSize, iconPxCtr.y);
                        ctx.stroke();

                        // resize button
                        ctx.beginPath();
                        ctx.arc(this.buttonResizePxCtr.x, this.buttonResizePxCtr.y, this.buttonRadiusSize, 0, 2 * Math.PI, false);
                        ctx.fillStyle = this.color;
                        ctx.fill();
                        ctx.fillStyle = 'white';
                        ctx.beginPath();
                        ctx.arc(this.buttonResizePxCtr.x, this.buttonResizePxCtr.y, this.buttonRadiusSize, 0, 2 * Math.PI, false);
                        ctx.stroke();
                        var resizeIconSize = 10;                        
                        ctx.beginPath();
                        ctx.moveTo(this.buttonResizePxCtr.x + resizeIconSize / 2, this.buttonResizePxCtr.y + resizeIconSize / 2);
                        ctx.lineTo(this.buttonResizePxCtr.x - resizeIconSize / 2, this.buttonResizePxCtr.y - resizeIconSize / 2);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(this.buttonResizePxCtr.x + resizeIconSize / 2 - 4, this.buttonResizePxCtr.y + resizeIconSize / 2);
                        ctx.lineTo(this.buttonResizePxCtr.x + resizeIconSize / 2, this.buttonResizePxCtr.y + resizeIconSize / 2);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(this.buttonResizePxCtr.x + resizeIconSize / 2, this.buttonResizePxCtr.y + resizeIconSize / 2 - 4);
                        ctx.lineTo(this.buttonResizePxCtr.x + resizeIconSize / 2, this.buttonResizePxCtr.y + resizeIconSize / 2);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(this.buttonResizePxCtr.x - resizeIconSize / 2 + 4, this.buttonResizePxCtr.y - resizeIconSize / 2);
                        ctx.lineTo(this.buttonResizePxCtr.x - resizeIconSize / 2, this.buttonResizePxCtr.y - resizeIconSize / 2);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(this.buttonResizePxCtr.x - resizeIconSize / 2, this.buttonResizePxCtr.y - resizeIconSize / 2 + 4);
                        ctx.lineTo(this.buttonResizePxCtr.x - resizeIconSize / 2, this.buttonResizePxCtr.y - resizeIconSize / 2);
                        ctx.stroke();

                        // duplicate button
                        ctx.beginPath();
                        ctx.arc(this.buttonDuplicatePxCtr.x, this.buttonDuplicatePxCtr.y, this.buttonRadiusSize, 0, 2 * Math.PI, false);
                        ctx.fillStyle = this.color;
                        ctx.fill();
                        ctx.fillStyle = 'white';
                        ctx.beginPath();
                        ctx.arc(this.buttonDuplicatePxCtr.x, this.buttonDuplicatePxCtr.y, this.buttonRadiusSize, 0, 2 * Math.PI, false);
                        ctx.stroke();
                        var duplicateIconSize = 10;
                        ctx.beginPath();
                        ctx.moveTo(this.buttonDuplicatePxCtr.x - duplicateIconSize / 2 + 2, this.buttonDuplicatePxCtr.y - 3);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x - duplicateIconSize / 2 + 2, this.buttonDuplicatePxCtr.y + duplicateIconSize - 6);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x + 4, this.buttonDuplicatePxCtr.y + duplicateIconSize - 6);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x + 4, this.buttonDuplicatePxCtr.y - 5);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x - duplicateIconSize / 2 + 4, this.buttonDuplicatePxCtr.y - 5);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x - duplicateIconSize / 2 + 2, this.buttonDuplicatePxCtr.y - 3);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(this.buttonDuplicatePxCtr.x - duplicateIconSize / 2 - 1, this.buttonDuplicatePxCtr.y - 1);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x - duplicateIconSize / 2 - 1, this.buttonDuplicatePxCtr.y + duplicateIconSize - 4);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x + 1, this.buttonDuplicatePxCtr.y + duplicateIconSize - 4);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x + 1, this.buttonDuplicatePxCtr.y - 3);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x - duplicateIconSize / 2 + 1, this.buttonDuplicatePxCtr.y - 3);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x - duplicateIconSize / 2 - 1, this.buttonDuplicatePxCtr.y - 1);
                        ctx.fillStyle = this.color;
                        ctx.fill();
                        ctx.fillStyle = 'white';
                        ctx.beginPath();
                        ctx.moveTo(this.buttonDuplicatePxCtr.x - duplicateIconSize / 2 - 1, this.buttonDuplicatePxCtr.y - 1);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x - duplicateIconSize / 2 - 1, this.buttonDuplicatePxCtr.y + duplicateIconSize - 4);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x + 1, this.buttonDuplicatePxCtr.y + duplicateIconSize - 4);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x + 1, this.buttonDuplicatePxCtr.y - 3);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x - duplicateIconSize / 2 + 1, this.buttonDuplicatePxCtr.y - 3);
                        ctx.lineTo(this.buttonDuplicatePxCtr.x - duplicateIconSize / 2 - 1, this.buttonDuplicatePxCtr.y - 1);
                        ctx.stroke();

                        // zoom-in button
                        ctx.beginPath();
                        ctx.arc(this.buttonZoomInPxCtr.x, this.buttonZoomInPxCtr.y, this.buttonRadiusSize, 0, 2 * Math.PI, false);
                        ctx.fillStyle = this.color;
                        ctx.fill();
                        ctx.fillStyle = 'white';
                        ctx.beginPath();
                        ctx.arc(this.buttonZoomInPxCtr.x, this.buttonZoomInPxCtr.y, this.buttonRadiusSize, 0, 2 * Math.PI, false);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(this.buttonZoomInPxCtr.x, this.buttonZoomInPxCtr.y - 4);
                        ctx.lineTo(this.buttonZoomInPxCtr.x, this.buttonZoomInPxCtr.y + 4);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(this.buttonZoomInPxCtr.x - 4, this.buttonZoomInPxCtr.y);
                        ctx.lineTo(this.buttonZoomInPxCtr.x + 4, this.buttonZoomInPxCtr.y);
                        ctx.stroke();
                        
                        // zoom-out button
                        ctx.beginPath();
                        ctx.arc(this.buttonZoomOutPxCtr.x, this.buttonZoomOutPxCtr.y, this.buttonRadiusSize, 0, 2 * Math.PI, false);
                        ctx.fillStyle = this.color;
                        ctx.fill();
                        ctx.fillStyle = 'white';
                        ctx.beginPath();
                        ctx.arc(this.buttonZoomOutPxCtr.x, this.buttonZoomOutPxCtr.y, this.buttonRadiusSize, 0, 2 * Math.PI, false);
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.moveTo(this.buttonZoomOutPxCtr.x - 4, this.buttonZoomOutPxCtr.y);
                        ctx.lineTo(this.buttonZoomOutPxCtr.x + 4, this.buttonZoomOutPxCtr.y);
                        ctx.stroke();
                    }
                }
                if (this.isOverEyeLeft) {
                    IG_internalDrawEllipse(ctx, this.tEyeLeftPx[0], this.tEyeLeftPx[1], this.tEyeLeftPx[3], this.tEyeLeftPx[2]);
                    var pxCtrEyeLeft = new Point((this.tEyeLeftPx[0].x + this.tEyeLeftPx[3].x) / 2, (this.tEyeLeftPx[0].y + this.tEyeLeftPx[3].y) / 2);                    
                    ctx.beginPath();
                    ctx.arc(pxCtrEyeLeft.x, pxCtrEyeLeft.y, 2, 0, 2 * Math.PI, false);
                    ctx.fillStyle = 'white';
                    ctx.fill();
                }
                else if (this.isOverEyeRight) {
                    IG_internalDrawEllipse(ctx, this.tEyeRightPx[0], this.tEyeRightPx[1], this.tEyeRightPx[3], this.tEyeRightPx[2]);
                    var pxCtrEyeRight = new Point((this.tEyeRightPx[0].x + this.tEyeRightPx[3].x) / 2, (this.tEyeRightPx[0].y + this.tEyeRightPx[3].y) / 2);
                    ctx.beginPath();
                    ctx.arc(pxCtrEyeRight.x, pxCtrEyeRight.y, 2, 0, 2 * Math.PI, false);
                    ctx.fillStyle = 'white';
                    ctx.fill();
                }
                else if (this.isOverMouth)
                    IG_internalDrawEllipse(ctx, this.tMouthPx[0], this.tMouthPx[1], this.tMouthPx[3], this.tMouthPx[2]);
            }
        }
        return true;
    }    
}

var IG_tFaceDescColors = ["rgba(0, 0, 200, 0.5)", "rgba(200, 0, 0, 0.5)", "rgba(0, 200, 0, 0.5)", "rgba(200, 0, 200, 0.5)", "rgba(0, 200, 200, 0.5)",
                        "rgba(200, 200, 0, 0.5)", "rgba(200, 100, 0, 0.5)", "rgba(100, 200, 0, 0.5)", "rgba(100, 100, 200, 0.5)", "rgba(200, 100, 200, 0.5)"];

function IG_FaceDescriptor() {
    this.nbFaces = IG_internalGetCurrentPictureProperty("NbFaces");
    if (this.nbFaces > 10)   // set the nb faces limit to 10
        this.nbFaces = 10;
    this.tFaces = {};
    var reg = new RegExp("[,]", "g");
    for (var idxFace = 0; idxFace < this.nbFaces; idxFace++) {
        var curFace = "Face" + idxFace;
        var faceDescr = IG_internalGetCurrentPictureProperty(curFace + "_TLTRDZ");
        var tRcFaceTLTR = faceDescr.split(reg);
        faceDescr = IG_internalGetCurrentPictureProperty(curFace + "_BLBRDZ");
        var tRcFaceBLBR = faceDescr.split(reg);
        faceDescr = IG_internalGetCurrentPictureProperty(curFace + "_EyeLeft_TLTRDZ");
        var tRcEyeLeftTLTR = faceDescr.split(reg);
        faceDescr = IG_internalGetCurrentPictureProperty(curFace + "_EyeLeft_BLBRDZ");
        var tRcEyeLeftBLBR = faceDescr.split(reg);
        faceDescr = IG_internalGetCurrentPictureProperty(curFace + "_EyeRight_TLTRDZ");
        var tRcEyeRightTLTR = faceDescr.split(reg);
        faceDescr = IG_internalGetCurrentPictureProperty(curFace + "_EyeRight_BLBRDZ");
        var tRcEyeRightBLBR = faceDescr.split(reg);
        faceDescr = IG_internalGetCurrentPictureProperty(curFace + "_Mouth_TLTRDZ");
        var tRcMouthTLTR = faceDescr.split(reg);
        faceDescr = IG_internalGetCurrentPictureProperty(curFace + "_Mouth_BLBRDZ");
        var tRcMouthBLBR = faceDescr.split(reg);
        faceDescr = IG_internalGetCurrentPictureProperty(curFace + "_Noze_TLTRDZ");
        var tRcNozeTLTR = faceDescr.split(reg);
        faceDescr = IG_internalGetCurrentPictureProperty(curFace + "_Noze_BLBRDZ");
        var tRcNoveBLBR = faceDescr.split(reg);
        var faceRot = IG_internalGetCurrentPictureProperty(curFace + "_Rot");
        var tFacesPt = {};
        tFacesPt[0] = new Point(Number(tRcFaceTLTR[0]), Number(tRcFaceTLTR[2]));
        tFacesPt[1] = new Point(Number(tRcFaceTLTR[1]), Number(tRcFaceTLTR[3]));
        tFacesPt[2] = new Point(Number(tRcFaceBLBR[0]), Number(tRcFaceBLBR[2]));
        tFacesPt[3] = new Point(Number(tRcFaceBLBR[1]), Number(tRcFaceBLBR[3]));
        var tEyeLeftPt = {};
        tEyeLeftPt[0] = new Point(Number(tRcEyeLeftTLTR[0]), Number(tRcEyeLeftTLTR[2]));
        tEyeLeftPt[1] = new Point(Number(tRcEyeLeftTLTR[1]), Number(tRcEyeLeftTLTR[3]));
        tEyeLeftPt[2] = new Point(Number(tRcEyeLeftBLBR[0]), Number(tRcEyeLeftBLBR[2]));
        tEyeLeftPt[3] = new Point(Number(tRcEyeLeftBLBR[1]), Number(tRcEyeLeftBLBR[3]));
        var tEyeRightPt = {};
        tEyeRightPt[0] = new Point(Number(tRcEyeRightTLTR[0]), Number(tRcEyeRightTLTR[2]));
        tEyeRightPt[1] = new Point(Number(tRcEyeRightTLTR[1]), Number(tRcEyeRightTLTR[3]));
        tEyeRightPt[2] = new Point(Number(tRcEyeRightBLBR[0]), Number(tRcEyeRightBLBR[2]));
        tEyeRightPt[3] = new Point(Number(tRcEyeRightBLBR[1]), Number(tRcEyeRightBLBR[3]));
        var tMouthPt = {};
        tMouthPt[0] = new Point(Number(tRcMouthTLTR[0]), Number(tRcMouthTLTR[2]));
        tMouthPt[1] = new Point(Number(tRcMouthTLTR[1]), Number(tRcMouthTLTR[3]));
        tMouthPt[2] = new Point(Number(tRcMouthBLBR[0]), Number(tRcMouthBLBR[2]));
        tMouthPt[3] = new Point(Number(tRcMouthBLBR[1]), Number(tRcMouthBLBR[3]));
        var tNozePt = {};
        tNozePt[0] = new Point(Number(tRcNozeTLTR[0]), Number(tRcNozeTLTR[2]));
        tNozePt[1] = new Point(Number(tRcNozeTLTR[1]), Number(tRcNozeTLTR[3]));
        tNozePt[2] = new Point(Number(tRcNoveBLBR[0]), Number(tRcNoveBLBR[2]));
        tNozePt[3] = new Point(Number(tRcNoveBLBR[1]), Number(tRcNoveBLBR[3]));
        this.tFaces[idxFace] = new IG_Face(IGWS_DEEPZOOM_DIV, tFacesPt, tEyeLeftPt, tEyeRightPt, tMouthPt, tNozePt, IG_tFaceDescColors[idxFace], faceRot);
    }

    this.mouseOver = function (divCurrentWorkspace, divDeepZoomPanel, ctx, curPx, event) {
        for (var idxFace = 0; idxFace < this.nbFaces; idxFace++) {
            if (this.tFaces[idxFace].mouseOver(divCurrentWorkspace, divDeepZoomPanel, ctx, curPx, event)) {
                if (this.tFaces[idxFace].isOverFace || this.tFaces[idxFace].isOverEyeLeft || this.tFaces[idxFace].isOverEyeRight
                        || this.tFaces[idxFace].isOverMouth || this.tFaces[idxFace].isOverNoze) {
                    for (var idxOtherFaces = 0; idxOtherFaces < this.nbFaces; idxOtherFaces++) {
                        if (idxOtherFaces != idxFace) {
                            this.tFaces[idxOtherFaces].isOverFace = false;
                            this.tFaces[idxOtherFaces].isOverEyeLeft = false;
                            this.tFaces[idxOtherFaces].isOverEyeRight = false;
                            this.tFaces[idxOtherFaces].isOverMouth = false;
                            this.tFaces[idxOtherFaces].isOverNoze = false;
                        }
                    }
                }
                this.paint(divCurrentWorkspace, divDeepZoomPanel, ctx, event);
                break;
            }
        }
    }

    this.mouseDrag = function (divCurrentWorkspace, divDeepZoomPanel, ctx, startPx, curPx, event) {
        for (var idxFace = 0; idxFace < this.nbFaces; idxFace++) {
            if (this.tFaces[idxFace].mouseDrag(divCurrentWorkspace, divDeepZoomPanel, ctx, startPx, curPx, event))
                return true;
        }
        return false;
    }

    this.mouseUp = function (divCurrentWorkspace, startPx, curPx, event) {
        for (var idxFace = 0; idxFace < this.nbFaces; idxFace++) {
            if (this.tFaces[idxFace].mouseUp(divCurrentWorkspace, startPx, curPx, event))
                return true;
        }
        return false;
    }

    this.mouseClick = function (divCurrentWorkspace, curPx, event) {
        var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
        for (var idxFace = 0; idxFace < this.nbFaces; idxFace++) {            
            switch (this.tFaces[idxFace].mouseClick(divCurrentWorkspace, curPx, event)) {
                case IGFACEDESCRIPTOR_BUTTON_DELETE:
                    this.nbFaces--;
                    var tNewFaces = {};
                    for (var idxNewFace = 0; idxNewFace < this.nbFaces; idxNewFace++) {
                        if (idxNewFace < idxFace) {
                            tNewFaces[idxNewFace] = this.tFaces[idxNewFace];
                        }
                        else {
                            tNewFaces[idxNewFace] = this.tFaces[idxNewFace + 1];
                        }
                    }
                    this.tFaces = tNewFaces;
                    if (this.nbFaces == 0)
                        IG_internalSetCurrentPictureProperty("FaceDescriptor", "NoValue");
                    divDeepZoomPanel.Viewer.viewport.zoomBy(0.75, divDeepZoomPanel.Viewer.viewport.pointFromPixel(curPx));
                    return true;
                case IGFACEDESCRIPTOR_BUTTON_DUPLICATE:
                    if (this.nbFaces < IGFACEDESCRIPTOR_MAXNBFACES) {
                        this.tFaces[this.nbFaces++] = this.tFaces[idxFace].Clone();
                        this.tFaces[this.nbFaces - 1].color = IG_tFaceDescColors[this.nbFaces - 1];
                        this.tFaces[this.nbFaces - 1].tStartDragFaceCenterPx = IG_internalGetSquareCenter(this.tFaces[this.nbFaces - 1].tFacePx);
                        this.tFaces[this.nbFaces - 1].drag(new Point(Math.min(this.tFaces[this.nbFaces - 1].tFacePx[0].x, Math.min(this.tFaces[this.nbFaces - 1].tFacePx[1].x, this.tFaces[this.nbFaces - 1].tFacePx[2].x))
                                                                - Math.max(this.tFaces[this.nbFaces - 1].tFacePx[0].x, Math.max(this.tFaces[this.nbFaces - 1].tFacePx[1].x, this.tFaces[this.nbFaces - 1].tFacePx[2].x)), 0));
                        this.tFaces[this.nbFaces - 1].tStartDragFaceCenterPx = null;
                        this.tFaces[this.nbFaces - 1].updatePt();
                    }
                    divDeepZoomPanel.Viewer.viewport.zoomBy(0.75, divDeepZoomPanel.Viewer.viewport.pointFromPixel(curPx));
                    return true;
                case IGFACEDESCRIPTOR_BUTTON_ZOOMIN:                    
                    divDeepZoomPanel.Viewer.viewport.zoomBy(1.11, divDeepZoomPanel.Viewer.viewport.pointFromPixel(curPx));
                    return true;
                case IGFACEDESCRIPTOR_BUTTON_ZOOMOUT:
                    divDeepZoomPanel.Viewer.viewport.zoomBy(0.9, divDeepZoomPanel.Viewer.viewport.pointFromPixel(curPx));
                    return true;
            }
        }
        return false;
    }

    this.paint = function (divCurrentWorkspace, divDeepZoomPanel, ctx, event) {
        this.update(divDeepZoomPanel);
        var minX = 1000000;
        var minY = 1000000;
        var maxX = 0;
        var maxY = 0;
        for (var idxFace = 0; idxFace < this.nbFaces; idxFace++) {
            if (this.tFaces[idxFace].tFacePx == null)
                return;
            if (this.tFaces[idxFace].tFacePx[3] == null)
                return;
            minX = Math.min(minX, Math.min(Math.min(this.tFaces[idxFace].tFacePx[0].x, this.tFaces[idxFace].tFacePx[1].x), Math.min(this.tFaces[idxFace].tFacePx[2].x, this.tFaces[idxFace].tFacePx[3].x)));
            minY = Math.min(minY, Math.min(Math.min(this.tFaces[idxFace].tFacePx[0].y, this.tFaces[idxFace].tFacePx[1].y), Math.min(this.tFaces[idxFace].tFacePx[2].y, this.tFaces[idxFace].tFacePx[3].y)));
            maxX = Math.max(maxX, Math.max(Math.max(this.tFaces[idxFace].tFacePx[0].x, this.tFaces[idxFace].tFacePx[1].x), Math.max(this.tFaces[idxFace].tFacePx[2].x, this.tFaces[idxFace].tFacePx[3].x)));
            maxY = Math.max(maxY, Math.max(Math.max(this.tFaces[idxFace].tFacePx[0].y, this.tFaces[idxFace].tFacePx[1].y), Math.max(this.tFaces[idxFace].tFacePx[2].y, this.tFaces[idxFace].tFacePx[3].y)));
        }
        ctx.clearRect(minX - 10, minY - 10, maxX - minX + 20, maxY - minY + 20);
        var allHidden = true;
        for (var idxFace = 0; idxFace < this.nbFaces; idxFace++) {
            if (this.tFaces[idxFace].paint(divCurrentWorkspace, divDeepZoomPanel, ctx, event))
                allHidden = false;
        }
        if (allHidden) {
            var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
            divDeepZoomPanel.Viewer.viewport.zoomBy(0.75, new Point(0.5, 0.5));
        }
    }

    this.forceRefresh = function () {
        var currentToolId = IGWS_WORKSPACE_DIV.getAttribute("currentToolSelected");
        if (currentToolId == IGTOOL_FACEDESCRIPTOR) {
            var canvas = IG_internalGetElementById("IG_overCanvas");
            var ctx = canvas.getContext("2d");
            ctx.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
            this.paint(IGWS_WORKSPACE_DIV, IGWS_DEEPZOOM_DIV, ctx);
        }
    }

    this.update = function (divDeepZoomPanel) {
        if (divDeepZoomPanel) {
            if (divDeepZoomPanel.Viewer) {
                for (var idxFace = 0; idxFace < this.nbFaces; idxFace++)
                    this.tFaces[idxFace].updatePx(divDeepZoomPanel);
            }
        }
    }

    this.scale = function (scaleX, scaleY) {
        for (var idxFace = 0; idxFace < this.nbFaces; idxFace++)
            this.tFaces[idxFace].scale(scaleX, scaleY);
    }
}

function IG_internalUpdateWorkspacePictureMenu(newMenu)
{
    var ulWorkspacePictures = IG_internalGetWorkspaceMenuList();
    IG_internalCleanNode(ulWorkspacePictures);
    for(var idxPicture = 0; idxPicture < newMenu.children.length; idxPicture++)
        ulWorkspacePictures.appendChild(newMenu.children[idxPicture].cloneNode(true));
    IG_internalUpdateMenuList();
}

function IG_internalGotoHistoryId(nHistoryIdx)
{
    var jsonData = { "HistoryId": nHistoryIdx };
    IG_publicAPI("GotoHistory", jsonData);
}

function IG_internalGotoHistory(jsonData, successCallback, errorCallback) {
    if (jsonData == null)
        jsonData = {};
    var frameId = IG_internalGetCurrentFrameId();
    if (frameId != null) {
        IG_internalAPI("GotoHistory", jsonData,
            function (data) {
                IG_internalSetCurrentPictureProperty("HistoryStep", jsonData.HistoryId);
                if (successCallback)
                    successCallback(data);
            }, errorCallback
        );
    }
    else if (errorCallback)
        errorCallback("Error", "Frame id is missing");        
}

function IG_internalSelectHistory(nHistorySelectedPos)
{        
    // build the history list
	var IG_ulHistoryManager = IG_internalGetElementById ("IG_ulHistoryManager");
	IG_internalCleanNode(IG_ulHistoryManager);
	var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
	var currentPictureHistory = divCurrentWorkspace.getAttribute ("currentPictureHistory");
    var reg = new RegExp("[_]+", "g"); 
    var regStep = new RegExp("[%]+", "g"); 
    var tCurrentPictureHistory = currentPictureHistory.split(reg);
	for (var idxHistory = tCurrentPictureHistory.length - 1; idxHistory >= 0; idxHistory--){
	    var tCurStep = tCurrentPictureHistory[idxHistory].split(regStep);
	    var curStepType = tCurStep[0];
	    if (tCurStep[1] || IG_tHISTORY[curStepType]) {
	        var curStepName = (tCurStep[1] ? tCurStep[1] : IG_tHISTORY[curStepType].text);
	        var newLi = document.createElement("li");
	        newLi.setAttribute("id", "history" + idxHistory.toString());
	        newLi.setAttribute("onclick", "IG_internalGotoHistoryId(Number(this.id.substr(7)))");
	        var newImgHistory = document.createElement("img");
	        newImgHistory.setAttribute("style", "margin-left: 5px; vertical-align: middle");
	        newImgHistory.setAttribute("alt", "h");
	        newImgHistory.setAttribute("src", IG_tHISTORY[curStepType].picture);
	        var newTextHistory = document.createElement("a");
	        newTextHistory.setAttribute("style", "margin-left: 20px; font-size:x-small; font-family:Verdana; color: #000000; cursor: pointer;");
	        newTextHistory.setAttribute("title", "Click to go to that step.");
	        newTextHistory.appendChild(document.createTextNode(curStepName));
	        if (idxHistory < nHistorySelectedPos) {
	            newLi.setAttribute("style", "width: 140px; height: 15px; background-color: #d3d3d3; list-style-type: none; margin: 0px 0px 0px 0px; padding: 0px 0px 0px 0px; font-size:x-small; font-family:Verdana; cursor: hand");
	        }
	        else if (idxHistory == nHistorySelectedPos) {
	            newLi.setAttribute("style", "width: 140px; height: 15px; background-color: #88afcb; border-color:#3494d6; border-width:1px; border-style:solid; list-style-type: none; margin: 0px 0px 0px 0px; padding: 0px 0px 0px 0px; font-size:x-small; font-family:Verdana; cursor: default");
	        }
	        else {
	            newLi.setAttribute("style", "width: 140px; height: 15px; background-color: #c8898b; border-color:#d63434; border-width:1px; border-style:solid; list-style-type: none; margin: 0px 0px 0px 0px; padding: 0px 0px 0px 0px; font-size:x-small; font-family:Verdana; cursor: hand");
	        }
	        newLi.appendChild(newImgHistory);
	        newLi.appendChild(newTextHistory);
	        IG_ulHistoryManager.appendChild(newLi);
	    }
	}
}

function IG_internalSelectLayer(nLayerSelectedPos, event, norequest)
{
    // get the current number of layers and layer visibility
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    var idxOldCurrentWorkingLayer = Number(divCurrentWorkspace.getAttribute("currentWorkingLayer"));
    var bShiftModifier = false;
    if (event){
        if (event.modifiers) {
            bShiftModifier = event.modifiers & Event.SHIFT_MASK;
        } 
        else {
            bShiftModifier = event.shiftKey;
        }
    }
    var reg = new RegExp("[,]+", "g"); 
    var currentLayersSelected = divCurrentWorkspace.getAttribute ("currentLayersSelected");
    var bOnlyOneLayerSelected = false;
    if (bShiftModifier && currentLayersSelected){
        var tLayersSelected = currentLayersSelected.split(reg);
        var bLayerSelected = false;
        var idxLayerSelected = 0;
	    for(; idxLayerSelected < tLayersSelected.length; idxLayerSelected++){
	        if (tLayersSelected[idxLayerSelected] == nLayerSelectedPos){
	            bLayerSelected = true;
	            break;
	        }
	    }
	    if (bLayerSelected){
	        // remove layer from selection
	        currentLayersSelected = "";
	        bOnlyOneLayerSelected = true;
	        for(var idxNewLayerSelected = 0; idxNewLayerSelected < tLayersSelected.length; idxNewLayerSelected++){
	            if (idxNewLayerSelected != idxLayerSelected){
	                if (currentLayersSelected != ""){
	                    currentLayersSelected += ",";	 
	                    bOnlyOneLayerSelected = false;
	                }               
	                currentLayersSelected += tLayersSelected[idxNewLayerSelected];
	            }
	        }
	        if (currentLayersSelected == "")    // layer selection cannot be empty
	            currentLayersSelected = nLayerSelectedPos.toString(); 
	    }
	    else{
	        // add layer to selection
	        currentLayersSelected += "," + nLayerSelectedPos.toString();
	    }        
    }
    else{
        currentLayersSelected = nLayerSelectedPos.toString();  
        bOnlyOneLayerSelected = true;      
    }
    if (bOnlyOneLayerSelected)
        divCurrentWorkspace.setAttribute ("currentWorkingLayer", currentLayersSelected);
    var idxCurrentWorkingLayer = Number(divCurrentWorkspace.getAttribute ("currentWorkingLayer"));
    divCurrentWorkspace.setAttribute ("currentLayersSelected", currentLayersSelected);
    var tCurrentLayersSelected = currentLayersSelected.split(reg);
    
    var currentPictureNbLayers = Number(divCurrentWorkspace.getAttribute ("currentPictureNbLayers"));
    var currentPictureLayerVisibility = divCurrentWorkspace.getAttribute ("currentPictureLayerVisibility");
    reg = new RegExp("[_]+", "g"); 
    var tCurrentPictureLayerVisibility = currentPictureLayerVisibility.split(reg);
        
    // get the current frame id and req id
	var regReq = new RegExp("(/DZ)+", "g");
    var tFrameIdReqGuid = IG_internalGetCurrentConcatFrameIdReqGuid().split(regReq); 
    var currentUrlFrameOutput = IG_internalGetUrlDomainOutput() + tFrameIdReqGuid[0] + "/";
    var currentReq = (tFrameIdReqGuid[1] == "/DZ" ? tFrameIdReqGuid[2] : tFrameIdReqGuid[1]);   // IE8/Chrome compatibility
    
    // build the layer list
	var IG_ulLayerManager = IG_internalGetElementById ("IG_ulLayerManager");
	IG_internalCleanNode (IG_ulLayerManager); 
	for (var idxLayer = currentPictureNbLayers - 1; idxLayer >= 0; idxLayer--){
	    var newLi = document.createElement ("li");	  
	    newLi.setAttribute ("id", "layer" + idxLayer.toString());
        newLi.setAttribute ("onclick", "IG_internalSelectLayer(Number(this.id.substr(5)), event)");
        var newInputVisible = document.createElement ("input");	
        newInputVisible.setAttribute ("type", "image");
        newInputVisible.setAttribute("onclick", "IG_internalSetLayerVisible(Number(this.parentNode.id.substr(5)), (this.src.substring(this.src.length - 13) == \"Invisible.png\"))");
        var newImgMini = document.createElement ("input");	
        var bSelected = false;
	    for(var idxLayerSelected = 0; idxLayerSelected < tCurrentLayersSelected.length; idxLayerSelected++){
	        if (tCurrentLayersSelected[idxLayerSelected] == idxLayer){
	            bSelected = true;
	            break;
	        }
	    }	    
        if (bSelected){
	        // selected layer
	        newLi.setAttribute ("style", "width: 140px; background-color: #81b9d7; border-color:#3494d6; border-width:1px; border-style:solid; list-style-type: none; margin-left: 0px; margin-top: 5px; margin-bottom: 5px; padding: 0px 0px 0px 0px; cursor: default");
	        newInputVisible.setAttribute ("style", "vertical-align: middle; margin-left: 10px; cursor: hand");
            if (tCurrentPictureLayerVisibility[idxLayer] == "1"){ 
                newInputVisible.setAttribute ("title", "Layer is visible. Click to hide.");        
                newInputVisible.setAttribute ("src", "images/Visible.png");        
            }
            else{
                newInputVisible.setAttribute ("title", "Layer is not visible. Click to show.");
                newInputVisible.setAttribute ("src", "images/Invisible.png");
            }
            newLi.appendChild (newInputVisible);
            if (idxLayer == idxCurrentWorkingLayer){
                var newImgTool = document.createElement ("img");	
                newImgTool.setAttribute ("src", "images/Tool.png");
                newImgTool.setAttribute ("style", "margin-left: 10px; vertical-align: middle");
                newImgTool.setAttribute ("alt", "layer");
                newLi.appendChild (newImgTool); 
                newImgMini.setAttribute ("style", "border-width: 1px; border-color: Black; border-style:solid; margin-left: 15px; margin-top: 5px; margin-bottom: 5px; vertical-align: middle");
            }
            else{
                newImgMini.setAttribute ("style", "border-width: 1px; border-color: Black; border-style:solid; margin-left: 40px; margin-top: 5px; margin-bottom: 5px; vertical-align: middle");
            }
            newImgMini.setAttribute ("onclick", "IG_internalSelectLayer(Number(this.parentNode.id.substr(5)), event)");
            newImgMini.setAttribute ("title", "Select this layer");
            newImgMini.setAttribute ("type", "image");
            newImgMini.setAttribute ("src", currentUrlFrameOutput + "L" + idxLayer.toString() + "-" + currentReq + ".jpg");
	    }
	    else{
	        // other layers
	        newLi.setAttribute ("style", "width: 140px; background-color: Transparent; list-style-type: none; margin-left: 0px; margin-top: 5px; margin-bottom: 5px; padding: 0px 0px 0px 0px; cursor: hand");
            newInputVisible.setAttribute ("style", "vertical-align: middle; margin-left: 10px");
            if (tCurrentPictureLayerVisibility[idxLayer] == "1"){
                newInputVisible.setAttribute ("title", "Layer is visible. Click to hide.");        
                newInputVisible.setAttribute ("src", "images/Visible.png");        
            }
            else{
                newInputVisible.setAttribute ("title", "Layer is not visible. Click to show.");
                newInputVisible.setAttribute ("src", "images/Invisible.png");
            }
            newLi.appendChild (newInputVisible);
            if (idxLayer == idxCurrentWorkingLayer){
                var newImgTool = document.createElement ("img");	
                newImgTool.setAttribute ("src", "images/Tool.png");
                newImgTool.setAttribute ("style", "margin-left: 10px; vertical-align: middle");
                newImgTool.setAttribute ("alt", "layer");
                newLi.appendChild (newImgTool); 
                newImgMini.setAttribute ("style", "border-width: 1px; border-color: Black; border-style:solid; margin-left: 15px; margin-top: 5px; margin-bottom: 5px; vertical-align: middle");
            }
            else{
                newImgMini.setAttribute ("style", "border-width: 1px; border-color: Black; border-style:solid; margin-left: 40px; margin-top: 5px; margin-bottom: 5px; vertical-align: middle");
            }
            newImgMini.setAttribute ("onclick", "IG_internalSelectLayer(Number(this.parentNode.id.substr(5)), event)");
            newImgMini.setAttribute ("title", "Select this layer");
            newImgMini.setAttribute ("type", "image");            
            newImgMini.setAttribute ("src", currentUrlFrameOutput + "L" + idxLayer.toString() + "-" + currentReq + ".jpg");
        }        
        newLi.appendChild (newImgMini);
        IG_ulLayerManager.appendChild (newLi);
    }

    if (bOnlyOneLayerSelected && idxOldCurrentWorkingLayer != nLayerSelectedPos && norequest == null) {
        var jsonData = { "FrameId": tFrameIdReqGuid[0], "LayerId": nLayerSelectedPos};
        IG_publicAPI("SelectLayer", jsonData);
    }
}

function IG_internalSelectWorkspacePicture(sConcatFrameIdReqGuid, sPictureName, bForceRefresh)
{
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    var currentWorkspaceDZ = IG_internalGetUrlDomainOutput() + sConcatFrameIdReqGuid + ".xml"; 
    var lastWorkspaceDZ = divCurrentWorkspace.getAttribute ("currentDZpath");
    if ((sConcatFrameIdReqGuid == null) || ((currentWorkspaceDZ == lastWorkspaceDZ) && !bForceRefresh))
        return; // if the frame has not changed, just leave
    if (currentWorkspaceDZ != null && sPictureName != null) {
        divCurrentWorkspace.setAttribute("currentDZpath", currentWorkspaceDZ);
        divCurrentWorkspace.setAttribute("currentPicName", sPictureName);
        if (divCurrentWorkspace.getAttribute("currentView") == "Workspace") {
            var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
            if (divDeepZoomPanel != null) {
                if (divDeepZoomPanel.Viewer != null) {
                    if (divDeepZoomPanel.Viewer.isOpen()) {
                        IG_internalGetSeadragon().dzOpen();
                        IG_internalSelectCurrentTool();
                    }
                }
            }
        }
    }
	
	// get the current picture index
	var currentPictureIndex = null;
	var ulWorkspacePictures = IG_internalGetWorkspaceMenuList();
	for(var idxPicture = 0; idxPicture < ulWorkspacePictures.children.length; idxPicture++)
    {
        if (ulWorkspacePictures.children[idxPicture].getAttribute("title") == sConcatFrameIdReqGuid){
            currentPictureIndex = idxPicture;
            break;
        }
    }	    
    if (currentPictureIndex == null)
        return;
    divCurrentWorkspace.setAttribute("currentPictureIndex", currentPictureIndex.toString());
    
    // get the current picture number of layers
    var currentNbLayers = divCurrentWorkspace.getAttribute ("currentNbLayers");
    var reg = new RegExp("[,]+", "g");
    var tNbLayers = currentNbLayers.split(reg);
    var currentPictureNbLayers = tNbLayers[currentPictureIndex]; 
    divCurrentWorkspace.setAttribute ("currentPictureNbLayers", currentPictureNbLayers);
    
    // get the current picture layer visibility
    var currentLayerVisibility = divCurrentWorkspace.getAttribute ("currentLayerVisibility");
    var tLayerVisibility = currentLayerVisibility.split(reg);
    var currentPictureLayerVisibility = tLayerVisibility[currentPictureIndex];
    divCurrentWorkspace.setAttribute ("currentPictureLayerVisibility", currentPictureLayerVisibility);
    
    IG_internalSelectLayer(0, null, true);
    
    // get the current picture history
    var currentHistory = divCurrentWorkspace.getAttribute ("currentHistory");
    var tHistory = currentHistory.split(reg);
    var currentPictureHistory = tHistory[currentPictureIndex];
    divCurrentWorkspace.setAttribute("currentPictureHistory", currentPictureHistory);

    // get the current picture properties
    var currentProperties = divCurrentWorkspace.getAttribute("currentAllFrameProperties");
    if (currentProperties)
        IG_internalUpdateAllFrameProperties(currentProperties);

    var historyStep = IG_internalGetCurrentPictureProperty("HistoryStep");
    if (historyStep) {
        var curIdxHistoryStep = Number(historyStep);
        IG_internalSelectHistory (curIdxHistoryStep);
    }
    else {
        var tCurrentPictureHistory = currentPictureHistory.split(new RegExp("[_]+", "g"));
        IG_internalSetCurrentPictureProperty("HistoryStep", tCurrentPictureHistory.length - 1);
        IG_internalSelectHistory (tCurrentPictureHistory.length - 1);
    }
}

function IG_internalOnPictureMenuClick(sender, event)
{
    IG_internalSelectWorkspacePicture(sender.getAttribute("title"), sender.innerHTML, true);
}

function IG_internalAddWorkspacePicture(sPictureId, reqGuid, sPictureProperties, sPictureName)
{
    var ulWorkspacePictures = IG_internalGetWorkspaceMenuList();
    
    // if the frame exists, update the reqGuid
    for(var idxPicture = 0; idxPicture < ulWorkspacePictures.children.length; idxPicture++)
    {
        if (ulWorkspacePictures.children[idxPicture].children[0].innerHTML == sPictureName)
        {
            var cloneUlWorkspacePictures = ulWorkspacePictures.cloneNode(true);
            cloneUlWorkspacePictures.children[idxPicture].setAttribute("title", sPictureId + "/DZ" + reqGuid);
            IG_internalUpdateWorkspacePictureMenu(cloneUlWorkspacePictures);
            return;
        }
    }
    
    var newLi = document.createElement ("li");	
    newLi.setAttribute("class", "cssMenui");
    newLi.setAttribute("title", sPictureId + "/DZ" + reqGuid);   
    newLi.setAttribute ("onclick", "IG_internalOnPictureMenuClick(this, event);");
     
    var newA = document.createElement ("a");	
    newA.setAttribute("class", "cssMenui");
    newA.href="#";
    newA.title="Open picture " + sPictureName;
    newA.appendChild(document.createTextNode (sPictureName));
    newLi.appendChild(newA);
    ulWorkspacePictures.appendChild(newLi);
    IG_internalUpdateWorkspacePictureMenu(ulWorkspacePictures.cloneNode(true));

    if (sPictureProperties) {
        // Update the frame properties
        var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
        var allFrameProperties = divCurrentWorkspace.getAttribute("currentAllFrameProperties");
        if (allFrameProperties != "")
            allFrameProperties += ",";
        allFrameProperties += sPictureProperties;
        divCurrentWorkspace.setAttribute("currentAllFrameProperties", allFrameProperties);
    }
}

function IG_internalAddWorkspacePictures (data)
{
    var ulWorkspacePictures = IG_internalGetWorkspaceMenuList();
    IG_internalCleanNode (ulWorkspacePictures);
    var reg = new RegExp("[,]+", "g");
    var tFrameIds = data.PictureIds.split(reg);
    var tFrameNames = data.PictureNames.split(reg);
    var tFrameLastReqGuids = data.LastPictureRequestGuids.split(reg);
    var tFrameProperties = data.FrameProperties.split(reg);
    
    for (var idxFrame = 0; idxFrame < tFrameNames.length; idxFrame++)
        IG_internalAddWorkspacePicture(tFrameIds[idxFrame], tFrameLastReqGuids[idxFrame], tFrameProperties[idxFrame], tFrameNames[idxFrame]);
}

function IG_internalUpdateFrameHistory(history)
{
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    divCurrentWorkspace.setAttribute("currentHistory", history);
}

function IG_internalUpdateAllFrameLayersAndHistory (nbLayers, layerVisibility, history)
{
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    divCurrentWorkspace.setAttribute("currentNbLayers", nbLayers);
    divCurrentWorkspace.setAttribute("currentLayerVisibility", layerVisibility);
    IG_internalUpdateFrameHistory(history);
}

var jsColText1 = null;
var jsColText2 = null;

function IG_internalUpdateColorSelector() {
    var txt1 = IG_internalGetElementById("IG_Texture1");
    var txt2 = IG_internalGetElementById("IG_Texture2");
    if (txt1 && txt2) {
        jsColText1 = new jscolor.color(txt1, { pickerClosable: true });
        jsColText2 = new jscolor.color(txt2, { pickerClosable: true });
        var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
        var color1 = divCurrentWorkspace.getAttribute("currentWorkspaceProperty_Texture1");
        var color2 = divCurrentWorkspace.getAttribute("currentWorkspaceProperty_Texture2");
        if (color1 != null && color2 != null) {
            var selectTexture = divCurrentWorkspace.getAttribute("currentWorkspaceProperty_SelectTexture") == "Texture1";
            if (selectTexture) {
                txt1.value = IG_internalFromColorNumberToAN(color1);
                txt2.value = IG_internalFromColorNumberToAN(color2);
            }
            else {
                txt1.value = IG_internalFromColorNumberToAN(color2);
                txt2.value = IG_internalFromColorNumberToAN(color1);
            }
            txt1.setAttribute("style", "width: 18px; background-Color: #" + txt1.value + "; color: #" + txt1.value);
            txt2.setAttribute("style", "width: 18px; background-Color: #" + txt2.value + "; color: #" + txt2.value);
        }
    }
}

function IG_internalUpdateWorkspaceProperties(properties) {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    divCurrentWorkspace.setAttribute("currentWorkspaceProperties", properties);
    var reg = new RegExp("[_]+", "g");
    var tCurrentWorkspaceProperties = properties.split(reg);
    for (var idxProp = 0; idxProp < tCurrentWorkspaceProperties.length; idxProp += 2)
        divCurrentWorkspace.setAttribute("currentWorkspaceProperty_" + tCurrentWorkspaceProperties[idxProp], tCurrentWorkspaceProperties[idxProp + 1]);
    IG_internalUpdateColorSelector();
}

function IG_internalUpdateAllFrameProperties(properties) {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    divCurrentWorkspace.setAttribute("currentAllFrameProperties", properties);
    // update current picture properties
    var currentPictureIndex = Number(divCurrentWorkspace.getAttribute("currentPictureIndex"));
    var regPic = new RegExp("[,]+", "g");
    var tProps = properties.split(regPic);
    if (currentPictureIndex >= tProps.length) {
        currentPictureIndex = tProps.length - 1;
        divCurrentWorkspace.setAttribute("currentPictureIndex", currentPictureIndex);
    }
    var curPicProps = tProps[currentPictureIndex];
    divCurrentWorkspace.setAttribute("currentPictureProperties", curPicProps);
    var reg = new RegExp("[_]+", "g");
    var tCurrentPictureProperties = curPicProps.split(reg);
    for (var idxFrameProp = 0; idxFrameProp < tCurrentPictureProperties.length; idxFrameProp += 2)
        IG_internalSetCurrentPictureProperty(tCurrentPictureProperties[idxFrameProp], tCurrentPictureProperties[idxFrameProp + 1]);
}

function IG_internalUpdateProperties(workspaceProps, frameProps) {
    IG_internalUpdateWorkspaceProperties(workspaceProps);
    IG_internalUpdateAllFrameProperties(frameProps);
}

function IG_internalShowWorkspace_callback(){
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    divCurrentWorkspace.setAttribute ("currentView", "Workspace");
    divCurrentWorkspace.setAttribute ("refreshSeadragon", "1");
    
    var IG_WorkspaceMenu = IG_internalGetElementById ("IG_WorkspaceMenu");
    var IG_divCurrentMenu = IG_internalGetElementById("IG_divCurrentMenu");
    if (IG_divCurrentMenu) {
        IG_internalCleanNode(IG_divCurrentMenu);
        var currentMenu = IG_WorkspaceMenu.cloneNode(true);
        currentMenu.id = "IG_currentMenu";
        IG_divCurrentMenu.appendChild(currentMenu);
    }
    IG.showImageLibrary(false);
    divCurrentWorkspace.style.display = "inherit";

    IG_internalUpdateColorSelector();
    IG_internalOnResize();
    IG_internalGetSeadragon().dzOpen();
}

function IG_internalShowWorkspace(data) {
    var ulWorkspacePictures = IG_internalGetWorkspaceMenuList();
    if ((data == null) && (ulWorkspacePictures.children.length == 0))
    {
        IG_internalShowAccount();
        IG_internalAlertClient("Your workspace is empty. Double click on a picture to open it, or click on \"Open\" in the \"Pictures\" menu to open the selected picture.");
        return;
    }

    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;         
    var divDeepZoomPanel = IGWS_DEEPZOOM_DIV; 
    if (data != null)
    {
        var IG_divViewports = IG_internalGetElementById ("IG_divViewports");
        IG_internalCleanNode(IG_divViewports);

        var reg = new RegExp("[,]+", "g");
        var picNames = data.PictureNames.split(reg);
        if (data.PictureNames != "")
        {
            // Workspace pictures
            IG_internalAddWorkspacePictures (data);
            
            // Picture viewports
            for (var idxPicture = 0; idxPicture < data.PictureViewportList.length; idxPicture++) {
                if (picNames[idxPicture] == null || data.PictureViewportList[idxPicture] == null)
                    break;
                var divViewport = document.createElement ("div");
                divViewport.setAttribute("id", "divViewport" + picNames[idxPicture]);
                divViewport.setAttribute ("centerx", data.PictureViewportList[idxPicture].X);
                divViewport.setAttribute ("centery", data.PictureViewportList[idxPicture].Y);
                divViewport.setAttribute("zoom", data.PictureViewportList[idxPicture].Z);
                IG_divViewports.appendChild (divViewport);
            }
        
            // Open the selected picture
            var tFrameIds = data.PictureIds.split(reg);
            var tFrameNames = data.PictureNames.split(reg);
            var selectedPicName = "";
            for (var idxPicture = 0; idxPicture < tFrameIds.length; idxPicture++)
            {
                if (tFrameIds[idxPicture] == data.SelectedPictureId)
                {
                    selectedPicName = tFrameNames[idxPicture];
                    break;
                }
            }   
            // Update layer and history managers
            IG_internalUpdateAllFrameLayersAndHistory(data.NbLayers, data.LayerVisibility, data.History);

            // Update Properties
            IG_internalUpdateProperties(data.WorkspaceProperties, data.FrameProperties);

            // Update current picture path
            if (data.SelectedPictureId != null && selectedPicName != null && data.SelectedLastPictureRequestGuid != null) {
                if (data.SelectedPictureId != "" && selectedPicName != "" && data.SelectedLastPictureRequestGuid != "")
                    IG_internalSelectWorkspacePicture(data.SelectedPictureId + "/DZ" + data.SelectedLastPictureRequestGuid, selectedPicName, false);
            }
        }
        if (data.CurrentView == "Account")
            IG_internalShowAccount();
        else
            IG_internalShowWorkspace_callback();
    }
    else{
        var jsonData = { "CurrentView" : "Workspace" };
        IG_publicAPI("SetCurrentView", jsonData);     
    }
}

function IG_internalOnPictureClosed(nbLayers, layerVisibility, history)
{
    var ulWorkspacePictures = IG_internalGetWorkspaceMenuList();
    var cloneUlWorkspacePictures = ulWorkspacePictures.cloneNode(false);
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    var currentDZPath = divCurrentWorkspace.getAttribute("currentDZpath");
    var strHeader = IG_internalGetUrlDomainOutput();
    var curConcatFrameIdReqGuid = currentDZPath.substring (strHeader.length, currentDZPath.lastIndexOf("."));
    var otherConcatFrameIdReqGuid = null;
    var otherConcatPictureName = null;
    for(var idxPicture = 0; idxPicture < ulWorkspacePictures.children.length; idxPicture++)
    {
        var concatFrameIdReqGuid = ulWorkspacePictures.children[idxPicture].getAttribute("title");
        if (concatFrameIdReqGuid != curConcatFrameIdReqGuid)
        {
            otherConcatFrameIdReqGuid = concatFrameIdReqGuid;
            otherConcatPictureName = ulWorkspacePictures.children[idxPicture].children[0].innerHTML;
            cloneUlWorkspacePictures.appendChild (ulWorkspacePictures.children[idxPicture].cloneNode(true));
        }
    }
    // update picture menu
    IG_internalUpdateWorkspacePictureMenu(cloneUlWorkspacePictures);
    // update layer manager and history
    IG_internalUpdateAllFrameLayersAndHistory(nbLayers, layerVisibility, history);
    IG_internalShowAccount();    
    
    if (otherConcatFrameIdReqGuid)
    {
        IG_internalShowWorkspace(null);
        IG_internalSelectWorkspacePicture(otherConcatFrameIdReqGuid, otherConcatPictureName, false);
    }
}

function IG_internalShowRightPanel(sender)
{
    var iframeOverCanvas = IG_internalGetElementById ("iframeOverCanvas");
    var iframeDeepZoom = IG_internalGetElementById ("iframeDeepZoom");
    var reg = new RegExp("(%|px)+", "g");
    var nWidth = Number ((iframeOverCanvas.getAttribute ("width").split(reg))[0]);
    var panelHistory = IG_internalGetElementById ("panelHistory");
    var panelLayer = IG_internalGetElementById ("panelLayer");
    if (panelLayer.style.display == "none"){
        panelLayer.style.display = ""; 
        panelHistory.style.display = ""; 
        sender.src = "images/pinDown.png";
        iframeOverCanvas.width = (nWidth - 100).toString();
        iframeDeepZoom.width = iframeOverCanvas.width;
    }
    else{
        panelLayer.style.display = "none";
        panelHistory.style.display = "none";
        sender.src = "images/pinUp.png";
        iframeOverCanvas.width = (nWidth + 100).toString();
        iframeDeepZoom.width = iframeOverCanvas.width;
    }
}

function IG_internalPopInUpperLayer() {
    var overLayer = IG_internalGetUpperLayer();
    if (overLayer){
        var IG_divOverCanvasBackup = IG_internalGetElementById("IG_divOverCanvasBackup");
        var IG_divUpperlayer = IG_internalGetElementById ("IG_divUpperlayer");
        if (IG_divUpperlayer){
            var canvas = IG_internalGetElementById("IG_overCanvas");
            var ctx = canvas.getContext("2d");
            ctx.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
            IG_divUpperlayer.removeChild (canvas);
            IG_divOverCanvasBackup.appendChild (canvas); 
            overLayer.popIn();
        }
    }
}

function IG_internalGetToolGroupIdx(toolGroupId) {
    var toolBox = IG_internalGetToolBox();
    for (var idxToolGroup = 0; idxToolGroup < toolBox.length; idxToolGroup++) {
        if (toolBox[idxToolGroup].id == toolGroupId)
            return idxToolGroup;
    }
}

function IG_internalPopOutUpperLayer()
{
    var tParams = {};
    tParams[0] = function (event) {   // MOUSE MOVE
        var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
        var currentToolGroupId = divCurrentWorkspace.getAttribute("currentToolGroup");
        var curPt = IG_internalGetCurrentPoint(event);
        if (curPt == null)
            return;
        var mouseX = IG_internalGetElementById("IG_mouseX");
        var mouseY = IG_internalGetElementById("IG_mouseY");
        IG_internalCleanNode(mouseX);
        IG_internalCleanNode(mouseY);
        var width = IG_internalGetCurrentPictureProperty("Width");
        var height = IG_internalGetCurrentPictureProperty("Height");
        mouseX.appendChild(document.createTextNode("X:" + (Math.round(curPt.x * width / 1000000)).toString()));
        mouseY.appendChild(document.createTextNode(" Y:" + (Math.round(curPt.y * height / 1000000)).toString()));
        var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
        var idxToolGroup = IG_internalGetToolGroupIdx(currentToolGroupId);
        var toolBox = IG_internalGetToolBox();
        if (divDeepZoomPanel.getAttribute("ismousedown") == "1" &&
            toolBox[idxToolGroup].mouseDragFunc) { // DRAG  
            divDeepZoomPanel.setAttribute("isdragging", "1");
            var pixel = divDeepZoomPanel.Utils.getMousePosition(event).minus
                        (divDeepZoomPanel.Utils.getElementPosition(divDeepZoomPanel.Viewer.elmt));
            var canvas = IG_internalGetElementById("IG_overCanvas");
            var ctx = canvas.getContext("2d");
            ctx.strokeStyle = "rgba(0, 0, 200, 0.2)";
            ctx.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
            var currentToolSelectedId = divCurrentWorkspace.getAttribute("currentToolSelected");
            var pixelStartX = parseInt(divDeepZoomPanel.getAttribute("mousedownX"));
            var pixelStartY = parseInt(divDeepZoomPanel.getAttribute("mousedownY"));

            IG_tLASSO[IG_tLASSO.length] = curPt.x; IG_tLASSO[IG_tLASSO.length] = curPt.y;
            IG_tLASSO_Visual[IG_tLASSO_Visual.length] = pixel.x; IG_tLASSO_Visual[IG_tLASSO_Visual.length] = pixel.y;
            toolBox[idxToolGroup].mouseDragFunc(toolBox[idxToolGroup], currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, pixelStartX, pixelStartY, IG_tLASSO_Visual, pixel, event);
        }
        else if (toolBox[idxToolGroup].mouseOverFunc) {  // MOUSE OVER
            var pixel = divDeepZoomPanel.Utils.getMousePosition(event).minus
                        (divDeepZoomPanel.Utils.getElementPosition(divDeepZoomPanel.Viewer.elmt));
            var canvas = IG_internalGetElementById("IG_overCanvas");
            var ctx = canvas.getContext("2d");
            var currentToolSelectedId = divCurrentWorkspace.getAttribute("currentToolSelected");
            toolBox[idxToolGroup].mouseOverFunc(toolBox[idxToolGroup], currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, pixel, event);
        }            
    }
    tParams[1] = function (event) {   // MOUSE DOWN
        var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
        var pixel = divDeepZoomPanel.Utils.getMousePosition(event).minus
            (divDeepZoomPanel.Utils.getElementPosition(divDeepZoomPanel.Viewer.elmt));
        divDeepZoomPanel.setAttribute("isdragging", "0");
        if (divDeepZoomPanel.getAttribute("ismousedown") == "0") {
            divDeepZoomPanel.setAttribute("ismousedown", "1");
            divDeepZoomPanel.setAttribute("mousedownX", pixel.x.toString());
            divDeepZoomPanel.setAttribute("mousedownY", pixel.y.toString());
            var curPt = IG_internalGetCurrentPoint(event);
            if (curPt == null)
                return;
            divDeepZoomPanel.setAttribute("mousedownPointX", curPt.x.toString());
            divDeepZoomPanel.setAttribute("mousedownPointY", curPt.y.toString());            
            IG_tLASSO = [];
            IG_tLASSO[0] = curPt.x; IG_tLASSO[1] = curPt.y;
            IG_tLASSO_Visual = [];
            var canvas = IG_internalGetElementById("IG_overCanvas");
            var ctx = canvas.getContext("2d");
            ctx.strokeStyle = "rgba(0, 0, 200, 0.2)";
            ctx.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
            var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
            var currentToolSelectedId = divCurrentWorkspace.getAttribute("currentToolSelected");
            var currentToolGroupId = divCurrentWorkspace.getAttribute("currentToolGroup");
            var idxToolGroup = IG_internalGetToolGroupIdx(currentToolGroupId);
            var toolBox = IG_internalGetToolBox();
            if (toolBox[idxToolGroup].mouseDownFunc)
                toolBox[idxToolGroup].mouseDownFunc(toolBox[idxToolGroup], currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, curPt, event);
        }
    }
    tParams[2] = function (event) {   // MOUSE UP
        var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
        var isClick = (divDeepZoomPanel.getAttribute("ismousedown") == "1");
        if (divDeepZoomPanel.getAttribute("isdragging") == "1")
            isClick = false;
        divDeepZoomPanel.setAttribute("isdragging", "0");
        divDeepZoomPanel.setAttribute("ismousedown", "0");
        var curPt = IG_internalGetCurrentPoint(event);
        if (curPt == null)
            return;
        var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
        var currentToolGroupId = divCurrentWorkspace.getAttribute("currentToolGroup");
        var currentToolSelectedId = divCurrentWorkspace.getAttribute("currentToolSelected");
        var pixelStartX = divDeepZoomPanel.getAttribute("mousedownPointX");
        var pixelStartY = divDeepZoomPanel.getAttribute("mousedownPointY");
        var idxToolGroup = IG_internalGetToolGroupIdx(currentToolGroupId);
        var toolBox = IG_internalGetToolBox();
        if (toolBox[idxToolGroup].mouseUpFunc(toolBox[idxToolGroup], currentToolSelectedId, pixelStartX, pixelStartY, IG_tLASSO, curPt, event)) {
            IG_internalPopInUpperLayer();
            var canvas = IG_internalGetElementById("IG_overCanvas");
            var ctx = canvas.getContext("2d");
            ctx.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
        }
        if (isClick && toolBox[idxToolGroup].mouseClickFunc) {   // MOUSE CLICK
            var pixel = divDeepZoomPanel.Utils.getMousePosition(event).minus
                        (divDeepZoomPanel.Utils.getElementPosition(divDeepZoomPanel.Viewer.elmt));
            toolBox[idxToolGroup].mouseClickFunc(toolBox[idxToolGroup], currentToolSelectedId, divDeepZoomPanel, pixelStartX, pixelStartY, IG_tLASSO, curPt, pixel, event);
        }
    }
    tParams[3] = function (event) {   // MOUSE SCROLL
        var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
        divDeepZoomPanel.setAttribute("ismousedown", "0");
        var pixel = divDeepZoomPanel.Utils.getMousePosition(event).minus
                        (divDeepZoomPanel.Utils.getElementPosition(divDeepZoomPanel.Viewer.elmt));
        var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
        var currentToolGroupId = divCurrentWorkspace.getAttribute("currentToolGroup");
        var currentToolSelectedId = divCurrentWorkspace.getAttribute("currentToolSelected");
        var canvas = IG_internalGetElementById("IG_overCanvas");
        var ctx = canvas.getContext("2d");
        ctx.strokeStyle = "rgba(0, 0, 200, 0.2)";
        ctx.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
        var idxToolGroup = IG_internalGetToolGroupIdx(currentToolGroupId);
        var toolBox = IG_internalGetToolBox();
        if (toolBox[idxToolGroup].mouseScrollFunc)
            toolBox[idxToolGroup].mouseScrollFunc(toolBox[idxToolGroup], currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, pixel, event);
    }
    tParams[4] = function (event) {   // Select
        var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
        if (divDeepZoomPanel) {
            if (divDeepZoomPanel.Viewer) {
                var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
                var currentToolGroupId = divCurrentWorkspace.getAttribute("currentToolGroup");
                var currentToolSelectedId = divCurrentWorkspace.getAttribute("currentToolSelected");
                var idxToolGroup = IG_internalGetToolGroupIdx(currentToolGroupId);
                var toolBox = IG_internalGetToolBox();
                var canvas = IG_internalGetElementById("IG_overCanvas");
                if (canvas && toolBox) {
                    var ctx = canvas.getContext("2d");
                    ctx.strokeStyle = "rgba(0, 0, 200, 0.2)";
                    ctx.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
                    if (toolBox[idxToolGroup]) {
                        if (toolBox[idxToolGroup].selectFunc)
                            toolBox[idxToolGroup].selectFunc(toolBox[idxToolGroup], currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, event);
                    }
                }
            }
        }
    }
    var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
    if (divDeepZoomPanel) {
        if (divDeepZoomPanel.Viewer) {
            divDeepZoomPanel.Viewer.addEventListener("animationfinish", function (event) {   // End of DZ animation
                var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
                var currentToolGroupId = divCurrentWorkspace.getAttribute("currentToolGroup");
                var idxToolGroup = IG_internalGetToolGroupIdx(currentToolGroupId);
                var toolBox = IG_internalGetToolBox();
                if (toolBox[idxToolGroup].mousePaintFunc) { // PAINT  
                    var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
                    var canvas = IG_internalGetElementById("IG_overCanvas");
                    var ctx = canvas.getContext("2d");
                    ctx.strokeStyle = "rgba(0, 0, 200, 0.2)";
                    ctx.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
                    var currentToolSelectedId = divCurrentWorkspace.getAttribute("currentToolSelected");
                    toolBox[idxToolGroup].mousePaintFunc(toolBox[idxToolGroup], currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, event);
                }
            });
        }
    }
    var overLayer = IG_internalGetUpperLayer();
    var divOverLayer = overLayer.popOut(tParams);
    var IG_divOverCanvasBackup = IG_internalGetElementById("IG_divOverCanvasBackup");
    if (IG_divOverCanvasBackup) {
        var canvas = IG_internalGetElementById("IG_overCanvas");
        IG_divOverCanvasBackup.removeChild(canvas);
        divOverLayer.appendChild(canvas);
    }
}

function IG_internalAppendOption(option, divOptions, idxToolGroup, idxToolTabOptions, idxToolOption) {
    switch (option.type) {
        case IGMODALTYPE_TEXTBOX:
            var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
            var newLabel = document.createElement("a");
            newLabel.id = "inputOptionLabel" + "_" + idxToolTabOptions.toString() + "_" + idxToolOption.toString();
            newLabel.appendChild(document.createTextNode(option.label + ": "));
            var newTextBox = document.createElement("input");
            newTextBox.id = "inputOptionTextBox_" + idxToolGroup.toString() + "_" + idxToolTabOptions.toString() + "_" + idxToolOption.toString();
            var curVal = option.prop ? Math.round(divCurrentWorkspace.getAttribute("currentWorkspaceProperty_" + option.prop)) : null;
            if (curVal == null) {
                curVal = option.value;
                divCurrentWorkspace.setAttribute("currentWorkspaceProperty_" + option.prop, curVal);
            }
            else {
                var toolBox = IG_internalGetToolBox();
                if (idxToolTabOptions == 0) {
                    // group option                    
                    toolBox[idxToolGroup].commonOptions[idxToolOption].value = curVal;
                }
                else {
                    // specific tool option
                    toolBox[idxToolGroup].options[idxToolTabOptions - 1].array[idxToolOption].value = curVal;
                }
            }
            newTextBox.value = curVal;
            newTextBox.style.width = option.size;
            newTextBox.onchange = function () {
                var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
                var tOptionIds = this.id.substring(19).split(new RegExp("[_]+", "g"));
                var toolBox = IG_internalGetToolBox();
                if (tOptionIds[1] == 0) {
                    // group option                    
                    toolBox[tOptionIds[0]].commonOptions[tOptionIds[2]].value = this.value;
                    if (toolBox[tOptionIds[0]].commonOptions[tOptionIds[2]].prop)
                        divCurrentWorkspace.setAttribute("currentWorkspaceProperty_" + toolBox[tOptionIds[0]].commonOptions[tOptionIds[2]].prop, this.value);
                }
                else {
                    // specific tool option
                    toolBox[tOptionIds[0]].options[tOptionIds[1] - 1].array[tOptionIds[2]].value = this.value;
                    if (toolBox[tOptionIds[0]].options[tOptionIds[1] - 1].array[tOptionIds[2]].prop)
                        divCurrentWorkspace.setAttribute("currentWorkspaceProperty_" + toolBox[tOptionIds[0]].options[tOptionIds[1] - 1].array[tOptionIds[2]].prop, this.value);
                }
            }
            divOptions.appendChild(newLabel);
            divOptions.appendChild(newTextBox);
            break;
        case IGMODALTYPE_COMBOBOX:
            var newLabel = document.createElement("a");
            newLabel.id = "inputOptionLabel" + "_" + idxToolTabOptions.toString() + "_" + idxToolOption.toString();
            newLabel.appendChild(document.createTextNode(option.label + ": "));
            var newSelect = document.createElement("select");
            newSelect.id = "selectOptionComboBox_" + idxToolGroup.toString() + "_" + idxToolTabOptions.toString() + "_" + idxToolOption.toString();
            newSelect.style.width = option.size;
            newSelect.onchange = function () {
                var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
                var tOptionIds = this.id.substring(21).split(new RegExp("[_]+", "g"));
                var toolBox = IG_internalGetToolBox();
                if (tOptionIds[1] == 0) {
                    // group option
                    toolBox[tOptionIds[0]].commonOptions[tOptionIds[2]].value = this.selectedIndex;
                }
                else {
                    // specific tool option
                    toolBox[tOptionIds[0]].options[tOptionIds[1] - 1].array[tOptionIds[2]].value = this.selectedIndex;
                }
            }
            for(var idxSel = 0; idxSel < option.array.length; idxSel++){
                var nextOption = document.createElement("option");
                nextOption.value = idxSel;
                nextOption.appendChild (document.createTextNode (option.array[idxSel]));
                newSelect.appendChild (nextOption);
            }
            newSelect.selectedIndex = option.value;
            divOptions.appendChild(newLabel);
            divOptions.appendChild(newSelect);
            break;
        case IGMODALTYPE_COLOR:
            var newLabel = document.createElement("a");
            newLabel.id = "inputOptionLabel" + "_" + idxToolTabOptions.toString() + "_" + idxToolOption.toString();
            newLabel.appendChild(document.createTextNode(option.label + ": "));
            var newDiv = document.createElement("div");
            var divCanvasColorSelector = IG_internalGetElementById("divCanvasColorSelector");
            IG_internalCopyNode(newDiv, divCanvasColorSelector);
            divOptions.appendChild(newLabel);
            divOptions.appendChild(newDiv);
            break;
    }
    var newSpace = document.createElement("a");
    newSpace.appendChild(document.createTextNode("  "));
    divOptions.appendChild(newSpace);
}

function IG_internalSelectTool(sender)
{
    if (!sender)
        sender = IG_internalGetElementById(IGTOOL_EDITOR_POINTER);
    if (!sender)
        return;
    var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
    if (divDeepZoomPanel)
        divDeepZoomPanel.setAttribute ("ismousedown", "0");
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    var bIsToolGroup = false;
    var toolSelected = sender;
    var toolBox = IG_internalGetToolBox();
    if (sender.id.substring (0, 11) == "IGTOOLGROUP"){
        bIsToolGroup = true;
        toolSelected = IG_internalGetElementById(toolBox[IG_internalFindToolGroupIndex(toolBox, sender.id)].current);
        if (sender.children.length > 0){
            if (sender.children[0].children.length > 0){
                if (sender.children[0].children[0].children.length > 0){
                    for (var idxTool = 0; idxTool < sender.children[0].children[0].children.length; idxTool++){
                        var curTool = sender.children[0].children[0].children[idxTool];
                        if ((curTool.clientLeft > 0) && 
                            (curTool.id != toolSelected.id)) {
                            bIsToolGroup = false;
                            toolSelected = curTool;
                            break;
                        }
                    }
                }
            }            
        }
    }
    var navControl = null;
    if (divDeepZoomPanel && divDeepZoomPanel.Viewer)
        navControl = divDeepZoomPanel.Viewer.getNavControl();
    if (sender.id == IGTOOL_EDITOR_POINTER) {
        if (navControl)
            navControl.style.display = "inherit";
        var mouseX = IG_internalGetElementById("IG_mouseX");
        var mouseY = IG_internalGetElementById("IG_mouseY");
        var width = IG_internalGetCurrentPictureProperty("Width");
        var height = IG_internalGetCurrentPictureProperty("Height");
        if (width != null && height != null) {
            IG_internalCleanNode(mouseX);
            IG_internalCleanNode(mouseY);
            mouseX.appendChild(document.createTextNode("W:" + width.toString()));
            mouseY.appendChild(document.createTextNode(" H:" + height.toString()));
        }
    }
    else if (navControl)  {
        navControl.style.display = "none";
    }

    var sToolGroup;
    if (bIsToolGroup){
        divCurrentWorkspace.setAttribute ("currentToolSelected", toolSelected.id);
        divCurrentWorkspace.setAttribute ("currentToolGroup", sender.id);
        sender.style.backgroundImage = "url('" + toolSelected.src + "')";
        sToolGroup = sender.id.substring(12);
    }
    else{        
        divCurrentWorkspace.setAttribute ("currentToolSelected", toolSelected.id);
        if (toolSelected.parentNode.parentNode.parentNode.id.substring (0, 11) == "IGTOOLGROUP")
            toolSelected.parentNode.parentNode.parentNode.style.backgroundImage = "url('" + toolSelected.src + "')";
        var reg = new RegExp("[_]+", "g");
        var tToolGroup = toolSelected.id.substring (6).split(reg);
        sToolGroup = (tToolGroup[0] == "" ? tToolGroup[1] : tToolGroup[0]); // IE8/Chrome compatibility
    }

    // manage tool options
    var IG_divIGTOOL_OPTIONS = IG_internalGetElementById("IG_divIGTOOL_OPTIONS");
    var divParentIGTOOL_OPTIONS = IG_divIGTOOL_OPTIONS.parentNode;
    IG_internalCleanNode(divParentIGTOOL_OPTIONS);
    IG_divIGTOOL_OPTIONS = document.createElement("div");
    IG_divIGTOOL_OPTIONS.id = "IG_divIGTOOL_OPTIONS";
    IG_divIGTOOL_OPTIONS.nbCommonOptions = 0;
    IG_divIGTOOL_OPTIONS.nbOptions = 0;    
    for (var idxToolGroup = 0; idxToolGroup < toolBox.length; idxToolGroup++) {
        var curToolGroup = toolBox[idxToolGroup];
        var curToolGroupHtmlElem = IG_internalGetElementById(curToolGroup.id);
        if (curToolGroupHtmlElem)
            curToolGroupHtmlElem.style.backgroundColor = "Transparent";
        for (var idxToolTabOptions = 0; idxToolTabOptions < curToolGroup.options.length; idxToolTabOptions++) {
            var curToolHtmlElem = IG_internalGetElementById(curToolGroup.options[idxToolTabOptions].id);
            if (curToolHtmlElem)
                curToolHtmlElem.style.backgroundColor = "Transparent";
        }
        if (curToolGroup.id.substring(12) == sToolGroup) {
            if (!bIsToolGroup) {
                curToolGroup.current = toolSelected.id;
                divCurrentWorkspace.setAttribute("currentToolGroup", curToolGroup.id);
            }
            // tool group options
            if (curToolGroup.commonOptions) {
                for (var idxToolGroupOption = 0; idxToolGroupOption < curToolGroup.commonOptions.length; idxToolGroupOption++) {
                    // tab 0 is reserved for group options  
                    IG_internalAppendOption(curToolGroup.commonOptions[idxToolGroupOption], IG_divIGTOOL_OPTIONS, idxToolGroup, 0, IG_divIGTOOL_OPTIONS.nbCommonOptions++);
                }
            }
            // specific tool options        
            for (var idxToolTabOptions = 0; idxToolTabOptions < curToolGroup.options.length; idxToolTabOptions++) {
                if (curToolGroup.options[idxToolTabOptions].id == toolSelected.id) {
                    var curToolTabOptions = curToolGroup.options[idxToolTabOptions];
                    if (curToolTabOptions.array) {
                        for (var idxToolOption = 0; idxToolOption < curToolTabOptions.array.length; idxToolOption++) {
                            IG_internalAppendOption(curToolTabOptions.array[idxToolOption], IG_divIGTOOL_OPTIONS, idxToolGroup, idxToolTabOptions + 1, IG_divIGTOOL_OPTIONS.nbOptions++);                        
                        }
                    }
                    break;
                }
            }
        }
    }
    divParentIGTOOL_OPTIONS.appendChild(IG_divIGTOOL_OPTIONS);
    
    // pop-in the canvas layer
    IG_internalPopInUpperLayer();

    // manage tool selection
    var currentToolGroupId = divCurrentWorkspace.getAttribute("currentToolGroup");
    var idxToolGroup = IG_internalGetToolGroupIdx(currentToolGroupId);
    if (toolSelected.id != IGTOOL_EDITOR_POINTER) {
        IG_internalPopOutUpperLayer();  // pop-out the canvas layer
        var canvas = IG_internalGetElementById("IG_overCanvas");
        var ctx = canvas.getContext("2d");
        ctx.clearRect(0, 0, canvas.clientWidth, canvas.clientHeight);
        if (toolSelected.id == IGTOOL_FACEDESCRIPTOR && divCurrentWorkspace.FaceDescriptor)
            divCurrentWorkspace.FaceDescriptor.paint(IGWS_WORKSPACE_DIV, IGWS_DEEPZOOM_DIV, ctx);
    }
    
    // highlight selected tool button
    toolSelected.style.backgroundColor = "#AAAAFF";
    // highlight selected tool group
    var toolGroup = IG_internalGetElementById(currentToolGroupId);
    if (toolGroup)
        toolGroup.style.backgroundColor = "#AAAAFF";
}

function IG_internalSelectCurrentTool() {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    var currentToolSelectedId = divCurrentWorkspace.getAttribute("currentToolSelected");
    var currentToolSelected = IG_internalGetElementById(currentToolSelectedId);
    IG_internalSelectTool(currentToolSelected); 
}

function IG_internalFindToolGroupIndex(toolBox, toolGroup) {
    for (var idxToolGroup = 0; idxToolGroup < toolBox.length; idxToolGroup++) {
        var curToolGroup = toolBox[idxToolGroup];
        if (curToolGroup.id == toolGroup)
            return idxToolGroup;
    }
    return -1;
}

function IG_internalCreateToolBox() {
    var IG_tableToolBox = IG_internalGetElementById("IG_tableToolBox");
    var toolBox = IG_internalGetToolBox();
    var nbToolBoxRows = toolBox.length / 2;
    var idxToolGroup = 0;
    for (var idxRow = 0; idxRow < nbToolBoxRows; idxRow++) {
        var newRow = document.createElement("tr");
        newRow.setAttribute("style", "height: 14px");
        var idxCol = 0;
        while ((idxCol++ < 2) && (idxToolGroup < toolBox.length)) {
            var newGroupToolTH = document.createElement("th");
            var newGroupToolDiv = document.createElement("div");
            newGroupToolDiv.setAttribute("id", "div" + toolBox[idxToolGroup].id);
            if (toolBox[idxToolGroup].options.length > 1) {  // Creation of ToolGroup
                var newGroupToolUL = document.createElement("ul");
                if (idxCol == 1)
                    newGroupToolUL.setAttribute("class", "cssMenu cssMenum0 cssMenuToolBox cssMenuToolBoxLeftColumn");
                else
                    newGroupToolUL.setAttribute("class", "cssMenu cssMenum0 cssMenuToolBox cssMenuToolBoxRightColumn");
                newGroupToolUL.setAttribute("id", toolBox[idxToolGroup].id);
                newGroupToolUL.setAttribute("onclick", "IG_internalSelectTool(this)");
                var newGroupToolLI = document.createElement("li");
                newGroupToolLI.setAttribute("class", "cssMenui0 cssMenuToolBox");
                var newGroupToolInputContainer = document.createElement("ul");
                var nHeight = (1 + toolBox[idxToolGroup].options.length / 2) * 20;
                newGroupToolInputContainer.setAttribute("style", "height: " + nHeight.toString() + "px;");
                var curToolGroupImage = null;
                for (var idxToolGroupElem = 0; idxToolGroupElem < toolBox[idxToolGroup].options.length; idxToolGroupElem++) {
                    if (toolBox[idxToolGroup].options[idxToolGroupElem].id == toolBox[idxToolGroup].current)
                        curToolGroupImage = toolBox[idxToolGroup].options[idxToolGroupElem].image;
                    var newToolInput = document.createElement("input");
                    newToolInput.setAttribute("class", "cssMenuToolBox");
                    newToolInput.setAttribute("id", toolBox[idxToolGroup].options[idxToolGroupElem].id);
                    newToolInput.setAttribute("type", "image");
                    newToolInput.setAttribute("onmousedown", "IG_internalSelectTool(this)");
                    newToolInput.setAttribute("src", "images/" + toolBox[idxToolGroup].options[idxToolGroupElem].image);
                    newGroupToolInputContainer.appendChild(newToolInput);
                }
                newGroupToolUL.setAttribute("style", "background-image:url('images/" + curToolGroupImage + "');");
                newGroupToolLI.appendChild(newGroupToolInputContainer);
                newGroupToolUL.appendChild(newGroupToolLI);
                newGroupToolDiv.appendChild(newGroupToolUL);
            }
            else {
                var newToolInput = document.createElement("input");
                newToolInput.setAttribute("class", "cssMenuToolBox");
                newToolInput.setAttribute("id", toolBox[idxToolGroup].current);
                newToolInput.setAttribute("type", "image");
                newToolInput.setAttribute("onclick", "IG_internalSelectTool(this)");
                newToolInput.setAttribute("src", "images/" + toolBox[idxToolGroup].options[0].image);
                newGroupToolDiv.appendChild(newToolInput);
            }
            newGroupToolTH.appendChild(newGroupToolDiv);
            newRow.appendChild(newGroupToolTH);
            idxToolGroup++;
        }
        IG_tableToolBox.appendChild(newRow);
    }
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    divCurrentWorkspace.setAttribute("currentToolSelected", "IGTOOL_FACEDESCRIPTOR");
}

function IG_internalFilterImage(jsonData, successCallback, errorCallback) {
    if (jsonData == null)
        jsonData = {};
    if (jsonData.LayerId == null)
        jsonData.LayerId = IG_internalGetCurrentWorkingLayer();
    if (jsonData.FilterId == null && jsonData.FaceEffectId == null) {
        jsonData.FilterId = IGIPFILTER_BLUR;
        jsonData.Params = "";
    }
    if (jsonData.Original == null)
        jsonData.Original = (IGWS_WORKSPACE_DIV.getAttribute("SaveHistory") != "1");
    if (jsonData.FaceEffectId != null) {
        if (jsonData.FilterId == null)
            jsonData.FilterId = IGIPFILTER_FACE_EFFECT;
        else if (jsonData.FilterId == "")
            jsonData.FilterId = IGIPFILTER_FACE_EFFECT;
        else if (jsonData.FilterId != IGIPFILTER_FACE_EFFECT) {
            if (errorCallback)
                errorCallback("filter error", "Parameters are incompatible");
            else
                IG_internalAlertClient("Parameters are incompatible");
            return;
        }
        if (jsonData.Params != null) {
            /*
            if (jsonData.Params.Param != null) {
                if (jsonData.Params.Param5 != null) {
                    if (errorCallback)
                        errorCallback("filter error", "Too many parameters");
                    else
                        IG_internalAlertClient("Too many parameters");
                    return;
                }
                if (jsonData.Params.Param4 != null)
                    jsonData.Params.Param5 = jsonData.Params.Param4;
                if (jsonData.Params.Param3 != null)
                    jsonData.Params.Param4 = jsonData.Params.Param3;
                if (jsonData.Params.Param2 != null)
                    jsonData.Params.Param3 = jsonData.Params.Param2;
                jsonData.Params.Param2 = jsonData.Params.Param;
                jsonData.Params.Param = jsonData.FaceEffectId;
            }
            else {
                jsonData.Params.Param = jsonData.FaceEffectId;
            }*/
            jsonData.Params.FaceEffectId = jsonData.FaceEffectId;
        }
        else {
            jsonData.Params = { "FaceEffectId": jsonData.FaceEffectId };
        }
    }
    if (jsonData.Params == null)
        jsonData.Params = "";
    else
        jsonData.Params = JSON.stringify(jsonData.Params).replace(new RegExp("[{}\"']", "g"), "").replace(new RegExp("[:]", "g"), "_");

    switch (jsonData.FilterId) {
        case IGIPFILTER_FACE_EFFECT:
            jsonData.Params += IG_internalGetFaceDescriptor();
            break;
        case IGIPFILTER_TRANSPARENCY:
            if (jsonData.Params == null || jsonData.Params == "") {
                IG_internalTransparency(successCallback, errorCallback);
                return;
            } break;
        case IGIPFILTER_BRIGHTNESS:
            if (jsonData.Params == null || jsonData.Params == "") {
                IG_internalBrightness(successCallback, errorCallback);
                return;
            } break;
        case IGIPFILTER_SATURATE:
            if (jsonData.Params == null || jsonData.Params == "") {
                IG_internalSaturate(successCallback, errorCallback);
                return;
            } break;
        case IGIPFILTER_CONTRAST:
            if (jsonData.Params == null || jsonData.Params == "") {
                IG_internalContrast(successCallback, errorCallback);
                return;
            } break;
    }
    IG_internalAPI("FilterImage", jsonData, successCallback, errorCallback);
    IG_internalSelectTool();
}

function IG_internalCloseAllImages(successCallback, errorCallback) {
    var ulWorkspacePictures = IG_internalGetWorkspaceMenuList();
    IG_internalCleanNode(ulWorkspacePictures);
    IG_internalUpdateMenuList();

    var jsonData = { "FrameId": "-1" };
    IG_internalAPI("CloseImage", jsonData,
        function (data) {
            IG_internalShowAccount();
            if (successCallback)
                successCallback(data);
        },
        function (status, jqXHR) {
            IG_internalShowAccount();
            if (errorCallback)
                errorCallback(status, jqXHR);
        }
    );
}

function IG_internalSaveAllImages(successCallback, errorCallback) {
    var ulWorkspacePictures = IG_internalGetWorkspaceMenuList();
    for (var idxPicture = 0; idxPicture < ulWorkspacePictures.children.length; idxPicture++) {
        var concatFrameIdReqGuid = ulWorkspacePictures.children[idxPicture].getAttribute("title");
        var reg = new RegExp("(/DZ)+", "g");
        var tFrameIdReqGuid = concatFrameIdReqGuid.split(reg);

        var jsonData = { "FrameId": tFrameIdReqGuid[0],
            "ImageName": ulWorkspacePictures.children[idxPicture].children[0].innerHTML
        };
        IG_internalAPI("SaveImage", jsonData, successCallback, errorCallback);
    }
}

function IG_internalChangeSelection(jsonData, successCallback, errorCallback) {
    if (jsonData == null)
        jsonData = {};
    if (jsonData.SelectionType == IGSELECTION_INVERT.toString()) {
        var reg = new RegExp("[,]", "g");
        var tRcSel = IG_internalGetCurrentPictureProperty("SelectionRect").split(reg);
        if (tRcSel[1] == -1 || tRcSel[3] == -1) {
            IG_internalAlertClient("You need to perform a selection first");
            return;
        }
    }
    IG_internalAPI("ChangeSelection", jsonData, successCallback, errorCallback);
}

function IG_internalCheckMoveLayerPositions(posLayer, up) {
    var destPos = up ? posLayer + 1 : posLayer - 1;
    var nbLayers = Number(IGWS_WORKSPACE_DIV.getAttribute("currentPictureNbLayers"));
    return (posLayer >= 0 && posLayer < nbLayers) &&
           (destPos >= 0 && destPos < nbLayers);
}

function IG_internalMoveLayerUp(jsonData, successCallback, errorCallback) {
    if (jsonData == null) {
        var sLayersSelected = IGWS_WORKSPACE_DIV.getAttribute("currentLayersSelected");
        var reg = new RegExp("[,]+", "g");
        var tLayersSelected = sLayersSelected.split(reg);
        var layerPosDest = (tLayersSelected[0] + 1).toString();
        jsonData = { "PosLayerFrom": tLayersSelected[0],
            "PosLayerDest": layerPosDest
        };
    }
    if (IG_internalCheckMoveLayerPositions(Number(jsonData.PosLayerFrom), true))
        IG_internalAPI("LayerMove", jsonData, successCallback, errorCallback);
    else if (errorCallback)
        errorCallback("Error", "Layer position error");
}

function IG_internalMoveLayerDown(jsonData, successCallback, errorCallback) {
    if (jsonData == null) {
        var sLayersSelected = IGWS_WORKSPACE_DIV.getAttribute("currentLayersSelected");
        var reg = new RegExp("[,]+", "g");
        var tLayersSelected = sLayersSelected.split(reg);
        var layerPosDest = (tLayersSelected[0] - 1).toString();
        var jsonData = { "PosLayerFrom": tLayersSelected[0],
            "PosLayerDest": layerPosDest
        };
    }
    if (IG_internalCheckMoveLayerPositions(Number(jsonData.PosLayerFrom), false))
        IG_internalAPI("LayerMove", jsonData, successCallback, errorCallback);
    else if (errorCallback)
        errorCallback("Error", "Layer position error");
}

function IG_internalMergeLayers(jsonData, successCallback, errorCallback) {
    var reg = new RegExp("[,]+", "g");
    var tLayersSelected = jsonData.PosLayers.split(reg);
    if ((tLayersSelected.length <= 1) || tLayersSelected.length >= 10) {
        IG_internalAlertClient("You must have between 2 and 10 layers to perform a merge");
        if (errorCallback)
            errorCallback("Error", "Layer number error");
    }
    IG_internalAPI("LayerMerge", jsonData, successCallback, errorCallback);
}

function IG_internalAddLayer() {
    IG_internalAPI("LayerAdd");
}

function IG_internalMergeLayers() {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    var sLayersSelected = divCurrentWorkspace.getAttribute("currentLayersSelected");
    var reg = new RegExp("[,]+", "g");
    var tLayersSelected = sLayersSelected.split(reg);
    if (tLayersSelected.length <= 1) {
        var currentPictureNbLayers = divCurrentWorkspace.getAttribute("currentPictureNbLayers");
        sLayersSelected = "";
        for (var idxLay = 0; idxLay < currentPictureNbLayers; idxLay++)
            sLayersSelected += idxLay + ",";
        sLayersSelected = sLayersSelected.substring(0, sLayersSelected.length - 1);
    }
    var jsonData = { "PosLayers": sLayersSelected };
    IG_internalAPI("LayerMerge", jsonData);
}

function IG_internalRemoveLayer() {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    var sLayersSelected = divCurrentWorkspace.getAttribute("currentLayersSelected");
    var reg = new RegExp("[,]+", "g");
    var tLayersSelected = sLayersSelected.split(reg);
    var jsonData = { "LayerId": tLayersSelected[0] };
    IG_internalAPI("LayerRemove", jsonData);
}

function IG_internalSetLayerVisible(nLayerSelectedPos, bVisible) {
    var jsonData = { "LayerId": nLayerSelectedPos.toString(),
        "Visible": bVisible ? "1" : "0"
    };
    IG_internalAPI("LayerSetVisible", jsonData);
}