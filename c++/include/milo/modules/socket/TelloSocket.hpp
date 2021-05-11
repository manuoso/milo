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

#ifndef __MILO_MODULES_SOCKET_TELLO_H__
#define __MILO_MODULES_SOCKET_TELLO_H__ 1

#ifdef HAS_BOOST

#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include <milo/modules/utils/conversions.hpp>
#include "milo/modules/logger/LogManager.hpp"

namespace milo{
namespace modules{
namespace socket{

    class TelloSocket
    {
        public:
            bool close();

            bool receiving();

            virtual void timeout();

        protected:
            bool create(int _port);

            void listen();

            virtual void process_packet(size_t r) = 0;

        protected:
            boost::asio::io_service ioService_;         
            boost::asio::ip::udp::socket *socket_ = nullptr;            

            std::thread thread_;                  
            std::mutex mtx_;               

            std::atomic<bool> useCout_ = false;
            
            std::atomic<bool> run_ = false;
            std::atomic<bool> receiving_ = false;              
            std::chrono::high_resolution_clock::time_point receiveTime_;           
            std::vector<unsigned char> buffer_;   

    };

}
} 
}

#endif

#endif
