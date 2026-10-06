cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME ROC_3)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    # Application framework
    ${CMAKE_CURRENT_SOURCE_DIR}/App/cubemx_entry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/app_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/mc_scheduler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/Motor/motor_control.c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/Motor/motor_state.c
    ${CMAKE_CURRENT_SOURCE_DIR}/App/Motor/motor_fault.c

    # Hardware adaptation and device drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/Platform/platform_pwm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Platform/platform_current.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Platform/platform_motor_hardware.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Platform/platform_timebase.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Platform/platform_encoder_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Platform/platform_uart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Platform/platform_fault.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Platform/platform_exti.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Platform/platform_led.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Device/AS5600/as5600.c

    # Motor-control algorithms
    ${CMAKE_CURRENT_SOURCE_DIR}/Control/Math/mc_transform.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Control/Math/mc_angle.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Control/Controller/pi_controller.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Control/Controller/current_controller.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Control/Controller/speed_controller.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Control/Modulation/svpwm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Control/Observer/speed_estimator.c

    # Communication protocol and telemetry
    ${CMAKE_CURRENT_SOURCE_DIR}/Protocol/protocol.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Protocol/command_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Protocol/telemetry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Protocol/vofa_backend.c


)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_CURRENT_SOURCE_DIR}/Config
    ${CMAKE_CURRENT_SOURCE_DIR}/Platform
    ${CMAKE_CURRENT_SOURCE_DIR}/Device/AS5600
    ${CMAKE_CURRENT_SOURCE_DIR}/Control/Math
    ${CMAKE_CURRENT_SOURCE_DIR}/Control/Controller
    ${CMAKE_CURRENT_SOURCE_DIR}/Control/Modulation
    ${CMAKE_CURRENT_SOURCE_DIR}/Control/Observer
    ${CMAKE_CURRENT_SOURCE_DIR}/App
    ${CMAKE_CURRENT_SOURCE_DIR}/App/Motor
    ${CMAKE_CURRENT_SOURCE_DIR}/Protocol

)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
