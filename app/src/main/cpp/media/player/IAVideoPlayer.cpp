//
// Created by Ryan on 2024/1/20.
//

#include <string>
extern "C"   //按照C语言方式编译api接口
{
//#include <libavformat/avformat.h>
//#include <libavcodec/avcodec.h>
//#include <libswscale/swscale.h>
//#include <libavutil/avutil.h>
//#include <libpostproc/postprocess.h>
//#include <libavutil/ffversion.h>
//#include <libavutil/imgutils.h>

#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
}
#include "android_log.h"

#include <android/native_window_jni.h>
#include <unistd.h>

using namespace std;

typedef struct PacketListEntry {
    struct PacketListEntry *next;
    AVPacket pkt;
} PacketListEntry;

typedef struct PacketList {
    PacketListEntry *head, *tail;
} PacketList;

typedef struct FFFormatContext {
    /**
     * The public context.
     */
    AVFormatContext pub;

    /**
     * Number of streams relevant for interleaving.
     * Muxing only.
     */
    int nb_interleaved_streams;

    /**
     * The interleavement function in use. Always set for muxers.
     */
    int (*interleave_packet)(struct AVFormatContext *s, AVPacket *pkt,
                             int flush, int has_packet);

    /**
     * This buffer is only needed when packets were already buffered but
     * not decoded, for example to get the codec parameters in MPEG
     * streams.
     */
    PacketList packet_buffer;

    /* av_seek_frame() support */
    int64_t data_offset; /**< offset of the first packet */

    /**
     * Raw packets from the demuxer, prior to parsing and decoding.
     * This buffer is used for buffering packets until the codec can
     * be identified, as parsing cannot be done without knowing the
     * codec.
     */
    PacketList raw_packet_buffer;
    /**
     * Packets split by the parser get queued here.
     */
    PacketList parse_queue;
    /**
     * The generic code uses this as a temporary packet
     * to parse packets or for muxing, especially flushing.
     * For demuxers, it may also be used for other means
     * for short periods that are guaranteed not to overlap
     * with calls to av_read_frame() (or ff_read_packet())
     * or with each other.
     * It may be used by demuxers as a replacement for
     * stack packets (unless they call one of the aforementioned
     * functions with their own AVFormatContext).
     * Every user has to ensure that this packet is blank
     * after using it.
     */
    AVPacket *parse_pkt;

    /**
     * Used to hold temporary packets for the generic demuxing code.
     * When muxing, it may be used by muxers to hold packets (even
     * permanent ones).
     */
    AVPacket *pkt;
    /**
     * Sum of the size of packets in raw_packet_buffer, in bytes.
     */
    int raw_packet_buffer_size;

    /**
     * Offset to remap timestamps to be non-negative.
     * Expressed in timebase units.
     * @see AVStream.mux_ts_offset
     */
    int64_t offset;

    /**
     * Timebase for the timestamp offset.
     */
    AVRational offset_timebase;

#if FF_API_COMPUTE_PKT_FIELDS2
    int missing_ts_warning;
#endif

    int inject_global_side_data;

    int avoid_negative_ts_use_pts;

    /**
     * Timestamp of the end of the shortest stream.
     */
    int64_t shortest_end;

    /**
     * Whether or not avformat_init_output has already been called
     */
    int initialized;

    /**
     * Whether or not avformat_init_output fully initialized streams
     */
    int streams_initialized;

    /**
     * ID3v2 tag useful for MP3 demuxing
     */
    AVDictionary *id3v2_meta;

    /*
     * Prefer the codec framerate for avg_frame_rate computation.
     */
    int prefer_codec_framerate;

    /**
     * Set if chapter ids are strictly monotonic.
     */
    int chapter_ids_monotonic;
} FFFormatContext;

class IAVideoPlayer {
private:
    char *mediaPath = NULL;
    AVFormatContext *pFormatCtx = NULL;
    const AVCodec *pCodec = NULL;
    AVCodecContext *pCodecCtx = NULL;
    ANativeWindow *nativeWindow = NULL;

    void avformat_init(AVFormatContext **ctx, char *input_cstr){
        //封装格式上下文，统领全局的结构体，保存了视频文件封装格式的相关信息
        *ctx = avformat_alloc_context();
        //打开输入视频文件
        const int openResult = avformat_open_input(ctx, input_cstr, NULL, NULL);
        if (openResult != 0) {
            LOGE("%s%s%d", "无法打开输入视频文件", input_cstr, openResult);
            return;
        }

        //获取视频文件信息
        if (avformat_find_stream_info(*ctx, NULL) < 0) {
            LOGE("%s", "无法获取视频文件信息");
            return;
        }
        LOGE("%s", "解封装器初始化--成功");
    }
    /**
     * 解封器反初始化
     */
    void avformat_uninit(AVFormatContext **ctx){
        avformat_free_context(*ctx);
    }
    /**
     * 解码器初始化
     */
    int avcodec_init(AVFormatContext *ctx, const AVCodec **codec, AVCodecContext **codecCtx) {
        //获取视频流的索引位置
        //遍历所有类型的流（音频流、视频流、字幕流），找到视频流
        int videoIndex = av_find_best_stream(ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
        if (videoIndex < 0) {
            LOGE("%s", "Couldn't find a video stream.");
            return videoIndex;
        }

        //只有知道视频的编码方式，才能够根据编码方式去找到解码器
        //获取视频流中的编解码参数
        AVCodecParameters *pCodecpar = ctx->streams[videoIndex]->codecpar;
        //根据编解码参数的编码id查找对应的解码器
        *codec = avcodec_find_decoder(pCodecpar->codec_id);  //寻找合适的解码器
        if (*codec == NULL) {
            LOGE("%s", "找不到解码器\n");
            return -1;
        }

        //根据编码器获取编解码上下文
        *codecCtx = avcodec_alloc_context3(*codec); //为AVCodecContext分配内存
        if (*codecCtx == NULL) {
            LOGE("%s", "Couldn't allocate decoder context.\n");
        }

        //avcodec_parameters_to_context()真正对AVCodecContext执行了内容拷贝
        if (avcodec_parameters_to_context(*codecCtx, pCodecpar) < 0) {
            LOGE("%s", "Couldn't copy decoder context.\n");
            return -1;
        }

        //打开解码器
        if (avcodec_open2(*codecCtx, *codec, NULL) < 0) {
            LOGE("%s", "解码器无法打开\n");
            return -1;
        }

        //输出视频信息
        LOGI("视频的文件格式：%s", ctx->iformat->name);
        LOGI("视频时长：%ld", (ctx->duration) / 1000000);
        LOGI("视频的宽高：%d,%d", (*codecCtx)->width, (*codecCtx)->height);
        LOGI("解码器的名称：%s", (*codec)->name);
        return videoIndex;
    }
    /**
     * 解码器反初始化
     */
    void avcodec_uninit(AVCodecContext **ctx) {
        avcodec_close(*ctx);
    }

public:
    IAVideoPlayer() {}
    ~IAVideoPlayer() {
        if (mediaPath != NULL) free(mediaPath);
    }

    void setMediaPath(const char* path) {
        mediaPath = strdup(path);
        LOGE("setMediaPath, path: %s", mediaPath);
    }

    void setNativeWindow(ANativeWindow *window) {
        nativeWindow = window;
        LOGE("setNativeWindow, window: %p", nativeWindow);
    }

    int prepare() {
        av_log_set_callback(ffp_log_callback_report);
        av_log_set_level(AV_LOG_DEBUG);
        //封装格式上下文，统领全局的结构体，保存了视频文件封装格式的相关信息
        avformat_init(&pFormatCtx, mediaPath);

        int videoIndex = avcodec_init(pFormatCtx, &pCodec, &pCodecCtx);
        if (videoIndex < 0) {
            LOGE("%s", "Couldn't find a video stream.");
            return -1;
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

        if(nativeWindow == NULL){
            LOGE("nativewindow获取失败");
            return -1;
        }

        // 视频缓冲区
        ANativeWindow_Buffer native_outBuffer;

        int got_picture, ret;
        int frame_count = 0;
        // 一帧一帧的读取压缩数据
        pFormatCtx->debug = FF_FDEBUG_TS;
        packet->flags = AV_PKT_FLAG_CORRUPT;
        while (av_read_frame(pFormatCtx, packet) >= 0) {
            const int genpts = pFormatCtx->flags & AVFMT_FLAG_GENPTS;
            FFFormatContext *const si = (FFFormatContext*)pFormatCtx;

            LOGE("genpts: %d, %d, %d, %d", genpts, si->packet_buffer.head, si->raw_packet_buffer.head, pFormatCtx->iformat->read_packet == NULL);

            //只要视频压缩数据（根据流的索引位置判断）
            if (packet->stream_index == videoIndex) {
                // 解码一帧视频压缩数据，得到视频像素数据
                ret = avcodec_send_packet(pCodecCtx, packet);
                if (ret < 0) {
                    LOGE("%s", "解码错误");
                    return -1;
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
            packet->flags = AV_PKT_FLAG_CORRUPT;
        }

        ANativeWindow_release(nativeWindow);
        av_frame_free(&frame);
        av_frame_free(&rgb_frame);

        avcodec_uninit(&pCodecCtx);
        avformat_uninit(&pFormatCtx);
    }
    int destoryPlayer() {

    }
};
