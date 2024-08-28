std_msgs/Header header
DrivingCorridor lane_with_goal_point # The lane containing the goal point.
DrivingCorridor ego_lane # The lane where the ego vehicle is located.
DrivingCorridor[] drivable_lanes_left # All the drivable lanes to the left.
DrivingCorridor[] drivable_lanes_right # All the drivable lanes to the right.
float32 deadline_target_lane # Spatial deadline parameter (meters), the target lane should be reached after this number of meters
int16 target_lane # The target/goal lane where we want to drive.

# Constants to represent lane change actions (target_lane)
int16 LANE_KEEP=0
int16 LANE_CHANGE_LEFT=-1
int16 LANE_CHANGE_RIGHT=1
int16 TAKE_NEXT_EXIT_LEFT=-2
int16 TAKE_NEXT_EXIT_RIGHT=2
