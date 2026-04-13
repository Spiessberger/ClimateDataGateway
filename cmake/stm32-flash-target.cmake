include_guard(GLOBAL)

include(CMakeParseArguments)

function(add_stm32_flash_target firmware_target)
    if(NOT TARGET ${firmware_target})
        message(FATAL_ERROR "add_stm32_flash_target: target '${firmware_target}' does not exist")
    endif()

    set(options)
    set(oneValueArgs TARGET_NAME FLASH_ADDRESS)
    cmake_parse_arguments(STM32_FLASH "${options}" "${oneValueArgs}" "" ${ARGN})

    if(NOT STM32_FLASH_TARGET_NAME)
        set(STM32_FLASH_TARGET_NAME flash)
    endif()

    if(NOT STM32_FLASH_FLASH_ADDRESS)
        set(STM32_FLASH_FLASH_ADDRESS 0x08000000)
    endif()

    set(arm_objcopy "${CMAKE_OBJCOPY}")
    if(NOT arm_objcopy)
        find_program(arm_objcopy NAMES arm-none-eabi-objcopy QUIET)
    endif()

    find_program(st_flash NAMES st-flash QUIET)

    if(arm_objcopy AND st_flash)
        add_custom_target(${STM32_FLASH_TARGET_NAME}
            COMMAND ${arm_objcopy} -O binary
                    $<TARGET_FILE:${firmware_target}>
                    $<TARGET_FILE_BASE_NAME:${firmware_target}>.bin
            COMMAND ${st_flash} --reset write
                    $<TARGET_FILE_BASE_NAME:${firmware_target}>.bin
                    ${STM32_FLASH_FLASH_ADDRESS}
            DEPENDS ${firmware_target}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating BIN and flashing STM32"
            VERBATIM
            USES_TERMINAL
        )
    else()
        add_custom_target(${STM32_FLASH_TARGET_NAME}
            COMMAND ${CMAKE_COMMAND} -E echo "flash target is unavailable: install arm-none-eabi-objcopy and st-flash"
            COMMAND ${CMAKE_COMMAND} -E false
            COMMENT "Flash tools are missing"
            VERBATIM
            USES_TERMINAL
        )
    endif()
endfunction()