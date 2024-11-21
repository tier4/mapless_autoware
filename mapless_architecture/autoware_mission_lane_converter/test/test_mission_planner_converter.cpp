// Copyright 2024 driveblocks GmbH, authors: Simon Eisenmann, Thomas Herrmann
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "autoware/mission_lane_converter/mission_lane_converter_node.hpp"
#include "gtest/gtest.h"
#include "rclcpp/rclcpp.hpp"

#include "geometry_msgs/msg/point.hpp"

namespace autoware::mapless_architecture
{

/**
 * @brief The fixture for testing the mission lane converter.
 *
 */
class MissionLaneConverterTest : public testing::Test
{
protected:
  MissionLaneConverterTest()
  {
    rclcpp::init(0, nullptr);  // Initialize ROS 2
  }

  ~MissionLaneConverterTest() override
  {
    rclcpp::shutdown();  // Shutdown ROS 2
  }
};

/**
 * @brief Mock class for MissionLaneConverterNode
 */
class MissionLaneConverterNodeMock : public MissionLaneConverterNode
{
public:
  explicit MissionLaneConverterNodeMock(const rclcpp::NodeOptions & options)
  : MissionLaneConverterNode(options, false)
  {
    // Mock constructor intentionally does nothing.
  }
};

/**
 * @brief Test the ConvertMissionToTrajectory() function.
 */
TEST_F(MissionLaneConverterTest, TestConvertMissionToTrajectoryExampleInputAndOutput)
{
  rclcpp::NodeOptions options;
  MissionLaneConverterNodeMock mission_converter(options);

  autoware_mapless_planning_msgs::msg::MissionLanesStamped mission_msg;

  // Set target lane to ego lane
  mission_msg.target_lane = 0;

  // Add a driving corridor to the ego lane
  mission_msg.ego_lane = autoware_mapless_planning_msgs::msg::DrivingCorridor();

  // Add points to the ego lane centerline
  mission_msg.ego_lane.centerline.push_back(geometry_msgs::msg::Point());
  mission_msg.ego_lane.centerline.back().x = 0.0;
  mission_msg.ego_lane.centerline.back().y = 0.0;

  // Get converted trajectory
  auto mission_to_trj = mission_converter.ConvertMissionToTrajectory(mission_msg);

  // Extract trajectory
  auto trj_msg = std::get<0>(mission_to_trj);

  EXPECT_EQ(trj_msg.points.back().pose.position.x, mission_msg.ego_lane.centerline.back().x);
  EXPECT_EQ(trj_msg.points.back().pose.position.y, mission_msg.ego_lane.centerline.back().y);
  EXPECT_EQ(trj_msg.points.back().pose.orientation.x, 0.0);
  EXPECT_EQ(trj_msg.points.back().pose.orientation.y, 0.0);
  EXPECT_EQ(trj_msg.points.back().pose.orientation.z, 0.0);
  EXPECT_EQ(trj_msg.points.back().pose.orientation.w, 1.0);

  // TEST 2: some more points on the ego lane
  // add points to the ego lane centerline
  mission_msg.ego_lane.centerline.push_back(geometry_msgs::msg::Point());
  mission_msg.ego_lane.centerline.back().x = 1.0;
  mission_msg.ego_lane.centerline.back().y = 1.0;
  mission_msg.ego_lane.centerline.push_back(geometry_msgs::msg::Point());
  mission_msg.ego_lane.centerline.back().x = 2.0;
  mission_msg.ego_lane.centerline.back().y = 2.0;

  // Convert
  mission_to_trj = mission_converter.ConvertMissionToTrajectory(mission_msg);

  // Extract trajectory
  trj_msg = std::get<0>(mission_to_trj);

  EXPECT_EQ(trj_msg.points.back().pose.position.x, mission_msg.ego_lane.centerline.back().x);
  EXPECT_EQ(trj_msg.points.back().pose.position.y, mission_msg.ego_lane.centerline.back().y);

  // TEST 3: neighbor lane left
  mission_msg.drivable_lanes_left.push_back(autoware_mapless_planning_msgs::msg::DrivingCorridor());
  mission_msg.drivable_lanes_left.back().centerline.push_back(geometry_msgs::msg::Point());
  mission_msg.drivable_lanes_left.back().centerline.back().x = 0.0;
  mission_msg.drivable_lanes_left.back().centerline.back().y = 0.0;
  mission_msg.drivable_lanes_left.back().centerline.push_back(geometry_msgs::msg::Point());
  mission_msg.drivable_lanes_left.back().centerline.back().x = 5.0;
  mission_msg.drivable_lanes_left.back().centerline.back().y = 3.0;

  // Set target lane to neighbor left
  mission_msg.target_lane = -1;

  // Convert
  mission_to_trj = mission_converter.ConvertMissionToTrajectory(mission_msg);

  // Extract trajectory
  trj_msg = std::get<0>(mission_to_trj);

  EXPECT_EQ(
    trj_msg.points.back().pose.position.x,
    mission_msg.drivable_lanes_left.back().centerline.back().x);
  EXPECT_EQ(
    trj_msg.points.back().pose.position.y,
    mission_msg.drivable_lanes_left.back().centerline.back().y);

  // TEST 4: neighbor lane right
  mission_msg.drivable_lanes_right.push_back(
    autoware_mapless_planning_msgs::msg::DrivingCorridor());
  mission_msg.drivable_lanes_right.back().centerline.push_back(geometry_msgs::msg::Point());
  mission_msg.drivable_lanes_right.back().centerline.back().x = 1.0;
  mission_msg.drivable_lanes_right.back().centerline.back().y = 1.2;
  mission_msg.drivable_lanes_right.back().centerline.push_back(geometry_msgs::msg::Point());
  mission_msg.drivable_lanes_right.back().centerline.back().x = 3.0;
  mission_msg.drivable_lanes_right.back().centerline.back().y = 3.6;

  // Set target lane to neighbor right
  mission_msg.target_lane = 1;

  // Convert
  mission_to_trj = mission_converter.ConvertMissionToTrajectory(mission_msg);

  // Extract trajectory
  trj_msg = std::get<0>(mission_to_trj);

  EXPECT_EQ(
    trj_msg.points.back().pose.position.x,
    mission_msg.drivable_lanes_right.back().centerline.back().x);
  EXPECT_EQ(
    trj_msg.points.back().pose.position.y,
    mission_msg.drivable_lanes_right.back().centerline.back().y);
}
}  // namespace autoware::mapless_architecture
