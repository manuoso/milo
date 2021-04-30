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

#include "milo/milo.h"
#include "milo/modules/joystick/JoystickBackend.h"

using namespace milo;
using namespace milo::modules::joystick;

bool fin = false;
JoystickBackend *jsbd;

// Replacement handler
void finishHandler(int _sig)
{
    std::cout << "Finish Handler: Catch signal " << _sig << std::endl;
    fin = true;
    delete jsbd;
}

int main(int _argc, char **_argv)
{
	signal(SIGINT, finishHandler);
    signal(SIGTERM, finishHandler);

	MILO *drone = nullptr;
	drone->createOnlyLogger();

    jsbd = new JoystickBackend();
    
	std::string event_type = "";
	int event_number = 0;
	double value;

	while(fin == false)
	{
        if(jsbd->readJoy(event_type, event_number, value))
		{
			std::cout << "[Event] Type: " << event_type << " Number: " << event_number << " Value: " << value << std::endl;
		}

	}

	return 0;
}