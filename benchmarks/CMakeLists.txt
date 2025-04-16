option(ENABLE_BM "Enables benchmark tests" OFF)

if(ENABLE_BM)
    # Enable Google Benchmark
    include(FetchContent)
    set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON CACHE BOOL "Let Benchmark download gtest")
    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(benchmark)

    option(ENABLE_BOOST "Enable Boost benchmark" OFF)
    if(ENABLE_BOOST)
        find_package(Boost REQUIRED)
        set(CMAKE_INCLUDE_CURRENT_DIR ON)
        add_definitions(-DUSE_BOOST)
    endif()

    # Optional Qt
    option(ENABLE_QT "Enable Qt benchmark" OFF)
    if(ENABLE_QT)
        find_package(Qt5 REQUIRED COMPONENTS Core)
        set(CMAKE_AUTOMOC ON)
        set(CMAKE_INCLUDE_CURRENT_DIR ON)
        add_definitions(-DUSE_QT)
    endif()

    # Benchmark executable
    add_executable(benchmark_pubsub
        main.cpp
    )

    # Includes
    target_include_directories(benchmark_pubsub PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    # Linking
    target_link_libraries(benchmark_pubsub
        benchmark::benchmark
        benchmark::benchmark_main
    )

    if(ENABLE_BOOST)
        target_include_directories(benchmark_pubsub PRIVATE
            ${Boost_INCLUDE_DIRS}
        )
        target_link_libraries(benchmark_pubsub ${Boost_LIBRARIES})
    endif()

    if(ENABLE_QT)
        target_link_libraries(benchmark_pubsub Qt5::Core)
    endif()
endif()