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

#include <thread>

#include <ros/ros.h>

#include <std_msgs/Int32MultiArray.h>
#include <std_srvs/SetBool.h>

bool run = true;

//---------------------------------------------------------------------------------------------------------------------
int main(int _argc, char **_argv) 
{
    // Override SIGINT handler
    ros::init(_argc, _argv, "milo_control");
    
    ros::AsyncSpinner rosSpinner(4); // Use 4 thread
    rosSpinner.start();

    ros::Rate rate(30);

    ros::NodeHandle nh;
    ros::ServiceClient takeoffReq = nh.serviceClient<std_srvs::SetBool>("/tello/takeoff");
    ros::ServiceClient landReq = nh.serviceClient<std_srvs::SetBool>("/tello/land");
    ros::ServiceClient emergencyReq = nh.serviceClient<std_srvs::SetBool>("/tello/emergency");

    ros::Publisher rcPub = nh.advertise<std_msgs::Int32MultiArray>("/tello/rc", 1);

    std::thread *threadRC;
    bool finish = false;
	while(!finish)
    {
        int mode = 0;
        std::cout << "Press: " << std::endl;
        std::cout << "(1) for takeoff" << std::endl;
        std::cout << "(0) for land" << std::endl;
        std::cout << "(2) for send RC" << std::endl;
        std::cout << "(8) for emergency" << std::endl;
        std::cout << "(9) for exit" << std::endl;
        std::cin >> mode;

        switch (mode){
            case 1:
            {   
                std_srvs::SetBool srv;
                srv.request.data = true;

                if(takeoffReq.call(srv))
                {
                    if(srv.response.success)
                        std::cout << "Service of TAKE OFF success" << std::endl;
                    else
                        std::cout << "Service of TAKE OFF failed" << std::endl;
                }
                else
                {
                    std::cout << "Failed to call service of TAKE OFF" << std::endl;
                }
                break;
            }
            case 2:
            {   
                int roll;
                std::cout << "Roll: ";
                std::cin >> roll;

                int pitch;
                std::cout << "Pitch: ";
                std::cin >> pitch;

                int throtle;
                std::cout << "Throtle: ";
                std::cin >> throtle;

                int yaw;
                std::cout << "Yaw: ";
                std::cin >> yaw;

                std_msgs::Int32MultiArray msgRC;
                msgRC.data.push_back(roll);
                msgRC.data.push_back(pitch);
                msgRC.data.push_back(throtle);
                msgRC.data.push_back(yaw);

                run = true;

                threadRC = new std::thread(
                [&]()
                {
                    while(run)
                    {
                        rcPub.publish(msgRC);
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                });

                int finishRC = 0;
                bool runSend = true;
                std::cout << "Press -1 to finish send rc: ";
                while(runSend)
                {
                    std::cin >> finishRC;
                    if (finishRC == -1)
                    {
                        run = false;
                        runSend = false;
                    }
                    else 
                    {
                        std::cout << "\nERROR. Try again" << std::endl;
                    }
                }

                break;
            }
            case 0:
            {   
                std_srvs::SetBool srv;
                srv.request.data = true;

                if(landReq.call(srv))
                {
                    if(srv.response.success)
                        std::cout << "Service of LAND success" << std::endl;
                    else
                        std::cout << "Service of LAND failed" << std::endl;
                }
                else
                {
                    std::cout << "Failed to call service of LAND" << std::endl;
                }
                break;
            }
            case 8:
            {   
                std_srvs::SetBool srv;
                srv.request.data = true;

                if(emergencyReq.call(srv))
                {
                    if(srv.response.success)
                        std::cout << "Service of EMERGENCY success" << std::endl;
                    else
                        std::cout << "Service of EMERGENCY failed" << std::endl;
                }
                else
                {
                    std::cout << "Failed to call service of EMERGENCY" << std::endl;
                }
                break;
            }
            case 9:
                finish = true;
                break;
        }

        rate.sleep();
	}

    return 0;
}
