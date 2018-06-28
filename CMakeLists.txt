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

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set (CMAKE_CXX_STANDARD 11)

# Set additional project information

PROJECT(SAVVY VERSION 0.5 LANGUAGES CXX)
set(COPYRIGHT "Copyright (c) 2017 Nikos Efthimiou. All rights reserved.")

############################  Options: Build What?  ############################
#option(DOWNLOAD_LibFreenect "Downlaod the Freenect libraries v.1" OFF)
#option(DOWNLOAD_LibFreenect2 "Downlaod the Freenect libraries v.2" OFF)
#option(LOCAL_LIBFREENECT "Use Local Freect librarie" OFF)
option(BUILD_DOC "Build documentation" ON)

############################  Options: Build How?  #############################
include(commons)
option_with_print(BUILD_SHARED_LIBS "Build internally built libraries as shared, not static" OFF)
option_with_default(CMAKE_BUILD_TYPE "Build type (Release or Debug)" Debug)
option_with_default(CMAKE_INSTALL_LIBDIR "Directory to which libraries installed" lib)

if(UNIX)
    # Find the Qt components
    find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
endif()

if(APPLE AND EXISTS /usr/local/opt/qt5)
    # Homebrew installs Qt5 (up to at least 5.9.1) in
    # /usr/local/qt5, ensure it can be found by CMake since
    # it is not in the default /usr/local prefix.
    list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt5")
endif()

if(WIN32 AND EXISTS ${QTDIR})
    list(APPEND CMAKE_PREFIX_PATH "${QTDIR}")
endif()

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

    # For building the documentation only in Release mode.
    if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
        if (BUILD_DOC)
            find_package(Doxygen)
            if (DOXYGEN_FOUND)
                # set input and output files
                set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
                set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

                # request to configure the file
                configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
                message("Doxygen build started")

                # note the option ALL which allows to build the docs together with the application
                add_custom_target( doc_doxygen ALL
                    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation with Doxygen"
                    VERBATIM )
            else (DOXYGEN_FOUND)
                message("Doxygen need to be installed to generate the doxygen documentation")
            endif()
        endif()
    endif()

    # -- This is required in order to build QT elements
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    add_library(ppdd_buildblock "")
    add_library(ppdd_display "")
    add_library(ppdd_tools "")
    add_library(ppdd_ui "")

    include(src/CMakeLists.txt)

    qt5_add_resources(PROJECT_RESOURCE resources.qrc)

    add_executable(${PROJECT_NAME} main.cpp ${PROJECT_RESOURCE})
    # Use the Widgets module from Qt 5.
    IF (WIN32)
    add_definitions(-D QWT_DLL)
    target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Gui ppdd_buildblock ppdd_ui ppdd_display ppdd_tools)
    ELSE()
    target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Gui -Wl,--start-group ppdd_buildblock ppdd_ui ppdd_display ppdd_tools -Wl,--end-group)
    ENDIF()

    set(CONFIGURED_ONCE TRUE CACHE INTERNAL
        "A flag showing that CMake has configured at least once.")
