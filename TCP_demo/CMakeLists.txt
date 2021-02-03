
add_executable(e_server e_server.cpp)
target_link_libraries(e_server)

add_executable(s_server e_server.cpp)
target_link_libraries(s_server)

add_executable(tcp_client tcp_client.cpp)
target_link_libraries(tcp_client pthread)











