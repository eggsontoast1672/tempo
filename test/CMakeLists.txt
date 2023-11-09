function(create_test testname)
    add_executable(${testname} src/${testname}.c)
    target_link_libraries(${testname} PRIVATE tempo)
    add_test(NAME ${testname} COMMAND ${testname})
endfunction()

enable_testing()

create_test(test_parser)
