// **********************************************************************
//
// Copyright (c) 2003-2012 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#pragma once

module IGServerController
{

interface IGServerControllerIce
{
	idempotent string ping();
	idempotent void reset();
	idempotent void shutdown();
    idempotent string sendRequest(string reqXML);	
	idempotent int getNbAvailableConnections();
    idempotent int getNbConnections();
	idempotent string getStatus();
};

};
