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


#include "milo/milo.h"

namespace milo{

    using namespace milo::modules::logger;
    
    MILO *MILO::milo_ = nullptr;

    //---------------------------------------------------------------------------------------------------------------------
    // PUBLIC FUNCTIONS
    //---------------------------------------------------------------------------------------------------------------------

    //---------------------------------------------------------------------------------------------------------------------
    // METHODS FOR INITIALIZATION
    //---------------------------------------------------------------------------------------------------------------------

	//---------------------------------------------------------------------------------------------------------------------
	MILO * MILO::create(bool _initState, bool _initCamera) 
    {
		if (!milo_)
			milo_ = new MILO(_initState, _initCamera);
        else
            LogManager::get()->error("[MILO] Someone tried to reinitialize the MILO system", true);

        return milo_;
	}

	//---------------------------------------------------------------------------------------------------------------------
	void MILO::close()
    {
		delete milo_;
	}

	//---------------------------------------------------------------------------------------------------------------------
    void MILO::createOnlyLogger()
    {
        LogManager::init("log");
    }

    //---------------------------------------------------------------------------------------------------------------------
    // UTILS
    //---------------------------------------------------------------------------------------------------------------------
    
    //---------------------------------------------------------------------------------------------------------------------
    bool MILO::isInit()
    {
        if (command_ == nullptr)
            return false;
        
        return true;
    }

    //---------------------------------------------------------------------------------------------------------------------
    // PRIVATE FUNCTIONS
    //---------------------------------------------------------------------------------------------------------------------

    //---------------------------------------------------------------------------------------------------------------------
    MILO::MILO(bool _initState, bool _initCamera) 
    {
        LogManager::init("log");
        command_ = new TelloCommand("192.168.10.1", 8889);

        if (command_->isInit())
            timeoutThread();
        else
            command_ = nullptr;

        if (_initState && command_ != nullptr)
            telemetry_ = new TelloTelemetry(8890);
        
        if (_initCamera && command_ != nullptr)
            camera_ = new TelloCamera(11111);
    }

    //---------------------------------------------------------------------------------------------------------------------
    MILO::~MILO() 
    {
        run_ = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        if (thread_.joinable())
            thread_.join();
        
        delete command_;
        delete telemetry_;
        delete camera_;
    }

    //---------------------------------------------------------------------------------------------------------------------
    void MILO::timeoutThread()
    {
        run_ = true;
        thread_ = std::thread(
        [&]()
        {
            while (run_)
            {
                float diff = command_->timeFromLastRecv();

                if (diff > 5.0)
                {
                    LogManager::get()->status("[MILO] Sending rc 0 0 0 0", false);
                    command_->send("rc 0 0 0 0");
                }
                else if(diff > 15)
                {
                    command_->timeout();

                    if(telemetry_ != nullptr)
                        telemetry_->timeout();
                    
                    if(camera_ != nullptr)
                        camera_->timeout();
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
    }

}