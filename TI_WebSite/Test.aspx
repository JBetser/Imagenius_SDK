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
            function OnRandom() {
                IG.api("FilterImage", { "FaceEffectId": IGIPFACE_EFFECT_RANDOM });
            }
            function OnSharpen() {
                IG.api("FilterImage", { "FaceEffectId": IGIPFACE_EFFECT_SHARPEN });
            }
            function OnCartoon() {
                IG.api("FilterImage", { "FaceEffectId": IGIPFACE_EFFECT_CARTOON });
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
			        <a onclick="OnHappy(1)" href="#">HAPPY1</a>
                    <a onclick="OnSadness(1)" style="margin-left:6em" href="#">SAD1</a>
                    <a onclick="OnAnger(1)" style="margin-left:6em" href="#">ANGRY1</a>                    
                    <a onclick="OnSurprised(1)" style="margin-left:6em" href="#">SURPRISED1</a>
                    <a onclick="OnUgly(1)" style="margin-left:6em" href="#">STUPID1</a>
                    <a onclick="OnSick(1)" style="margin-left:6em" href="#">SICK1</a>
                    <a onclick="OnHooligan(1)" style="margin-left:6em" href="#">HOOLIGAN1</a>                    
			    </div>
                <div>
			        <a onclick="OnHappy(2)" href="#">HAPPY2</a>
                    <a onclick="OnSadness(2)" style="margin-left:6em" href="#">SAD2</a>
                    <a onclick="OnAnger(2)" style="margin-left:6em" href="#">ANGRY2</a>                    
                    <a onclick="OnSurprised(2)" style="margin-left:6em" href="#">SURPRISED2</a>
                    <a onclick="OnUgly(2)" style="margin-left:6em" href="#">STUPID2</a>
                    <a onclick="OnSick(2)" style="margin-left:6em" href="#">SICK2</a>
                    <a onclick="OnHooligan(2)" style="margin-left:6em" href="#">HOOLIGAN2</a>
			    </div>
                <div>
			        <a onclick="OnHappy(3)" href="#">HAPPY3</a>
                    <a onclick="OnSadness(3)" style="margin-left:6em" href="#">SAD3</a>
                    <a onclick="OnAnger(3)" style="margin-left:6em" href="#">ANGRY3</a>                    
                    <a onclick="OnSurprised(3)" style="margin-left:6em" href="#">SURPRISED3</a>
                    <a onclick="OnUgly(3)" style="margin-left:6em" href="#">STUPID3</a>
                    <a onclick="OnSick(3)" style="margin-left:6em" href="#">SICK3</a>
                    <a onclick="OnHooligan(3)" style="margin-left:6em" href="#">HOOLIGAN3</a>                    
			    </div>                    
                <div>
                    <a onclick="OnRandom()" href="#">RANDOM</a>
                    <a onclick="OnSharpen(100,100,100)" style="margin-left:6em" href="#">SHARPEN</a>
                    <a onclick="OnCartoon()" style="margin-left:6em" href="#">CARTOON</a>
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