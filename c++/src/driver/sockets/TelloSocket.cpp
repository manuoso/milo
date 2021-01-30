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

#include "milo/driver/sockets/TelloSocket.h"

namespace milo{
    //---------------------------------------------------------------------------------------------------------------------
    bool TelloSocket::close(){
        run_ = false;
        ioService_.stop();

        boost::system::error_code ec;
        if(socket_){
            socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
            socket_->close();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if(thread_.joinable()){
            thread_.join();
        }   

        if(ec.value() == 0){
            return true;            
        }else{
            return false;
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    bool TelloSocket::create(int _port){
        try{
            socket_ = new boost::asio::ip::udp::socket(ioService_, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), _port));
        }catch(std::exception &e){
            std::cerr << e.what() << std::endl;
            socket_ = nullptr;
        }

        if(socket_ != nullptr){
            return true;            
        }else{
            return false;
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    void TelloSocket::listen()
    {
        run_ = true;
        thread_ = std::thread(
        [&]()
        {
            while(run_){
                size_t r = socket_->receive(boost::asio::buffer(buffer_));
                process_packet(r);
            }
        });
    }

    //---------------------------------------------------------------------------------------------------------------------
    bool TelloSocket::receiving()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return receiving_;
    }

    //---------------------------------------------------------------------------------------------------------------------
    void TelloSocket::timeout()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        receiving_ = false;
    }

}