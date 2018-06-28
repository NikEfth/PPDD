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
     NAMES qwt qwtd qwt-qt3 qwt-qt4 qwt-qt5 IMPORTED)

if(UNIX OR APPLE)
find_library (GSL_LIBRARY
	NAMES gsl IMPORTED)

find_library (GSL_BLAS_LIBRARY
NAMES gslcblas IMPORTED)
endif()

if(WIN32)
find_library (GSL_LIBRARY
  NAMES gsl_ddl gsl_ddl_d IMPORTED)

find_library (GSL_BLAS_LIBRARY
NAMES gslcblas_ddl gslcblas_ddl_d IMPORTED)
endif()

include_directories(${QWT_INCLUDE_DIR})

find_package(ITK QUIET)

if (ITK_FOUND) 
  message(STATUS "ITK libraries added.")
  set(HAVE_ITK ON)
  include(${ITK_USE_FILE})
  find_library( ITK_LIBRARIES NAMES ITKCommon IMPORTED)
else()
  message(STATUS "ITK support disabled.")
set(HAVE_ITK OFF)
endif()

# Find the Qt components
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# -- This is required in order to build QT elements
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(STIR_INCLUDE_DIRS "default value" CACHE FILEPATH "description")
set(STIR_LIBRARY "default value" CACHE FILEPATH "description")
include_directories( ${STIR_INCLUDE_DIRS})
link_directories(${STIR_LIBRARY})

find_library (STIR_LIBRARY
  	NAMES IO data_buildblock buildblock eval_buildblock listmode_buildblock modelling_buildblock numerics_buildblock Shape_buildblock display)

message(STATUS ${STIR_LIBRARIES})

IF (WIN32)
	add_definitions(-D QWT_DLL)
	target_link_libraries(ppdd_display PUBLIC ${ITK_LIBRARIES} Qt5::Core Qt5::Widgets ${QWT_LIBRARY} Qt5::Gui IO data_buildblock buildblock eval_buildblock listmode_buildblock modelling_buildblock numerics_buildblock Shape_buildblock display ${GSL_LIBRARY} ${GSL_BLAS_LIBRARY})
ELSE()
  target_link_libraries(ppdd_display PUBLIC ${ITK_LIBRARIES} Qt5::Core Qt5::Widgets ${QWT_LIBRARY} Qt5::Gui IO data_buildblock buildblock eval_buildblock listmode_buildblock modelling_buildblock numerics_buildblock Shape_buildblock display ${GSL_LIBRARY} ${GSL_BLAS_LIBRARY})
ENDIF()


install(TARGETS ppdd_display DESTINATION lib)
