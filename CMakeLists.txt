cmake_minimum_required(VERSION 3.5)
project(Tempo)

add_executable(tempo src/lexer.c src/main.c)
target_include_directories(tempo PRIVATE include)
