cmake_minimum_required(VERSION 3.5)
project(Tempo)

add_library(tempo
  SHARED
    src/chunk.c
    src/lexer.c
    src/parser.c
    src/value.c)
target_include_directories(tempo PUBLIC include)

add_executable(hello src/main.c)
target_link_libraries(hello PRIVATE tempo)

add_subdirectory(test)
