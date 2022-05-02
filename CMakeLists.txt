cmake_minimum_required(VERSION 3.10)

project (pyser)

aux_source_directory(src SRC)

add_executable(pyser
    ${SRC}
)

target_compile_features(pyser PRIVATE cxx_std_20)

target_include_directories(pyser
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/src
)

