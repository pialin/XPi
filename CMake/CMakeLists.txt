cmake_minimum_required(VERSION 2.8)		#最低的CMake版本要求
project(XPi)					#定义项目名称
set (CMAKE_CXX_STANDARD 11)			#添加C++11支持
find_package(OpenCV REQUIRED)			#寻找所需的OpenCV库，可能需要将OpenCV_DIR变量设置为包含OpenCVConfig.cmake文件的路径
message(STATUS "OpenCV library status:")	#OpenCVConfig.cmake中设定了一些关于OpenCV的变量的值，比如OpenCV的版本信息、库所在路径、头文件所在路径等
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

if(CMAKE_VERSION VERSION_LESS "2.8.11")		#如果CMake的版本较低，需要手动将OpenCV头文件所在路径加入搜索范围
  include_directories(${OpenCV_INCLUDE_DIRS})
endif()

add_executable(XPi ../Source/CPlusPlus/OpenCameraWindow.cpp)
						#添加程序源代码文件
target_link_libraries(XPi ${OpenCV_LIBS})	#添加OpenCV链接库的路径
