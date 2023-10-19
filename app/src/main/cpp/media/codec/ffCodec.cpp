//
// Created by Ryan on 2023/10/20.
//

#include "ffCodec.h"

/**
 * 解封装器初始化
 */
void avformat_init(AVFormatContext **pFormatCtx, char *input_cstr){
    //封装格式上下文，统领全局的结构体，保存了视频文件封装格式的相关信息
    *pFormatCtx = avformat_alloc_context();
    //打开输入视频文件
    const int openResult = avformat_open_input(pFormatCtx, input_cstr, NULL, NULL);
    if (openResult != 0) {
        LOGE("%s%s%d", "无法打开输入视频文件", input_cstr, openResult);
        return;
    }

    //获取视频文件信息
    if (avformat_find_stream_info(*pFormatCtx, NULL) < 0) {
        LOGE("%s", "无法获取视频文件信息");
        return;
    }
    LOGE("%s", "解封装器初始化--成功");
}

/**
 * 解封器反初始化
 */
void avformat_uninit(AVFormatContext **pFormatCtx){
    avformat_free_context(*pFormatCtx);
}

/**
 * 解码器初始化
 */
int avcodec_init(AVFormatContext *pFormatCtx, AVCodec **pCodec, AVCodecContext **pCodecCtx) {
    //获取视频流的索引位置
    //遍历所有类型的流（音频流、视频流、字幕流），找到视频流
    int videoIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (videoIndex < 0) {
        LOGE("%s", "Couldn't find a video stream.");
        return videoIndex;
    }

    //只有知道视频的编码方式，才能够根据编码方式去找到解码器
    //获取视频流中的编解码参数
    AVCodecParameters *pCodecpar = pFormatCtx->streams[videoIndex]->codecpar;
    //根据编解码参数的编码id查找对应的解码器
    *pCodec = avcodec_find_decoder(pCodecpar->codec_id);  //寻找合适的解码器
    if (*pCodec == NULL) {
        LOGE("%s", "找不到解码器\n");
        return -1;
    }

    //根据编码器获取编解码上下文
    *pCodecCtx = avcodec_alloc_context3(*pCodec); //为AVCodecContext分配内存
    if (*pCodecCtx == NULL) {
        LOGE("%s", "Couldn't allocate decoder context.\n");
    }

    //avcodec_parameters_to_context()真正对AVCodecContext执行了内容拷贝
    if (avcodec_parameters_to_context(*pCodecCtx, pCodecpar) < 0) {
        LOGE("%s", "Couldn't copy decoder context.\n");
        return -1;
    }

    //打开解码器
    if (avcodec_open2(*pCodecCtx, *pCodec, NULL) < 0) {
        LOGE("%s", "解码器无法打开\n");
        return -1;
    }

    //输出视频信息
    LOGI("视频的文件格式：%s", pFormatCtx->iformat->name);
    LOGI("视频时长：%lld", (pFormatCtx->duration) / 1000000);
    LOGI("视频的宽高：%d,%d", (*pCodecCtx)->width, (*pCodecCtx)->height);
    LOGI("解码器的名称：%s", (*pCodec)->name);
    return videoIndex;
}

/**
 * 解码器反初始化
 */
void avcodec_uninit(AVCodecContext **pCodecCtx) {
    avcodec_close(*pCodecCtx);
}
