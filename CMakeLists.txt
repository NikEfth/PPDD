#   Copyright 2017 Nikos Efthimiou (nikos.efthimiou AT gmail.com)

#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at

#       http://www.apache.org/licenses/LICENSE-2.0

#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.


cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

project(Sino_viewer VERSION 0.1)

# Set additional project information
set(COMPANY "UoH")
set(COPYRIGHT "Copyright (c) 2017 Nikos. All rights reserved.")
#set(IDENTIFIER "com.example.Example")

if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")
endif()

#### we need the boost library from boost.org
set(BOOST_ROOT CACHE PATH "root of Boost")
find_package( Boost 1.36.0 REQUIRED )
include_directories(${Boost_INCLUDE_DIRS})

# Find ROOT ...  in any case. 

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

find_path(QWT_INCLUDE_DIR NAMES qwt_plot.h PATH_SUFFIXES qwt qwt-qt5)
set ( QWT_INCLUDE_DIRS ${QWT_INCLUDE_DIR} )
include_directories(${QWT_INCLUDE_DIR})

find_library ( QWT_LIBRARY
     NAMES qwt qwt-qt3 qwt-qt4 qwt-qt5
   )

set ( QWT_LIBRARIES ${QWT_LIBRARY} )

find_package(STIR)

include_directories(/home/nikos/Workspace/install_STIR/include)
#include_directories(${CMAKE_SOURCE_DIR})

file(GLOB HEADERS "*.h")
file(GLOB SOURCES "*.cpp")
file(GLOB UIS "*.ui")
file(GLOB RESOURCES "*.qrc")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.


# Find the QtWidgets library
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

LINK_DIRECTORIES(/home/nikos/Workspace/install_STIR/lib)

# Tell CMake to create the helloworld executable
add_executable(${PROJECT_NAME} ${SOURCES} ${UIS} ${RESOURCES})

# Use the Widgets module from Qt 5.
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets ${QWT_LIBRARIES} Qt5::Gui -Wl,--start-group IO data_buildblock buildblock eval_buildblock listmode_buildblock modelling_buildblock numerics_buildblock Shape_buildblock display -Wl,--end-group)

