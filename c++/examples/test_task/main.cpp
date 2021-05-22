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


#include <signal.h>

#include "milo/milo.hpp"
#include "milo/modules/utils/task.hpp"

using namespace milo::modules::utils;
using namespace std::chrono;

class TaskTest : public Task
{
    public:
        TaskTest(unsigned int _duration) : Task(_duration)
        { 
            start_ = high_resolution_clock::now(); 
            this->run();
        }
        
        ~TaskTest(){ }

        void step()
        {
            auto end = high_resolution_clock::now();
		    auto delay = duration_cast<milliseconds>(end - start_);

            std::cout << "Holi each :" << delay.count() << std::endl;

            start_ = high_resolution_clock::now();
        }
    private:
        high_resolution_clock::time_point start_;

};

bool fin = false;

// Replacement handler
void finishHandler(int _sig)
{
    std::cout << "Finish Handler: Catch signal " << _sig << std::endl;
    fin = true;
}

int main(int _argc, char **_argv)
{
    signal(SIGINT, finishHandler);
    signal(SIGTERM, finishHandler);

    TaskTest tt(1000);

    while (!fin)
    {
        sleep(1);
    }

    tt.stop();
    
    return 0;
}