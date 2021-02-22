//---------------------------------------------------------------------------------------------------------------------
//  MILO
//---------------------------------------------------------------------------------------------------------------------
//  Copyright 2020 Manuel Pérez Jiménez (a.k.a. manuoso) manuperezj@gmail.com
//---------------------------------------------------------------------------------------------------------------------
//  Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
//  and associated documentation files (the "Software"), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify, merge, publish, distribute, 
//  sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all copies or substantial 
//  portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
//  BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
//  OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//---------------------------------------------------------------------------------------------------------------------

#ifndef MILO_TELLO_COMMAND_H_
#define MILO_TELLO_COMMAND_H_

#include "milo/driver/sockets/CommandSocket.h"

namespace milo{
    class TelloCommand
    {
        public:
            TelloCommand(std::string _ip, int _port);

            ~TelloCommand();

            bool isInit();

            bool isReceiving();

            bool isWaiting();

            bool isRespond();

            void timeout();

            float timeSendRecv();

            float timeFromLastRecv();

            void setControl();

            void setCamera(bool _enable);

            void rc_control(int _roll, int _pitch, int _throttle, int _yaw);

            void move(std::string _dir, int _dist);

            void rotate(std::string _dir, int _angle);

            void send(std::string _cmd);

        private:
            CommandSocket *commandSocket_ = nullptr;

            std::mutex mtx_;    

    };
}

#endif