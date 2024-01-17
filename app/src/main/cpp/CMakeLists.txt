# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("iavideo")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

INCLUDE_DIRECTORIES(
        ${CMAKE_SOURCE_DIR}/util
        ${CMAKE_SOURCE_DIR}/media/codec)

file(GLOB src-files
        ${CMAKE_SOURCE_DIR}/native-lib.cpp
        ${CMAKE_SOURCE_DIR}/FFMedia.c
        ${CMAKE_SOURCE_DIR}/media/codec/*.c
        )

add_library( # Sets the name of the library.
        iavideo

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${src-files}
        )

# 声明变量 third_part_jni_path、third_part_header_path
# ../jni 表示当前路径的上一级路径，之后上一级路径查找子目录 jni
#set(third_part_jni_path ../jni)
get_filename_component(third_part_jni_path ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs ABSOLUTE)
MESSAGE("third_part_jni_path = ${third_part_jni_path}")
# 头部文件所在的目录
set(third_part_header_path ${third_part_jni_path}/include)
# so 库所存在的文件夹
set(third_part_lib_path  ${third_part_jni_path}/arm64-v8a)
MESSAGE("third_part_lib_path ${third_part_lib_path}")
INCLUDE_DIRECTORIES(${third_part_header_path})


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.



# 导入 ffmpeg 相关的动态库
# 1、导入 avcodec
# add_library
# 参数1：库的名称
# 参数2：表示共享库
# 参数3：表示导入
add_library(avcodec SHARED IMPORTED)
# set_target_properties
# 参数1：库的名称
# 参数2：表示设置路径的属性，就是导入本地路径
# 参数3：本地路径
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ${third_part_lib_path}/libavcodec.so)

# 2、导入 avdevice
add_library(avdevice
        SHARED
        IMPORTED)
set_target_properties(avdevice
        PROPERTIES IMPORTED_LOCATION
        ${third_part_lib_path}/libavdevice.so)

# 3、导入 avfilter
add_library(avfilter
        SHARED
        IMPORTED)
set_target_properties(avfilter
        PROPERTIES IMPORTED_LOCATION
        ${third_part_lib_path}/libavfilter.so)
# 4、导入 avformat
add_library(avformat
        SHARED
        IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ${third_part_lib_path}/libavformat.so)
# 5、导入 avutil
add_library(avutil
        SHARED
        IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${third_part_lib_path}/libavutil.so)
# 6、导入 postproc
add_library(postproc
        SHARED
        IMPORTED)
set_target_properties(postproc
        PROPERTIES IMPORTED_LOCATION
        ${third_part_lib_path}/libpostproc.so)
# 7、导入 swresample
add_library(swresample
        SHARED
        IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ${third_part_lib_path}/libswresample.so)
# 8、导入 swscale
add_library(swscale
        SHARED
        IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ${third_part_lib_path}/libswscale.so)

target_link_libraries( # Specifies the target library.
        iavideo
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        postproc
        swresample
        swscale
        android
        # Links the target library to the log library
        # included in the NDK.
        log)