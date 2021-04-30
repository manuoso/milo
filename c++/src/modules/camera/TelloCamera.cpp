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


#include "milo/modules/camera/TelloCamera.h"

namespace milo{
namespace modules{
namespace camera{

    using namespace milo::modules::logger;

    //---------------------------------------------------------------------------------------------------------------------
    TelloCamera::TelloCamera(int _port)
    {
        cameraSocket_ = new driver::CameraSocket(_port);
        if (cameraSocket_ != nullptr)
            LogManager::get()->status("[TELLO_CAMERA] Init Camera Socket", true);
        else
            LogManager::get()->error("[TELLO_CAMERA] Not initialize Camera Socket", true);
    }

    //---------------------------------------------------------------------------------------------------------------------
    TelloCamera::~TelloCamera()
    {
        delete cameraSocket_;
    }
    
    //---------------------------------------------------------------------------------------------------------------------
    bool TelloCamera::isReceiving()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return cameraSocket_->receiving();
    }

    //---------------------------------------------------------------------------------------------------------------------
    void TelloCamera::timeout()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return cameraSocket_->timeout();
    }

    //---------------------------------------------------------------------------------------------------------------------
    cv::Mat TelloCamera::getImage()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return cameraSocket_->getFrame();
    }

}
}
}
