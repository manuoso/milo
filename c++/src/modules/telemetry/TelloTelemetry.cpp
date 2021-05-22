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


#ifdef HAS_BOOST

#include "milo/modules/telemetry/TelloTelemetry.hpp"

namespace milo{
namespace modules{
namespace telemetry{

    using namespace milo::modules::logger;

    //---------------------------------------------------------------------------------------------------------------------
    TelloTelemetry::TelloTelemetry(bool _useCout, int _port)
        : run_(false)
    {
        useCout_.store(_useCout);
        stateSocket_ = new driver::StateSocket(useCout_.load(), _port);
        if (stateSocket_ != nullptr)
        {
            LogManager::get()->status("[TELLO_TELEMETRY] Init State Socket", useCout_.load());
            decodeThread();
        }
        else
        {
            LogManager::get()->error("[TELLO_TELEMETRY] Not initialize State Socket", useCout_.load());
        }
    }

    //---------------------------------------------------------------------------------------------------------------------
    TelloTelemetry::~TelloTelemetry()
    {
        run_ = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (thread_.joinable())
            thread_.join();
        
        delete stateSocket_;
    }
    
    //---------------------------------------------------------------------------------------------------------------------
    bool TelloTelemetry::isReceiving()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return stateSocket_->receiving();
    }

    //---------------------------------------------------------------------------------------------------------------------
    void TelloTelemetry::timeout()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return stateSocket_->timeout();
    }

    //---------------------------------------------------------------------------------------------------------------------
    TelloTelemetry::TelemetryData TelloTelemetry::getAllTelemetry()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return data_;
    }

    //---------------------------------------------------------------------------------------------------------------------
    std::vector<int> TelloTelemetry::getRPY()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        std::vector<int> rpy = {data_.pitch, data_.roll, data_.yaw};
        return rpy;
    }

    //---------------------------------------------------------------------------------------------------------------------
    std::vector<int> TelloTelemetry::getVelocity()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        std::vector<int> vel = {data_.vx, data_.vy, data_.vz};
        return vel;
    }

    //---------------------------------------------------------------------------------------------------------------------
    int TelloTelemetry::getTOF()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return data_.tof;
    }

    //---------------------------------------------------------------------------------------------------------------------
    int TelloTelemetry::getHeight()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return data_.h;
    }

    //---------------------------------------------------------------------------------------------------------------------
    int TelloTelemetry::getBattery()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return data_.bat;
    }

    //---------------------------------------------------------------------------------------------------------------------
    int TelloTelemetry::getTimeOfFlight()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return data_.time;
    }

    //---------------------------------------------------------------------------------------------------------------------
    std::vector<float> TelloTelemetry::getAcceleration()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        std::vector<float> acc = {data_.ax, data_.ay, data_.az};
        return acc;
    }

    //---------------------------------------------------------------------------------------------------------------------
    void TelloTelemetry::decodeThread()
    {
        run_ = true;
        thread_ = std::thread(
        [&]()
        {
            while (run_)
            {
                std::map<std::string, std::string> data = stateSocket_->getDecodedData();
                decodeData(data);
                // std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }

    //---------------------------------------------------------------------------------------------------------------------
    void TelloTelemetry::decodeData(std::map<std::string, std::string> _data)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        
        if (_data.find("pitch") != _data.end())
        {
            data_.pitch = std::stoi(_data["pitch"]);
            data_.roll = std::stoi(_data["roll"]);
            data_.yaw = std::stoi(_data["yaw"]);
            data_.vx = std::stoi(_data["vgx"]);
            data_.vy = std::stoi(_data["vgy"]);
            data_.vz = std::stoi(_data["vgz"]);
            // data_.templ = std::stoi(_data["templ"]);
            // data_.temph = std::stoi(_data["templ"]);
            data_.tof = std::stoi(_data["tof"]);
            data_.h = std::stoi(_data["h"]);
            data_.bat = std::stoi(_data["bat"]);
            // data_.baro = std::stof(_data["baro"]);
            data_.time = std::stoi(_data["time"]);
            data_.ax = std::stof(_data["agx"]);
            data_.ay = std::stof(_data["agy"]);
            data_.az = std::stof(_data["agz"]);
        }
    }

}
}
}

#endif
