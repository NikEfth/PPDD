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

#SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

#set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Set additional project information

PROJECT(PPDD VERSION 0.5 LANGUAGES CXX)
set(COPYRIGHT "Copyright (c) 2017 Nikos Efthimiou. All rights reserved.")

# gcc specific stuff
if (CMAKE_COMPILER_IS_GNUCC)
     add_definitions(-Wall -Wno-deprecated)
     set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -std=gnu++0x -ffast-math")
endif ()

if((${CMAKE_CXX_COMPILER_ID} MATCHES "AppleClang") OR
   (APPLE AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND
    ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# Find the Qt components
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# -- This is required in order to build QT elements
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(ppdd_buildblock "")
add_library(ppdd_display "")
add_library(ppdd_tools "")
add_library(ppdd_ui "")
# add_library(ppdd_icons "")

include(src/CMakeLists.txt)
#ADD_SUBDIRECTORY(src)

add_executable(${PROJECT_NAME} main.cpp resources.qrc)
# Use the Widgets module from Qt 5.
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Gui  -Wl,--start-group ppdd_buildblock ppdd_ui ppdd_display ppdd_tools -Wl,--end-group)


set(CONFIGURED_ONCE TRUE CACHE INTERNAL
    "A flag showing that CMake has configured at least once.")
