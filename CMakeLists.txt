cmake_minimum_required(VERSION 3.10)

project (pyser)

set(SOURCES
    src/main.cpp
)

add_executable(pyser
    ${SOURCES}
)

target_include_directories(pyser
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/src
)
