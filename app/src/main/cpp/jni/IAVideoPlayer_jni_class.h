//
// Created by Ryan on 2024/1/20.
//

#ifndef IAVIDEO_IAVIDEOPLAYER_JNI_CLASS_H
#define IAVIDEO_IAVIDEOPLAYER_JNI_CLASS_H

#include <jni.h>

void loadClass_IAVideoPlayer(JNIEnv* env);

void set_java_IAVideoPlayer_mNativeMediaPlayer(JNIEnv* env, jobject obj, jlong IAVideoPlayer);

jlong get_java_IAVideoPlayer_mNativeMediaPlayer(JNIEnv* env, jobject obj);
#endif //IAVIDEO_IAVIDEOPLAYER_JNI_CLASS_H
