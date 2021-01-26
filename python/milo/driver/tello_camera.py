"""
    Some methods are based on: https://github.com/dji-sdk/Tello-Python
"""

import socket
import threading
import time
import numpy as np
import cv2

class Tello_Camera:
    def __init__(self):
        self._frame = None          
        self._cap = cv2.VideoCapture('udp://0.0.0.0:11111')

    def __del__(self):
        self._cap.release()

    # ----------------------------------------------------------------------------------------------------

    def close(self):
        self._cap.release()

    # ----------------------------------------------------------------------------------------------------

    def get_frame(self):
        _, self._frame = self._cap.read()
        return self._frame

