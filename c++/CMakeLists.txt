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
project(milo VERSION 1.0 LANGUAGES C CXX)

#########################################
######	Configure project    
#########################################

# Some build options
option(BUILD_APPS "Build Applications module" OFF)
option(BUILD_EXAMPLES "Build example module" ON)

#########################################
######	Loading 3rd party libraries
#########################################

if( NOT CMAKE_BUILD_TYPE )
  	set( CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
		FORCE )
endif()

list(APPEND MILO_DEFINITIONS "")

#########################################

set(Boost_USE_MULTITHREAD ON)
find_package(Boost REQUIRED COMPONENTS system thread regex)

find_package(OpenCV REQUIRED)

# Look for <linux/joystick.h>
include(CheckIncludeFiles)
check_include_files(linux/joystick.h HAVE_LINUX_JOYSTICK_H)
if(${HAVE_LINUX_JOYSTICK_H})
	file(GLOB_RECURSE JOY_SOURCE_FILES "src/modules/joystick/*.c" "src/modules/joystick/*.cpp")
	file(GLOB_RECURSE JOY_HEADER_FILES "include/milo/modules/joystick/*.h" "include/milo/modules/joystick/*.hpp" "include/milo/modules/joystick/*.inl")
else()
  	message(WARNING "Not found <linux/joystick.h>")
	file(GLOB_RECURSE JOY_SOURCE_FILES "")
	file(GLOB_RECURSE JOY_HEADER_FILES "")
endif()

#########################################
######	Library
#########################################

# Collect all sources
file(GLOB_RECURSE CMD_SOURCE_FILES "src/modules/command/*.c" "src/modules/command/*.cpp")
file(GLOB_RECURSE CMD_HEADER_FILES "include/milo/modules/command/*.h" "include/milo/modules/command/*.hpp" "include/milo/modules/command/*.inl")

file(GLOB_RECURSE CAM_SOURCE_FILES "src/modules/camera/*.c" "src/modules/camera/*.cpp")
file(GLOB_RECURSE CAM_HEADER_FILES "include/milo/modules/camera/*.h" "include/milo/modules/camera/*.hpp" "include/milo/modules/camera/*.inl")

file(GLOB_RECURSE TEL_SOURCE_FILES "src/modules/telemetry/*.c" "src/modules/telemetry/*.cpp")
file(GLOB_RECURSE TEL_HEADER_FILES "include/milo/modules/telemetry/*.h" "include/milo/modules/telemetry/*.hpp" "include/milo/modules/telemetry/*.inl")

file(GLOB_RECURSE SOC_SOURCE_FILES "src/modules/socket/*.c" "src/modules/socket/*.cpp")
file(GLOB_RECURSE SOC_HEADER_FILES "include/milo/modules/socket/*.h" "include/milo/modules/socket/*.hpp" "include/milo/modules/socket/*.inl")

file(GLOB_RECURSE LOG_SOURCE_FILES "src/modules/logger/*.c" "src/modules/logger/*.cpp")
file(GLOB_RECURSE LOG_HEADER_FILES "include/milo/modules/logger/*.h" "include/milo/modules/logger/*.hpp" "include/milo/modules/logger/*.inl")

file(GLOB_RECURSE MILO_SOURCE_FILES "src/milo.c" "src/milo.cpp")
file(GLOB_RECURSE MILO_HEADER_FILES "include/milo.inl" "include/milo.h")

add_library(${PROJECT_NAME} SHARED ${MILO_SOURCE_FILES} ${MILO_HEADER_FILES} 
									${JOY_SOURCE_FILES} ${JOY_HEADER_FILES}
									${CMD_SOURCE_FILES} ${CMD_HEADER_FILES}
									${CAM_SOURCE_FILES} ${CAM_HEADER_FILES}
									${TEL_SOURCE_FILES} ${TEL_HEADER_FILES}
									${SOC_SOURCE_FILES} ${SOC_HEADER_FILES}
									${LOG_SOURCE_FILES} ${LOG_HEADER_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include) 

target_compile_definitions(${PROJECT_NAME} PUBLIC ${MILO_DEFINITIONS})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

#########################################

target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES})

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC avcodec swscale avutil)

target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${OpenCV_LIBRARIES})

#########################################
######	Documentation
#########################################

# find_package(Doxygen)
# if(DOXYGEN_FOUND)
# 	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
# 	add_custom_target(	doc_rgbd_tools
# 						${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
# 						WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
# 						COMMENT "Generating API documentation with Doxygen" VERBATIM )
# endif(DOXYGEN_FOUND)

#########################################
######	Applications        
#########################################

if(${BUILD_APPS})
	add_subdirectory(applications)
endif(${BUILD_APPS})

#########################################
######	Examples
#########################################

if(${BUILD_EXAMPLES})
	add_subdirectory(examples)
endif(${BUILD_EXAMPLES})

#########################################
######	Install
#########################################

include(CMakePackageConfigHelpers)

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  	set(CMAKE_INSTALL_PREFIX "/usr/local")
endif() 

install(TARGETS milo
	EXPORT miloTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	INCLUDES DESTINATION ${CMAKE_INSTALL_PREFIX}
	)

write_basic_package_version_file(
	miloConfigVersion.cmake
	VERSION ${PACKAGE_VERSION}
	COMPATIBILITY AnyNewerVersion
	)

# This installs the include folder
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX} FILES_MATCHING PATTERN "*.h")
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX} FILES_MATCHING PATTERN "*.inl")

export(TARGETS milo NAMESPACE milo:: FILE miloTargets.cmake)

# This generates miloTargets.cmake
install(EXPORT miloTargets
    FILE miloTargets.cmake
    NAMESPACE milo::
    DESTINATION lib/cmake/milo
    )

configure_file(cmake/miloConfig.cmake.in cmake/miloConfig.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cmake/miloConfig.cmake"
	DESTINATION lib/cmake/milo
	)

# uninstall target 
if(NOT TARGET uninstall) 
	configure_file( 
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in" 
		"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake" 
		IMMEDIATE @ONLY) 
 
  add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake) 
endif() 
