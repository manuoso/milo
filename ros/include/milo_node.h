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

#ifndef MILO_NODE_H_
#define MILO_NODE_H_

#include <ros/ros.h>
#include <ros/xmlrpc_manager.h>

#include <signal.h>

#include <tf/tf.h>

#include <std_msgs/String.h>
#include <std_msgs/Int32MultiArray.h>
#include <sensor_msgs/Imu.h> 
#include <sensor_msgs/BatteryState.h> 
#include <geometry_msgs/PoseStamped.h> 
 
#include <sensor_msgs/Image.h> 
#include <sensor_msgs/image_encodings.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <std_srvs/SetBool.h>

#include "milo/milo.h"


class MiloNode
{
    public:
        enum class eState{
            WAIT,
            LAND,
            TAKEOFF,
            RC,
            EXIT
        };
        
        MiloNode(){};
        ~MiloNode(){};

        bool init();

        bool run();

        bool finalize();

    private:
        bool landService(std_srvs::SetBool::Request &_req, std_srvs::SetBool::Response &_res);

        bool takeoffService(std_srvs::SetBool::Request &_req, std_srvs::SetBool::Response &_res);

        void rcCallback(const std_msgs::Int32MultiArray::ConstPtr& _msg);

    private:
        bool telemetryThread();

        bool cameraThread();

    private:
        eState state_;

        milo::MILO *drone_ = nullptr;

        std::thread telemThread_, camThread_;

        ros::Publisher statusPub_, imuPub_, batPub_, angHeightPub_, imagePub_;
        ros::Subscriber rcSub_;
        ros::ServiceServer landSrv_, takeoffSrv_;

        bool fin_ = false;

};

#endif
