
cmake_minimum_required(VERSION 3.25)
project(Heartburner LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Automatically find all .cpp files in src/
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add include directory
target_include_directories(${PROJECT_NAME} PRIVATE include)

# find and link all .dll.a files in lib/
file(GLOB_RECURSE DLL_A_FILES "${CMAKE_SOURCE_DIR}/lib/*.dll.a")
target_link_libraries(${PROJECT_NAME} PRIVATE ${DLL_A_FILES})

# Collect all .dll files inside 'lib'
file(GLOB_RECURSE DLL_FILES "${CMAKE_SOURCE_DIR}/lib/*.dll")

# after our build is done, check each dll file and add it to the build folder if needed
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${DLL_FILES}
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)