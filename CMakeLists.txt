cmake_minimum_required(VERSION 3.2)

# set the project name and version
project(AsioLearning VERSION 1.0)

include_directories("asio-1.18/include")
add_executable(${PROJECT_NAME} main.cpp )

# 折叠ALL_BUILD 和 ZERO_CHECK 项:
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# VS默认启动项设置:
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})




