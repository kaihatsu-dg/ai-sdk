cmake_minimum_required(VERSION 3.10)

project(struct2depth)

#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_SYSTEM_PROCESSOR arm)

message (STATUS "CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}")
message (STATUS "CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")

set(SYS_ARCH linux_aarch64)
#add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

#if(SYS_ARCH STREQUAL "linux_armhf")
#    set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/../3rdparty/opencv/arm-linux-gnueabihf-aw/lib/cmake/opencv4)
#elseif(SYS_ARCH STREQUAL "linux_aarch64")
#    set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/../3rdparty/opencv/aarch64-linux-sunxi-glibc_1020/lib/cmake/opencv4)
#elseif(SYS_ARCH STREQUAL "android_aarch64")
#    set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/../3rdparty/opencv/opencv-4.6.0-android/sdk/native/jni/abi-${ANDROID_ABI})
#else()
#    message(FATAL_ERROR "SYS_ARCH is not linux_armhf, linux_aarch64 or android_aarch64.")
#endif()
    
find_package(OpenCV REQUIRED)


set(VIP_LIBS NBGlinker VIPhal)


# Add the source in project root directory
aux_source_directory(. DIRSRCS)
#aux_source_directory(../libawnn_viplite NPULIB_DIRSRCS)

# Add header file include directories
include_directories(./ ../common ../common/include ../libawnn_viplite ../libawutils)
#include_directories(./ ../common/include)

# Add block directories

# Target
add_executable(struct2depth ${DIRSRCS} ${NPULIB_DIRSRCS})
target_link_libraries(struct2depth ${OpenCV_LIBS} ${VIP_LIBS})
