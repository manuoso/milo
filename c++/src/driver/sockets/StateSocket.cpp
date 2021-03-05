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

#include "milo/driver/sockets/StateSocket.h"

namespace milo{
    //---------------------------------------------------------------------------------------------------------------------
    StateSocket::StateSocket(int _port)
    {
        if(create(_port)){
            buffer_ = std::vector<unsigned char>(1024);
            listen();
        }else{
            std::cout << "[STATE_SOCKET] Socket not initialized" << std::endl;
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    StateSocket::~StateSocket(){
        close();
        delete socket_;
    }

    //---------------------------------------------------------------------------------------------------------------------
    std::map<std::string, std::string> StateSocket::getDecodedData(){
        std::lock_guard<std::mutex> lock(mtx_);
        return decodedData_;
    }

    //---------------------------------------------------------------------------------------------------------------------
    // Process a state packet from the drone, runs at 10Hz
    void StateSocket::process_packet(size_t r)
    {
        std::lock_guard<std::mutex> lock(mtx_);

        receiveTime_ = std::chrono::high_resolution_clock::now();

        if (!receiving_) {
            receiving_ = true;
        }

        // Split on ";" and ":" and generate a key:value map
        std::map<std::string, std::string> fields;
        std::string raw(buffer_.begin(), buffer_.begin() + r);
        std::regex re("([^:]+):([^;]+);");
        for (auto i = std::sregex_iterator(raw.begin(), raw.end(), re); i != std::sregex_iterator(); ++i) {
            auto match = *i;
            fields[match[1]] = match[2];
        }

        decodedData_ = fields;

    }

}