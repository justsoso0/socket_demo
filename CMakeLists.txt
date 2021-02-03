#工程名称
project(DEMO) 
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-as-needed -std=c++11 -fPIC -DBOOST_NO_CXX11_SCOPED_ENUMS ")
# set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}")

#查找boost库
find_package(Boost)
if(Boost_INCLUDE_DIR)
  message("Boost found in ${Boost_INCLUDE_DIR}")
  include_directories(${Boost_INCLUDE_DIR})
else(Boost_INCLUDE_DIR)
  message(FATAL_ERROR "Boost_INCLUDE_DIR not found ! Add it to your CMAKE_PREFIX_PATH !")
endif(Boost_INCLUDE_DIR)
link_directories(${Boost_LIBRARY_DIR})

# add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY)




#根据版本标识符添加库
if(${CMAKE_USR_OS} EQUAL 0)
    message(STATUS "link directories of linux-os-x86")

elseif(${CMAKE_USR_OS} EQUAL 1)
    message(STATUS "link directories of linux-os-arm")
else()
    message(FATAL_ERROR "link directory failed!")
endif()

# 设置编译选项
add_subdirectory(TCP_demo)
add_subdirectory(UDP_demo)












