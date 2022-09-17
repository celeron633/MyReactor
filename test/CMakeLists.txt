# search sources
aux_source_directory(. test_srcs)

# build unit test
add_executable(EventLoopTest EventLoopTest.cpp)
target_link_libraries(EventLoopTest net::library base::library)
