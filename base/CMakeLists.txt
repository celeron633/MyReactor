# search sources
aux_source_directory(. base_srcs)

# build 
add_library(base ${base_srcs})
add_library(base::library ALIAS base)

# link
target_link_libraries(base Threads::Threads)
