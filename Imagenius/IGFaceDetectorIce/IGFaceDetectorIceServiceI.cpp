// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <Ice/Ice.h>
#include <IGFaceDetectorIceServiceI.h>
#include <IGFaceDetectorIceI.h>

using namespace std;

extern "C"
{

//
// Factory function
//
ICE_DECLSPEC_EXPORT IceBox::Service*
create(Ice::CommunicatorPtr communicator)
{
    return new IGFaceDetectorIceServiceI;
}

}

void
IGFaceDetectorIceServiceI::start(const string& name, const Ice::CommunicatorPtr& communicator, const Ice::StringSeq& args)
{
    _adapter = communicator->createObjectAdapter("IGFaceDetectorIce-" + name);

    
    string helloIdentity = communicator->getProperties()->getProperty("IGFaceDetectorIce.Identity");

    IGFaceDetector::IGFaceDetectorIcePtr hello = new IGFaceDetectorIceI(name);
    _adapter->add(hello, communicator->stringToIdentity(helloIdentity));
    _adapter->activate();
}

void
IGFaceDetectorIceServiceI::stop()
{
    _adapter->destroy();
}
