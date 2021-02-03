message(STATUS "in <wxh-x86.cmake> ")
# set(CMAKE_SYSTEM_NAME linux)
set(CMAKE_C_COMPILER   /usr/bin/gcc-4.8)
set(CMAKE_CXX_COMPILER /usr/bin/g++-4.8)

#设置x86版本标识符
set(CMAKE_USR_OS 0)
if(${CMAKE_USR_OS} EQUAL 0)
    message(STATUS "IN <OS-LINUX X86>")
elseif(${CMAKE_USR_OS} EQUAL 1)
    message(STATUS "IN <OS-ARM-LINUX>")
endif()
