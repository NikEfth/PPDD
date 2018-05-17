
# Find the Qt components
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# -- This is required in order to build QT elements
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Check for libBoard for high quality printing
set(ALGLIB_BASE "" CACHE FILEPATH "Headers path for ALGLIB - Non Linear Regression Tool")
#set(LIBBOARD_LIB "" CACHE FILEPATH "Libs path for libBoard - high quality printing")

include_directories(${ALGLIB_BASE})

if(ALGLIB_BASE)
    message("ALGLIB found!")
    target_sources(ppdd_tools
        PRIVATE
        "${ALGLIB_BASE}/linalg.cpp"
         "${ALGLIB_BASE}/ap.cpp"
         "${ALGLIB_BASE}/interpolation.cpp"
          "${ALGLIB_BASE}/solvers.cpp"
          "${ALGLIB_BASE}/optimization.cpp"
          "${ALGLIB_BASE}/integration.cpp"
         "${ALGLIB_BASE}/statistics.cpp"
         "${ALGLIB_BASE}/fasttransforms.cpp"
         "${ALGLIB_BASE}/alglibinternal.cpp"
         "${ALGLIB_BASE}/alglibmisc.cpp"
         "${ALGLIB_BASE}/dataanalysis.cpp"
         "${ALGLIB_BASE}/diffequations.cpp"
         "${ALGLIB_BASE}/specialfunctions.cpp"
)
endif()

target_sources(ppdd_tools
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ToolManager.h"
        "${CMAKE_CURRENT_LIST_DIR}/ToolManager.inl"
        "${CMAKE_CURRENT_LIST_DIR}/ToolManager.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/ToolManager.ui"
        "${CMAKE_CURRENT_LIST_DIR}/workermanager.h"
        "${CMAKE_CURRENT_LIST_DIR}/workermanager.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/workermanager.ui"
        "${CMAKE_CURRENT_LIST_DIR}/ivim_toolbox.h"
        "${CMAKE_CURRENT_LIST_DIR}/ivim_toolbox.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/ivim_toolbox.ui"
        "${CMAKE_CURRENT_LIST_DIR}/tool.h"
        "${CMAKE_CURRENT_LIST_DIR}/tool.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/worker.h"
        "${CMAKE_CURRENT_LIST_DIR}/worker.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/worker_splitter.h"
        "${CMAKE_CURRENT_LIST_DIR}/worker_splitter.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/worker_thresholder.h"
        "${CMAKE_CURRENT_LIST_DIR}/worker_thresholder.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/worker_math.h"
        "${CMAKE_CURRENT_LIST_DIR}/worker_math.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/pointertool.h"
        "${CMAKE_CURRENT_LIST_DIR}/pointertool.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/linetool.h"
        "${CMAKE_CURRENT_LIST_DIR}/linetool.cpp"
         "${CMAKE_CURRENT_LIST_DIR}/eclipsetool.h"
        "${CMAKE_CURRENT_LIST_DIR}/eclipsetool.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/recttool.h"
        "${CMAKE_CURRENT_LIST_DIR}/recttool.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/pointercrosstool.h"
        "${CMAKE_CURRENT_LIST_DIR}/pointercrosstool.cpp"
) 

#target_link_libraries(ppdd_more_tools PUBLIC )

target_link_libraries(ppdd_tools PUBLIC Qt5::Core Qt5::Widgets Qt5::Gui -Wl,--start-group IO data_buildblock buildblock eval_buildblock recon_buildblock listmode_buildblock modelling_buildblock numerics_buildblock Shape_buildblock display -Wl,--end-group )

target_include_directories(ppdd_tools PUBLIC "${ALGLIB_BASE}" "${CMAKE_CURRENT_LIST_DIR}")