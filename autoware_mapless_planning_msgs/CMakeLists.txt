cmake_minimum_required(VERSION 3.5)
project(autoware_mapless_planning_msgs)

find_package(ament_cmake_auto REQUIRED)
ament_auto_find_build_dependencies()

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/DrivingCorridor.msg"
  "msg/MissionLanesStamped.msg"
  "msg/Mission.msg"
  "msg/Segment.msg"
  "msg/RoadSegments.msg"
  "msg/Linestring.msg"
  "msg/LocalMap.msg"
  DEPENDENCIES
    geometry_msgs
    std_msgs
    nav_msgs
  # ADD_LINTER_TESTS
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_auto_package()
