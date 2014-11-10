/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_VrpnBaseFlexServer_h_GUID_BA2E66A9_F0F3_4BBE_5248_62C5B7E5CBDE
#define INCLUDED_VrpnBaseFlexServer_h_GUID_BA2E66A9_F0F3_4BBE_5248_62C5B7E5CBDE

// Internal Includes
#include <ogvr/Util/Verbosity.h>
#include <ogvr/Util/TimeValue.h>

// Library/third-party includes
#include <vrpn_BaseClass.h>

// Standard includes
// - none

namespace ogvr {
/// @brief Basic implementation of a vrpn_BaseClass server.
class vrpn_BaseFlexServer : public vrpn_BaseClass {
  public:
    vrpn_BaseFlexServer(const char *name, vrpn_Connection *conn)
        : vrpn_BaseClass(name, conn) {
        vrpn_BaseClass::init();
    }
    virtual ~vrpn_BaseFlexServer() {}

    virtual void mainloop() {
        /// @todo service device here? Device ends up being serviced in this
        /// object's owner, the VrpnConnectionDevice.
        server_mainloop();
    }
    void sendData(util::time::TimeValue const &timestamp, vrpn_uint32 msgID,
                  const char *bytestream, size_t len) {
        struct timeval now;
        util::time::toStructTimeval(now, timestamp);
        d_connection->pack_message(len, now, msgID, d_sender_id, bytestream,
                                   vrpn_CONNECTION_LOW_LATENCY);
    }

  protected:
    virtual int register_types() { return 0; }
};
} // namespace ogvr
#endif // INCLUDED_VrpnBaseFlexServer_h_GUID_BA2E66A9_F0F3_4BBE_5248_62C5B7E5CBDE