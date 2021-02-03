message(STATUS "in <wxh_arm.cmake> ")
# set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER   /usr/bin/arm-linux-gnueabihf-gcc-4.8)
set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++-4.8)


#设置arm版本标识符
set(CMAKE_USR_OS 1)
if(${CMAKE_USR_OS} EQUAL 0)
    message(STATUS "IN <OS-LINUX X86>")
elseif(${CMAKE_USR_OS} EQUAL 1)
    message(STATUS "IN <OS-ARM-LINUX>")
endif()
