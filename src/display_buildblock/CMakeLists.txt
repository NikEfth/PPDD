
target_sources(ppdd_display
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/display_screen.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/display_screen.h"
        "${CMAKE_CURRENT_LIST_DIR}/display_screen.inl"
        "${CMAKE_CURRENT_LIST_DIR}/display_screen_container.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/display_screen_container.h"
        "${CMAKE_CURRENT_LIST_DIR}/display_screen_container.ui"
        "${CMAKE_CURRENT_LIST_DIR}/display_screen_raster.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/display_screen_raster.h"
        "${CMAKE_CURRENT_LIST_DIR}/display_screen_curve.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/display_screen_curve.h"
        "${CMAKE_CURRENT_LIST_DIR}/canvaspicker.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/canvaspicker.h"
		    "${CMAKE_CURRENT_LIST_DIR}/bar_screen.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/bar_screen.h"
        "${CMAKE_CURRENT_LIST_DIR}/barscreenhistogram.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/barscreenhistogram.h"
        "${CMAKE_CURRENT_LIST_DIR}/barscreenwl.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/barscreenwl.h"
        "${CMAKE_CURRENT_LIST_DIR}/screen_manager.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/screen_manager_projdata.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/screen_manager.ui"
    	  "${CMAKE_CURRENT_LIST_DIR}/screen_manager.h"
    	  "${CMAKE_CURRENT_LIST_DIR}/screen_manager.inl"
    	  "${CMAKE_CURRENT_LIST_DIR}/screen_manager_projdata.h"
    	  "${CMAKE_CURRENT_LIST_DIR}/screen_manager_projdata.inl"
    	  "${CMAKE_CURRENT_LIST_DIR}/screen_manager_imagedata.h"
    	  "${CMAKE_CURRENT_LIST_DIR}/screen_manager_imagedata.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/listroistat.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/listroistat.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/listroistat.ui"
) 

# QWT - required
find_path(QWT_INCLUDE_DIR NAMES qwt_plot.h PATH_SUFFIXES qwt qwt-qt5 REQUIRED)
set ( QWT_INCLUDE_DIRS ${QWT_INCLUDE_DIR})
find_library ( QWT_LIBRARY
     NAMES qwt qwt-qt3 qwt-qt4 qwt-qt5 IMPORTED)

include_directories(${QWT_INCLUDE_DIR})

find_package(ITK QUIET)

if (ITK_FOUND) 
  message(STATUS "ITK libraries added.")
  set(HAVE_ITK ON)
  include(${ITK_USE_FILE})
  find_library( ITK_LIBRARIES NAMES ITKCommon IMPORTED)
else()
  message(STATUS "ITK support disabled.")
endif()

# Find the Qt components
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# -- This is required in order to build QT elements
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# STIR required
# find_package(STIR 3.1.0)
# if (STIR_FOUND)

#   # need to add this explicitly to get the STIR_REGISTRIES to compile
#   include_directories(STIR_INCLUDE_DIRS ${STIR_INCLUDE_DIR})
#   find_library(STIR_LIBRARY NAMES 
#   	IO data_buildblock buildblock eval_buildblock listmode_buildblock modelling_buildblock numerics_buildblock Shape_buildblock display)

# else()
  #message(WARNING "STIR not found. This is probably not what you want. If you need it, set STIR_DIR.")
  set(STIR_DIR /home/nikos/Documents/install-HSTIR_outwards-Desktop-Debug/)
  include_directories(STIR_INCLUDE_DIRS ${STIR_DIR}/include)
  link_directories(${STIR_DIR}/lib)

  find_library (STIR_LIBRARY
  	NAMES IO data_buildblock buildblock eval_buildblock listmode_buildblock modelling_buildblock numerics_buildblock Shape_buildblock display
  	PATHS /home/nikos/Documents/install-HSTIR_outwards-Desktop-Debug/lib)
# endif()

message(STATUS ${STIR_LIBRARIES})

# Check for libBoard for high quality printing
set(LIBBOARD_BASE "" CACHE FILEPATH "Headers path for libBoard - high quality printing")
#set(LIBBOARD_LIB "" CACHE FILEPATH "Libs path for libBoard - high quality printing")

include_directories(${LIBBOARD_BASE}/include)

find_library(LIBBOARD_LIBRARY
  NAMES board
  PATHS ${LIBBOARD_BASE}/lib)

if(LIBBOARD_LIBRARY)
  message("libBoard library found!")
  set(LIBBOARD_FOUND ON)
else()
  message(WARNING "libBoard library not found")
  set(LIBBOARD_FOUND OFF)
endif()

target_link_libraries(ppdd_display PUBLIC ${LIBBOARD_LIBRARY} ${ITK_LIBRARIES} Qt5::Core Qt5::Widgets ${QWT_LIBRARY} Qt5::Gui -Wl,--start-group IO data_buildblock buildblock eval_buildblock listmode_buildblock modelling_buildblock numerics_buildblock Shape_buildblock display -Wl,--end-group libgsl.so libgslcblas.so)
#target_include_directories(ppdd_display PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
install(TARGETS ppdd_display DESTINATION lib)
