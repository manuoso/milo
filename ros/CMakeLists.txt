##---------------------------------------------------------------------------------------------------------------------
##  MILO
##---------------------------------------------------------------------------------------------------------------------
##  Copyright 2021 Manuel Pérez Jiménez (a.k.a. manuoso) manuperezj@gmail.com
##---------------------------------------------------------------------------------------------------------------------
##  Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
##  and associated documentation files (the "Software"), to deal in the Software without restriction, 
##  including without limitation the rights to use, copy, modify, merge, publish, distribute, 
##  sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
##  furnished to do so, subject to the following conditions:
##
##  The above copyright notice and this permission notice shall be included in all copies or substantial 
##  portions of the Software.
##
##  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
##  BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
##  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES 
##  OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
##  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
##---------------------------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.14 FATAL_ERROR)
project(milo_ros)

#########################################
###### Loading 3rd party libraries.######
#########################################

find_package(catkin REQUIRED COMPONENTS
                roscpp
				std_msgs
                sensor_msgs
                geometry_msgs
				std_srvs
                cv_bridge
                image_transport
				)

catkin_package(CATKIN_DEPENDS 
                roscpp 
                std_msgs 
                sensor_msgs
                geometry_msgs
                std_srvs
                cv_bridge
                image_transport
)

#########################################

set(Boost_USE_MULTITHREAD ON)

find_package(Boost REQUIRED COMPONENTS system thread regex)

find_package(OpenCV)

#########################################

# Collect all sources
file(GLOB_RECURSE MILO_ROS_SRC_FILES "src/*.c" "src/*.cpp" "../c++/src/*.cpp")
file(GLOB_RECURSE MILO_ROS_HDR_FILES "include/*.inl" "include/*.h" "../c++/include/*.h")

# Add Executable
add_executable(${PROJECT_NAME} ${MILO_ROS_SRC_FILES} ${MILO_ROS_HDR_FILES})
include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/../c++/include)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

target_include_directories(${PROJECT_NAME} PUBLIC ${catkin_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${catkin_LIBRARIES})

target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES})

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC avcodec swscale avutil)

target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${OpenCV_LIBRARIES})
