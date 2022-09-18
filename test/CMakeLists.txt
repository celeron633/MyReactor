# build unit test
add_executable(EventLoopTest EventLoopTest.cc)
target_link_libraries(EventLoopTest net::library base::library)

add_executable(TcpServerTest TcpServerTest.cc)
target_link_libraries(TcpServerTest net::library base::library)