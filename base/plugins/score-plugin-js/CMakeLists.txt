cmake_minimum_required(VERSION 3.1)
project(score_plugin_js LANGUAGES CXX)

if(NOT TARGET score_plugin_engine)
    return()
endif()

score_common_setup()

# Packages
find_package(Qt5 5.3 REQUIRED COMPONENTS Core Widgets Qml)
if(NOT TARGET score_plugin_engine)
  return()
endif()

# Files & main target
set(HDRS
  "${CMAKE_CURRENT_SOURCE_DIR}/score_plugin_js.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Commands/EditScript.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Commands/JSCommandFactory.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Executor/Component.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Executor/JSAPIWrapper.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Executor/ProcessScript.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Executor/StateComponent.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Inspector/JSInspectorFactory.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/Inspector/JSInspectorWidget.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/JSProcessFactory.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/JSProcessMetadata.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/JSProcessModel.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/JS/JSStateProcess.hpp"
)
set(SRCS
"${CMAKE_CURRENT_SOURCE_DIR}/JS/JSStateProcess.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/JS/JSProcessModel.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/JS/JSProcessModelSerialization.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/JS/Executor/StateComponent.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/JS/Executor/Component.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/JS/Executor/JSAPIWrapper.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/JS/Commands/EditScript.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/JS/Commands/JSCommandFactory.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/JS/Inspector/JSInspectorWidget.cpp"

"${CMAKE_CURRENT_SOURCE_DIR}/score_plugin_js.cpp"
)

add_library(${PROJECT_NAME} ${SRCS} ${HDRS})
# TODO make this automatic in some way, and extend to other components
score_generate_command_list_file(${PROJECT_NAME} "${HDRS}")

target_link_libraries(${PROJECT_NAME} PUBLIC
                     Qt5::Core Qt5::Widgets Qt5::Qml
                     score_lib_base score_plugin_engine score_lib_process score_lib_inspector score_lib_device score_plugin_deviceexplorer)

setup_score_plugin(${PROJECT_NAME})
