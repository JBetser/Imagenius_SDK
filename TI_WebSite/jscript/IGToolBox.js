/*****************************************************************************\

Javascript "Imagenius Toolbox Definition" library
 
@version: 1.0 - 2012.05.15
@author: Jonathan Betser
\*****************************************************************************/

var IGTOOL_EDITOR_POINTER = 'IGTOOL_EDITOR_POINTER';
var IGTOOL_EDITOR_MOVER = 'IGTOOL_EDITOR_MOVER';
var IGTOOL_EDITOR_ROTATEANDRESIZE = 'IGTOOL_EDITOR_ROTATEANDRESIZE';
var IGTOOL_SELECTOR_LASSO = 'IGTOOL_SELECTOR_LASSO';
var IGTOOL_SELECTOR_SQUARE = 'IGTOOL_SELECTOR_SQUARE';
var IGTOOL_SELECTOR_MAGIC = 'IGTOOL_SELECTOR_MAGIC';
var IGTOOL_SELECTOR_LPE = 'IGTOOL_SELECTOR_LPE';
var IGTOOL_DRAW_PENCIL = 'IGTOOL_DRAW_PENCIL';
var IGTOOL_RUBBER = 'IGTOOL_RUBBER';
var IGTOOL_PICKCOLOR = 'IGTOOL_PICKCOLOR';
var IGTOOL_COLOR_PAINT = 'IGTOOL_COLOR_PAINT';
var IGTOOL_COLOR_GRADIENT = 'IGTOOL_COLOR_GRADIENT';
var IGTOOL_MORPHING = 'IGTOOL_MORPHING';
var IGTOOL_FACEDESCRIPTOR = 'IGTOOL_FACEDESCRIPTOR';

function IG_ToolBox() {
    this.tTOOLBOX = [{ 'id': 'IGTOOLGROUP_EDITOR', 'current': 'IGTOOL_EDITOR_POINTER', 'commonOptions': null,
        'options': [{ 'id': 'IGTOOL_EDITOR_POINTER', 'array': null, 'image': 'Pointer.png' },
                                { 'id': 'IGTOOL_EDITOR_MOVER', 'array': null, 'image': 'mover.png' },
                                { 'id': 'IGTOOL_EDITOR_ROTATEANDRESIZE', 'array': [{ 'type': IGMODALTYPE_COMBOBOX, 'label': 'Select Operation', 'value': 0, 'size': 120, 'array': ['rotate', 'resize', 'rotate&resize']}], 'image': 'rotateAndResize.png'}],
        'mouseUpFunc': IG_ToolBox.toolBoxMouseUp_mover,
        'mouseDragFunc': IG_ToolBox.toolBoxMouseDrag_mover
    },
    { 'id': 'IGTOOLGROUP_SELECTOR', 'current': 'IGTOOL_SELECTOR_LASSO',
        'commonOptions': [{ 'type': IGMODALTYPE_COMBOBOX, 'label': 'Select Operation', 'value': 0, 'size': 70, 'array': ['replace', 'add', 'remove']}],
        'options': [{ 'id': 'IGTOOL_SELECTOR_LASSO', 'array': null, 'image': 'StepChangeSelectionLasso2.png' },
                                                { 'id': 'IGTOOL_SELECTOR_SQUARE', 'array': null, 'image': 'StepChangeSelection2.png' },
                                                { 'id': 'IGTOOL_SELECTOR_MAGIC', 'array': [{ 'type': IGMODALTYPE_TEXTBOX, 'label': 'Tolerance', 'value': 20, 'size': 30, 'prop': null}], 'image': 'StepChangeSelectionMagic2.png' },
                                                { 'id': 'IGTOOL_SELECTOR_LPE', 'array': null, 'image': 'StepChangeSelectionLPE2.png'}],
        'mouseUpFunc': IG_ToolBox.toolBoxMouseUp_selection,
        'mouseDragFunc': IG_ToolBox.toolBoxMouseDrag_selection
    },
    { 'id': 'IGTOOLGROUP_DRAW', 'current': 'IGTOOL_DRAW_PENCIL',
        'commonOptions': [{ 'type': IGMODALTYPE_TEXTBOX, 'label': 'Brush size', 'value': 25, 'size': 30, 'prop': 'BrushSize'}],
        'options': [{ 'id': 'IGTOOL_DRAW_PENCIL', 'array': null, 'image': 'Pencil.png' },
                                                { 'id': 'IGTOOL_DRAW_BRUSH', 'array': null, 'image': 'brush.png'}],
        'mouseUpFunc': IG_ToolBox.toolBoxMouseUp_pencil,
        'mouseDragFunc': IG_ToolBox.toolBoxMouseDrag_pencil
    },
    { 'id': 'IGTOOLGROUP_RUBBER', 'current': 'IGTOOL_RUBBER', 'commonOptions': [{ 'type': IGMODALTYPE_TEXTBOX, 'label': 'Size', 'value': 25, 'size': 30, 'prop': 'BrushSize' },
                                                                                { 'type': IGMODALTYPE_TEXTBOX, 'label': 'Transparency', 'value': 0, 'size': 40}], 'options': [{ 'id': 'IGTOOL_RUBBER', 'array': null, 'image': 'rubber.png'}],
        'mouseUpFunc': IG_ToolBox.toolBoxMouseUp_rubber,
        'mouseDragFunc': IG_ToolBox.toolBoxMouseDrag_rubber
    },
    { 'id': 'IGTOOLGROUP_PICKCOLOR', 'current': 'IGTOOL_PICKCOLOR',
        'commonOptions': null, 'options': [{ 'id': 'IGTOOL_PICKCOLOR', 'array': null, 'image': 'pipette.png'}], // [{ 'type': IGMODALTYPE_COMBOBOX, 'label': 'Surface type', 'value': 0, 'size': 70, 'array': ['Plain', 'Texture']}], 'image': 'pipette.png'}],
        'mouseUpFunc': IG_ToolBox.toolBoxMouseUp_pickColor
    },
    { 'id': 'IGTOOLGROUP_COLOR', 'current': 'IGTOOL_COLOR_PAINT',
        'commonOptions': null, 'options': [{ 'id': 'IGTOOL_COLOR_PAINT', 'array': [{ 'type': IGMODALTYPE_COMBOBOX, 'label': 'Surface type', 'value': 0, 'size': 70, 'array': ['Plain', 'Texture'] },
                                                                                  { 'type': IGMODALTYPE_TEXTBOX, 'label': 'Tolerance', 'value': 20, 'size': 30, 'prop': null}], 'image': 'colorpaint.png'
        },
                                           { 'id': 'IGTOOL_COLOR_GRADIENT', 'array': [{ 'type': IGMODALTYPE_COMBOBOX, 'label': 'Gradient type', 'value': 0, 'size': 100, 'array': ['Directionnal', 'Radial']}], 'image': 'colorselector.png'}],
        'mouseUpFunc': IG_ToolBox.toolBoxMouseUp_color,
        'mouseDragFunc': IG_ToolBox.toolBoxMouseDrag_color
    },
    { 'id': 'IGTOOLGROUP_MORPHING', 'current': 'IGTOOL_MORPHING',
        'commonOptions': [{ 'type': IGMODALTYPE_COMBOBOX, 'label': 'Morphing Type', 'value': 2, 'size': 100, 'array': ['radial', 'directional', 'wide']}],
        'options': [{ 'id': 'IGTOOL_MORPHING', 'array': null, 'image': 'morphing.png'}],
        'mouseUpFunc': IG_ToolBox.toolBoxMouseUp_morphing,
        'mouseDragFunc': IG_ToolBox.toolBoxMouseDrag_morphing
    },
    { 'id': 'IGTOOLGROUP_FACEDESCRIPTOR', 'current': 'IGTOOL_FACEDESCRIPTOR',
        'commonOptions': null,
        'options': [{ 'id': 'IGTOOL_FACEDESCRIPTOR', 'array': null, 'image': 'FaceDesc.png'}],
        'mouseUpFunc': IG_ToolBox.toolBoxMouseUp_faceDesc,
        'mouseDownFunc': IG_ToolBox.toolBoxMouseDown_faceDesc,
        'mouseClickFunc': IG_ToolBox.toolBoxMouseClick_faceDesc,
        'mouseDragFunc': IG_ToolBox.toolBoxMouseDrag_faceDesc,
        'mouseOverFunc': IG_ToolBox.toolBoxMouseOver_faceDesc,
        'mouseScrollFunc': IG_ToolBox.toolBoxMouseScroll_faceDesc,
        'mousePaintFunc': IG_ToolBox.toolBoxMousePaint_faceDesc,
        'selectFunc': this.toolBoxSelect_faceDesc
    }];

    this.faceDescDragging = false;
}

IG_ToolBox.toolBoxMouseOver_faceDesc = function (toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, curPt, event) {
    if (divCurrentWorkspace.FaceDescriptor)
        divCurrentWorkspace.FaceDescriptor.mouseOver(divCurrentWorkspace, divDeepZoomPanel, ctx, curPt, event);
}

IG_ToolBox.toolBoxMousePaint_faceDesc = function (toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, event) {
    if (divCurrentWorkspace.FaceDescriptor)
        divCurrentWorkspace.FaceDescriptor.paint(divCurrentWorkspace, divDeepZoomPanel, ctx, event);
}

IG_ToolBox.toolBoxMouseDrag_selection = function (toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    if (currentToolSelectedId == "IGTOOL_SELECTOR_SQUARE") {
        ctx.fillStyle = "rgba(0, 0, 200, 0.5)";
        ctx.beginPath();
        ctx.fillRect(pixelStartX, pixelStartY,
                        curPt.x - pixelStartX, curPt.y - pixelStartY);
        ctx.closePath();
    }
    else if ((currentToolSelectedId == "IGTOOL_SELECTOR_LASSO") ||
                (currentToolSelectedId == "IGTOOL_SELECTOR_LPE")) {
        ctx.strokeStyle = "rgba(0, 0, 200, 0.5)";
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.moveTo(pixelStartX, pixelStartY);
        for (var idxCoord = 0; idxCoord < tMouseCoords.length; idxCoord++) {
            if (idxCoord % 2 == 1) {
                ctx.lineTo(tMouseCoords[idxCoord - 1], tMouseCoords[idxCoord]);
            }
        }
        ctx.stroke();
    }
}

IG_ToolBox.toolBoxMouseDrag_pencil = function (toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    var brushSize = toolGroup.commonOptions[0].value;
    var ptBrushSize = new Point((parseFloat(brushSize) + 5) * divDeepZoomPanel.Viewer.viewport.getZoom(), 0);
    ctx.lineWidth = ptBrushSize.x;
    var selectedTexture = (divCurrentWorkspace.getAttribute("currentWorkspaceProperty_SelectTexture") == "Texture1"
            ? "currentWorkspaceProperty_Texture1" : "currentWorkspaceProperty_Texture2");
    var color = divCurrentWorkspace.getAttribute(selectedTexture);
    ctx.strokeStyle = IG_internalFromColorNumberToRGB(color);
    ctx.beginPath();
    ctx.moveTo(pixelStartX, pixelStartY);
    for (var idxCoord = 0; idxCoord < tMouseCoords.length; idxCoord++) {
        if (idxCoord % 2 == 1) {
            ctx.lineTo(tMouseCoords[idxCoord - 1], tMouseCoords[idxCoord]);
        }
    }
    ctx.stroke();
}

IG_ToolBox.toolBoxMouseDrag_rubber = function (toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    var brushSize = toolGroup.commonOptions[0].value;
    var ptBrushSize = new Point((parseFloat(brushSize) + 5) * divDeepZoomPanel.Viewer.viewport.getZoom(), 0);
    ctx.lineWidth = ptBrushSize.x;
    ctx.strokeStyle = "rgba(255, 255, 255, 0.5)";
    ctx.beginPath();
    ctx.moveTo(pixelStartX, pixelStartY);
    for (var idxCoord = 0; idxCoord < tMouseCoords.length; idxCoord++) {
        if (idxCoord % 2 == 1) {
            ctx.lineTo(tMouseCoords[idxCoord - 1], tMouseCoords[idxCoord]);
        }
    }
    ctx.stroke();
}

IG_ToolBox.toolBoxMouseDrag_morphing = function (toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    ctx.lineWidth = 2;
    ctx.strokeStyle = "rgba(0, 0, 200, 0.5)";
    ctx.beginPath();
    ctx.moveTo(pixelStartX, pixelStartY);
    ctx.lineTo(curPt.x, curPt.y);
    ctx.stroke();
    var N = Math.sqrt(Math.pow(curPt.x - pixelStartX, 2) + Math.pow(curPt.y - pixelStartY, 2));
    var ux = (curPt.x - pixelStartX) / N;
    var uy = (curPt.y - pixelStartY) / N;
    ctx.beginPath();
    ctx.moveTo(curPt.x, curPt.y);
    ctx.lineTo(curPt.x - 15 * ux + 7 * uy, curPt.y - 15 * uy - 7 * ux);
    ctx.stroke();
    ctx.beginPath();
    ctx.moveTo(curPt.x, curPt.y);
    ctx.lineTo(curPt.x - 15 * ux - 7 * uy, curPt.y - 15 * uy + 7 * ux);
    ctx.stroke();
}

IG_ToolBox.toolBoxMouseDrag_faceDesc = function (toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    if (divCurrentWorkspace.FaceDescriptor) {
        if (divCurrentWorkspace.FaceDescriptor.mouseDrag(divCurrentWorkspace, divDeepZoomPanel, ctx, new Point(pixelStartX, pixelStartY), curPt, event)) {
            faceDescDragging = true;
        }
        else {
            var ptStart = divDeepZoomPanel.Viewer.viewport.pointFromPixel(new Point(pixelStartX, pixelStartY));
            divDeepZoomPanel.Viewer.viewport.panBy(ptStart.minus(divDeepZoomPanel.Viewer.viewport.pointFromPixel(curPt)));
            var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
            divDeepZoomPanel.setAttribute("mousedownX", curPt.x);
            divDeepZoomPanel.setAttribute("mousedownY", curPt.y);
        }
    }
}

IG_ToolBox.toolBoxMouseDown_faceDesc = function (toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, curPt, event) {
}

IG_ToolBox.toolBoxMouseScroll_faceDesc = function (toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, curPt, event) {
    IG_ToolBox.toolBoxMousePaint_faceDesc(toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, event);
    var zoomFct = (1.0 + Number(event.wheelDelta) / 1000);
    divDeepZoomPanel.Viewer.viewport.zoomBy(zoomFct, divDeepZoomPanel.Viewer.viewport.pointFromPixel(curPt));
}

IG_ToolBox.toolBoxSelect_faceDesc = function (toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, event) {
    IG_ToolBox.toolBoxMousePaint_faceDesc(toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, event);
    if (divDeepZoomPanel.Viewer.viewport != null)
        divDeepZoomPanel.Viewer.viewport.goHome();
}

IG_ToolBox.toolBoxMouseDrag_color = function (toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    if (currentToolSelectedId == "IGTOOL_COLOR_GRADIENT") {
        ctx.lineWidth = 2;
        ctx.strokeStyle = "rgba(0, 0, 200, 0.5)";
        ctx.beginPath();
        ctx.moveTo(pixelStartX, pixelStartY);
        ctx.lineTo(curPt.x, curPt.y);
        ctx.stroke();
    }
}

IG_ToolBox.toolBoxMouseDrag_mover = function (toolGroup, currentToolSelectedId, divCurrentWorkspace, divDeepZoomPanel, ctx, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    var reg = new RegExp("[,]", "g");
    var tRcSel = IG_internalGetCurrentPictureProperty("SelectionRectDZ").split(reg);
    var pxTopLeft = 0.0;
    var pxTopRight = 0.0;
    var pxBottomLeft = 0.0;
    var pxBottomRight = 0.0;
    if (tRcSel[1] == -1 || tRcSel[3] == -1) {
        var picWidth = Number(IG_internalGetCurrentPictureProperty("Width"));
        var picHeight = Number(IG_internalGetCurrentPictureProperty("Height"));
        pxTopLeft = divDeepZoomPanel.Viewer.viewport.pixelFromPoint(new Point(0.0, 0.0));
        pxTopRight = divDeepZoomPanel.Viewer.viewport.pixelFromPoint(new Point(Math.max(1.0, picWidth / picHeight), 0.0));
        pxBottomLeft = divDeepZoomPanel.Viewer.viewport.pixelFromPoint(new Point(0.0, Math.max(1.0, picHeight / picWidth)));
        pxBottomRight = divDeepZoomPanel.Viewer.viewport.pixelFromPoint(new Point(Math.max(1.0, picWidth / picHeight), Math.max(1.0, picHeight / picWidth)));
    }
    else {
        pxTopLeft = divDeepZoomPanel.Viewer.viewport.pixelFromPoint(new Point(Number(tRcSel[0]), Number(tRcSel[2])));
        pxTopRight = divDeepZoomPanel.Viewer.viewport.pixelFromPoint(new Point(Number(tRcSel[1]), Number(tRcSel[2])));
        pxBottomLeft = divDeepZoomPanel.Viewer.viewport.pixelFromPoint(new Point(Number(tRcSel[0]), Number(tRcSel[3])));
        pxBottomRight = divDeepZoomPanel.Viewer.viewport.pixelFromPoint(new Point(Number(tRcSel[1]), Number(tRcSel[3])));
    }
    var pxCenter = new Point((pxTopLeft.x + pxTopRight.x) / 2, (pxTopLeft.y + pxBottomLeft.y) / 2);
    var startVec = new Point(pixelStartX - pxCenter.x, pixelStartY - pxCenter.y);
    var endVec = new Point(curPt.x - pxCenter.x, curPt.y - pxCenter.y);
    ctx.lineWidth = 2;
    ctx.strokeStyle = "rgba(0, 0, 200, 1.0)";
    ctx.fillStyle = "rgba(0, 0, 200, 0.5)";
    if (currentToolSelectedId == "IGTOOL_EDITOR_MOVER") {
        if (tRcSel[1] != -1 && tRcSel[3] != -1) {
            pxTopLeft = pxTopLeft.plus(endVec);
            pxTopRight = pxTopRight.plus(endVec);
            pxBottomLeft = pxBottomLeft.plus(endVec);
            pxBottomRight = pxBottomRight.plus(endVec);
            ctx.beginPath();
            ctx.fillRect(pxTopLeft.x, pxTopLeft.y,
                        pxBottomRight.x - pxTopLeft.x, pxBottomRight.y - pxTopLeft.y);
            ctx.closePath();
            ctx.beginPath();
            ctx.moveTo(pxTopLeft.x, pxTopLeft.y);
            ctx.lineTo(pxTopRight.x, pxTopRight.y);
            ctx.lineTo(pxBottomRight.x, pxBottomRight.y);
            ctx.lineTo(pxBottomLeft.x, pxBottomLeft.y);
            ctx.lineTo(pxTopLeft.x, pxTopLeft.y);
            ctx.stroke();
            pixelStartX = pxCenter.x;
            pixelStartY = pxCenter.y;
        }
        ctx.beginPath();
        ctx.moveTo(pixelStartX, pixelStartY);
        ctx.lineTo(curPt.x, curPt.y);
        ctx.stroke();
        var N = Math.sqrt(Math.pow(curPt.x - pixelStartX, 2) + Math.pow(curPt.y - pixelStartY, 2));
        var ux = (curPt.x - pixelStartX) / N;
        var uy = (curPt.y - pixelStartY) / N;
        ctx.beginPath();
        ctx.moveTo(curPt.x, curPt.y);
        ctx.lineTo(curPt.x - 15 * ux + 7 * uy, curPt.y - 15 * uy - 7 * ux);
        ctx.stroke();
        ctx.beginPath();
        ctx.moveTo(curPt.x, curPt.y);
        ctx.lineTo(curPt.x - 15 * ux - 7 * uy, curPt.y - 15 * uy + 7 * ux);
        ctx.stroke();
    } else if (currentToolSelectedId == "IGTOOL_EDITOR_ROTATEANDRESIZE") {
        ctx.beginPath();
        ctx.moveTo(pxTopLeft.x, pxTopLeft.y);
        ctx.lineTo(pxTopRight.x, pxTopRight.y);
        ctx.lineTo(pxBottomRight.x, pxBottomRight.y);
        ctx.lineTo(pxBottomLeft.x, pxBottomLeft.y);
        ctx.lineTo(pxTopLeft.x, pxTopLeft.y);
        ctx.stroke();
        if (toolGroup.options[2].array[0].value == 1 || toolGroup.options[2].array[0].value == 2) {  // resize
            var offset = Math.max(Math.abs(endVec.x), Math.abs(endVec.y));
            pxTopLeft.x = pxCenter.x - offset;
            pxTopLeft.y = pxCenter.y - offset;
            pxTopRight.x = pxCenter.x + offset;
            pxTopRight.y = pxCenter.y - offset;
            pxBottomLeft.x = pxCenter.x - offset;
            pxBottomLeft.y = pxCenter.y + offset;
            pxBottomRight.x = pxCenter.x + offset;
            pxBottomRight.y = pxCenter.y + offset;
        }
        if (toolGroup.options[2].array[0].value == 0 || toolGroup.options[2].array[0].value == 2) {  // rotate
            ctx.beginPath();
            ctx.moveTo(pixelStartX, pixelStartY);
            ctx.lineTo(pxCenter.x, pxCenter.y);
            ctx.lineTo(curPt.x, curPt.y);
            ctx.stroke();
            startVec = startVec.divide(startVec.norm());
            endVec = endVec.divide(endVec.norm());
            var scalProd = endVec.x * startVec.x + endVec.y * startVec.y;
            var vectProd = endVec.x * startVec.y - endVec.y * startVec.x;
            var angle = Math.acos(scalProd);
            if (vectProd < 0)
                angle *= -1;
            pxTopLeft = pxCenter.plus(pxTopLeft.minus(pxCenter).rotate(angle));
            pxTopRight = pxCenter.plus(pxTopRight.minus(pxCenter).rotate(angle));
            pxBottomLeft = pxCenter.plus(pxBottomLeft.minus(pxCenter).rotate(angle));
            pxBottomRight = pxCenter.plus(pxBottomRight.minus(pxCenter).rotate(angle));
        }
        ctx.beginPath();
        ctx.moveTo(pxTopLeft.x, pxTopLeft.y);
        ctx.lineTo(pxTopRight.x, pxTopRight.y);
        ctx.lineTo(pxBottomRight.x, pxBottomRight.y);
        ctx.lineTo(pxBottomLeft.x, pxBottomLeft.y);
        ctx.lineTo(pxTopLeft.x, pxTopLeft.y);
        ctx.fill();
        ctx.beginPath();
        ctx.moveTo(pxTopLeft.x, pxTopLeft.y);
        ctx.lineTo(pxTopRight.x, pxTopRight.y);
        ctx.lineTo(pxBottomRight.x, pxBottomRight.y);
        ctx.lineTo(pxBottomLeft.x, pxBottomLeft.y);
        ctx.lineTo(pxTopLeft.x, pxTopLeft.y);
        ctx.stroke();
    }
}

IG_ToolBox.toolBoxMouseUp_selection = function (toolGroup, currentToolSelectedId, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    var selectOperation = IGSELECTION_REPLACE;
    if (toolGroup.commonOptions[0].value == 1)
        selectOperation = IGSELECTION_ADD;
    else if (toolGroup.commonOptions[0].value == 2)
        selectOperation = IGSELECTION_REMOVE;
    if (currentToolSelectedId == "IGTOOL_SELECTOR_SQUARE") {
        if ((curPt.x == pixelStartX) && (curPt.y == pixelStartY)) {
            if (selectOperation == IGSELECTION_REPLACE)
                IG_ChangeSelection(IGSELECTION_CLEAR, [0, 0, -1, -1]);
        }
        else {
            var tCoords = {};
            tCoords[0] = pixelStartX;
            tCoords[1] = pixelStartY;
            tCoords[2] = curPt.x.toString();
            tCoords[3] = curPt.y.toString();
            IG_ChangeSelection(IGSELECTION_SQUARE | selectOperation, tCoords);
        }
    } else if ((currentToolSelectedId == "IGTOOL_SELECTOR_LASSO") ||
                        (currentToolSelectedId == "IGTOOL_SELECTOR_LPE")) {
        if ((curPt.x == pixelStartX) && (curPt.y == pixelStartY) && (tMouseCoords.length < 3)) {  // min 3 pts for lasso
            if (selectOperation == IGSELECTION_REPLACE)
                IG_ChangeSelection(IGSELECTION_CLEAR, [0, 0, -1, -1]);
        }
        else {
            IG_ChangeSelection((currentToolSelectedId == "IGTOOL_SELECTOR_LASSO" ?
                                                IGSELECTION_LASSO :
                                                IGSELECTION_LPE) | selectOperation, tMouseCoords);
        }
    } else if (currentToolSelectedId == "IGTOOL_SELECTOR_MAGIC") {
        var tCoords = {};
        tCoords[0] = pixelStartX;
        tCoords[1] = pixelStartY;
        tCoords[2] = toolGroup.options[2].array[0].value; // tolerance
        IG_ChangeSelection(IGSELECTION_MAGIC | selectOperation, tCoords);
    }
    return true;
}

IG_ToolBox.toolBoxMouseUp_pencil = function (toolGroup, currentToolSelectedId, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    var sTabPoints = "";
    if (tMouseCoords) {
        var idxPt = 0;
        while (tMouseCoords[idxPt]) {
            sTabPoints = sTabPoints + tMouseCoords[idxPt++] + ",";
        }
        sTabPoints = sTabPoints.substring(0, sTabPoints.length - 1);
    }
    else {
        sTabPoints = "0,0";
    }
    var transp = 80;
    if (currentToolSelectedId == "IGTOOL_DRAW_PENCIL")
        transp = 255;
    var jsonData = { "TabPoints": sTabPoints,
        "Transparency": transp,
        "BrushSize": parseInt(toolGroup.commonOptions[0].value)
    };
    IG_publicAPI("DrawLines", jsonData);
    return true;
}

IG_ToolBox.toolBoxMouseUp_rubber = function (toolGroup, currentToolSelectedId, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    var sTabPoints = "";
    if (tMouseCoords) {
        var idxPt = 0;
        while (tMouseCoords[idxPt]) {
            sTabPoints = sTabPoints + tMouseCoords[idxPt++] + ",";
        }
        sTabPoints = sTabPoints.substring(0, sTabPoints.length - 1);
    }
    else {
        sTabPoints = "0,0";
    }
    var jsonData = { "TabPoints": sTabPoints,
        "Transparency": Math.floor(2.55 * (100 - parseInt(toolGroup.commonOptions[1].value, 10))),
        "BrushSize": parseInt(toolGroup.commonOptions[0].value)
    };
    IG_publicAPI("DrawLines", jsonData);
    return true;
}

IG_ToolBox.toolBoxMouseUp_mover = function (toolGroup, currentToolSelectedId, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    var reg = new RegExp("[,]", "g");
    var tRcSel = IG_internalGetCurrentPictureProperty("SelectionRect").split(reg);
    var ptCenter = new Point(0, 0);
    if (tRcSel[1] != -1 && tRcSel[3] != -1)
        ptCenter = new Point(Number(parseInt(tRcSel[0]) + parseInt(tRcSel[1])) / 2,
                                 Number(parseInt(tRcSel[2]) + parseInt(tRcSel[3])) / 2);
    if (currentToolSelectedId == "IGTOOL_EDITOR_MOVER") {
        var ptDirection = new Point(0, 0);
        if (tRcSel[1] == -1 || tRcSel[3] == -1)
            ptDirection = curPt.minus(new Point(parseInt(pixelStartX), parseInt(pixelStartY)));
        else
            ptDirection = curPt.minus(ptCenter);

        var jsonData = { "DirectionX": parseInt(ptDirection.x),
            "DirectionY": parseInt(ptDirection.y)
        };
        IG_publicAPI("MovePixels", jsonData);
    }
    else if (currentToolSelectedId == "IGTOOL_EDITOR_ROTATEANDRESIZE") {
        if (tRcSel[1] == -1 || tRcSel[3] == -1) {
            var picWidth = Number(IG_internalGetCurrentPictureProperty("Width"));
            var picHeight = Number(IG_internalGetCurrentPictureProperty("Height"));
            pxTopLeft = new Point(0.0, 0.0);
            pxTopRight = new Point(IG_PIXEL_SELECTOR_PRECISION * Math.min(1.0, picWidth / picHeight), 0.0);
            pxBottomLeft = new Point(0.0, IG_PIXEL_SELECTOR_PRECISION * Math.min(1.0, picHeight / picWidth));
            pxBottomRight = new Point(IG_PIXEL_SELECTOR_PRECISION * Math.min(1.0, picWidth / picHeight), IG_PIXEL_SELECTOR_PRECISION * Math.min(1.0, picHeight / picWidth));
            ptCenter = new Point((pxTopLeft.x + pxTopRight.x) / 2, (pxTopLeft.y + pxBottomLeft.y) / 2);
            rate = Math.round(2 * Math.max(Math.abs(curPt.x - ptCenter.x), Math.abs(curPt.y - ptCenter.y)));
        }
        else {
            var recWidth = Number(parseInt(tRcSel[1]) - parseInt(tRcSel[0]));
            var recHeight = Number(parseInt(tRcSel[3]) - parseInt(tRcSel[2]));
            rate = Math.round(2 * IG_PIXEL_SELECTOR_PRECISION * Math.max(Math.abs(curPt.x - ptCenter.x), Math.abs(curPt.y - ptCenter.y)) / Math.max(recWidth, recHeight));
        }
        var startVecX = Number(parseInt(pixelStartX) - ptCenter.x);
        var startVecY = Number(parseInt(pixelStartY) - ptCenter.y);
        var normStartVec = Math.sqrt(Math.pow(startVecX, 2) + Math.pow(startVecY, 2));
        startVecX /= normStartVec;
        startVecY /= normStartVec;
        var endVecX = Number(curPt.x - ptCenter.x);
        var endVecY = Number(curPt.y - ptCenter.y);
        var normEndVec = Math.sqrt(Math.pow(endVecX, 2) + Math.pow(endVecY, 2));
        endVecX /= normEndVec;
        endVecY /= normEndVec;
        var scalProd = endVecX * startVecX + endVecY * startVecY;
        var vectProd = endVecX * startVecY - endVecY * startVecX;
        var angle = Math.acos(scalProd);
        if (vectProd > 0)
            angle *= -1;
        angle = Math.round(angle * 180 / Math.PI);
        if (toolGroup.options[2].array[0].value == 0) {   // rotate
            rate = IG_PIXEL_SELECTOR_PRECISION;
        }
        else if (toolGroup.options[2].array[0].value == 1) {  // resize
            angle = 0;
        }
        var jsonData = { "Angle": angle,
            "Rate": rate
        };
        IG_publicAPI("RotateAndResize", jsonData);
    }
    return true;
}

IG_ToolBox.toolBoxMouseUp_pickColor = function (toolGroup, currentToolSelectedId, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    var jsonData = { "Coords": pixelStartX + "," + pixelStartY };
    IG_publicAPI("PickColor", jsonData);
    return true;
}

IG_ToolBox.toolBoxMouseUp_color = function (toolGroup, currentToolSelectedId, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    if (currentToolSelectedId == "IGTOOL_COLOR_GRADIENT") {
        var dirX = curPt.x - pixelStartX;
        var dirY = curPt.y - pixelStartY;
        var jsonData = { "FilterId": IGIPFILTER_PAINTGRADIENT,
            "Params": { "PosX": pixelStartX, "PosY": pixelStartY, "DirectionX": dirX, "DirectionY": dirY, "Type": toolGroup.options[1].array[0].value }
        };
        IG_publicAPI("FilterImage", jsonData);
    }
    else {
        var jsonData = { "FilterId": IGIPFILTER_PAINT,
            "Params": { "PosX": pixelStartX, "PosY": pixelStartY, "Type": toolGroup.options[0].array[0].value, "Tolerance": toolGroup.options[0].array[1].value }
        };
        IG_publicAPI("FilterImage", jsonData);
    }
    return true;
}

IG_ToolBox.toolBoxMouseUp_morphing = function (toolGroup, currentToolSelectedId, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    var dirX = curPt.x - pixelStartX;
    var dirY = curPt.y - pixelStartY;
    var jsonData = { "FilterId": IGIPFILTER_MORPHING,
        "Params": { "Strength": 0, "Type": toolGroup.commonOptions[0].value, "PosX": pixelStartX, "PosY": pixelStartY, "DirectionX": dirX, "DirectionY": dirY }
    };
    IG_publicAPI("FilterImage", jsonData);
    return true;
}

IG_ToolBox.toolBoxMouseUp_faceDesc = function (toolGroup, currentToolSelectedId, pixelStartX, pixelStartY, tMouseCoords, curPt, event) {
    faceDescDragging = false;
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    if (divCurrentWorkspace.FaceDescriptor)
        divCurrentWorkspace.FaceDescriptor.mouseUp(divCurrentWorkspace, new Point(pixelStartX, pixelStartY), curPt, event);
}

IG_ToolBox.toolBoxMouseClick_faceDesc = function (toolGroup, currentToolSelectedId, divDeepZoomPanel, pixelStartX, pixelStartY, tMouseCoords, curPt, pixel, event) {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    var divDeepZoomPanel = IGWS_DEEPZOOM_DIV;
    var zoomFct = 1.2;
    if (divCurrentWorkspace.FaceDescriptor) {
        if (divCurrentWorkspace.FaceDescriptor.mouseClick(divCurrentWorkspace, pixel, event))
            zoomFct = 1.0;
    }
    divDeepZoomPanel.Viewer.viewport.zoomBy(zoomFct, divDeepZoomPanel.Viewer.viewport.pointFromPixel(pixel));
}

var IG_tbox = null;

function IG_internalGetToolBox() {
    if (IG_tbox == null)
        IG_tbox = new IG_ToolBox();
    return IG_tbox.tTOOLBOX;
}