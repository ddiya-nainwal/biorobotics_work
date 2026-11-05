#!/usr/bin/env python3
"""
Serpenoid gait node for snakelib (ROS2 Jazzy).

Computes joint angles with the compound serpenoid equations
(Tesch et al., "Parameterized and Scripted Gaits for Modular Snake Robots")
and publishes them to the pybullet_interface.

Run (with the sim already launched):
    ros2 run snake_gaits gait_node --ros-args -p gait:=sidewinding
Switch gait live:
    ros2 param set /gait_node gait rolling

ASSUMPTION TO VERIFY: /snake/joint_commands uses sensor_msgs/JointState.
Check with `ros2 topic list -t`. If it's a snakelib_msgs type, change the
import, the create_publisher type, and the fields filled in _tick().
"""
import math

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState

# Starting-point parameters -- tune these in sim.
# e_* = even-indexed joints, o_* = odd-indexed joints.
# A: amplitude (rad), Om: spatial freq (rad/module), w: temporal freq (rad/s),
# b: offset (rad), delta: phase between the two waves (rad)
GAITS = {
    "sidewinding": dict(e_A=0.6, e_Om=0.8, e_w=2.0, e_b=0.0,
                        o_A=0.6, o_Om=0.8, o_w=2.0, o_b=0.0, delta=math.pi / 4),
    "rolling":     dict(e_A=0.4, e_Om=0.0, e_w=2.0, e_b=0.0,
                        o_A=0.4, o_Om=0.0, o_w=2.0, o_b=0.0, delta=math.pi / 2),
    "lateral_undulation": dict(e_A=0.8, e_Om=0.6, e_w=2.0, e_b=0.0,
                               o_A=0.0, o_Om=0.0, o_w=0.0, o_b=0.0, delta=0.0),
    "linear_progression": dict(e_A=0.0, e_Om=0.0, e_w=0.0, e_b=0.0,
                               o_A=0.6, o_Om=1.2, o_w=2.0, o_b=0.0, delta=0.0),
    # Static test: constant bend on even joints. Should form a smooth C-arc
    # if the sign flip is right; a zig-zag means it's wrong.
    "sign_test":   dict(e_A=0.0, e_Om=0.0, e_w=0.0, e_b=0.3,
                        o_A=0.0, o_Om=0.0, o_w=0.0, o_b=0.0, delta=0.0),
}


class GaitNode(Node):
    def __init__(self):
        super().__init__("gait_node")
        self.declare_parameter("gait", "sidewinding")
        self.declare_parameter("rate_hz", 50.0)
        self.declare_parameter("apply_sign_flip", True)
        self.declare_parameter("speed_scale", 1.0)   # scales temporal freq

        self.joint_names = None
        self.t0 = self.get_clock().now()

        self.create_subscription(JointState, "/snake/joint_states", self._on_states, 10)
        self.pub = self.create_publisher(JointState, "/snake/joint_commands", 10)

        rate = self.get_parameter("rate_hz").value
        self.create_timer(1.0 / rate, self._tick)
        self.get_logger().info("Waiting for /snake/joint_states to learn joint names...")

    def _on_states(self, msg: JointState):
        if self.joint_names is None and msg.name:
            self.joint_names = list(msg.name)
            self.get_logger().info(f"Found {len(self.joint_names)} joints: {self.joint_names}")

    def serpenoid(self, n: int, t: float, p: dict) -> float:
        """Compound serpenoid angle for joint n at time t."""
        s = self.get_parameter("speed_scale").value
        if n % 2 == 0:
            angle = p["e_b"] + p["e_A"] * math.sin(p["e_Om"] * n + s * p["e_w"] * t)
        else:
            angle = p["o_b"] + p["o_A"] * math.sin(p["o_Om"] * n + s * p["o_w"] * t + p["delta"])

        # Joint n is anti-parallel to joint n-2 -> flip every other same-type joint.
        if self.get_parameter("apply_sign_flip").value:
            angle *= (-1) ** (n // 2)
        return angle

    def _tick(self):
        if self.joint_names is None:
            return
        gait = self.get_parameter("gait").value
        if gait not in GAITS:
            self.get_logger().warn(f"Unknown gait '{gait}'. Options: {list(GAITS)}",
                                   throttle_duration_sec=2.0)
            return

        t = (self.get_clock().now() - self.t0).nanoseconds * 1e-9
        p = GAITS[gait]

        msg = JointState()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.name = self.joint_names
        msg.position = [self.serpenoid(n, t, p) for n in range(len(self.joint_names))]
        self.pub.publish(msg)


def main():
    rclpy.init()
    node = GaitNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
