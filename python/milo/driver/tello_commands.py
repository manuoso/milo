"""
    Some methods are based on: https://github.com/dji-sdk/Tello-Python
"""

import socket
import threading
import time

class Tello_Commands:
    def __init__(self, ip, port):
        self.command_timeout = 0.3

        self._fin = False
        self._abort = False
        self._response = None 

        self.tello_address = (ip, port)
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._socket.bind(('0.0.0.0', port))

        self._recv_thread = threading.Thread(target=self.__receive_thread)
        self._recv_thread.daemon = True
        self._recv_thread.start()
        
        print('[Tello_Commands] activate sdk:', self.act_sdk())

    def __del__(self):
        self._socket.close()

    # ----------------------------------------------------------------------------------------------------

    def __receive_thread(self):
        while not self._fin:
            try:
                self._response, _ = self._socket.recvfrom(3000)
                #print(self.response)
            except socket.error as exc:
                print ("Caught exception socket.error : %s" % exc)

    def __set_abort(self):
        self._abort = True

    def __get_response(self):
        timer_cmd = threading.Timer(self.command_timeout, self.__set_abort)

        self._abort = False

        timer_cmd.start()
        while self._response is None:
            if self._abort is True:
                break
        timer_cmd.cancel()
        
        if self._response is None:
            result = 'none'
        else:
            result = self._response.decode('utf-8')

        self._response = None

        return result

    # ----------------------------------------------------------------------------------------------------

    def close(self):
        self._fin = True
        self._socket.close()

    # ----------------------------------------------------------------------------------------------------

    def act_sdk(self):
        self._socket.sendto(b'command', self.tello_address)
        result = self.__get_response()
        if result == 'ok':
            return True
        else:
            return False
    
    def act_cam(self, enable):
        if enable:
            self._socket.sendto(b'streamon', self.tello_address)
        else:
            self._socket.sendto(b'streamoff', self.tello_address)
        result = self.__get_response()
        if result == 'ok':
            return True
        else:
            return False

    def send_cmd(self, cmd):
        self._socket.sendto(cmd.encode('utf-8'), self.tello_address)
        result = self.__get_response()
        if result == 'ok':
            return True
        else:
            return False
