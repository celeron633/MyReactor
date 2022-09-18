aux_source_directory(. HttpServer_srcs)

# build && link
add_executable(HttpServer ${HttpServer_srcs})
target_link_libraries(HttpServer net::library base::library ${JSONCPP_LIBRARIES})