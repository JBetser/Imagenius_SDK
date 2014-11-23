<%@ Page Language="C#" AutoEventWireup="true" CodeFile="Test.aspx.cs"
    Inherits="IGPEWeb.Test" %>
<html>
	<head>
		<title>Imagenius SDK test page</title>
		<link rel="stylesheet" href="css/style.css"/>
        <style type="text/css">
        * {
	        margin: 0;
        }
        html, body {
	        height: 100%;
	        overflow: hidden;
        }
        .wrapper {
	        min-height: 100%;
	        height: auto !important;
	        height: 100%;
	        margin: 100px 50px -30px 50px; /* the bottom margin is the negative value of the footer's height */
        }
        .footer, .push {
	        height: 30px; /* .push must be the same height as .footer */
        }
        </style>
        <script type="text/javascript" src="jscript/jquery-1.9.1.js"></script>
        <script type="text/javascript" src="jscript/seadragon-min.js"></script>		
	    <script type="text/javascript" src="jscript/IGWSHelper.js"></script>
	    <script type="text/javascript" src="jscript/IGModal.js"></script>
	    <script type="text/javascript" src="jscript/IGWSAccount.js"></script>
	    <script type="text/javascript" src="jscript/IGToolBox.js"></script>
	    <script type="text/javascript" src="jscript/IGWSWorkspace.js"></script>
	    <script type="text/javascript" src="jscript/IGWSClient.js"></script>
	    <script type="text/javascript" src="jscript/IGRequest.js"></script>
	    <script type="text/javascript" src="jscript/jscolor/jscolor.js"></script>
	    <script type="text/javascript" src="jscript/IGSeadragon.js"></script>	
	    <script type="text/javascript" src="jscript/imageniusAPI.js"></script>  
        <script type="text/javascript">    
            function startImagenius() {
                IG.initDemo("divDeepZoomContainer");
            }

            function OnOpenImage() {
                var inputImage = document.getElementById("inputImage");
                IG.api("OpenImage", { "ImageName": inputImage.value }, function (data) {
                    $("#divDeepZoomContainer").css('backgroundImage', 'none');
                });
            }

            function OnHappy(force) {
                IG.api("FilterImage", { "FaceEffectId": IGIPFACE_EFFECT_MORPHING,
                    "Params": { "Param": IGIPMORPHING_TYPE_SMILE,
                        "Param2": force * 250000,
                        "Param3": force * 250000
                    }
                });
            }

            function OnAnger(force) {
                IG.api("FilterImage", { "FaceEffectId": IGIPFACE_EFFECT_MORPHING,
                    "Params": { "Param": IGIPMORPHING_TYPE_ANGRY,
                        "Param2": force * 250000
                    }
                });
            }

            function OnSadness(force) {
                IG.api("FilterImage", { "FaceEffectId": IGIPFACE_EFFECT_MORPHING,
                    "Params": { "Param": IGIPMORPHING_TYPE_SAD,
                        "Param2": force * 250000
                    }
                });
            }

            function OnUgly(force) {
                IG.api("FilterImage", { "FaceEffectId": IGIPFACE_EFFECT_MORPHING,
                    "Params": { "Param": IGIPMORPHING_TYPE_UGLY,
                        "Param2": force * 250000
                    }
                });
            }

            function OnSick(force) {
                IG.api("FilterImage", { "FaceEffectId": IGIPFACE_EFFECT_POX,
                    "Params": { "Param": force * 250000 }
                });
            }

            function OnHooligan(force) {
                IG.api("FilterImage", { "FaceEffectId": IGIPFACE_EFFECT_HOOLIGAN,
                    "Params": { "Param": force * 250000 }
                });
            }

            function OnSurprised(force) {
                IG.api("FilterImage", { "FaceEffectId": IGIPFACE_EFFECT_SURPRISED,
                    "Params": { "Param": force * 250000 }
                });
            }

            function OnPython(scriptId) {
                scripts = ['test1', 'test2', 'test3']
                IG.api("FilterImage", { "FilterId": IGIPFILTER_PYTHON,
                    "Params": { "Script": scripts[scriptId-1] }
                });
            }
            function OnRandom() {
                IG.api("FilterImage", { "FaceEffectId": IGIPFACE_EFFECT_RANDOM });
            }
            function OnSharpen() {
                IG.api("FilterImage", { "FaceEffectId": IGIPFACE_EFFECT_SHARPEN });
            }
            function OnCartoon() {
                IG.api("FilterImage", { "FilterId": IGIPFILTER_CARTOON });
            }
            function OnPaper() {
                IG.api("FilterImage", { "FilterId": IGIPFILTER_PAPER });
            }
            function OnWaterPainting() {
                IG.api("FilterImage", { "FilterId": IGIPFILTER_WATERPAINTING });
            }
            function OnSepia() {
                IG.api("FilterImage", { "FilterId": IGIPFILTER_SEPIA });
            }
            function OnHaloSepia() {
                IG.api("FilterImage", { "FilterId": IGIPFILTER_HALOSEPIA });
            }
            function OnOilPainting() {
                IG.api("FilterImage", { "FilterId": IGIPFILTER_OILPAINTING });
            }
            function OnBlackWhite() {
                IG.api("FilterImage", { "FilterId": IGIPFILTER_BW });
            }
            function OnVintage() {
                IG.api("FilterImage", { "FilterId": IGIPFILTER_VINTAGE });
            }
            function OnClay() {
                IG.api("FilterImage", { "FilterId": IGIPFILTER_CLAY });
            }
        </script>
	</head>
	<body onload="startImagenius()">        
        <div>
            <div>
			    <div>
			        <a onclick="OnOpenImage()" href="#">Load Image</a>
                    <input id="inputImage" type="text" value="couple1.jpg"/>
			    </div>
                <div>
			        <a onclick="OnHappy(3)" href="#">HAPPY</a>
                    <a onclick="OnSadness(3)" style="margin-left:4em" href="#">SAD</a>
                    <a onclick="OnAnger(3)" style="margin-left:4em" href="#">ANGRY</a>                    
                    <a onclick="OnSurprised(3)" style="margin-left:4em" href="#">SURPRISED</a>
                    <a onclick="OnUgly(3)" style="margin-left:4em" href="#">STUPID</a>
                    <a onclick="OnSick(3)" style="margin-left:4em" href="#">SICK</a>
                    <a onclick="OnHooligan(3)" style="margin-left:4em" href="#">HOOLIGAN</a>
                    <a onclick="OnRandom()" style="margin-left:4em" href="#">RANDOM</a>                
			    </div>                    
                <div>  
                    <a onclick="OnPython(3)" href="#">PYTHON</a>                     
                    <a onclick="OnSharpen(100,100,100)" style="margin-left:4em" href="#">SHARPEN</a>
                    <a onclick="OnCartoon()" style="margin-left:4em" href="#">CARTOON</a>
                    <a onclick="OnPaper()" style="margin-left:4em" href="#">PAPER</a>
                    <a onclick="OnWaterPainting()" style="margin-left:4em" href="#">WATER</a>
                    <a onclick="OnOilPainting()" style="margin-left:4em" href="#">OIL</a>
                    <a onclick="OnClay()" style="margin-left:4em" href="#">CLAY</a>
                    <a onclick="OnVintage()" style="margin-left:4em" href="#">VINTAGE</a>                    
                </div>
                <div>
                    <a onclick="OnBlackWhite()" href="#">BLACK WHITE</a>
                    <a onclick="OnSepia()" style="margin-left:4em" href="#">SEPIA</a>
                    <a onclick="OnHaloSepia()" style="margin-left:4em" href="#">H-SEPIA</a>                    
                </div>
			</div>	
		</div>
        <div class="wrapper">
           <div id="divDeepZoomContainer">
           </div>  
           <div class="push"> 
            </div>                                          
        </div>
        <div class="footer"> 
            <p>Copyright &copy; 2013 BitlSoft &mdash; <a href="http://bitlsoft.com/" title="Imagenius test page">Imagenius test page</a></p>
        </div> 	
	</body>
</html>