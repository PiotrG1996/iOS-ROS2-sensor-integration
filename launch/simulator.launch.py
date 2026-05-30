from launch import LaunchDescription
from launch_ros.actions import Node
import os

def load_file(package_dir, relative_path):
    path = os.path.join(package_dir, relative_path)
    with open(path, 'r') as fh:
        return fh.read()

def generate_launch_description():
    this_dir = os.path.dirname(__file__)
    urdf_path = os.path.join(this_dir, '..', 'urdf', 'iphone.urdf')
    robot_desc = ''
    try:
        with open(urdf_path, 'r') as f:
            robot_desc = f.read()
    except Exception:
        robot_desc = ''

    simulator = Node(
        package='sensorstream_driver',
        executable='iphone_simulator',
        name='iphone_simulator',
        output='screen'
    )

    rsp = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{'robot_description': robot_desc}]
    )

    return LaunchDescription([
        simulator,
        rsp,
    ])
