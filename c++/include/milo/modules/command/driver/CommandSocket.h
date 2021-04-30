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


// Those class are based in: https://github.com/clydemcqueen/tello_ros/blob/master/tello_driver/

#ifndef __MILO_MODULES_COMMAND_DRIVER_H__
#define __MILO_MODULES_COMMAND_DRIVER_H__ 1

#include "milo/modules/logger/LogManager.h"

#include "milo/modules/socket/TelloSocket.h"

namespace milo{
namespace modules{
namespace command{
namespace driver{

    class CommandSocket : public socket::TelloSocket
    {
        public:
            CommandSocket(std::string _ip, int _port);

            virtual ~CommandSocket();

            bool isInit();
            
            void timeout() override;

            bool waiting();

            bool respond();

            std::chrono::high_resolution_clock::time_point receive_time();

            std::chrono::high_resolution_clock::time_point send_time();

            void send(std::string _cmd);

        private:
            void process_packet(size_t r) override;

        private:
            boost::asio::ip::udp::endpoint remotEndpoint_;

            std::chrono::high_resolution_clock::time_point sendTime_;  
            bool waiting_ = false;    
            bool respond_ = false;

    };

}
}
}
}

#endif
