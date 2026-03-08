cmake_minimum_required(VERSION 3.12)

project(async)

option(WITH_BOOST_TEST "Whether to build Boost test" ON)
option(WITH_DOXYGEN "Build Doxygen documentation" OFF)


# ===== Библиотека async =====
add_library(async SHARED async.cpp)
target_include_directories(async PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)
set_target_properties(async PROPERTIES PUBLIC_HEADER "async.h")


# Демонстрационное приложение
add_executable(async_demo main.cpp)
target_link_libraries(async_demo PRIVATE async)


# Boost.Test
if(WITH_BOOST_TEST AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)
    # Для тестов нужен unit_test_framework
     find_package(Boost REQUIRED COMPONENTS unit_test_framework)
    
    add_executable(async_tests test.cpp async.cpp)
    
    set_target_properties(async_tests PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
    
    target_include_directories(async_tests PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(async_tests PRIVATE ${Boost_LIBRARIES})
    
    target_compile_definitions(async_tests PRIVATE BOOST_TEST_NO_LIB)
    
    enable_testing()
    add_test(NAME AllTests COMMAND async_tests)
    
    message(STATUS "Boost.Test enabled")
endif()

# Установка
install(TARGETS async
    LIBRARY DESTINATION lib          # libasync.so
    PUBLIC_HEADER DESTINATION include # async.h
)
install(TARGETS async_demo RUNTIME DESTINATION bin)


if (MSVC)
    target_compile_options(async PRIVATE /W4)
else ()
    target_compile_options(async PRIVATE
        -Wall -Wextra -pedantic
    )
endif()

install(TARGETS async RUNTIME DESTINATION bin)

include(CPack)
