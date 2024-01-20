//
// Created by Ryan on 2024/1/20.
//

#include <jni.h>
#include "IAVideoPlayer_jni_class.h"
#include "IAVideoPlayer.cpp"

extern "C"
JNIEXPORT void JNICALL
Java_ryan_media_iavideo_IAVideoPlayer__1native_1init(JNIEnv *env, jobject thiz) {
    loadClass_IAVideoPlayer(env);
    IAVideoPlayer iaVideoPlayer = IAVideoPlayer();
    set_java_IAVideoPlayer_mNativeMediaPlayer(env, thiz, reinterpret_cast<jlong>(&iaVideoPlayer));
}

extern "C"
JNIEXPORT void JNICALL
Java_ryan_media_iavideo_IAVideoPlayer__1setMediaPath(JNIEnv *env, jobject thiz, jstring path) {
    IAVideoPlayer *iaVideoPlayer = reinterpret_cast<IAVideoPlayer *>(get_java_IAVideoPlayer_mNativeMediaPlayer(
            env, thiz));
    if (iaVideoPlayer == NULL) {
        LOGE("IAVideoPlayer is null, check is init");
        return;
    }
    const char *input_cstr = env->GetStringUTFChars( path, NULL);
    iaVideoPlayer->setMediaPath(input_cstr);
    // 注意这里要及时释放字符串的
    env->ReleaseStringUTFChars(path, input_cstr);
}

extern "C"
JNIEXPORT void JNICALL
Java_ryan_media_iavideo_IAVideoPlayer__1setSurface(JNIEnv *env, jobject thiz, jobject surface) {
    IAVideoPlayer *iaVideoPlayer = reinterpret_cast<IAVideoPlayer *>(get_java_IAVideoPlayer_mNativeMediaPlayer(
            env, thiz));
    if (iaVideoPlayer == NULL) {
        LOGE("IAVideoPlayer is null, check is init");
        return;
    }
    // 取到nativewindow用于渲染
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (nativeWindow == NULL) {
        LOGE("get nativeWindow fail");
        return;
    }
    iaVideoPlayer->setNativeWindow(nativeWindow);
}

extern "C"
JNIEXPORT void JNICALL
Java_ryan_media_iavideo_IAVideoPlayer__1prepare(JNIEnv *env, jobject thiz) {
    IAVideoPlayer *iaVideoPlayer = reinterpret_cast<IAVideoPlayer *>(get_java_IAVideoPlayer_mNativeMediaPlayer(
            env, thiz));
    if (iaVideoPlayer == NULL) {
        LOGE("IAVideoPlayer is null, check is init");
        return;
    }
    iaVideoPlayer->prepare();
}