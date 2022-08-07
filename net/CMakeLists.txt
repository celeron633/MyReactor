# search sources
aux_source_directory(. net_srcs)

# build
add_library(net ${net_srcs})
add_library(net::library ALIAS net)

# link
target_link_libraries(net base::library)

# unit test
add_executable(EventLoopTest EventLoop.cc)
target_link_libraries(EventLoopTest net::library base::library)
