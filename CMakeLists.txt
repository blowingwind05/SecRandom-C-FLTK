cmake_minimum_required(VERSION 3.15)
project(SecRandomApp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 寻找 FLTK 库
find_package(FLTK REQUIRED)

# 包含头文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${FLTK_INCLUDE_DIR})

# 收集所有的 cpp 代码
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 编译可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接 FLTK 和其他必要的库
target_link_libraries(${PROJECT_NAME} ${FLTK_LIBRARIES})
