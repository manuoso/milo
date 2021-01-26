"""
    Some methods are based on: https://github.com/dji-sdk/Tello-Python
"""

import socket
import threading
import time

class Tello_Telemetry:
    def __init__(self):
        self._fin = False
        self._response = None 

        self._angles = [0.0, 0.0, 0.0]
        self._velocity = [0.0, 0.0, 0.0]
        self._temp = [0.0, 0.0]
        self._tof = 0.0
        self._height = 0.0
        self._battery = 0.0
        self._baro = 0.0
        self._flytime = 0.0
        self._acceleration = [0.0, 0.0, 0.0]

        self._socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._socket.bind(('0.0.0.0', 8890))

        self._recv_thread = threading.Thread(target=self.__receive_thread)
        self._recv_thread.daemon = True
        self._recv_thread.start()

    def __del__(self):
        self._socket.close()

    # ----------------------------------------------------------------------------------------------------

    def __receive_thread(self):
        while not self._fin:
            try:
                self._response, _ = self._socket.recvfrom(3000)
                self.__decode_response(self._response.decode('utf-8'))
                # print(self.response)
                # Example response raw: b'pitch:0;roll:0;yaw:0;vgx:0;vgy:0;vgz:0;templ:72;temph:74;tof:10;h:0;bat:93;baro:-42.45;time:0;agx:0.00;agy:-2.00;agz:-999.00;\r\n'
                # Example response with decode: pitch:0;roll:0;yaw:0;vgx:0;vgy:0;vgz:0;templ:53;temph:55;tof:10;h:0;bat:94;baro:-42.44;time:0;agx:-4.00;agy:-1.00;agz:-998.00;
            except socket.error as exc:
                print ("Caught exception socket.error : %s" % exc)

    def __decode_response(self, response):
        telem_all = response.split(';')
        if telem_all[0].split(':')[0] == 'pitch':
            self._angles = [float(telem_all[0].split(':')[1]), float(telem_all[1].split(':')[1]), float(telem_all[2].split(':')[1])]
            self._velocity = [float(telem_all[3].split(':')[1]), float(telem_all[4].split(':')[1]), float(telem_all[5].split(':')[1])]
            self._temp = [float(telem_all[6].split(':')[1]), float(telem_all[7].split(':')[1])]
            self._tof = float(telem_all[8].split(':')[1])
            self._height = float(telem_all[9].split(':')[1])
            self._battery = float(telem_all[10].split(':')[1])
            self._baro = float(telem_all[11].split(':')[1])
            self._flytime = float(telem_all[12].split(':')[1])
            self._acceleration = [float(telem_all[13].split(':')[1]), float(telem_all[14].split(':')[1]), float(telem_all[15].split(':')[1])]

    # ----------------------------------------------------------------------------------------------------

    def close(self):
        self._fin = True
        self._socket.close()

    # ----------------------------------------------------------------------------------------------------

    def get_response_raw(self):
        return self._response

    def get_all(self):
        return [self._angles, self._velocity, self._temp, self._tof, self._height, self._battery, self._baro, self._flytime, self._acceleration]

    def get_angles(self):
        return self._angles

    def get_velocity(self):
        return self._velocity

    def get_temp(self):
        return self._temp

    def get_tof(self):
        return self._tof

    def get_height(self):
        return self._height

    def get_battery(self):
        return self._battery

    def get_baro(self):
        return self._baro

    def get_flytime(self):
        return self._flytime

    def get_acceleration(self):
        return self._acceleration