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

#ifndef MILO_TELLO_TELEMETRY_H_
#define MILO_TELLO_TELEMETRY_H_

#include "milo/driver/sockets/StateSocket.h"
#include <string>    

namespace milo{
    class TelloTelemetry
    {
        public:
            // Example: “pitch:%d;roll:%d;yaw:%d;vgx:%d;vgy%d;vgz:%d;templ:%d;temph:%d;tof:%d;h:%d;bat:%d;baro:%.2f; time:%d;agx:%.2f;agy:%.2f;agz:%.2f;\r\n”
            struct TelemetryData{
                int pitch = 0;
                int roll = 0;
                int yaw = 0;
                int vx = 0;
                int vy = 0;
                int vz = 0;
                // int templ = 0;
                // int temph = 0;
                int tof = 0;
                int h = 0;
                int bat = 0;
                // float baro = 0;
                int time = 0;
                float ax = 0;
                float ay = 0;
                float az = 0;
            };

        public:
            TelloTelemetry(int _port);

            ~TelloTelemetry();

            bool isReceiving();

            void timeout();

            TelemetryData getAllTelemetry();

            std::vector<int> getRPY();

            std::vector<int> getVelocity();

            int getTOF();

            int getHeight();

            int getBattery();

            int getTimeOfFlight();

            std::vector<float> getAcceleration();

        private:
            void decodeThread();

            void decodeData(std::map<std::string, std::string> _data);

        private:
            StateSocket *stateSocket_ = nullptr;
            TelemetryData data_;

            std::thread thread_;                  
            std::mutex mtx_;    
            bool run_ = false;

    };
}

#endif
