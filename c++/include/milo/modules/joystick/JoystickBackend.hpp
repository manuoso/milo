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


#ifndef __MILO_MODULES_JOYSTICK_BACKEND_H__
#define __MILO_MODULES_JOYSTICK_BACKEND_H__ 1

#include <linux/input.h>
#include <linux/joystick.h>

#include <atomic>

#include <memory>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#include <string>
#include <cstring>
#include <cstdlib>

#include <vector>
#include <cmath>
#include <algorithm>

#include <limits.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "milo/modules/logger/LogManager.hpp"

namespace milo{
namespace modules{
namespace joystick{

    class JoystickBackend
    {
        public:
            JoystickBackend(bool _useCout);
            JoystickBackend(bool _useCout, const std::string &_name);

            ~JoystickBackend();

            bool isInit();
            bool closeJoy();
            
            int getAxisCount();
            int getButtonsCount();

            void setDeadzone(double _deadzone);
            bool setFeedback(struct ff_effect _effect);
            bool setCorrection(std::vector<int> _type, std::vector<int> _prec, std::vector<int> _coef);

            bool readJoy(std::string &_eventType, int &_eventNumber, double &_value);
        
        private:
            std::string identifyJoy();

            std::string identifyFF(const std::string &_name);

            bool openJoy(const std::string &_joy, const std::string &_ff);

        private:
            std::atomic<bool> init_;
            std::atomic<bool> useCout_;
            int fd_, ff_;
            int axisCount_, buttonCount_;

            struct ff_effect joyEffect_;
            fd_set set_;
            js_event event_;

            std::atomic<bool> updateEffect_;

            double deadzone_;
            double scale_;
            double unscaledDeadzone_;

    };

}
}
}

#endif
