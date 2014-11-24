/*****************************************************************************\

Javascript "Imagenius FaceBook Integration" library
 
@version: 1.0 - 2012.11.06
@author: Jonathan Betser
\*****************************************************************************/
function fbGetToken(successCallback) {
    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
    if (!divCurrentWorkspace)
        return null;
    var currentFBToken = divCurrentWorkspace.getAttribute("fbToken");
    if (!currentFBToken) {
        if (typeof FB !== 'undefined') {
            FB.getLoginStatus(function (response) {
                if (response.status === 'connected') {
                    var divCurrentWorkspace = IGWS_WORKSPACE_DIV;
                    if (divCurrentWorkspace) {
                        divCurrentWorkspace.setAttribute("fbToken", response.authResponse.accessToken);
                        ConnectFbUser(successCallback);
                    }
                }
            });
        }
    }
    return currentFBToken;
}

function fbGetUserId(callback, successCallback) {
    var fbToken = fbGetToken();
    if (fbToken) {
        FB.api("/me?access_token=" + fbToken, function (response) {
            var reg = new RegExp("[ ]+", "g");
            callback(response.name.replace(reg, "_"), response.id + "@facebook.com", successCallback);
        });
    }
}

function ConnectFbUser_callback(userName, userEmail, successCallback) {
    var jsonData = { 'UserName': userName,
        'UserEmail': userEmail
    };
    IG.api("ConnectFbGuest", jsonData, successCallback,
       function (status, jqXHR) {
           IG.showPopupMessage(jqXHR, true, status);
       }
    );
}

function ConnectFbUser(successCallback) {
    fbGetUserId(ConnectFbUser_callback, successCallback);
}

function uploadPhoto(sender, event) {
    var fileName = IG_internalGetFileName(sender.title);
    var IG_divImageLibrary = IG_internalGetElementById("IG_divImageLibrary");
    if (IG_internalIsImageInLibrary(IG_divImageLibrary, fileName)) {
        IG.api("OpenImage", { "ImageName": fileName }, function () {
            var divSelectAlbums = IG_internalGetElementById("divSelectAlbums");
            divSelectAlbums.style.display = "none";
        },
            function (status, jqXHR) {
                IG.showPopupMessage(jqXHR, true, status);
            }
        );
    }
    else{
        var jsonData = { "ImageURL": sender.title };
        IG.api("UploadImage", jsonData, function () {
            var divSelectAlbums = IG_internalGetElementById("divSelectAlbums");
            divSelectAlbums.style.display = "none";
        },
            function (status, jqXHR) {
                IG.showPopupMessage(jqXHR, true, status);
            }
        );
    }
}


function addPhotos(dataPhotos, fbToken) {
    if (dataPhotos.length > 0) {
        var bRefresh = false;
        var divAlbums = IG_internalGetElementById("divAlbums");
        IG_internalCleanNode(divAlbums);
        for (var idxPhoto = 0; idxPhoto < dataPhotos.length; idxPhoto++) {
            var newSpan = document.createElement("span");
            newSpan.setAttribute("style", "display: inline-block; margin: 5px; padding: 0px; vertical-align: top");
            var spanId = "Span" + dataPhotos[idxPhoto].id;
            newSpan.setAttribute("id", spanId);
            newSpan.setAttribute("title", dataPhotos[idxPhoto].source);
            newSpan.setAttribute("onclick", "uploadPhoto(this, event);");
            newSpan.setAttribute("ondblclick", "uploadPhoto(this, event);");

            var newImg = document.createElement("img");
            newImg.setAttribute("id", dataPhotos[idxPhoto].id);
            newImg.setAttribute("src", dataPhotos[idxPhoto].picture);
            newImg.setAttribute("style", "padding: 0px; margin-left: 0px; margin-top: 0px");

            newSpan.appendChild(newImg);
            divAlbums.appendChild(newSpan);

            var labelSelectAlbum = IG_internalGetElementById("LabelSelectAlbum");
            labelSelectAlbum.style.display = "none";
            var labelSelectPicture = IG_internalGetElementById("LabelSelectPicture");
            labelSelectPicture.style.display = "inherit";
            var divSelectAlbums = IG_internalGetElementById("divSelectAlbums");
            divSelectAlbums.scrollTop = divSelectAlbums.clientHeight;
        }
    }
    else {
        IG.showPopupMessage("You do not have any pictures in this album.\nPlease upload some pictures first, or pick another album.\nYou can also use Twitter.", false);
    }
}

function fbGetPhotos(sender, event) {
    var fbToken = fbGetToken();
    if (fbToken) {
        FB.api(sender.title + "/photos?access_token=" + fbToken + "&limit=200", function (response) {
            addPhotos(response.data, fbToken);
        });
    }
}

function addAlbums(dataAlbums, fbToken) {
    if (dataAlbums) {
        if (dataAlbums.length > 0) {
            var bRefresh = false;
            var divAlbums = IG_internalGetElementById("divAlbums");
            IG_internalCleanNode(divAlbums);
            var curTr = null;
            for (var idxAlb = 0; idxAlb < dataAlbums.length; idxAlb++) {
                var newSpan = document.createElement("span");
                newSpan.setAttribute("style", "display: inline-block; margin: 5px; padding: 0px; vertical-align: top");
                newSpan.setAttribute("id", "Span" + dataAlbums[idxAlb].cover_photo);
                newSpan.setAttribute("title", dataAlbums[idxAlb].id);
                divAlbums.appendChild(newSpan);

                if (dataAlbums[idxAlb].cover_photo) {
                    FB.api(dataAlbums[idxAlb].cover_photo + "?access_token=" + fbToken, function (graphAlbum) {
                        var newSpan = IG_internalGetElementById("Span" + graphAlbum.id);
                        newSpan.setAttribute("onclick", "fbGetPhotos(this, event);");
                        newSpan.setAttribute("ondblclick", "fbGetPhotos(this, event);");

                        var newImg = document.createElement("img");
                        newImg.setAttribute("id", graphAlbum.id);
                        newImg.setAttribute("src", graphAlbum.picture);
                        newImg.setAttribute("style", "padding: 0px; margin-left: 0px; margin-top: 0px");

                        newSpan.appendChild(newImg);

                        var labelSelectPicture = IG_internalGetElementById("LabelSelectPicture");
                        labelSelectPicture.style.display = "none";
                        var labelSelectAlbum = IG_internalGetElementById("LabelSelectAlbum");
                        labelSelectAlbum.style.display = "inherit";
                        var divSelectAlbums = IG_internalGetElementById("divSelectAlbums");
                        divSelectAlbums.scrollTop = divSelectAlbums.clientHeight;
                    });
                }
            }
        }
    } else {
        IG.showPopupMessage("You do not have any albums on Facebook.\nPlease upload some pictures first, or try with Twitter.", false);
    }
}

function fbGetAlbums() {
    var fbToken = fbGetToken();
    if (fbToken) {
        FB.api("/me/albums?access_token=" + fbToken, function (response) {
            IG.showImageLibrary(false);
            addAlbums(response.data, fbToken);
        });        
    }
}

function getPictureComment() {
    var comment = IG_internalGetElementById("commentTextBox").value;
    if (comment == null)
        comment = "No comment";
    return comment;
}

function fbUploadPhotoToAlbum(path, albumId, callback){
    var fbToken = fbGetToken();
    if (fbToken) {
        var comment = getPictureComment();
        var photo = { 'message': comment,
                      'url': path
                    };
        FB.api("/" + albumId + "/photos?access_token=" + fbToken, "post", photo, function (response) {
            if (!response) {
                callback("FAIL");
                return;
            }
            if (callback){
                if (response.id)
                    callback("OK");
                else
                    callback("FAIL");
            }
        });
    }
    else
        callback("FAIL");
}

function fbSaveToAlbum(callback) {      
    var fbToken = fbGetToken();
    if (fbToken) {
        FB.api("/me/albums?access_token=" + fbToken, function (resp) {
            if (!resp) {
                callback("FAIL");
                return;
            }
            var albumId = null;
            for (var i = 0, l = resp.data.length; i < l; i++) {
                var album = resp.data[i];
                if (album.name == "Beetle Morph")
                    albumId = album.id;
            }
            var currentPicName = IG_internalGetCurrentPictureName();
            if (currentPicName) {
                var jsonData = { 'ImageName': currentPicName };
                IG.api("DownloadImage", jsonData,
                    function (data) {
                        if (albumId == null) {
                            //Create beetle morph album
                            var album = { 'message': 'Contains the pictures generated with Beetle Morph',
                                'name': 'Beetle Morph'
                            };
                            FB.api("/me/albums?access_token=" + fbToken, 'post', album, function (response) {
                                fbUploadPhotoToAlbum(data.RequestParams.PathList[0], response.id, callback);
                            });
                        }
                        else {
                            fbUploadPhotoToAlbum(data.RequestParams.PathList[0], albumId, callback);
                        }
                    }
                );
            }
        });
    }
    else
        callback("FAIL");
}

function fbPostPhotoToWall(callback) {    
    var fbToken = fbGetToken();
    if (fbToken) {
        var currentPicName = IG_internalGetCurrentPictureName();
        if (currentPicName) {
            var jsonData = { 'ImageName': currentPicName };
            IG.api("DownloadImage", jsonData,
                function (data) {
                    var comment = getPictureComment();
                    var wallPost = {
                        'message': comment + " -- Done with Beetle Morph",
                        'picture': IG.urlAppli + data.RequestParams.PathList[0]
                    };
                    FB.api("/me/feed?access_token=" + fbToken, "post", wallPost, function (response) {
                        if (!response) {
                            callback("FAIL");
                            return;
                        }
                        if (callback && response) {
                            if (response.id)
                                callback("OK");
                            else
                                callback("FAIL");
                        }
                    });
                }
            );
        }
    }
    else
        callback("FAIL");
}

function fbSendToFriends(callback) {
   var currentPicName = IG_internalGetCurrentPictureName();
   if (currentPicName) {
       var jsonData = { 'ImageName': currentPicName };
       IG.api("DownloadImage", jsonData,
           function (data) {
               var comment = getPictureComment();

               var sendToFriends = {
                   'method': 'send',
                   'name': comment,
                   'link': IG.urlAppli + data.RequestParams.PathList[0]
               };

               // Use FB.ui to send the Request(s)
               FB.ui(sendToFriends, function (response) {
                   if (callback && response) {
                       if (response.success)
                           callback("OK");
                       else
                           callback("FAIL");
                   }
               });
           }
       );
   }
}

// Here we run a very simple test of the Graph API after login is successful. 
// This testAPI() function is only called in those cases. 
function testAPI() {
    console.log('Welcome!  Fetching your information.... ');
    FB.api('/me', function (response) {
        console.log('Good to see you, ' + response.name + '.');
    });
}