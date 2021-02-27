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

#include "milo_node.h"


//---------------------------------------------------------------------------------------------------------------------
bool MiloNode::init() {

    drone_ = milo::MILO::create(true, true);

    drone_->command()->setControl();
    while(drone_->command()->isWaiting()){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if(drone_->command()->isRespond()){
        std::cout << "[MILO_NODE] Set control: TRUE" << std::endl;
    }else{
        std::cout << "[MILO_NODE] Set control: FALSE" << std::endl;
        return false;
    }

    drone_->command()->setCamera(true);
    while(drone_->command()->isWaiting()){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if(drone_->command()->isRespond()){
        std::cout << "[MILO_NODE] Set Camera: TRUE" << std::endl;
    }else{
        std::cout << "[MILO_NODE] Set Camera: FALSE" << std::endl;
        return false;
    }

    // ****************************************************************************************************

    ros::NodeHandle nh;
    // Status
    statusPub_              = nh.advertise<std_msgs::String>("/tello/status", 1);

    // Services
    landSrv_                = nh.advertiseService("/tello/land", &MiloNode::landService, this);
    takeoffSrv_             = nh.advertiseService("/tello/takeoff", &MiloNode::takeoffService, this);

    // Topic for moving 
    rcSub_                  = nh.subscribe("/tello/rc", 1, &MiloNode::rcCallback, this);

    // Topics of the Telemetry
    imuPub_                 = nh.advertise<sensor_msgs::Imu>("/tello/imu", 1);
    batPub_                 = nh.advertise<sensor_msgs::BatteryState>("/tello/battery", 1);
    angHeightPub_           = nh.advertise<geometry_msgs::PoseStamped>("/tello/angle_height", 1);
    
    // Topics for Camera
    imagePub_               = nh.advertise<sensor_msgs::Image> ("/tello/camera", 1);

    // ****************************************************************************************************

    telemThread_ = std::thread(&MiloNode::telemetryThread, this);
    camThread_ = std::thread(&MiloNode::cameraThread, this);

    state_ = eState::WAIT;
    
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool MiloNode::run() {

    ros::Rate rate(30);

    std_msgs::String msgStatus;

    while(fin_ == false && ros::ok()){
        switch(state_){
            case eState::WAIT:
            {   
                msgStatus.data = "WAIT";
                break;
            }
            case eState::LAND:
            {   
                msgStatus.data = "LAND";
                break;
            }
            case eState::TAKEOFF:
            {   
                msgStatus.data = "TAKE_OFF";
                break;
            }
            case eState::RC:
            {   
                msgStatus.data = "RC";
                break;
            }
            case eState::EXIT:
            {
                std::cout << "\n [MILO_NODE] EXIT..." << std::endl;
                msgStatus.data = "EXIT";
                finalize();
                break;
            }
        }

        statusPub_.publish(msgStatus);
        rate.sleep();
    }
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool MiloNode::finalize() {
    fin_ = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if(telemThread_.joinable()){
        telemThread_.join();
    }   
    if(camThread_.joinable()){
        camThread_.join();
    }
    drone_->close();
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool MiloNode::landService(std_srvs::SetBool::Request &_req, std_srvs::SetBool::Response &_res){
    state_ = eState::LAND;

    if(_req.data){
        drone_->command()->send("land");
    }
    _res.success = true;
    
    state_ = eState::WAIT;

    return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool MiloNode::takeoffService(std_srvs::SetBool::Request &_req, std_srvs::SetBool::Response &_res){
    state_ = eState::TAKEOFF;

    if(_req.data){
        drone_->command()->send("takeoff");
    }
    _res.success = true;
    
    state_ = eState::WAIT;

    return true;
}

//---------------------------------------------------------------------------------------------------------------------
void MiloNode::rcCallback(const std_msgs::Int32MultiArray::ConstPtr& _msg){
    state_ = eState::RC;

    // roll, pitch, throttle, yaw
    drone_->command()->rc_control(_msg->data[0], _msg->data[1], _msg->data[2], _msg->data[3]);

    state_ = eState::WAIT;
}

//---------------------------------------------------------------------------------------------------------------------
bool MiloNode::telemetryThread() {
    std::cout << "[MILO_NODE] Start Telemetry Thread" << std::endl;

    ros::Rate rate(50);

    while (fin_ == false && ros::ok()) { 

        if(!drone_->telemetry()->isReceiving())
            continue;

        milo::TelloTelemetry::TelemetryData telem = drone_->telemetry()->getAllTelemetry();

        geometry_msgs::PoseStamped msgAngHeight;
        msgAngHeight.header.stamp = ros::Time::now();
        msgAngHeight.pose.position.x = telem.tof;
        msgAngHeight.pose.position.z = telem.h;
        msgAngHeight.pose.orientation.x = telem.roll;
        msgAngHeight.pose.orientation.y = telem.pitch;
        msgAngHeight.pose.orientation.z = telem.yaw;

        sensor_msgs::BatteryState msgBattery;
        msgBattery.header.stamp = msgAngHeight.header.stamp;
        msgBattery.percentage = telem.bat;
        msgBattery.current = telem.time;

        sensor_msgs::Imu msgImu;
        msgImu.header.stamp = msgBattery.header.stamp;
        msgImu.angular_velocity.x = telem.vx;
        msgImu.angular_velocity.y = telem.vy;
        msgImu.angular_velocity.z = telem.vz;
        msgImu.linear_acceleration.x = telem.ax;
        msgImu.linear_acceleration.y = telem.ay; 
        msgImu.linear_acceleration.z = telem.az; 

        angHeightPub_.publish(msgAngHeight);
        batPub_.publish(msgBattery);
        imuPub_.publish(msgImu);

        rate.sleep();
    }
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool MiloNode::cameraThread(){
    std::cout << "[MILO_NODE] Start Camera Thread" << std::endl;

    ros::Rate rate(50);

    while (fin_ == false && ros::ok()) { 
        if(!drone_->camera()->isReceiving())
            continue;

        cv::Mat img = drone_->camera()->getImage();

        if(img.empty())
            continue;

        std_msgs::Header header;
        header.stamp = ros::Time::now();
        const sensor_msgs::ImagePtr msgImage = cv_bridge::CvImage(header, "bgr8", img).toImageMsg();
        imagePub_.publish(msgImage);

        rate.sleep();
    }
    return true;
}