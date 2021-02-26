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

MiloNode *miloNode;

//---------------------------------------------------------------------------------------------------------------------
// Replacement handler
void finishHandler(int _sig){
    std::cout << "Finish Handler: Catch signal " << _sig << std::endl;
    bool finish = miloNode->finalize();
    if(!finish){
        std::cout << "App not finished cleanly, please check it." << std::endl;
    }
}

//---------------------------------------------------------------------------------------------------------------------
// Replacement "shutdown" XMLRPC callback
void shutdownCallback(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result){
    int num_params = 0;
    if (params.getType() == XmlRpc::XmlRpcValue::TypeArray)
        num_params = params.size();

    if (num_params > 1){
        std::string reason = params[1];
        ROS_WARN("Shutdown request received. Reason: [%s]", reason.c_str());
        bool finish = miloNode->finalize();
        if(!finish){
            std::cout << "App not finished cleanly, please check it." << std::endl;
        }
    }

    result = ros::xmlrpc::responseInt(1, "", 0);
}

//---------------------------------------------------------------------------------------------------------------------
int main(int _argc, char **_argv) {

    signal(SIGINT, finishHandler);
    signal(SIGTERM, finishHandler);

    // Override SIGINT handler
    ros::init(_argc, _argv, "milo_node", ros::init_options::NoSigintHandler);

    // Override XMLRPC shutdown
    ros::XMLRPCManager::instance()->unbind("shutdown");
    ros::XMLRPCManager::instance()->bind("shutdown", shutdownCallback);
    
    ros::AsyncSpinner rosSpinner(4); // Use 4 thread
    rosSpinner.start();

    miloNode = new MiloNode();
    
    if(!miloNode->init()){
        std::cout << "Error initializing the application" << std::endl;
        return -1;
    }

    bool finish = false;
	while(!finish){
        finish = miloNode->run();
	}

    return 0;
}
