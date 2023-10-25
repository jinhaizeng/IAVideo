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
int avcodec_init(AVFormatContext *pFormatCtx, const AVCodec **pCodec, AVCodecContext **pCodecCtx) {
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
    LOGI("视频时长：%ld", (pFormatCtx->duration) / 1000000);
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


void avcodec_basePlay(const char *input_cstr, ANativeWindow *nativeWindow) {
    //封装格式上下文，统领全局的结构体，保存了视频文件封装格式的相关信息
    LOGE("%s", "1111");

    AVFormatContext *pFormatCtx;
    avformat_init(&pFormatCtx, input_cstr);
    LOGE("%s", "2222");

    AVCodec *pCodec;
    AVCodecContext *pCodecCtx;
    int videoIndex = avcodec_init(pFormatCtx, &pCodec, &pCodecCtx);
    if (videoIndex < 0) {
        LOGE("%s", "Couldn't find a video stream.");
        return;
    }

    // 准备读取
    // AVPacket用于存储一帧一帧的压缩数据（H264）
    // 缓冲区，开辟空间
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    // AVFrame用于存储解码后的像素数据(YUV)
    // 内存分配
    AVFrame *frame = av_frame_alloc(); //分配一个AVFrame结构体,AVFrame结构体一般用于存储原始数据，指向解码后的原始帧
    AVFrame *rgb_frame = av_frame_alloc(); //分配一个AVFrame结构体，指向存放转换成rgb后的帧

    // 缓存区
    uint8_t  *out_buffer= (uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGBA,
                                                                        pCodecCtx->width,pCodecCtx->height, 1));
    // 与缓存区相关联，设置rgb_frame缓存区：av_image_fill_arrays() 把AVFrame的data成员关联到某个地址空间，此处是为后面av_read_frame()和sws_scale()处理后的帧信息输出提供存储位置
    av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, out_buffer,AV_PIX_FMT_RGBA,pCodecCtx->width,pCodecCtx->height, 1);

    // 用于转码（缩放）的参数，转之前的宽高，转之后的宽高，格式等
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                                pCodecCtx->pix_fmt,
                                                pCodecCtx->width, pCodecCtx->height,
                                                AV_PIX_FMT_RGBA,
                                                SWS_BICUBIC, NULL, NULL, NULL);

    if(nativeWindow==0){
        LOGE("nativewindow获取失败")
        return;
    }

    // 视频缓冲区
    ANativeWindow_Buffer native_outBuffer;

    int got_picture, ret;
    int frame_count = 0;
    // 一帧一帧的读取压缩数据
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        //只要视频压缩数据（根据流的索引位置判断）
        if (packet->stream_index == videoIndex) {
            // 解码一帧视频压缩数据，得到视频像素数据
            ret = avcodec_send_packet(pCodecCtx, packet);
            if (ret < 0) {
                LOGE("%s", "解码错误");
                return;
            }
            got_picture = avcodec_receive_frame(pCodecCtx, frame);

            LOGE("%s%d", "got_picture:", got_picture);

            //为0说明解码完成，非0正在解码
            if (!got_picture) {
                // 绘制之前配置nativewindow 设置窗口buffer的大小和格式
                ANativeWindow_setBuffersGeometry(nativeWindow,pCodecCtx->width,pCodecCtx->height,WINDOW_FORMAT_RGBA_8888);
                //上锁
                ANativeWindow_lock(nativeWindow, &native_outBuffer, NULL);

                //AVFrame转为像素格式RGBA8888，宽高
                //2 6输入、输出数据
                //3 7输入、输出画面一行的数据的大小 AVFrame 转换是一行一行转换的
                //4 输入数据第一列要转码的位置 从0开始
                //5 输入画面的高度
                sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height,
                          rgb_frame->data, rgb_frame->linesize);

                // rgb_frame是有画面数据
                uint8_t *dst= (uint8_t *) native_outBuffer.bits;
                // 拿到一行有多少个字节 RGBA
                int destStride= native_outBuffer.stride*4;
                // RGBA像素数据的首地址
                uint8_t* src= rgb_frame->data[0];
                // RGBA实际内存一行数量
                int srcStride = rgb_frame->linesize[0];
                for (int i = 0; i < pCodecCtx->height; ++i) {
                    // 将rgb_frame中每一行的数据复制给nativewindow
                    // memcpy函数一定要报这个dst和src的一行数据大小是一致，不然会直接crash
                    memcpy(dst + i * destStride,  src + i * srcStride, srcStride);
                }
                // 解锁
                ANativeWindow_unlockAndPost(nativeWindow);
                usleep(1000 * 16);

                frame_count++;
                LOGI("解码第%d帧, 文件大小%d, frame->linesize: %d, rgb_frame->linesize[0]: %d", frame_count, sizeof(dst), frame->linesize[0], rgb_frame->linesize[0]);
            }
        }

        //释放资源
        av_packet_unref(packet);
    }

    ANativeWindow_release(nativeWindow);
    av_frame_free(&frame);
    av_frame_free(&rgb_frame);

    avcodec_uninit(&pCodecCtx);
    avformat_uninit(&pFormatCtx);
}
