#pragma once

#include <IceBox/IceBox.h>

class IGFaceDetectorIceServiceI : public ::IceBox::Service
{
public:

    virtual void start(const ::std::string&,
                       const ::Ice::CommunicatorPtr&,
                       const ::Ice::StringSeq&);

    virtual void stop();

private:

    ::Ice::ObjectAdapterPtr _adapter;
};

