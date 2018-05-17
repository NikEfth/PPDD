target_sources(ppdd_ui
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/mainwindow.h"
        "${CMAKE_CURRENT_LIST_DIR}/progressBar.qml"
        "${CMAKE_CURRENT_LIST_DIR}/mainwindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/mainwindow.ui"
        "${CMAKE_CURRENT_LIST_DIR}/myqlistwidgetitem.h"
        "${CMAKE_CURRENT_LIST_DIR}/myqlistwidgetitem.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/analysisnema.h"
        "${CMAKE_CURRENT_LIST_DIR}/analysisnema.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/analysisnema.ui"
        "${CMAKE_CURRENT_LIST_DIR}/contrastman.h"
        "${CMAKE_CURRENT_LIST_DIR}/contrastman.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/contrastman.ui"
        "${CMAKE_CURRENT_LIST_DIR}/thresholdman.h"
        "${CMAKE_CURRENT_LIST_DIR}/thresholdman.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/thresholdman.ui"
        "${CMAKE_CURRENT_LIST_DIR}/mathman.h"
        "${CMAKE_CURRENT_LIST_DIR}/mathman.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/mathman.ui"
        "${CMAKE_CURRENT_LIST_DIR}/panel_cross_pointer.h"
        "${CMAKE_CURRENT_LIST_DIR}/panel_cross_pointer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/panel_cross_pointer.ui"
        "${CMAKE_CURRENT_LIST_DIR}/panellinepointer.h"
        "${CMAKE_CURRENT_LIST_DIR}/panellinepointer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/panellinepointer.ui"
        "${CMAKE_CURRENT_LIST_DIR}/panelrectpointer.h"
        "${CMAKE_CURRENT_LIST_DIR}/panelrectpointer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/panelrectpointer.ui"
        "${CMAKE_CURRENT_LIST_DIR}/panelellpointer.h"
        "${CMAKE_CURRENT_LIST_DIR}/panelellpointer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/panelellpointer.ui"
)

# Find the Qt components
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# -- This is required in order to build QT elements
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

target_link_libraries(ppdd_ui PUBLIC ppdd_buildblock Qt5::Core Qt5::Widgets Qt5::Gui)
# target_compile_definitions(myLib PUBLIC USE_BARRY)
target_include_directories(ppdd_ui PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
