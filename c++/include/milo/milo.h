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


#ifndef MILO_H_
#define MILO_H_

#include "milo/modules/logger/LogManager.h"

#include "milo/modules/command/TelloCommand.h"
#include "milo/modules/telemetry/TelloTelemetry.h"
#include "milo/modules/camera/TelloCamera.h"

namespace milo{
    using namespace modules::command;
    using namespace modules::telemetry;
    using namespace modules::camera;

    using namespace milo::modules::logger;

    class MILO {
        public:
            //---------------------------------------------------------------------------------------------------------------------
            // METHODS FOR INITIALIZATION
	        //---------------------------------------------------------------------------------------------------------------------
            
            /// Create the system. 
            static MILO * create(bool _initState, bool _initCamera);

            /// Close the system.
            static void close();

            void createOnlyLogger();

            //---------------------------------------------------------------------------------------------------------------------
            // MODULES
	        //---------------------------------------------------------------------------------------------------------------------

            TelloCommand * command(){return command_;}

            TelloTelemetry * telemetry(){return telemetry_;}

            TelloCamera * camera(){return camera_;}

            //---------------------------------------------------------------------------------------------------------------------
            // UTILS
	        //---------------------------------------------------------------------------------------------------------------------

            /// This method tells if MILO has been correctly init
            /// \return true if MILO is correctly init
            bool isInit();

        private:
            /// Constructor with given configuration for backend
            MILO(bool _initState, bool _initCamera);

            /// Destructor
            virtual ~MILO(); 

            void timeoutThread();

        private:
		    static MILO *milo_;

            TelloCommand *command_ = nullptr;
            TelloTelemetry *telemetry_ = nullptr;
            TelloCamera *camera_ = nullptr;

            bool run_;
            std::thread thread_;                  

    };
}

#endif
