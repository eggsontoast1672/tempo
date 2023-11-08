cmake_minimum_required(VERSION 3.5)
project(Tempo)

add_executable(tempo src/chunk.c src/compiler.c src/lexer.c src/main.c src/value.c)
target_include_directories(tempo PRIVATE include)
