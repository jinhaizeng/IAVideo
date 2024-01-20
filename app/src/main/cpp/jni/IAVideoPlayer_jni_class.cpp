//
// Created by Ryan on 2024/1/20.
//

#include "IAVideoPlayer_jni_class.h"
#include "LogUtils.h"

class IAVideoPlayer_jni_class {
public:
    jclass classId;
    jfieldID field_mNativeMediaPlayer;
};

static IAVideoPlayer_jni_class sIAVideoPlayerClass = IAVideoPlayer_jni_class();

void loadClass_IAVideoPlayer(JNIEnv* env) {
    const char* name;
    const char* sign;

    name = "ryan/media/iavideo/IAVideoPlayer";
    sIAVideoPlayerClass.classId = env->FindClass(name);
    if (sIAVideoPlayerClass.classId == NULL) {
        LOGE("loadClass fail, get classId fail")
        return;
    }

    name = "mNativeMediaPlayer";
    sign = "J";
    sIAVideoPlayerClass.field_mNativeMediaPlayer = env->GetFieldID(sIAVideoPlayerClass.classId, name, sign);
    if (sIAVideoPlayerClass.field_mNativeMediaPlayer == NULL) {
        LOGE("loadClass fail, get mNativeMediaPlayer fail")
        return;
    }
}

void set_java_IAVideoPlayer_mNativeMediaPlayer(JNIEnv* env, jobject obj, jlong IAVideoPlayer) {
    env->SetLongField(obj, sIAVideoPlayerClass.field_mNativeMediaPlayer, IAVideoPlayer);
}

jlong get_java_IAVideoPlayer_mNativeMediaPlayer(JNIEnv* env, jobject obj) {
    return env->GetLongField(obj, sIAVideoPlayerClass.field_mNativeMediaPlayer);
}
