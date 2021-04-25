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


#ifndef MILO_JOY_JOYSTICK_H_
#define MILO_JOY_JOYSTICK_H_

#include <linux/input.h>
#include <linux/joystick.h>

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

namespace milo{
    class JoystickBackend
    {
        public:
            JoystickBackend();
            JoystickBackend(const std::string &_name);

            ~JoystickBackend();

            bool isInit();
            
            int getAxisCount();
            int getButtonsCount();

            bool setFeedback(struct ff_effect _effect);

            bool readJoy(std::string &_eventType, int &_eventNumber, double &_value);
        
        private:
            std::string identifyJoy();

            std::string identifyFF(const std::string &_name);

            bool openJoy(const std::string &_joy, const std::string &_ff);

            bool closeJoy();

        private:
            bool init_;
            int fd_, ff_;
            int axisCount_, buttonCount_;

            struct ff_effect joyEffect_;
            fd_set set_;
            js_event event_;

            bool updateEffect_;

            double deadzone_;
            double scale_;
            double unscaledDeadzone_;

    };
}

#endif
