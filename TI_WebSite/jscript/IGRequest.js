/*****************************************************************************\

Javascript "Imagenius Web Service Requests" library
 
@version: 1.0 - 2012.05.15
@author: Jonathan Betser
\*****************************************************************************/
var IGIPFILTER_BLUR = 0;
var IGIPFILTER_GRADIENT = 1;
var IGIPFILTER_GRADIENT_MORPHO = 2;
var IGIPFILTER_GRAY = 3;
var IGIPFILTER_SKIN = 4;
var IGIPFILTER_SKIN_UNNOISED = 5;
var IGIPFILTER_EYES = 6;
var IGIPFILTER_FFT = 7;
var IGIPFILTER_FACE_EFFECT = 8;
var IGIPFILTER_COLORIZE = 9;
var IGIPFILTER_BRIGHTNESS = 10;
var IGIPFILTER_CONTRAST = 11;
var IGIPFILTER_MORPHING = 12;
var IGIPFILTER_NEGATIVE = 13;
var IGIPFILTER_TRANSPARENCY = 14;
var IGIPFILTER_PAINT = 15;
var IGIPFILTER_PAINTGRADIENT = 16;
var IGIPFILTER_REPAIR = 17;
var IGIPFILTER_EYES_COLOR	=	18;
var IGIPFILTER_SATURATE = 19;
var IGIPFILTER_SHARPEN = 20;
var IGIPFILTER_MATIFY = 21;
var IGIPFILTER_THRESHOLD = 22;
var IGIPFILTER_AUTOROTATE = 23;
var IGIPFILTER_SKETCH = 24;
var IGIPFILTER_CARTOON = 25;
var IGIPFILTER_SEPIA = 26;
var IGIPFILTER_VINTAGE = 27;
var IGIPFILTER_OILPAINTING = 28;
var IGIPFILTER_WATERPAINTING = 29;
var IGIPFILTER_DITHER = 30;
var IGIPFILTER_CLAY = 31;
var IGIPFILTER_PYTHON = 32;
var IGIPFILTER_PAPER = 33;
var IGIPFILTER_HALOSEPIA = 34;
var IGIPFILTER_BW = 35;

var IGIPFACE_EFFECT_COLORIZE = 0;
var IGIPFACE_EFFECT_BRIGHTNESS = 1;
var IGIPFACE_EFFECT_SHARPEN = 2;
var IGIPFACE_EFFECT_BLUR = 3;
var IGIPFACE_EFFECT_SATURATE = 4;
var IGIPFACE_EFFECT_REDEYE = 5;
var IGIPFACE_EFFECT_MORPHING = 6;
var IGIPFACE_EFFECT_REPAIR = 7;
var IGIPFACE_EFFECT_EYE_COLOR = 8;
var IGIPFACE_EFFECT_MATIFY = 9;
var IGIPFACE_EFFECT_POX = 10;
var IGIPFACE_EFFECT_HOOLIGAN = 11;
var IGIPFACE_EFFECT_SURPRISED = 12;
var IGIPFACE_EFFECT_RANDOM = 13;
var IGIPFACE_EFFECT_CARTOON = 14;

var IGIPMORPHING_TYPE_SMILE	= 0;
var IGIPMORPHING_TYPE_SAD	=	1;
var IGIPMORPHING_TYPE_ANGRY = 2;
var IGIPMORPHING_TYPE_BIGNOSE = 3;
var IGIPMORPHING_TYPE_CROSSEYED = 4;
var IGIPMORPHING_TYPE_CHINESE = 5;
var IGIPMORPHING_TYPE_UGLY = 6;

var IGIPINDEX_LPE = 100;
var IGIPINDEX_FACE = 101;

var IGSELECTION_CLEAR = 1;
var IGSELECTION_SQUARE = 2;
var IGSELECTION_LASSO = 4;
var IGSELECTION_ADD = 8;
var IGSELECTION_REMOVE = 16;
var IGSELECTION_REPLACE = 32;
var IGSELECTION_MAGIC = 64;
var IGSELECTION_LPE = 128;
var IGSELECTION_FACES = 256;
var IGSELECTION_INVERT = 513;
var IGSELECTION_EYES = 1024;
var IGSELECTION_MOUTH = 2048;

// requests with internal API
function IG_internalSaveImage(successCallback, errorCallback) {
    var jsonData = { "FrameId": IG_internalGetCurrentFrameId(),
                     "ImageName": IG_internalGetCurrentPictureName()  };
    IG_internalAPI("SaveImage", jsonData, successCallback, errorCallback);
}

function IG_internalSaveImageBM(successCallback, errorCallback) {
    var jsonData = { "FrameId": IG_internalGetCurrentFrameId(),
                     "ImageName": IG_internalGetCurrentPictureName()};
    IG_internalAPI("SaveImageBM", jsonData, successCallback, errorCallback);
}

function IG_internalSwapColors() {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    var selectedTexture = (divCurrentWorkspace.getAttribute("currentWorkspaceProperty_SelectTexture") == "Texture1" ? "1" : "0");
    var jsonData = { "FrameId": "0",
                     "PropIds": "SelectTexture",
                     "PropVals": selectedTexture};
    IG_internalAPI("ChangeProperty", jsonData);
}

function IG_internalChangeColor(sender) {
    var jsonData = { "FrameId": "0",
                     "PropIds": (sender.id == "IG_Texture2" ? "Texture2" : "Texture1"),
                     "PropVals": IG_internalFromColorANToNumber(sender.value).toString()};
    IG_internalAPI("ChangeProperty", jsonData);
}

function IG_internalChangeColorAN(value) {
    var jsonData = { "FrameId": "0",
        "PropIds": "Texture1",
        "PropVals": IG_internalFromColorANToNumber(value).toString()};
    IG_internalAPI("ChangeProperty", jsonData);
}


// dialog boxes with public API
function IG_internalSaveImageCustom(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Custom save";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var imageName = tOptions[0];
            var idxExt = imageName.lastIndexOf(".");
            if (idxExt >= 0) {
                var imageNameExt = imageName.substring(idxExt).toUpperCase();
                if ((imageNameExt != ".JPG") &&
                (imageNameExt != ".PNG") &&
                (imageNameExt != ".BMP") &&
                (imageNameExt != ".IG")) {
                    imageName += tOptions[1];
                }
            }
            else {
                imageName += tOptions[1];
            }
            var jsonData = { "FrameId": IG_internalGetCurrentFrameId(),
                "ImageName": imageName
            };
            IG_publicAPI("SaveImage", jsonData);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 2;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = IG_internalGetCurrentPictureName();
    tParams[idxParam++] = "Name";
    tParams[idxParam++] = IGMODALTYPE_COMBOBOX;
    var tTypes = {};
    var nbTypes = 4;
    tTypes[0] = ".ig"; tTypes[1] = ".jpg";
    tTypes[2] = ".png"; tTypes[3] = ".bmp";
    tParams[idxParam++] = nbTypes;
    for (var idxItParam = 0; idxItParam < nbTypes; idxItParam++) {
        tParams[idxParam++] = tTypes[idxItParam];
    }
    tParams[idxParam++] = "Type";
    modal.popOut(tParams);
}

function IG_internalTransparency(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Transparency";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FilterId": IGIPFILTER_TRANSPARENCY,
                "Params": { "Strength": 100 - parseInt(tOptions[0], 10) }
            };
            IG_publicAPI("FilterImage", jsonData, successCallback, errorCallback);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 1;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Alpha(%)";
    modal.popOut(tParams);
}

function IG_internalBrightness(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Brightness";
    tParams[idxParam++] = function Brightness_result(result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FilterId": IGIPFILTER_BRIGHTNESS,
                "Params": { "Strength": parseInt(tOptions[0], 10) * (tOptions[1] == "Add" ? 1 : -1) }
            };
            IG_publicAPI("FilterImage", jsonData, successCallback, errorCallback);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 2;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Brightness(%)";
    tParams[idxParam++] = IGMODALTYPE_COMBOBOX;
    var tTypes = {};
    var nbTypes = 2;
    tTypes[0] = "Add"; tTypes[1] = "Reduce";
    tParams[idxParam++] = nbTypes;
    for (var idxItParam = 0; idxItParam < nbTypes; idxItParam++) {
        tParams[idxParam++] = tTypes[idxItParam];
    }
    tParams[idxParam++] = "Function";
    modal.popOut(tParams);
}

function IG_internalContrast(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Contrast";
    tParams[idxParam++] = function Contrast_result(result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FilterId": IGIPFILTER_CONTRAST,
                "Params": { "Strength": parseInt(tOptions[0], 10) * (tOptions[1] == "Add" ? 1 : -1) }
            };
            IG_publicAPI("FilterImage", jsonData);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 2;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Contrast(%)";
    tParams[idxParam++] = IGMODALTYPE_COMBOBOX;
    var tTypes = {};
    var nbTypes = 2;
    tTypes[0] = "Add"; tTypes[1] = "Reduce";
    tParams[idxParam++] = nbTypes;
    for (var idxItParam = 0; idxItParam < nbTypes; idxItParam++) {
        tParams[idxParam++] = tTypes[idxItParam];
    }
    tParams[idxParam++] = "Function";
    modal.popOut(tParams);
}

function IG_internalSaturate(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Saturate";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FilterId": IGIPFILTER_SATURATE,
                "Params": { "Strength": parseInt(tOptions[0], 10) * (tOptions[1] == "Add" ? 1 : -1) }
            };
            IG_publicAPI("FilterImage", jsonData, successCallback, errorCallback);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 2;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Saturate(%)";
    tParams[idxParam++] = IGMODALTYPE_COMBOBOX;
    var tTypes = {};
    var nbTypes = 2;
    tTypes[0] = "Add"; tTypes[1] = "Reduce";
    tParams[idxParam++] = nbTypes;
    for (var idxItParam = 0; idxItParam < nbTypes; idxItParam++) {
        tParams[idxParam++] = tTypes[idxItParam];
    }
    tParams[idxParam++] = "Function";
    modal.popOut(tParams);
}

function IG_internalChangeFrameProperties(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Change Properties";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "PropIds": "Width,Height",
                "PropVals": tOptions[0] + "," + tOptions[1]
            };
            IG_publicAPI("ChangeProperty", jsonData);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 2;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = IG_internalGetCurrentPictureProperty("Width");
    tParams[idxParam++] = "Width(px)";
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = IG_internalGetCurrentPictureProperty("Height");
    tParams[idxParam++] = "Height(px)";
    modal.popOut(tParams);
}

function IG_FaceHappy(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Happy";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_MORPHING,
                "Params": { "Param": IGIPMORPHING_TYPE_SMILE, "Param2": parseInt(tOptions[0]) * 10000, "Param3": parseInt(tOptions[1]) * 10000 }
            };
            IG_publicAPI("FilterImage", jsonData, successCallback, errorCallback);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 2;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Left strength(%)";
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Right strength(%)";
    modal.popOut(tParams);
}

function IG_FaceSharpen(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Sharpen face";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_SHARPEN,
                "Params": { "Param": parseInt(tOptions[0]) * 10000, "Param2": parseInt(tOptions[0]) * 10000, "Param3": parseInt(tOptions[0]) * 10000 }
            };
            IG_publicAPI("FilterImage", jsonData, successCallback, errorCallback);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 1;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Strength(%)";
    modal.popOut(tParams);
}

function IG_FaceSoften(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Soften face";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_REPAIR,
                "Params": { "Param": parseInt(tOptions[0]) * 10000, "Param2": parseInt(tOptions[0]) * 10000, "Param3": parseInt(tOptions[0]) * 10000 }
            };
            IG_publicAPI("FilterImage", jsonData, successCallback, errorCallback);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 1;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Strength(%)";
    modal.popOut(tParams);
}

function IG_FaceCartoon(successCallback, errorCallback) {
    var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_CARTOON };
    IG_publicAPI("FilterImage", jsonData, successCallback, errorCallback);
}

function IG_FaceAnger(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Angry";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_MORPHING,
                "Params": { "Param": IGIPMORPHING_TYPE_ANGRY, "Param2": parseInt(tOptions[0]) * 10000 }
            };
            IG_publicAPI("FilterImage", jsonData, successCallback, errorCallback);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 1;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Strength(%)";
    modal.popOut(tParams);
}

function IG_FaceSadness(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Sad";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_MORPHING,
                "Params": { "Param": IGIPMORPHING_TYPE_SAD, "Param2": parseInt(tOptions[0]) * 10000 }
            };
            IG_publicAPI("FilterImage", jsonData, successCallback, errorCallback);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 1;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Strength(%)";
    modal.popOut(tParams);
}

function IG_FaceUgly(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Ugly";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_MORPHING,
                "Params": { "Param": IGIPMORPHING_TYPE_UGLY, "Param2": parseInt(tOptions[0]) * 10000 }
            };
            IG_publicAPI("FilterImage", jsonData, successCallback, errorCallback);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 1;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Strength(%)";
    modal.popOut(tParams);
}

function IG_FaceSick(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Sick";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_POX,
                "Params": { "Param": parseInt(tOptions[0]) * 10000 }
            };
            IG_publicAPI("FilterImage", jsonData, successCallback, errorCallback);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 1;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Strength(%)";
    modal.popOut(tParams);
}

function IG_FaceHooligan(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Hooligan";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_HOOLIGAN,
                "Params": { "Param": parseInt(tOptions[0]) * 10000 }
            };
            IG_publicAPI("FilterImage", jsonData, successCallback, errorCallback);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 1;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Strength(%)";
    modal.popOut(tParams);
}

function IG_FaceSurprised(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Surprised";
    tParams[idxParam++] = function (result, tOptions, successCallback, errorCallback) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_SURPRISED,
                "Params": { "Param": parseInt(tOptions[0]) * 10000 }
            };
            IG_publicAPI("FilterImage", jsonData, successCallback, errorCallback);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 1;
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "50";
    tParams[idxParam++] = "Strength(%)";
    modal.popOut(tParams);
}

function IG_FaceColorizeEyes(successCallback, errorCallback) {
    var modal = new IG_Modal();
    var idxParam = 0;
    var tParams = {};
    tParams[idxParam++] = "Colorize eyes";
    tParams[idxParam++] = function FaceColorizeEyes_result(result, tOptions) {
        if (result == IGMODALRETURN_OK) {
            var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_EYE_COLOR,
                "Params": { "Param": (tOptions[0] == "Auto" ? 1 : 0),
                    "Param2": tOptions[1]
                }
            };
            IG_publicAPI("FilterImage", jsonData);
        }
    };
    tParams[idxParam++] = successCallback;
    tParams[idxParam++] = errorCallback;
    tParams[idxParam++] = 2;
    tParams[idxParam++] = IGMODALTYPE_COMBOBOX;
    var tTypes = {};
    var nbTypes = 2;
    tTypes[0] = "Auto"; tTypes[1] = "Centered";
    tParams[idxParam++] = nbTypes;
    for (var idxItParam = 0; idxItParam < nbTypes; idxItParam++) {
        tParams[idxParam++] = tTypes[idxItParam];
    }
    tParams[idxParam++] = "Pupil detection";
    tParams[idxParam++] = IGMODALTYPE_TEXTBOX;
    tParams[idxParam++] = "30";
    tParams[idxParam++] = "Strength(%)";
    modal.popOut(tParams);
}

// other requests with public API
function IG_FaceRandom(successCallback, errorCallback) {
    var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_RANDOM };
    IG_publicAPI("FilterImage", jsonData);
}

function IG_FaceRemoveRedEyes(successCallback, errorCallback) {
    var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_REDEYE };
    IG_publicAPI("FilterImage", jsonData);
}

function IG_FaceColorizeSkin(successCallback, errorCallback) {
    var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_COLORIZE,
        "Params": { "Param": 50, "Param2": 0 }
    };
    IG_publicAPI("FilterImage", jsonData);
}

function IG_FaceColorizeSkinFun(successCallback, errorCallback) {
    var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_COLORIZE,
        "Params": { "Param": 50, "Param2": 1}
    };
    IG_publicAPI("FilterImage", jsonData);
}

function IG_FaceMatifySkin(successCallback, errorCallback) {
    var jsonData = { "FaceEffectId": IGIPFACE_EFFECT_MATIFY,
        "Params": { "Param": 30}
    };
    IG_publicAPI("FilterImage", jsonData);
}

function IG_ChangeSelection(mask, tCoords) {
    if (tCoords == null)
        tCoords = [0, 0, -1, -1];
    var sTabPoints = "";
    if (tCoords) {
        var idxPt = 0;
        while (tCoords[idxPt]) {
            sTabPoints = sTabPoints + tCoords[idxPt++] + ",";
        }
        sTabPoints = sTabPoints.substring(0, sTabPoints.length - 1);
    }
    else {
        sTabPoints = "0,0";
    }
    var jsonData = { "SelectionType": mask.toString(),
                      "TabPoints": sTabPoints };
    IG_publicAPI("ChangeSelection", jsonData);
}
