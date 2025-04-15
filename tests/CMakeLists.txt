include(Catch)

file(GLOB TEST_SOURCES "*.cpp")

foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    target_link_libraries(${test_name} PRIVATE Catch2::Catch2WithMain)
    
    # Use Catch2 to auto-register test cases with CTest
    catch_discover_tests(${test_name})
endforeach()
