cmake_minimum_required(VERSION 3.18)

# Create the project
project(Transmitter)

# Include the RadioLib directory within your project
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/RadioLib/src")

# Add the main executable
add_executable(${PROJECT_NAME} 1262_file_tx.cpp)

# Add the file_tx executable
add_executable(file_tx 1262_file_tx.cpp)

# Find and link the WiringPi library
find_library(WIRINGPI_LIB wiringPi)

if(NOT WIRINGPI_LIB)
  message(FATAL_ERROR "WiringPi library not found")
endif()

# Link the WiringPi library to both executables
target_link_libraries(${PROJECT_NAME} ${WIRINGPI_LIB})
target_link_libraries(file_tx ${WIRINGPI_LIB})

# Manually include all RadioLib source files (including subdirectories) for both executables
file(GLOB_RECURSE RADIO_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/RadioLib/src/*.cpp")
target_sources(${PROJECT_NAME} PRIVATE ${RADIO_LIB_SOURCES})
target_sources(file_tx PRIVATE ${RADIO_LIB_SOURCES})

# Copy the text files to the build directory for the main executable
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/test_tx.txt $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Copy the text files to the build directory for the file_tx executable
add_custom_command(TARGET file_tx POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/test_tx.txt $<TARGET_FILE_DIR:file_tx>
)
