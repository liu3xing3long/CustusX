/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXOPENIGTLINKSTREAMERSERVICE_H
#define CXOPENIGTLINKSTREAMERSERVICE_H

#include "org_custusx_core_openigtlink3_Export.h"
#include "cxStreamerService.h"
#include "cxOpenIGTLinkStreamer.h"


namespace cx
{
typedef boost::shared_ptr<class NetworkHandler> NetworkHandlerPtr;
typedef boost::shared_ptr<class OpenIGTLinkTrackingSystemService> OpenIGTLinkTrackingSystemServicePtr;


class org_custusx_core_openigtlink3_EXPORT OpenIGTLinkStreamerService : public StreamerService
{

public:
	OpenIGTLinkStreamerService(NetworkHandlerPtr networkHandler, TrackingServicePtr trackingService);
    ~OpenIGTLinkStreamerService();

    virtual QString getName();
    virtual QString getType() const;
    virtual std::vector<PropertyPtr> getSettings(QDomElement root);
    virtual StreamerPtr createStreamer(QDomElement root);

private slots:
	void stopTrackingAndOpenIGTLinkClient();
private:
	OpenIGTLinkStreamerPtr mStreamer;
	NetworkHandlerPtr mConnection;
	TrackingServicePtr mTrackingService;
	bool mStartedTrackingAndOpenIGTLinkFromHere;

	StringPropertyBasePtr getIPOption(QDomElement root);
	DoublePropertyBasePtr getStreamPortOption(QDomElement root);
	BoolPropertyBasePtr trackAndStream(QDomElement root);
	OpenIGTLinkTrackingSystemServicePtr getOpenIGTLinkTrackingSystemService();
	void startTracking(QDomElement root);
	void configureTracking(QDomElement root);
};
typedef boost::shared_ptr<OpenIGTLinkStreamerService> OpenIGTLinkStreamerServicePtr;
} //namespace cx

#endif //CXOPENIGTLINKSTREAMERSERVICE_H
