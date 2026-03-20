
cmake_minimum_required(VERSION 3.25)
project(Heartburner LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(DLL_NAME ${PROJECT_NAME}_game)

# Flag .EXE specific files
set(EXE_EXCLUSIVE
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/arena.cpp
)

# Create the file references
file(GLOB_RECURSE DLL_A_FILES "${CMAKE_SOURCE_DIR}/lib/*.dll.a")
file(GLOB_RECURSE DLL_FILES "${CMAKE_SOURCE_DIR}/lib/*.dll")
file(GLOB_RECURSE DLL_STUFF "src/*.cpp")
list(REMOVE_ITEM DLL_STUFF ${EXE_EXCLUSIVE})

# EXE
add_executable(${PROJECT_NAME} ${EXE_EXCLUSIVE})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${DLL_A_FILES})

# GAME DLL
add_library(${DLL_NAME} SHARED ${DLL_STUFF})
target_include_directories(${DLL_NAME} PRIVATE include)
target_link_libraries(${DLL_NAME} PRIVATE ${DLL_A_FILES})

# Copy DLLs
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL_FILES}
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)