#设置cmake的最低版本和项目名称
cmake_minimum_required(VERSION 3.0)
project(mprpc)

#生成debug版本，可以进行gdb调试(不需要调试就注释掉)
#set(CMAKE_BUILD_TYPE "Debug")

#设置项目可执行文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#设置项目库文件输出的路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#PROJECT_SOURCE_DIR是顶级cmake文件所在的目录即是/home/zgq/项目/mprpc
#PROJECT_BINARY_DIR指的是cmake生成的make文件所在的路径一般指build的路径
#设置项目编译头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)
#设置项目库文件的搜索路径
link_directories(${PROJECT_SOURCE_DIR}/lib)


add_subdirectory(src)
add_subdirectory(example)