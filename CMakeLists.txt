cmake_minimum_required(VERSION 3.26)

# Prevent CMake from testing the toolchain
set(CMAKE_C_COMPILER_FORCED   1)
set(CMAKE_CXX_COMPILER_FORCED 1)

project(VLineVBaseFirmware)

set(CMAKE_CXX_STANDARD 17)

set(TOOLCHAIN_PREFIX arm-none-eabi-)
find_program(BINUTILS_PATH ${TOOLCHAIN_PREFIX}gcc NO_CACHE)

if (NOT BINUTILS_PATH)
    message(FATAL_ERROR "ARM GCC toolchain not found")
endif ()

get_filename_component(ARM_TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)
# Without that flag CMake is not able to pass test compilation check
if (${CMAKE_VERSION} VERSION_EQUAL "3.6.0" OR ${CMAKE_VERSION} VERSION_GREATER "3.6")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
else ()
    set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nano.specs")
endif ()


set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_AR ${TOOLCHAIN_PREFIX}gcc-ar)
set(CMAKE_RANLIB ${TOOLCHAIN_PREFIX}gcc-ranlib)

execute_process(COMMAND ${CMAKE_C_COMPILER} -print-sysroot
        OUTPUT_VARIABLE ARM_GCC_SYSROOT OUTPUT_STRIP_TRAILING_WHITESPACE)

add_compile_definitions(LPC175x_6x _GNU_SOURCE)

set(CFLAGS "-mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections \
        -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra " )

set(CXXFLAGS "-mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections \
        -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra \
        -std=gnu++17 -fabi-version=0 -fno-exceptions \
        -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics")

# Default C compiler flags
set(CMAKE_C_FLAGS_DEBUG_INIT "${CFLAGS} -g3 -Og -Wall -DDEBUG ")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG_INIT}" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELEASE_INIT "${CFLAGS} -O3 -Wall")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE_INIT}" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_MINSIZEREL_INIT "${CFLAGS} -Os -Wall")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL_INIT}" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "${CFLAGS} -O2 -g -Wall")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO_INIT}" CACHE STRING "" FORCE)
# Default C++ compiler flags
set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CXXFLAGS} -g3 -Og -Wall -DDEBUG ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG_INIT}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE_INIT "${CXXFLAGS} -O3 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "${CXXFLAGS} -Os -Wall")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL_INIT}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "${CXXFLAGS} -O2 -g -Wall")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT}" CACHE STRING "" FORCE)

set(CMAKE_OBJCOPY ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_SIZE_UTIL ${ARM_TOOLCHAIN_DIR}/${TOOLCHAIN_PREFIX}size CACHE INTERNAL "size tool")

set(CMAKE_SYSROOT ${ARM_GCC_SYSROOT})
set(CMAKE_FIND_ROOT_PATH ${BINUTILS_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(
        CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -nostartfiles -Wl,-Map=VLineVBaseFirmware.map"
)

include_directories(/usr/lib/arm-none-eabi/lib)
include_directories(/usr/lib/gcc/arm-none-eabi/12.2.1/include)
include_directories(/usr/lib/gcc/arm-none-eabi/12.2.1/include-fixed)

include_directories(AVCLan/include)
include_directories(include)
include_directories(system/driver)
include_directories(system/driver/CMSIS-Driver)
include_directories(system/driver/Config)
include_directories(system/include)
include_directories(system/include/arm)
include_directories(system/include/cmsis)
include_directories(system/include/cortexm)
include_directories(system/include/diag)
include_directories(system/include/lpc1765)


add_executable(VLineVBaseFirmware.elf
        AVCLan/include/AVCLanMessage.h
        AVCLan/include/CDChanger.h
        AVCLan/include/Device.h
        AVCLan/include/Driver.h
        AVCLan/include/DriverBase.h
        AVCLan/include/DriverRx.h
        AVCLan/include/DriverTx.h
        AVCLan/include/IEBusMessage.h
        AVCLan/include/InputListener.h
        AVCLan/include/Protocol.h
        AVCLan/AVCLanMessage.cpp
        AVCLan/CDChanger.cpp
        AVCLan/Device.cpp
        AVCLan/Driver.cpp
        AVCLan/DriverBase.cpp
        AVCLan/DriverRx.cpp
        AVCLan/DriverTx.cpp
        AVCLan/IEBusMessage.cpp
        AVCLan/InputListener.cpp
        AVCLan/Protocol.cpp
        include/power.h
        include/timer.h
        include/uart.h
        include/util.h
        include/VCoreCommunication.h
        ldscripts/libs.ld
        ldscripts/mem.ld
        ldscripts/sections.ld
        src/main.cpp
        src/power.cpp
        src/timer.cpp
        src/uart.cpp
        src/util.cpp
        src/VCoreCommunication.cpp
        src/write.c
        system/driver/CMSIS-Driver/Driver_CAN.h
        system/driver/CMSIS-Driver/Driver_Common.h
        system/driver/CMSIS-Driver/Driver_ETH.h
        system/driver/CMSIS-Driver/Driver_ETH_MAC.h
        system/driver/CMSIS-Driver/Driver_ETH_PHY.h
        system/driver/CMSIS-Driver/Driver_Flash.h
        system/driver/CMSIS-Driver/Driver_I2C.h
        system/driver/CMSIS-Driver/Driver_MCI.h
        system/driver/CMSIS-Driver/Driver_NAND.h
        system/driver/CMSIS-Driver/Driver_SAI.h
        system/driver/CMSIS-Driver/Driver_SPI.h
        system/driver/CMSIS-Driver/Driver_Storage.h
        system/driver/CMSIS-Driver/Driver_USART.h
        system/driver/CMSIS-Driver/Driver_USB.h
        system/driver/CMSIS-Driver/Driver_USBD.h
        system/driver/CMSIS-Driver/Driver_USBH.h
        system/driver/CMSIS-Driver/Driver_WiFi.h
        system/driver/Config/RTE_Device.h
        system/driver/GPDMA_LPC17xx.c
        system/driver/GPDMA_LPC17xx.h
        system/driver/GPIO_LPC17xx.c
        system/driver/GPIO_LPC17xx.h
        system/driver/I2S_LPC17xx.c
        system/driver/I2S_LPC17xx.h
        system/driver/PIN_LPC17xx.c
        system/driver/PIN_LPC17xx.h
        system/driver/RTE_Components.h
        system/driver/UART_LPC17xx.c
        system/driver/UART_LPC17xx.h
        system/include/arm/semihosting.h
        system/include/cmsis/arm_common_tables.h
        system/include/cmsis/arm_const_structs.h
        system/include/cmsis/arm_math.h
        system/include/cmsis/cmsis_armcc.h
        system/include/cmsis/cmsis_armcc_V6.h
        system/include/cmsis/cmsis_armclang.h
        system/include/cmsis/cmsis_compiler.h
        system/include/cmsis/cmsis_device.h
        system/include/cmsis/cmsis_gcc.h
        system/include/cmsis/cmsis_iccarm.h
        system/include/cmsis/cmsis_version.h
        system/include/cmsis/core_armv8mbl.h
        system/include/cmsis/core_armv8mml.h
        system/include/cmsis/core_cm0.h
        system/include/cmsis/core_cm0plus.h
        system/include/cmsis/core_cm1.h
        system/include/cmsis/core_cm23.h
        system/include/cmsis/core_cm3.h
        system/include/cmsis/core_cm33.h
        system/include/cmsis/core_cm4.h
        system/include/cmsis/core_cm7.h
        system/include/cmsis/core_cmFunc.h
        system/include/cmsis/core_cmInstr.h
        system/include/cmsis/core_cmSimd.h
        system/include/cmsis/core_sc000.h
        system/include/cmsis/core_sc300.h
        system/include/cmsis/LPC17xx.h
        system/include/cmsis/mpu_armv7.h
        system/include/cmsis/mpu_armv8.h
        system/include/cmsis/system_LPC17xx.h
        system/include/cmsis/tz_context.h
        system/include/cortexm/exception-handlers.h
        system/include/diag/trace.h
        system/include/lpc1765/lpc17xx_clkpwr.h
        system/include/lpc1765/lpc17xx_libcfg_default.h
        system/include/lpc1765/lpc17xx_pinsel.h
        system/include/lpc1765/lpc17xx_timer.h
        system/include/lpc1765/lpc_types.h
        system/src/cmsis/startup_LPC17xx.c
        system/src/cmsis/system_LPC17xx.c
        system/src/cortexm/exception-handlers.c
        system/src/cortexm/initialize-hardware.c
        system/src/cortexm/reset-hardware.c
        system/src/diag/trace-impl.c
        system/src/diag/trace.c
        system/src/lpc1765/lpc17xx_clkpwr.c
        system/src/lpc1765/lpc17xx_libcfg_default.c
        system/src/lpc1765/lpc17xx_pinsel.c
        system/src/lpc1765/lpc17xx_timer.c
        system/src/newlib/assert.c
        system/src/newlib/cxx.cpp
        system/src/newlib/exit.c
        system/src/newlib/sbrk.c
        system/src/newlib/startup.c
        system/src/newlib/syscalls.c)


target_link_libraries(VLineVBaseFirmware.elf
        -L${CMAKE_SOURCE_DIR}/ldscripts
        -Tmem.ld
        -Tlibs.ld
        -Tsections.ld
        --specs=nano.specs)