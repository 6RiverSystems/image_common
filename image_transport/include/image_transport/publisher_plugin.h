#ifndef IMAGE_TRANSPORT_PUBLISHER_PLUGIN_H
#define IMAGE_TRANSPORT_PUBLISHER_PLUGIN_H

#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include "image_transport/single_subscriber_publisher.h"

namespace image_transport {

/**
 * \brief Base class for plugins to ImagePublisher.
 */
class PublisherPlugin : boost::noncopyable
{
public:
  virtual ~PublisherPlugin() {}

  /**
   * \brief Get a string identifier for the transport provided by
   * this plugin.
   *
   * @todo Make pure virtual when removing 0.1-compatibility.
   */
  virtual std::string getTransportName() const
  {
    return "unknown";
  }

  /**
   * \brief Get a string identifier for the transport provided by
   * this plugin.
   *
   * @deprecated Use getTransportName() instead.
   */
  ROSCPP_DEPRECATED virtual std::string getTransportType() const { return getTransportName(); }

  //! @deprecated For 0.1 compatibility only.
  ROSCPP_DEPRECATED virtual std::string getDefaultTopic(const std::string& base_topic) const
  {
    return base_topic + "/" + getTransportType();
  }

  /**
   * \brief Advertise a topic, simple version.
   *
   * @todo Make non-virtual when removing 0.1-compatibility.
   */
  virtual void advertise(ros::NodeHandle& nh, const std::string& base_topic, uint32_t queue_size,
                 bool latch = true)
  {
    advertiseImpl(nh, base_topic, queue_size, SubscriberStatusCallback(),
                  SubscriberStatusCallback(), ros::VoidPtr(), latch);
  }
  
  /**
   * \brief Advertise a topic with subscriber status callbacks.
   */
  void advertise(ros::NodeHandle& nh, const std::string& base_topic, uint32_t queue_size,
                 const SubscriberStatusCallback& connect_cb,
                 const SubscriberStatusCallback& disconnect_cb = SubscriberStatusCallback(),
                 const ros::VoidPtr& tracked_object = ros::VoidPtr(), bool latch = true)
  {
    advertiseImpl(nh, base_topic, queue_size, connect_cb, disconnect_cb, tracked_object, latch);
  }

  /**
   * \brief Returns the number of subscribers that are currently connected to
   * this PublisherPlugin.
   */
  virtual uint32_t getNumSubscribers() const = 0;

  /**
   * \brief Returns the topic that this PublisherPlugin will publish on.
   */
  virtual std::string getTopic() const = 0;

  /**
   * \brief Publish an image using the transport associated with this PublisherPlugin.
   */
  virtual void publish(const sensor_msgs::Image& message) const = 0;

  /**
   * \brief Publish an image using the transport associated with this PublisherPlugin.
   */
  virtual void publish(const sensor_msgs::ImageConstPtr& message) const
  {
    publish(*message);
  }

  /**
   * \brief Shutdown any advertisements associated with this PublisherPlugin.
   */
  virtual void shutdown() = 0;

  /**
   * \brief Return the lookup name of the PublisherPlugin associated with a specific
   * transport identifier.
   */
  static std::string getLookupName(const std::string& transport_name)
  {
    return transport_name + "_pub";
  }

protected:
  /**
   * \brief Advertise a topic. Must be implemented by the subclass.
   *
   * @todo Make pure virtual when removing 0.1-compatibility.
   */
  virtual void advertiseImpl(ros::NodeHandle& nh, const std::string& base_topic, uint32_t queue_size,
                             const SubscriberStatusCallback& connect_cb,
                             const SubscriberStatusCallback& disconnect_cb,
                             const ros::VoidPtr& tracked_object, bool latch)
  {
    ROS_ERROR("Publisher plugin for '%s' is incompatible with Publisher. It may work with ImagePublisher "
              "(deprecated), but should be updated to the image_transport 0.2 interface.",
              getTransportType().c_str());
    //ROS_BREAK();
  }
};

} //namespace image_transport

#endif
