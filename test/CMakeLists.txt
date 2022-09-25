# build unit test
add_executable(EventLoopTest EventLoopTest.cc)
target_link_libraries(EventLoopTest net::library base::library)

add_executable(TcpServerTest TcpServerTest.cc)
target_link_libraries(TcpServerTest net::library base::library util::library)

add_executable(TimestampTest TimestampTest.cc)
target_link_libraries(TimestampTest base::library)

add_executable(TimerQueueTest TimerQueueTest.cc)
target_link_libraries(TimerQueueTest base::library net::library)

add_executable(LogTest LogTest.cc)
target_link_libraries(LogTest base::library net::library)

add_executable(TcpClientTest TcpClientTest.cc)
target_link_libraries(TcpClientTest net::library base::library util::library)

add_executable(HttpServerTest HttpServerTest.cc)
target_link_libraries(HttpServerTest net::library base::library util::library)

add_executable(UtilTest UtilTest.cc)
target_link_libraries(UtilTest net::library base::library util::library)