import time
import cv2

from driver.tello_commands import Tello_Commands
from driver.tello_telemetry import Tello_Telemetry
from driver.tello_camera import Tello_Camera

class Milo:
    """Core class to interact with Tello."""

    def __init__(self, init_telemetry=True, init_cam=True):
        self._commands = Tello_Commands('192.168.10.1', 8889)

        if init_telemetry:
            self.telemetry = Tello_Telemetry()
        else:
            self.telemetry = None

        if init_cam:
            if self.enable_cam():
                print('[Milo] Enable Cam: True')
                self.camera = Tello_Camera()
            else:
                print('[Milo] Enable Cam: False')
                self.camera = None
        else:
            self.camera = None   
        
    def __del__(self):
        self._commands.close()
        if self.telemetry is not None:
            self.telemetry.close()
        if self.camera is not None:
            self.camera.close()

    # ----------------------------------------------------------------------------------------------------

    def close(self):
        self._commands.close()
        if self.telemetry is not None:
            self.telemetry.close()
        if self.camera is not None:
            self.camera.close()

    # ----------------------------------------------------------------------------------------------------

    # def is_active_control(self):
    #     return self._commands.get_control_sdk()

    def enable_control(self):
        return self._commands.act_sdk()

    def release_control(self):
        return 'Not implemented'

    def enable_cam(self):
        return self._commands.act_cam(True)

    def disable_cam(self):
        return self._commands.act_cam(False)

    # ----------------------------------------------------------------------------------------------------

    def emergency_stop(self):
        return self._commands.send_cmd('emergency')

    def takeoff(self):
        return self._commands.send_cmd('takeoff') 

    def land(self):
        return self._commands.send_cmd('land') 

    def rc_control(self, roll, pitch, throttle, yaw):
        """
        Send RC control via four channels.

        The Tello API expects roll from -100 to 100, pitch -100 to 100, throttle from -100 to 100, yaw from -100 to 100

        Args:
            roll (int): roll RC.
            pitch (int): pitch RC.
            throttle (int): throttle RC.
            yaw (int): yaw RC.

        Returns:
            True is 'OK' or False is 'FALSE'.
        """
        
        roll = int(roll)
        pitch = int(pitch)
        throttle = int(throttle)
        yaw = int(yaw)

        return self._commands.send_cmd('rc %s %s %s %s' % (roll, pitch, throttle, yaw)) 

    def move(self, direction, distance):
        """
        Moves in a direction for a distance.

        The Tello API expects distances from 20 to 500 centimeters.

        Args:
            direction (str): Direction to move, 'up', 'down', 'forward', 'back', 'right' or 'left'.
            distance (int): Distance to move.

        Returns:
            True is 'OK' or False is 'FALSE'.
        """
        distance = int(distance)

        return self._commands.send_cmd('%s %s' % (direction, distance)) 

    def rotate(self, direction, angle):
        """
        Rotates in a direction for a angle.

        The Tello API expects angle from 1 to 360 degrees (CHECK IF THE API DOC IS CORRECT OR NOT BECAUSE IT SAID 1 TO 3600).

        Args:
            direction (str): Direction to rotate, 'cw', 'ccw'.
            angle (int): degrees to rotate.

        Returns:
            True is 'OK' or False is 'FALSE'.
        """

        angle = int(angle)

        return self._commands.send_cmd('%s %s' % (direction, angle)) 

def main():
    print('Init test Milo-Core')
    piojo = Milo(True, True) 
    cv2.namedWindow('Tello', cv2.WINDOW_AUTOSIZE)
    while True:
        try: 
            print(piojo.telemetry.get_battery())
            # print(piojo.is_active_control())
            frame = piojo.camera.get_frame()
            if frame is None or frame.size == 0:
                continue 
            cv2.imshow('Tello', frame)
            cv2.waitKey(1)
        except KeyboardInterrupt:
            print ('\n Exit Test \n')
            piojo.close()  
            break

if __name__ == "__main__":
    main()