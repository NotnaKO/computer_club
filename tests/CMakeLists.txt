# GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/releases/download/v1.15.2/googletest-1.15.2.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test cases

file(GLOB_RECURSE RESOURCE_FILES CONFIGURE_DEPENDS
        LIST_DIRECTORIES false
        "${CMAKE_CURRENT_SOURCE_DIR}/cases/*"
)
set(CASES_STAMP "${CMAKE_CURRENT_BINARY_DIR}/cases.stamp")
add_custom_command(
        OUTPUT ${CASES_STAMP}
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/cases"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/cases" "${CMAKE_CURRENT_BINARY_DIR}/cases"
        COMMAND ${CMAKE_COMMAND} -E touch ${CASES_STAMP}
        DEPENDS ${RESOURCE_FILES}
        COMMENT "Syncing test cases"
)

add_custom_target(
        SyncResources ALL DEPENDS ${CASES_STAMP}
)


# Executable
add_executable(computeClub_tests
        test_main.cpp
        ../src/club.cpp
        ../src/tableState.cpp
        ../src/outputAdapter.cpp
)

target_link_libraries(computeClub_tests gtest_main)
include_directories(${CMAKE_SOURCE_DIR}/src)

add_dependencies(computeClub_tests SyncResources)

# Computer Club executable
add_dependencies(computeClub_tests computerClub)
file(GLOB_RECURSE EXEC_FILES CONFIGURE_DEPENDS
        LIST_DIRECTORIES false
        "${CMAKE_SOURCE_DIR}/src/*"
)

set(EXEC_STAMP "${CMAKE_CURRENT_BINARY_DIR}/exec.stamp")
add_custom_command(
        OUTPUT ${EXEC_STAMP}
        COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE_DIR:computeClub_tests>/computerClub
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:computerClub> $<TARGET_FILE_DIR:computeClub_tests>/computerClub
        COMMAND ${CMAKE_COMMAND} -E touch ${EXEC_STAMP}
        DEPENDS ${EXEC_FILES}
        COMMENT "Syncing executable"
)
add_custom_target(
        SyncExec ALL DEPENDS ${EXEC_STAMP}
)
add_dependencies(computeClub_tests SyncExec)
