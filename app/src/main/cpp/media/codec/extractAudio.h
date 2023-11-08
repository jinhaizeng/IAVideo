//
// Created by Ryan on 2023/10/31.
//

#ifndef IAVIDEO_EXTRACTAUDIO_H
#define IAVIDEO_EXTRACTAUDIO_H

// 编码
#include "libavcodec/avcodec.h"
// 封装格式处理
#include "libavformat/avformat.h"
// 像素处理
#include "libswscale/swscale.h"
#include "libavutil/channel_layout.h"
#include "libswresample/swresample.h"

#include <jni.h>
#include <libavutil/imgutils.h>
#include <android/native_window_jni.h>
#include <unistd.h>
#include "LogUtils.h"

void extractAudio(const char *input_cstr, JNIEnv *env, jobject instance, const char *output_cstr);
void extractAudioNoDecode(const char *srcPath, const char *dstPath);
#endif //IAVIDEO_EXTRACTAUDIO_H
