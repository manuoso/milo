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


#ifndef __MILO_MODULES_UTILS_STATE_H__
#define __MILO_MODULES_UTILS_STATE_H__ 1

namespace milo{
namespace modules{
namespace utils{
    
    class StateManager;

    class State
    {   
        public:
            virtual ~State()
            {
                // Left in blank intentionally
            }

            void setState(StateManager *_state)
            {
                this->sm_ = _state;
            }

            virtual void task() = 0;

        protected:
            StateManager *sm_;

    };

    class StateManager
    {        
        public:
            StateManager(State *_state)
                :   state_(_state)
            {
                this->transitionTo(_state);
            }
        
            ~StateManager()
            {
                delete state_;
            }

            void transitionTo(State *_state)
            {
                if (this->state_ != nullptr)
                    delete this->state_;

                this->state_ = _state;
                this->state_->setState(this);
                this->state_->task();
            }

        private:
            State *state_;

    };

}
}
}

#endif