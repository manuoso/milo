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


#include "milo/modules/command/driver/CommandSocket.h"

namespace milo{
namespace modules{
namespace command{
namespace driver{

    using namespace milo::modules::logger;

    //---------------------------------------------------------------------------------------------------------------------
    CommandSocket::CommandSocket(bool _useCout, std::string _ip, int _port) 
        : waiting_(false), respond_(false)
    {
        useCout_ = _useCout;
        remotEndpoint_ = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(_ip), _port);
        if (create(38065))
        {
            sendTime_ = std::chrono::high_resolution_clock::now();
            receiveTime_ = std::chrono::high_resolution_clock::now();
            buffer_ = std::vector<unsigned char>(1024);
            listen();
        }
        else
        {
            LogManager::get()->error("[COMMAND_SOCKET] Socket not initialized", useCout_);
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    CommandSocket::~CommandSocket()
    {
        close();
        delete socket_;
    }

    //---------------------------------------------------------------------------------------------------------------------
    bool CommandSocket::isInit()
    {
        if (socket_ != nullptr)
            return true;            
        else
            return false;
    }

    //---------------------------------------------------------------------------------------------------------------------
    void CommandSocket::timeout()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        receiving_ = false;

        if (waiting_) 
        {
            LogManager::get()->error("[COMMAND_SOCKET] Error TIMEOUT", useCout_);
            waiting_ = false;
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    bool CommandSocket::waiting()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return waiting_;
    }

    //---------------------------------------------------------------------------------------------------------------------
    bool CommandSocket::respond()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return respond_;
    }

    //---------------------------------------------------------------------------------------------------------------------
    std::chrono::high_resolution_clock::time_point CommandSocket::receive_time()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return receiveTime_;
    }

    //---------------------------------------------------------------------------------------------------------------------
    std::chrono::high_resolution_clock::time_point CommandSocket::send_time()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return sendTime_;
    }

    //---------------------------------------------------------------------------------------------------------------------
    void CommandSocket::send(std::string _cmd)
    {
        std::lock_guard<std::mutex> lock(mtx_);

        if (!waiting_) 
        {
            LogManager::get()->status("[COMMAND_SOCKET] sending: " + _cmd, false);
            socket_->send_to(boost::asio::buffer(_cmd), remotEndpoint_);
            sendTime_ = std::chrono::high_resolution_clock::now();

            // Wait for a response for all commands except "rc"
            if (_cmd.rfind("rc", 0) != 0) 
                waiting_ = true;
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    void CommandSocket::process_packet(size_t r)
    {
        std::lock_guard<std::mutex> lock(mtx_);

        receiveTime_ = std::chrono::high_resolution_clock::now();

        if (!receiving_) 
            receiving_ = true;

        std::string str = std::string(buffer_.begin(), buffer_.begin() + r);

        respond_ = (str == "ok") ? true : false;
        waiting_ = false;
    }

}
}
}
}
