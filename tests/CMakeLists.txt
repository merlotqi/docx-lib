add_executable(docx_tests test_headers.cpp)
target_link_libraries(docx_tests PRIVATE docx::docx)
docx_set_warnings(docx_tests)

add_test(NAME docx_tests COMMAND docx_tests)
