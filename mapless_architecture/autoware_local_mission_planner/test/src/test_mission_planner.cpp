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

#include "autoware/local_mission_planner/mission_planner_node.hpp"
#include "autoware/local_mission_planner_common/helper_functions.hpp"
#include "gtest/gtest.h"

#include "geometry_msgs/msg/pose.hpp"

#include <iostream>

namespace autoware::mapless_architecture
{
// Note: Lanelets and Segments are basically the same!

/**
 * @brief Mock class for MissionPlannerNode
 */
class MissionPlannerNodeMock : public MissionPlannerNode
{
public:
  explicit MissionPlannerNodeMock(const rclcpp::NodeOptions & options)
  : MissionPlannerNode(options, false)
  {
    // Mock constructor intentionally does nothing.
  }

  // Setter for mission
  void SetMission(const Direction & mission) { mission_ = mission; }
};

autoware_mapless_planning_msgs::msg::RoadSegments CreateSegments()
{
  // Local variables
  const int n_segments = 3;

  // Fill lanelet2 message
  autoware_mapless_planning_msgs::msg::RoadSegments message;
  message.segments.resize(n_segments);  // Initialize segments vector with n_segments

  // Initialize each segment's linestrings and poses
  for (int i = 0; i < n_segments; ++i) {
    // Initialize each linestring and poses within each segment
    for (int j = 0; j < 2; ++j) {                          // Assuming 2 linestrings per segment
      message.segments[i].linestrings[j].poses.resize(2);  // Assuming 2 poses per linestring
    }
  }

  // Global position
  geometry_msgs::msg::PoseStamped msg_global_pose;
  msg_global_pose.header.frame_id = "base_link";

  message.segments[0].id = 0;
  message.segments[1].id = 1;
  message.segments[2].id = 2;

  tf2::Quaternion quaternion;

  // Vehicle position in global cosy
  message.pose.position.x = 0.0;
  message.pose.position.y = 0.0;
  quaternion.setRPY(0, 0, 0);
  message.pose.orientation.x = quaternion.getX();
  message.pose.orientation.y = quaternion.getY();
  message.pose.orientation.z = quaternion.getZ();
  message.pose.orientation.w = quaternion.getW();

  // Street layout in current local cosy = global cosy since vehicle position is
  // at the origin in the global cosy
  message.segments[0].linestrings[0].poses[0].position.x = -2.0;
  message.segments[0].linestrings[0].poses[0].position.y = -0.5;
  message.segments[0].linestrings[0].poses[0].position.z = 0.0;
  message.segments[0].linestrings[0].poses[1].position.x = 10.0;
  message.segments[0].linestrings[0].poses[1].position.y = -0.5;
  message.segments[0].linestrings[0].poses[1].position.z = 0.0;

  message.segments[0].linestrings[1].poses[0].position.x = -2.0;
  message.segments[0].linestrings[1].poses[0].position.y = 0.5;
  message.segments[0].linestrings[1].poses[0].position.z = 0.0;
  message.segments[0].linestrings[1].poses[1].position.x = 10.0;
  message.segments[0].linestrings[1].poses[1].position.y = 0.5;
  message.segments[0].linestrings[1].poses[1].position.z = 0.0;

  message.segments[1].linestrings[0].poses[0].position.x = 0.0;
  message.segments[1].linestrings[0].poses[0].position.y = 1.0;
  message.segments[1].linestrings[0].poses[0].position.z = 0.0;
  message.segments[1].linestrings[0].poses[1].position.x = 10.0;
  message.segments[1].linestrings[0].poses[1].position.y = 1.0;
  message.segments[1].linestrings[0].poses[1].position.z = 0.0;

  message.segments[1].linestrings[1].poses[0].position.x = 0.0;
  message.segments[1].linestrings[1].poses[0].position.y = 2.0;
  message.segments[1].linestrings[1].poses[0].position.z = 0.0;
  message.segments[1].linestrings[1].poses[1].position.x = 10.0;
  message.segments[1].linestrings[1].poses[1].position.y = 2.0;
  message.segments[1].linestrings[1].poses[1].position.z = 0.0;

  message.segments[2].linestrings[0].poses[0].position.x = 10.0;
  message.segments[2].linestrings[0].poses[0].position.y = -0.5;
  message.segments[2].linestrings[0].poses[0].position.z = 0.0;
  message.segments[2].linestrings[0].poses[1].position.x = 20.0;
  message.segments[2].linestrings[0].poses[1].position.y = -0.5;
  message.segments[2].linestrings[0].poses[1].position.z = 0.0;

  message.segments[2].linestrings[1].poses[0].position.x = 10.0;
  message.segments[2].linestrings[1].poses[0].position.y = 0.5;
  message.segments[2].linestrings[1].poses[0].position.z = 0.0;
  message.segments[2].linestrings[1].poses[1].position.x = 20.0;
  message.segments[2].linestrings[1].poses[1].position.y = 0.5;
  message.segments[2].linestrings[1].poses[1].position.z = 0.0;

  // Define connections
  message.segments[0].neighboring_segment_id = {0, 1};
  message.segments[1].neighboring_segment_id = {0, 1};
  message.segments[2].neighboring_segment_id = {-1, -1};
  message.segments[0].successor_segment_id = {2};
  message.segments[1].successor_segment_id = {-1};
  message.segments[2].successor_segment_id = {-1};

  return message;
}

autoware_mapless_planning_msgs::msg::RoadSegments GetTestRoadModelForRecenterTests()
{
  // Local variables
  const int n_segments = 2;

  // Fill lanelet2 message
  autoware_mapless_planning_msgs::msg::RoadSegments message;
  message.segments.resize(n_segments);  // Initialize segments vector with n_segments

  // Initialize each segment's linestrings and poses
  for (int i = 0; i < n_segments; ++i) {
    // Initialize each linestring and poses within each segment
    for (int j = 0; j < 2; ++j) {                          // Assuming 2 linestrings per segment
      message.segments[i].linestrings[j].poses.resize(2);  // Assuming 2 poses per linestring
    }
  }

  // Global position
  geometry_msgs::msg::PoseStamped msg_global_pose;
  msg_global_pose.header.frame_id = "base_link";

  message.segments[0].id = 0;
  message.segments[1].id = 1;

  tf2::Quaternion quaternion;

  // Vehicle position in global cosy
  message.pose.position.x = 0.0;
  message.pose.position.y = 0.0;
  quaternion.setRPY(0, 0, 0);
  message.pose.orientation.x = quaternion.getX();
  message.pose.orientation.y = quaternion.getY();
  message.pose.orientation.z = quaternion.getZ();
  message.pose.orientation.w = quaternion.getW();

  // Street layout in current local cosy = global cosy since vehicle position is
  // at the origin in the global cosy
  message.segments[0].linestrings[0].poses[0].position.x = 0.0;
  message.segments[0].linestrings[0].poses[0].position.y = -0.5;
  message.segments[0].linestrings[0].poses[0].position.z = 0.0;
  message.segments[0].linestrings[0].poses[1].position.x = 10.0;
  message.segments[0].linestrings[0].poses[1].position.y = -0.5;
  message.segments[0].linestrings[0].poses[1].position.z = 0.0;

  message.segments[0].linestrings[1].poses[0].position.x = 0.0;
  message.segments[0].linestrings[1].poses[0].position.y = 0.5;
  message.segments[0].linestrings[1].poses[0].position.z = 0.0;
  message.segments[0].linestrings[1].poses[1].position.x = 10.0;
  message.segments[0].linestrings[1].poses[1].position.y = 0.5;
  message.segments[0].linestrings[1].poses[1].position.z = 0.0;

  message.segments[1].linestrings[0].poses[0].position.x = 0.0;
  message.segments[1].linestrings[0].poses[0].position.y = 1.0;
  message.segments[1].linestrings[0].poses[0].position.z = 0.0;
  message.segments[1].linestrings[0].poses[1].position.x = 10.0;
  message.segments[1].linestrings[0].poses[1].position.y = 1.0;
  message.segments[1].linestrings[0].poses[1].position.z = 0.0;

  message.segments[1].linestrings[1].poses[0].position.x = 0.0;
  message.segments[1].linestrings[1].poses[0].position.y = 2.0;
  message.segments[1].linestrings[1].poses[0].position.z = 0.0;
  message.segments[1].linestrings[1].poses[1].position.x = 10.0;
  message.segments[1].linestrings[1].poses[1].position.y = 2.0;
  message.segments[1].linestrings[1].poses[1].position.z = 0.0;

  int16_t neigh_1 = 1;
  std::vector<int16_t> neighboring_ids = {neigh_1};
  message.segments[0].neighboring_segment_id = neighboring_ids;
  std::vector<int16_t> neighboring_ids_2 = {};
  message.segments[1].neighboring_segment_id = neighboring_ids_2;

  return message;
}

std::tuple<std::vector<lanelet::Lanelet>, std::vector<LaneletConnection>> CreateLane()
{
  // Local variables
  const int n_segments = 2;

  // Fill lanelet2 message
  autoware_mapless_planning_msgs::msg::RoadSegments message;
  message.segments.resize(n_segments);  // Initialize segments vector with n_segments

  // Initialize each segment's linestrings and poses
  for (int i = 0; i < n_segments; ++i) {
    // Initialize each linestring and poses within each segment
    for (int j = 0; j < 2; ++j) {                          // Assuming 2 linestrings per segment
      message.segments[i].linestrings[j].poses.resize(2);  // Assuming 2 poses per linestring
    }
  }

  // Global position
  geometry_msgs::msg::PoseStamped msg_global_pose;
  msg_global_pose.header.frame_id = "base_link";

  message.segments[0].id = 0;
  message.segments[1].id = 1;

  tf2::Quaternion quaternion;

  // Vehicle position in global cosy
  message.pose.position.x = 0.0;
  message.pose.position.y = 0.0;
  quaternion.setRPY(0, 0, 0);
  message.pose.orientation.x = quaternion.getX();
  message.pose.orientation.y = quaternion.getY();
  message.pose.orientation.z = quaternion.getZ();
  message.pose.orientation.w = quaternion.getW();

  // Street layout in current local cosy = global cosy since vehicle position is
  // at the origin in the global cosy
  message.segments[0].linestrings[0].poses[0].position.x = -2.0;
  message.segments[0].linestrings[0].poses[0].position.y = -0.5;
  message.segments[0].linestrings[0].poses[0].position.z = 0.0;
  message.segments[0].linestrings[0].poses[1].position.x = 10.0;
  message.segments[0].linestrings[0].poses[1].position.y = -0.5;
  message.segments[0].linestrings[0].poses[1].position.z = 0.0;

  message.segments[0].linestrings[1].poses[0].position.x = -2.0;
  message.segments[0].linestrings[1].poses[0].position.y = 0.5;
  message.segments[0].linestrings[1].poses[0].position.z = 0.0;
  message.segments[0].linestrings[1].poses[1].position.x = 10.0;
  message.segments[0].linestrings[1].poses[1].position.y = 0.5;
  message.segments[0].linestrings[1].poses[1].position.z = 0.0;

  message.segments[1].linestrings[0].poses[0].position.x = 10.0;
  message.segments[1].linestrings[0].poses[0].position.y = -0.5;
  message.segments[1].linestrings[0].poses[0].position.z = 0.0;
  message.segments[1].linestrings[0].poses[1].position.x = 20.0;
  message.segments[1].linestrings[0].poses[1].position.y = -0.5;
  message.segments[1].linestrings[0].poses[1].position.z = 0.0;

  message.segments[1].linestrings[1].poses[0].position.x = 10.0;
  message.segments[1].linestrings[1].poses[0].position.y = 0.5;
  message.segments[1].linestrings[1].poses[0].position.z = 0.0;
  message.segments[1].linestrings[1].poses[1].position.x = 20.0;
  message.segments[1].linestrings[1].poses[1].position.y = 0.5;
  message.segments[1].linestrings[1].poses[1].position.z = 0.0;

  // Define connections
  message.segments[0].successor_segment_id = {1};
  message.segments[0].neighboring_segment_id = {-1, -1};
  message.segments[1].successor_segment_id = {-1};
  message.segments[1].neighboring_segment_id = {-1, -1};

  // Initialize MissionPlannerNodeMock
  rclcpp::NodeOptions options;
  MissionPlannerNodeMock mission_planner(options);

  // Output
  std::vector<LaneletConnection> lanelet_connections;
  std::vector<lanelet::Lanelet> converted_lanelets;

  mission_planner.ConvertInput2LaneletFormat(message, converted_lanelets, lanelet_connections);
  return std::make_tuple(converted_lanelets, lanelet_connections);
}

/**
 * @brief Test distance between point and LineString calculation.
 *
 * This function tests CalculateDistanceBetweenPointAndLineString_() which
 * calculates the distance between a point and a LineString.
 */
TEST(MissionPlanner, CalculateDistanceBetweenPointAndLineString)
{
  // Initialize ROS 2
  rclcpp::init(0, nullptr);

  // Create an example Linestring
  lanelet::LineString2d linestring;

  lanelet::Point2d p1(0, 1.0,
                      0.0);  // First argument is ID (set it to 0 for now)
  linestring.push_back(p1);
  lanelet::Point2d p2(0, 2.0,
                      0.0);  // First argument is ID (set it to 0 for now)
  linestring.push_back(p2);
  lanelet::Point2d p3(0, 3.0,
                      0.0);  // First argument is ID (set it to 0 for now)
  linestring.push_back(p3);

  // Create an example point
  lanelet::BasicPoint2d point(1.0, 1.0);

  // Initialize MissionPlannerNodeMock
  rclcpp::NodeOptions options;
  MissionPlannerNodeMock mission_planner(options);

  // Run function
  double distance = mission_planner.CalculateDistanceBetweenPointAndLineString(linestring, point);

  // Check if distance is near to 1.0 (with allowed error of 0.001)
  EXPECT_NEAR(distance, 1.0, 0.001);

  // Shutdown ROS 2
  rclcpp::shutdown();
}

/**
 * @brief Test GetPointOnLane_() function.
 *
 * This function tests GetPointOnLane_() which
 * returns a point on a given lane in a given distance (x axis).
 */
TEST(MissionPlanner, GetPointOnLane)
{
  // Initialize ROS 2
  rclcpp::init(0, nullptr);

  // Create some example segments
  autoware_mapless_planning_msgs::msg::RoadSegments road_segments = CreateSegments();

  // Initialize MissionPlannerNodeMock
  rclcpp::NodeOptions options;
  MissionPlannerNodeMock mission_planner(options);

  // Convert road model
  std::vector<LaneletConnection> lanelet_connections;
  std::vector<lanelet::Lanelet> lanelets;

  mission_planner.ConvertInput2LaneletFormat(road_segments, lanelets, lanelet_connections);

  // Get a point from the tested function which has the x value 3.0 and lies on
  // the centerline of the lanelets
  lanelet::BasicPoint2d point = mission_planner.GetPointOnLane({0, 1}, 3.0, lanelets);

  // Check if x value of the point is near to 3.0 (with an allowed error of
  // 0.001)
  EXPECT_NEAR(point.x(), 3.0, 0.001);

  // Get a point from the tested function which has the x value 100.0 and lies
  // on the centerline of the lanelets
  point = mission_planner.GetPointOnLane({0, 1}, 100.0,
                                         lanelets);  // Far away (100m)

  // Check if x value of the point is near to 10.0 (with an allowed error of
  // 0.001)
  EXPECT_NEAR(point.x(), 10.0, 0.001);

  // Shutdown ROS 2
  rclcpp::shutdown();
}

/**
 * @brief Test RecenterGoalPoint() function.
 */
TEST(MissionPlanner, RecenterGoalPoint)
{
  // Initialize ROS 2
  rclcpp::init(0, nullptr);

  // Initialize MissionPlannerNodeMock
  rclcpp::NodeOptions options;
  MissionPlannerNodeMock mission_planner(options);

  // Get a local road model for testing
  autoware_mapless_planning_msgs::msg::RoadSegments road_segments =
    GetTestRoadModelForRecenterTests();

  // Used for the output
  std::vector<LaneletConnection> lanelet_connections;
  std::vector<lanelet::Lanelet> converted_lanelets;

  mission_planner.ConvertInput2LaneletFormat(
    road_segments, converted_lanelets, lanelet_connections);

  // TEST 1: point on centerline of origin lanelet
  // define a test point and re-center onto its centerline
  lanelet::BasicPoint2d goal_point(0.0, 0.0);
  lanelet::BasicPoint2d goal_point_recentered = RecenterGoalPoint(goal_point, converted_lanelets);

  EXPECT_NEAR(goal_point_recentered.x(), 0.0, 1e-5);
  EXPECT_NEAR(goal_point_recentered.y(), 0.0, 1e-5);

  // TEST 2: point left to centerline of origin lanelet
  // define a test point which is not on the centerline
  goal_point.x() = 1.0;
  goal_point.y() = 0.25;
  goal_point_recentered = RecenterGoalPoint(goal_point, converted_lanelets);

  // Expect re-centered point to lie on y coordinate 0
  EXPECT_NEAR(goal_point_recentered.x(), goal_point.x(), 1e-5);
  EXPECT_NEAR(goal_point_recentered.y(), 0.0, 1e-5);

  // TEST 3: point right to centerline of origin lanelet
  // define a test point which is not on the centerline
  goal_point.x() = 8.0;
  goal_point.y() = -0.2;
  goal_point_recentered = RecenterGoalPoint(goal_point, converted_lanelets);

  // Expect re-centered point to lie on y coordinate 0 but with x coord equal to
  // goal_point (removes lateral error/noise)
  EXPECT_NEAR(goal_point_recentered.x(), goal_point.x(), 1e-5);
  EXPECT_NEAR(goal_point_recentered.y(), 0.0, 1e-5);

  // TEST 4: point left of centerline of neighboring lanelet -> tests if correct
  // lanelet is selected for re-centering define a test point which is not on
  // the centerline
  goal_point.x() = 2.0;
  goal_point.y() = 1.75;
  goal_point_recentered = RecenterGoalPoint(goal_point, converted_lanelets);

  // Expect re-centered point to lie on y coordinate 0 but with x coord equal to
  // goal_point (removes lateral error/noise)
  EXPECT_NEAR(goal_point_recentered.x(), goal_point.x(), 1e-5);
  EXPECT_NEAR(goal_point_recentered.y(), 1.5, 1e-5);

  // TEST 5: point outside of local road model, goal point should remain
  // untouched
  goal_point.x() = 11.0;
  goal_point.y() = -2.0;
  goal_point_recentered = RecenterGoalPoint(goal_point, converted_lanelets);

  EXPECT_EQ(goal_point_recentered.x(), goal_point.x());
  EXPECT_EQ(goal_point_recentered.y(), goal_point.y());

  // Shutdown ROS 2
  rclcpp::shutdown();
}

/**
 * @brief Test IsOnGoalLane_() function.
 *
 * This function tests IsOnGoalLane_() which
 * returns true if the vehicle is on the goal lane (defined by the goal point).
 */
TEST(MissionPlanner, IsOnGoalLane)
{
  // Initialize ROS 2
  rclcpp::init(0, nullptr);

  // Create some example segments
  autoware_mapless_planning_msgs::msg::RoadSegments road_segments = CreateSegments();

  // Initialize MissionPlannerNodeMock
  rclcpp::NodeOptions options;
  MissionPlannerNodeMock mission_planner(options);

  // Convert road model
  std::vector<LaneletConnection> lanelet_connections;
  std::vector<lanelet::Lanelet> lanelets;

  mission_planner.ConvertInput2LaneletFormat(road_segments, lanelets, lanelet_connections);

  // Define a point with x = 1.0 and y = 0.0
  lanelet::BasicPoint2d point(1.0, 0.0);

  // Check if the point is on the lane: should be true
  EXPECT_EQ(mission_planner.IsOnGoalLane(0, point, lanelets, lanelet_connections), true);

  // Define a point with x = 100.0 and y = 100.0
  lanelet::BasicPoint2d point2(100.0, 100.0);

  // Check if the point is on the lane: should be false
  EXPECT_EQ(mission_planner.IsOnGoalLane(0, point2, lanelets, lanelet_connections), false);

  // Define a point with x = 15.0 and y = 0.0
  lanelet::BasicPoint2d point3(15.0, 0.0);

  // Check if the point is on the lane: should be true
  EXPECT_EQ(mission_planner.IsOnGoalLane(0, point3, lanelets, lanelet_connections), true);

  // Shutdown ROS 2
  rclcpp::shutdown();
}

/**
 * @brief Test CheckIfGoalPointShouldBeReset_() function.
 *
 * If the x value of the goal point is negative, the goal point should be reset.
 */
TEST(MissionPlanner, CheckIfGoalPointShouldBeReset)
{
  // Initialize ROS 2
  rclcpp::init(0, nullptr);

  // Create some example segments
  autoware_mapless_planning_msgs::msg::RoadSegments road_segments = CreateSegments();

  autoware_mapless_planning_msgs::msg::LocalMap local_map;
  local_map.road_segments = road_segments;

  // Initialize MissionPlannerNodeMock
  rclcpp::NodeOptions options;
  MissionPlannerNodeMock mission_planner(options);

  // Convert road model
  std::vector<LaneletConnection> lanelet_connections;
  std::vector<lanelet::Lanelet> lanelets;

  mission_planner.ConvertInput2LaneletFormat(road_segments, lanelets, lanelet_connections);

  // TEST 1: check if goal point is reset in non-default mission
  // Define a goal point with negative x value
  lanelet::BasicPoint2d point(-1.0, 0.0);
  mission_planner.goal_point(point);

  // Set a non-default mission to make the goal point reset work
  mission_planner.SetMission(left);

  // Call function which is tested
  mission_planner.CheckIfGoalPointShouldBeReset(lanelets, lanelet_connections);

  // Check if the goal point is reset
  EXPECT_EQ(
    mission_planner.goal_point().x(),
    20);  // Projection is to x = 10, since this is the highest x value
          // on the right neighbor lane!

  // TEST 2: check if goal point reset is skipped in default mission
  mission_planner.goal_point(point);

  // Set a default mission to make the goal point reset work
  mission_planner.SetMission(stay);

  // Call function which is tested
  mission_planner.CheckIfGoalPointShouldBeReset(lanelets, lanelet_connections);

  // Check if the goal point is reset
  EXPECT_EQ(
    mission_planner.goal_point().x(),
    point.x());  // goal point should equal the input point since goal point
                 // is not reset in the default mission (ego lane following)

  // Shutdown ROS 2
  rclcpp::shutdown();
}

/**
 * @brief Test CalculateLanes_() function.
 */
TEST(MissionPlanner, CalculateLanes)
{
  // Initialize ROS 2
  rclcpp::init(0, nullptr);

  // Create some example lanelets: Two lanelets 0 and 1, 1 is the successor of
  // 0, the ego lanelet is 0
  auto tuple = CreateLane();
  std::vector<lanelet::Lanelet> lanelets = std::get<0>(tuple);
  std::vector<LaneletConnection> lanelet_connections = std::get<1>(tuple);

  // Initialize MissionPlannerNodeMock
  rclcpp::NodeOptions options;
  MissionPlannerNodeMock mission_planner(options);

  // Call function which is tested
  Lanes result = mission_planner.CalculateLanes(lanelets, lanelet_connections);

  // Get lanes
  std::vector<int> ego_lane_idx = result.ego;
  std::vector<int> left_lane_idx = result.left[0];
  std::vector<int> right_lane_idx = result.right[0];

  // Check if left lane is empty
  EXPECT_EQ(left_lane_idx.empty(), true);  // Expect empty lane

  // Check if right lane is empty
  EXPECT_EQ(right_lane_idx.empty(), true);  // Expect empty lane

  // Check if ego lane is correct
  EXPECT_EQ(
    ego_lane_idx[0],
    0);  // Expect ego lane = {0, 1} (1 is index of successor of ego lanelet)

  EXPECT_EQ(
    ego_lane_idx[1],
    1);  // Expect ego lane = {0, 1} (1 is index of successor of ego lanelet)

  // Shutdown ROS 2
  rclcpp::shutdown();
}

/**
 * @brief Test CreateMarkerArray_() function.
 */
TEST(MissionPlanner, CreateMarkerArray)
{
  // Initialize ROS 2
  rclcpp::init(0, nullptr);

  // Create some example lanelets
  auto tuple = CreateLane();
  std::vector<lanelet::Lanelet> lanelets = std::get<0>(tuple);
  std::vector<LaneletConnection> lanelet_connections = std::get<1>(tuple);

  // Initialize MissionPlannerNodeMock
  rclcpp::NodeOptions options;
  MissionPlannerNodeMock mission_planner(options);

  // Create empty message
  autoware_mapless_planning_msgs::msg::RoadSegments message;

  // Calculate centerlines, left and right bounds
  std::vector<lanelet::ConstLineString3d> centerlines;
  std::vector<lanelet::ConstLineString3d> left_bounds;
  std::vector<lanelet::ConstLineString3d> right_bounds;

  for (const lanelet::Lanelet & l : lanelets) {
    auto centerline = l.centerline();
    auto bound_left = l.leftBound();
    auto bound_right = l.rightBound();

    centerlines.push_back(centerline);
    left_bounds.push_back(bound_left);
    right_bounds.push_back(bound_right);
  }

  // Call function which is tested
  auto marker_array = CreateMarkerArray(centerlines, left_bounds, right_bounds, message);

  // Check if marker_array is empty
  EXPECT_EQ(marker_array.markers.empty(),
            false);  // Expect not empty marker_array

  // Check first point of marker array (should be first point on the centerline)
  EXPECT_EQ(marker_array.markers[0].points[0].x,
            -2.0);  // Expect -2.0 for x

  // Check first point of marker array (should be first point on the centerline)
  EXPECT_EQ(marker_array.markers[0].points[0].y,
            0.0);  // Expect 0.0 for y

  // Shutdown ROS 2
  rclcpp::shutdown();
}

/**
 * @brief Test CreateDrivingCorridor_() function.
 */
TEST(MissionPlanner, CreateDrivingCorridor)
{
  // Initialize ROS 2
  rclcpp::init(0, nullptr);

  // Create some example lanelets
  auto tuple = CreateLane();
  std::vector<lanelet::Lanelet> lanelets = std::get<0>(tuple);
  std::vector<LaneletConnection> lanelet_connections = std::get<1>(tuple);

  // Initialize MissionPlannerNodeMock
  rclcpp::NodeOptions options;
  MissionPlannerNodeMock mission_planner(options);

  // Call function which is tested
  autoware_mapless_planning_msgs::msg::DrivingCorridor driving_corridor =
    CreateDrivingCorridor({0, 1}, lanelets);

  // Check if x value of first point in centerline is -2.0
  EXPECT_EQ(driving_corridor.centerline[0].x, -2.0);

  // Check if y value of first point in centerline is 0.0
  EXPECT_EQ(driving_corridor.centerline[0].y, 0.0);

  // Check if x value of first point in left bound is -2.0
  EXPECT_EQ(driving_corridor.bound_left[0].x, -2.0);

  // Check if y value of first point in left bound is -0.5
  EXPECT_EQ(driving_corridor.bound_left[0].y, -0.5);

  // Check if x value of first point in right bound is -2.0
  EXPECT_EQ(driving_corridor.bound_right[0].x, -2.0);

  // Check if y value of first point in right bound is 0.5
  EXPECT_EQ(driving_corridor.bound_right[0].y, 0.5);

  // Shutdown ROS 2
  rclcpp::shutdown();
}

}  // namespace autoware::mapless_architecture
