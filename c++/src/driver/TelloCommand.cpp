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

#include "milo/driver/TelloCommand.h"

namespace milo{
    //---------------------------------------------------------------------------------------------------------------------
    TelloCommand::TelloCommand(std::string _ip, int _port)
    {
        commandSocket_ = new CommandSocket(_ip, _port);
        if(commandSocket_ != nullptr){
            std::cout << "Init Command Socket" << std::endl;
        }else{
            std::cout << "Not initialize Command Socket" << std::endl;
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    TelloCommand::~TelloCommand(){
        delete commandSocket_;
    }
    
    //---------------------------------------------------------------------------------------------------------------------
    bool TelloCommand::isInit(){
        return commandSocket_->isInit();
    }

    //---------------------------------------------------------------------------------------------------------------------
    bool TelloCommand::isReceiving(){
        std::lock_guard<std::mutex> lock(mtx_);
        return commandSocket_->receiving();
    }

    //---------------------------------------------------------------------------------------------------------------------
    bool TelloCommand::isWaiting(){
        std::lock_guard<std::mutex> lock(mtx_);
        return commandSocket_->waiting();
    }

    //---------------------------------------------------------------------------------------------------------------------
    bool TelloCommand::isRespond(){
        std::lock_guard<std::mutex> lock(mtx_);
        return commandSocket_->respond();
    }

    //---------------------------------------------------------------------------------------------------------------------
    void TelloCommand::timeout(){
        std::lock_guard<std::mutex> lock(mtx_);
        return commandSocket_->timeout();
    }

    //---------------------------------------------------------------------------------------------------------------------
    float TelloCommand::timeSendRecv(){
        std::lock_guard<std::mutex> lock(mtx_);
        float delay = (std::chrono::duration_cast<std::chrono::milliseconds>(commandSocket_->receive_time() - commandSocket_->send_time())).count();
        return delay;
    }

    //---------------------------------------------------------------------------------------------------------------------
    float TelloCommand::timeFromLastRecv(){
        std::lock_guard<std::mutex> lock(mtx_);
        float delay = (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - commandSocket_->receive_time())).count();
        return delay;
    }

    //---------------------------------------------------------------------------------------------------------------------
    void TelloCommand::setControl(){
        std::lock_guard<std::mutex> lock(mtx_);
        commandSocket_->send("command");
    }

    //---------------------------------------------------------------------------------------------------------------------
    void TelloCommand::setCamera(bool _enable){
        std::lock_guard<std::mutex> lock(mtx_);
        if(_enable){
            commandSocket_->send("streamon");
        }else{
            commandSocket_->send("streamoff");
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    void TelloCommand::rc_control(int _roll, int _pitch, int _throttle, int _yaw){
        std::lock_guard<std::mutex> lock(mtx_);
        std::string msg = "rc " + std::to_string(_roll) + " " + std::to_string(_pitch) + " " + std::to_string(_throttle) + " " + std::to_string(_yaw);
        commandSocket_->send(msg);
    }

    //---------------------------------------------------------------------------------------------------------------------
    void TelloCommand::move(std::string _dir, int _dist){
        std::lock_guard<std::mutex> lock(mtx_);
        std::string msg = _dir + " " + std::to_string(_dist);
        commandSocket_->send(msg);
    }

    //---------------------------------------------------------------------------------------------------------------------
    void TelloCommand::rotate(std::string _dir, int _angle){
        std::lock_guard<std::mutex> lock(mtx_);
        std::string msg = _dir + " " + std::to_string(_angle);
        commandSocket_->send(msg);
    }

    //---------------------------------------------------------------------------------------------------------------------
    void TelloCommand::send(std::string _cmd){
        std::lock_guard<std::mutex> lock(mtx_);
        commandSocket_->send(_cmd);
    }

}