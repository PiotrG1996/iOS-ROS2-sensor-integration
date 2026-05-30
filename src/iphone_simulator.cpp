#include <chrono>
#include <memory>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "sensor_msgs/msg/point_cloud2.hpp"
#include "sensor_msgs/msg/point_field.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "sensor_msgs/msg/nav_sat_fix.hpp"
#include "std_msgs/msg/header.hpp"

using namespace std::chrono_literals;

class IphoneSimulator : public rclcpp::Node
{
public:
  IphoneSimulator()
  : Node("iphone_simulator")
  {
    color_pub_ = this->create_publisher<sensor_msgs::msg::Image>("/color_image", 1);
    depth_pub_ = this->create_publisher<sensor_msgs::msg::Image>("/depth_image", 1);
    pc_pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>("/pointcloud", 1);
    imu_pub_ = this->create_publisher<sensor_msgs::msg::Imu>("/imu/data", 10);
    gps_pub_ = this->create_publisher<sensor_msgs::msg::NavSatFix>("/gps/fix", 10);

    timer_ = this->create_wall_timer(100ms, std::bind(&IphoneSimulator::publish_loop, this));
  }

private:
  void publish_loop()
  {
    auto now = this->now();

    // Color image (small 64x48 RGB)
    sensor_msgs::msg::Image color;
    color.header.stamp = now;
    color.header.frame_id = "camera_link";
    color.height = 48;
    color.width = 64;
    color.encoding = "rgb8";
    color.is_bigendian = false;
    color.step = color.width * 3;
    color.data.assign(color.height * color.step, 128);
    color_pub_->publish(color);

    // Depth image (mono16)
    sensor_msgs::msg::Image depth;
    depth.header.stamp = now;
    depth.header.frame_id = "camera_depth";
    depth.height = 48;
    depth.width = 64;
    depth.encoding = "mono16";
    depth.is_bigendian = false;
    depth.step = depth.width * 2;
    depth.data.resize(depth.height * depth.step);
    // leave zeros (close depth)
    depth_pub_->publish(depth);

    // Simple PointCloud2 with one point
    sensor_msgs::msg::PointCloud2 pc;
    pc.header.stamp = now;
    pc.header.frame_id = "camera_depth";
    pc.height = 1;
    pc.width = 1;
    sensor_msgs::msg::PointField pf;
    pf.name = "x"; pf.offset = 0; pf.datatype = sensor_msgs::msg::PointField::FLOAT32; pf.count = 1;
    pc.fields = {pf, pf, pf};
    // set offsets correctly: x=0,y=4,z=8
    pc.fields[0].name = "x"; pc.fields[0].offset = 0;
    pc.fields[1].name = "y"; pc.fields[1].offset = 4;
    pc.fields[2].name = "z"; pc.fields[2].offset = 8;
    pc.is_bigendian = false;
    pc.point_step = 12; // 3 * 4 bytes
    pc.row_step = pc.point_step * pc.width;
    pc.is_dense = true;
    pc.data.resize(pc.row_step * pc.height);
    float x = 0.5f, y = 0.0f, z = 0.0f;
    memcpy(&pc.data[0], &x, 4);
    memcpy(&pc.data[4], &y, 4);
    memcpy(&pc.data[8], &z, 4);
    pc_pub_->publish(pc);

    // IMU
    sensor_msgs::msg::Imu imu;
    imu.header.stamp = now;
    imu.header.frame_id = "imu_link";
    imu.orientation.w = 1.0;
    imu.angular_velocity.x = 0.0;
    imu.angular_velocity.y = 0.0;
    imu.angular_velocity.z = 0.0;
    imu.linear_acceleration.x = 0.0;
    imu.linear_acceleration.y = 0.0;
    imu.linear_acceleration.z = 0.0;
    imu_pub_->publish(imu);

    // GPS
    sensor_msgs::msg::NavSatFix fix;
    fix.header.stamp = now;
    fix.header.frame_id = "gps_link";
    fix.latitude = 0.0;
    fix.longitude = 0.0;
    fix.altitude = 0.0;
    gps_pub_->publish(fix);
  }

  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr color_pub_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr depth_pub_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pc_pub_;
  rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr imu_pub_;
  rclcpp::Publisher<sensor_msgs::msg::NavSatFix>::SharedPtr gps_pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<IphoneSimulator>());
  rclcpp::shutdown();
  return 0;
}
