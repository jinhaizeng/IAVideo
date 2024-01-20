//
// Created by Ryan on 2023/10/20.
//

#ifndef IAVIDEO_LOGUTILS_H
#define IAVIDEO_LOGUTILS_H
#include <jni.h>
#include <android/log.h>

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"IAVideo_cpp_log",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"IAVideo_cpp_log",FORMAT,##__VA_ARGS__);
#endif //IAVIDEO_LOGUTILS_H
