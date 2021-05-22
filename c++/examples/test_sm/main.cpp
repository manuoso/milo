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


#include <signal.h>

#include "milo/milo.hpp"
#include "milo/modules/utils/state.hpp"

using namespace milo::modules::utils;

bool fin = false;

class InitialState : public State
{
    public: 
        void task() override
        {
            std::cout << "Holi soy Initial State" << std::endl;
        }
};

class OtherState : public State
{
    public: 
        void task() override
        {
            std::cout << "Holi soy Other State" << std::endl;
        }
};

class AnotherState : public State
{
    public: 
        void task() override
        {
            std::cout << "Holi soy Another State" << std::endl;
        }
};

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

    StateManager *sm = new StateManager(new InitialState);

    while (!fin)
    {
        int choose = 0;
        std::cout << "Choose the state you want" << std::endl;
        std::cout << "1 -> OtherState" << std::endl;
        std::cout << "2-> AnotherState" << std::endl;
        std::cin >> choose;

        if (choose == 1)
            sm->transitionTo(new OtherState);
        
        if (choose == 2)
            sm->transitionTo(new AnotherState);

    }

    delete sm;

    return 0;
}