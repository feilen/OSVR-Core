/** @file
    @brief Wrapper for a vrpn_Tracker_VideoBasedHMDTracker

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/AnalogInterfaceC.h>

// Generated JSON header file
#include "com_osvr_VideoBasedHMDTracker_json.h"

// Library/third-party includes
#include <opencv2/core/core.hpp> // for basic OpenCV types
#include <opencv2/core/operations.hpp>
#include <opencv2/highgui/highgui.hpp> // for image capture
#include <opencv2/imgproc/imgproc.hpp> // for image scaling

#include <boost/noncopyable.hpp>

// Standard includes
#include <iostream>
#include <sstream>

// Anonymous namespace to avoid symbol collision
namespace {

class VideoBasedHMDTracker : boost::noncopyable {
  public:
      VideoBasedHMDTracker(OSVR_PluginRegContext ctx, int cameraNum = 0, int channel = 0)
          : m_camera(cameraNum), m_channel(channel) {

        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        /// Indicate that we'll want 1 analog channel.
        osvrDeviceAnalogConfigure(opts, &m_analog, 1);

        /// Come up with a device name
        std::ostringstream os;
        os << "VideoBasedHMDTracker" << cameraNum << "_" << channel;

        /// Create an asynchronous (threaded) device
        m_dev.initAsync(ctx, os.str(), opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(
            com_osvr_VideoBasedHMDTracker_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {
        if (!m_camera.isOpened()) {
            // Couldn't open the camera.  Failing silently for now. Maybe the
            // camera will be plugged back in later.
            return OSVR_RETURN_SUCCESS;
        }

        // Trigger a camera grab.
        bool grabbed = m_camera.grab();
        if (!grabbed) {
            // No frame available.
            return OSVR_RETURN_SUCCESS;
        }
        bool retrieved = m_camera.retrieve(m_frame, m_channel);
        if (!retrieved) {
            return OSVR_RETURN_FAILURE;
        }

        /// Report the value of channel 0
        osvrDeviceAnalogSetValue(m_dev, m_analog, 0.0, 0);
        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_AnalogDeviceInterface m_analog;
    cv::VideoCapture m_camera;
    int m_channel;
    cv::Mat m_frame;
    cv::Mat m_scaledFrame;
};

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false) {}

    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {
        if (m_found) {
            return OSVR_RETURN_SUCCESS;
        }

        {
            // Autodetect camera
            cv::VideoCapture cap(0);
            if (!cap.isOpened()) {
                // Failed to find camera
                return OSVR_RETURN_FAILURE;
            }
        }

        m_found = true;

        /// Create our device object, passing the context, and register
        /// the function to call
        osvr::pluginkit::registerObjectForDeletion(ctx, new VideoBasedHMDTracker(ctx));

        return OSVR_RETURN_SUCCESS;
    }

  private:
    /// @brief Have we found our device yet? (this limits the plugin to one
    /// instance)
    bool m_found;
};
} // namespace

OSVR_PLUGIN(com_osvr_VideoBasedHMDTracker) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}
