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


#ifndef __MILO_MODULES_CAMERA_TELLO_CAMERA_H__
#define __MILO_MODULES_CAMERA_TELLO_CAMERA_H__ 1

#include "milo/modules/logger/LogManager.h"

#include "milo/modules/camera/driver/CameraSocket.h"

namespace milo{
namespace modules{
namespace camera{
    
    class TelloCamera
    {
        public:
            TelloCamera(bool _useCout, int _port);

            ~TelloCamera();

            bool isReceiving();

            void timeout();

            cv::Mat getImage();

        private:
            driver::CameraSocket *cameraSocket_ = nullptr;

            bool useCout_;
            std::mutex mtx_;    

    };

}
}
}

#endif
