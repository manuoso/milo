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


#pragma once

#include <atomic>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <chrono>
#include <memory>

namespace milo{
namespace modules{
namespace utils{

    class Task {
        public:
            Task(unsigned int _duration);

            void setDuration(unsigned int _duration);

            virtual ~Task();

            virtual void run();

            virtual void stop();

            virtual void step() = 0;

        private:

            class Runnable
            {
            public:
                Runnable(Task* _task);
                virtual ~Runnable();
                void run();

            private:
                Task* task_;
            };
        
        private:
            std::atomic<bool> run_;

            std::chrono::microseconds duration_;

            std::mutex lock_;
            std::shared_ptr<std::thread> thread_;
            std::shared_ptr<Runnable> taskRunner_;
        };

}
}
}
