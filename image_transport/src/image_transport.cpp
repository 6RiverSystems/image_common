/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2009, Willow Garage, Inc.
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

#include "image_transport/camera_common.h"
#include "image_transport/image_transport.h"
#include "image_transport/publisher_plugin.h"
#include "image_transport/subscriber_plugin.h"

#include <pluginlib/class_loader.hpp>

namespace image_transport
{

struct ImageTransport::Impl
{
  // rclcpp::Node::SharedPtr node_;
  PubLoaderPtr pub_loader_;
  SubLoaderPtr sub_loader_;

  Impl()
  // : node_(node),
  :  pub_loader_(std::make_shared<PubLoader>("image_transport",
      "image_transport::PublisherPlugin") ),
    sub_loader_(std::make_shared<SubLoader>("image_transport",
      "image_transport::SubscriberPlugin") )
  {
  }

  ~Impl() {
    std::cout << "~ImageTransport::Impl" << std::endl;
  }
};

ImageTransport::ImageTransport()
: impl_(new Impl())
{
}

ImageTransport::~ImageTransport()
{
  std::cout << "~ImageTransport" << std::endl;
}

Publisher ImageTransport::advertise(
  rclcpp::Node::SharedPtr node, const std::string & base_topic, rmw_qos_profile_t custom_qos)
{
  return Publisher(node, base_topic, impl_->pub_loader_, custom_qos);
}

Subscriber ImageTransport::subscribe(
  rclcpp::Node::SharedPtr node,
  const std::string & base_topic,
  const std::function<void(const sensor_msgs::msg::Image::ConstSharedPtr &)> & callback,
  rmw_qos_profile_t custom_qos)
{
  return Subscriber(node, base_topic, callback, impl_->sub_loader_, custom_qos);
}

CameraPublisher ImageTransport::advertiseCamera(
  rclcpp::Node::SharedPtr node,
  const std::string & base_topic, rmw_qos_profile_t custom_qos)
{
  return advertiseCamera(node, base_topic, custom_qos);
}

CameraSubscriber ImageTransport::subscribeCamera(
  rclcpp::Node::SharedPtr node,
  const std::string & base_topic,
  const CameraSubscriber::Callback & callback,
  rmw_qos_profile_t custom_qos)
{
  return CameraSubscriber(*this, node, base_topic, callback, custom_qos);
}

std::vector<std::string> ImageTransport::getDeclaredTransports() const
{
  std::vector<std::string> transports = impl_->sub_loader_->getDeclaredClasses();
  // Remove the "_sub" at the end of each class name.
  for (std::string & transport: transports) {
    std::cout << "getDeclaredTransports: " << transport << std::endl;
    transport = erase_last_copy(transport, "_sub");
  }
  return transports;
}

std::vector<std::string> ImageTransport::getLoadableTransports() const
{
  std::vector<std::string> loadableTransports;

  for (const std::string & transportPlugin: impl_->sub_loader_->getDeclaredClasses()) {
    // If the plugin loads without throwing an exception, add its
    // transport name to the list of valid plugins, otherwise ignore
    // it.
    try {
      std::shared_ptr<image_transport::SubscriberPlugin> sub =
        impl_->sub_loader_->createUniqueInstance(transportPlugin);
      // Remove the "_sub" at the end of each class name.
      loadableTransports.push_back(erase_last_copy(transportPlugin, "_sub"));
    } catch (const pluginlib::LibraryLoadException & e) {
    } catch (const pluginlib::CreateClassException & e) {
    }
  }

  return loadableTransports;

}

} //namespace image_transport
