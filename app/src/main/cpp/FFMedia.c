//
// Created by Ryan on 2023/10/20.
//
#include <jni.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <ffCodec.h>
#ifdef __cplusplus
}
#endif

JNIEXPORT void JNICALL
Java_ryan_media_iavideo_IAVideoCodec__1init(JNIEnv *env, jobject thiz) {
// TODO: implement init()
}

JNIEXPORT void JNICALL
Java_ryan_media_iavideo_IAVideoCodec__1decodeToMp3(JNIEnv *env, jclass jcls, jstring input_jstr, jstring output_jstr) {
    // TODO: implement _decodeToMp3()

}
JNIEXPORT void JNICALL
Java_ryan_media_iavideo_IAVideoCodec__1play(JNIEnv *env, jobject thiz, jstring input,
                                            jobject surface) {
    LOGE("%s%p", "1111 env ", env);
    //需要转码的视频文件(输入的视频文件)
    const char *input_cstr = (*env)->GetStringUTFChars(env, input, NULL);
    LOGE("%s%s", "get input source: ", input_cstr);
    // 取到nativewindow用于渲染
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    LOGE("%s%p", "get nativeWindow: ", nativeWindow);

    avcodec_basePlay(input_cstr, nativeWindow);
    (*env)->ReleaseStringUTFChars(env, input, input_cstr);
}