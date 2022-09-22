aux_source_directory(. util_srcs)

# build 
add_library(util ${util_srcs})
add_library(util::library ALIAS util)