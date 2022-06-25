aux_source_directory(. net_srcs)
add_library(net ${net_srcs})
add_library(net::library ALIAS net)
target_link_libraries(net base::library)

add_executable(EventLoopTest EventLoop.cc)
target_link_libraries(EventLoopTest net::library base::library)
