

add_executable(udp_client udp_client.cpp)
target_link_libraries(tcp_client)

add_executable(udp_server udp_server.cpp)
target_link_libraries(udp_server)












