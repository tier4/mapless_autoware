std_msgs/Header header
int16 target_lane # The target/goal lane where we want to drive.
DrivingCorridor lane_with_goal_point # The lane containing the goal point.
DrivingCorridor ego_lane # The lane where the ego vehicle is located.
DrivingCorridor[] drivable_lanes_left # All the drivable lanes to the left.
DrivingCorridor[] drivable_lanes_right # All the drivable lanes to the right.
float32 deadline_target_lane # Spatial deadline parameter (meters), the target lane should be reached after this number of meters
