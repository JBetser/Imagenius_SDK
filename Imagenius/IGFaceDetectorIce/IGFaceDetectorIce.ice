// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#pragma once

module IGFaceDetector
{

["cpp:type:wstring"]
interface IGFaceDetectorIce
{
	idempotent void faceDetection(string pictureId, string pictureName, int rotationIndex, bool bSemaphore);
	idempotent bool faceDetectionStarted(string pictureId, int proxyId, int angle);
	idempotent void faceDetectionFinished(string pictureId, int proxyId, int nbFaces, int angle, int score, bool isCancelled);
};

};
