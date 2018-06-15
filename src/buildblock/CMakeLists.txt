
target_sources(ppdd_buildblock
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/common.h"
        "${CMAKE_CURRENT_LIST_DIR}/configuration.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/configuration.h"
        "${CMAKE_CURRENT_LIST_DIR}/configuration.ui"
        "${CMAKE_CURRENT_LIST_DIR}/aboutdialog.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/aboutdialog.h"
        "${CMAKE_CURRENT_LIST_DIR}/aboutdialog.ui"
        "${CMAKE_CURRENT_LIST_DIR}/dialog.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/dialog.h"
        "${CMAKE_CURRENT_LIST_DIR}/dialog.ui"
        "${CMAKE_CURRENT_LIST_DIR}/dataseries.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/dataseries.h"
) 

# QWT - required
find_path(QWT_INCLUDE_DIR NAMES qwt_plot.h PATH_SUFFIXES qwt qwt-qt5 REQUIRED)
set ( QWT_INCLUDE_DIRS ${QWT_INCLUDE_DIR})
find_library ( QWT_LIBRARY
     NAMES qwt qwt-qt3 qwt-qt4 qwt-qt5 IMPORTED)

include_directories(${QWT_INCLUDE_DIR})

# Find the Qt components
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# -- This is required in order to build QT elements
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

IF (WIN32)
	add_definitions(-D QWT_DLL)
	target_link_libraries(ppdd_buildblock PUBLIC Qt5::Core Qt5::Widgets ${QWT_LIBRARY} Qt5::Gui)
ELSE()
     target_link_libraries(ppdd_buildblock PUBLIC Qt5::Core Qt5::Widgets ${QWT_LIBRARY} Qt5::Gui)
ENDIF()

install(TARGETS ppdd_buildblock DESTINATION lib)
