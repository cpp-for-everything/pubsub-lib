file(GLOB EXAMPLE_SOURCES "*.cpp")

foreach(example ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example} NAME_WE)
    add_executable(${example_name} ${example})
    target_include_directories(${example_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    target_link_libraries(${example_name} PUBLIC pubsub)
endforeach()
