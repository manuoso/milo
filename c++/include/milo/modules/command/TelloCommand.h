//---------------------------------------------------------------------------------------------------------------------
//  MILO
//---------------------------------------------------------------------------------------------------------------------
//  Copyright 2021 Manuel Pérez Jiménez (a.k.a. manuoso) manuperezj@gmail.com
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


#ifndef __MILO_MODULES_COMMAND_TELLO_COMMAND_H__
#define __MILO_MODULES_COMMAND_TELLO_COMMAND_H__ 1

#include "milo/modules/logger/LogManager.h"

#include "milo/modules/command/driver/CommandSocket.h"

namespace milo{
namespace modules{
namespace command{

    class TelloCommand
    {
        public:
            TelloCommand(bool _useCout, std::string _ip, int _port);

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

            void takeoff();

            void land();
            
            void emergency();

            void rc_control(int _roll, int _pitch, int _throttle, int _yaw);

            void move(std::string _dir, int _dist);

            void rotate(std::string _dir, int _angle);

            void send(std::string _cmd);

        private:
            driver::CommandSocket *commandSocket_ = nullptr;

            bool useCout_;
            std::mutex mtx_;    

    };

}
}
}

#endif