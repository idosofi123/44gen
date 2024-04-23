cmake_minimum_required(VERSION 3.16)

project(44gen LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCE_FILE_LIST
"src/Main.cpp"
)

add_executable(44gen ${SOURCE_FILE_LIST})

target_compile_features(44gen PRIVATE cxx_std_20)