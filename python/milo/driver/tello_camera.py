"""
    Some methods are based on: https://github.com/dji-sdk/Tello-Python
"""

import socket
import threading
import time
import numpy as np
import cv2

import h264decoder

class Tello_Camera:
    def __init__(self):
        self._fin = False

        self._frame = None          
        self._decoder = h264decoder.H264Decoder()

        # self._cap = cv2.VideoCapture('udp://0.0.0.0:11111')
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._socket.bind(('0.0.0.0', 11111))

        self._recv_thread = threading.Thread(target=self.__receive_thread)
        self._recv_thread.daemon = True
        self._recv_thread.start()

    def __del__(self):
        self._socket.close()
        # self._cap.release()

    # ----------------------------------------------------------------------------------------------------

    def __receive_thread(self):
        buffer_data = b''
        while not self._fin:
            try:
                response, _ = self._socket.recvfrom(2048)
                # buffer_data += response.decode('utf-8')
                buffer_data += response
                #print(response)

                if len(response) < 1460:
                    self.__h264_decode(buffer_data)
                    buffer_data = b''

            except socket.error as exc:
                print ("Caught exception socket.error : %s" % exc)

    def __h264_decode(self, packet_data):
        frames = self._decoder.decode(packet_data)
        for framedata in frames:
            (frame, w, h, ls) = framedata
            if frame is not None:
                #print('frame size %i bytes, w %i, h %i, linesize %i' % (len(frame), w, h, ls))

                frame = np.frombuffer(frame, dtype=np.ubyte, count=len(frame))
                frame = frame.reshape((h, ls//3, 3))
                frame = frame[:,:w,:]

                frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

                self._frame = frame

    # ----------------------------------------------------------------------------------------------------

    def close(self):
        # self._cap.release()
        self._fin = True
        self._socket.close()

    # ----------------------------------------------------------------------------------------------------

    def get_frame(self):
        # _, self._frame = self._cap.read()
        return self._frame

