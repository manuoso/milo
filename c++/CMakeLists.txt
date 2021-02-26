##---------------------------------------------------------------------------------------------------------------------
##  MILO
##---------------------------------------------------------------------------------------------------------------------
##  Copyright 2020 Manuel Pérez Jiménez (a.k.a. manuoso) manuperezj@gmail.com
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
project(milo VERSION 0.1 LANGUAGES C CXX)

#########################################
######	   Configure project       ######
#########################################

# Some build options
option(BUILD_TOOLS "Build tools module" OFF)
option(BUILD_EXAMPLES "Build example module" ON)

#########################################
###### Loading 3rd party libraries ######
#########################################

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
       FORCE )
endif()

# set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/" ${CMAKE_MODULE_PATH})

list(APPEND MILO_DEFINITIONS "")

#########################################

set(Boost_USE_MULTITHREAD ON)

find_package(Boost REQUIRED COMPONENTS system thread regex)

find_package(OpenCV)

#########################################
######			 Library           ######
#########################################

# Collect all sources
file(GLOB_RECURSE MILO_SOURCE_FILES "src/*.c" "src/*.cpp")
file(GLOB_RECURSE MILO_HEADER_FILES "include/*.inl" "include/*.h")

add_library(${PROJECT_NAME} SHARED ${MILO_SOURCE_FILES} ${MILO_HEADER_FILES})

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
######			Documentation	   ######
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
######			 Tools             ######
#########################################

if(${BUILD_TOOLS})
	add_subdirectory(tools)
endif(${BUILD_TOOLS})

#########################################
######			 Examples          ######
#########################################

if(${BUILD_EXAMPLES})
	add_subdirectory(examples)
endif(${BUILD_EXAMPLES})

#########################################
######			 Install           ######
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

configure_file(miloConfig.cmake.in miloConfig.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/miloConfig.cmake"
        DESTINATION lib/cmake/milo
        )

# uninstall target 
if(NOT TARGET uninstall) 
  configure_file( 
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" 
      "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" 
      IMMEDIATE @ONLY) 
 
  add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake) 
endif() 
