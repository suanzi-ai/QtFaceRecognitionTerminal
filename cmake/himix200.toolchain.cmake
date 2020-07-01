# set cross-compiled system type, it's better not use the type which cmake
# cannot recognized.
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# when hislicon SDK was installed, toolchain was installed in the path as below:
set(CROSS_PREFIX
    /opt/hisi-linux/x86-arm/arm-himix200-linux/bin/arm-himix200-linux-)
set(CROSS_HOST arm-himix200-linux)
set(CMAKE_C_COMPILER ${CROSS_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${CROSS_PREFIX}g++)

# set searching rules for cross-compiler
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# set ${CMAKE_C_FLAGS} and ${CMAKE_CXX_FLAGS}flag for cross-compiled process
set(CROSS_COMPILATION_ARM himix200)
set(CROSS_COMPILATION_ARCHITECTURE armv7-a)

# set g++ param
set(CMAKE_CXX_FLAGS
    "-std=c++11 -march=armv7-a -mfloat-abi=softfp -mfpu=neon-vfpv4 -fopenmp ${CMAKE_CXX_FLAGS}"
)

# other settings
set(CMAKE_CXX_FLAGS "-DCV__EXCEPTION_PTR=0 ${CMAKE_CXX_FLAGS}")

add_definitions(-D__HISI__)
