//
// Created by Ryan on 2023/10/20.
//

#ifndef IAVIDEO_FFCODEC_H
#define IAVIDEO_FFCODEC_H

// 编码
#include "libavcodec/avcodec.h"
// 封装格式处理
#include "libavformat/avformat.h"
// 像素处理
#include "libswscale/swscale.h"
#include <jni.h>
#include <libavutil/imgutils.h>
#include <android/native_window_jni.h>
#include <unistd.h>
#include "LogUtils.h"
#include "android_log.h"

void avformat_init(AVFormatContext **pFormatCtx, char *input_cstr);

void avcodec_basePlay(const char *input_cstr, ANativeWindow *nativeWindow);

void avcodec_basePlayAudio(const char *input_cstr, JNIEnv *env, jobject instance);
#endif //IAVIDEO_FFCODEC_H
