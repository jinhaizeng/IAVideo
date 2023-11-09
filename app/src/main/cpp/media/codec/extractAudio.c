//
// Created by Ryan on 2023/10/31.
//

#include "extractAudio.h"
#include "ffCodec.h"

void extractAudio(const char *input_cstr, JNIEnv *env, jobject instance, const char *output_cstr) {
    av_log_set_callback(ffp_log_callback_report);
    av_log_set_level(AV_LOG_DEBUG);
    //封装格式上下文，统领全局的结构体，保存了视频文件封装格式的相关信息
    AVFormatContext *pFormatCtx;
    avformat_init(&pFormatCtx, input_cstr);

    AVCodec *pCodec;
    AVCodecContext *pCodecCtx;

    //获取视频流的索引位置
    //遍历所有类型的流（音频流、视频流、字幕流），找到音频
    int audio_stream_idx = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audio_stream_idx < 0) {
        LOGE("%s", "Couldn't find a video stream.");
        return;
    }

    //只有知道视频的编码方式，才能够根据编码方式去找到解码器
    //获取视频流中的编解码参数
    AVCodecParameters *pCodecpar = pFormatCtx->streams[audio_stream_idx]->codecpar;
    //根据编解码参数的编码id查找对应的解码器
    pCodec = avcodec_find_decoder(pCodecpar->codec_id);  //寻找合适的解码器
    if (pCodec == NULL) {
        LOGE("%s", "找不到解码器\n");
        return;
    }

    //根据编码器获取编解码上下文
    pCodecCtx = avcodec_alloc_context3(pCodec); //为AVCodecContext分配内存
    if (pCodecCtx == NULL) {
        LOGE("%s", "Couldn't allocate decoder context.\n");
        return;
    }

    //avcodec_parameters_to_context()真正对AVCodecContext执行了内容拷贝
    if (avcodec_parameters_to_context(pCodecCtx, pCodecpar) < 0) {
        LOGE("%s", "Couldn't copy decoder context.\n");
        return;
    }

    //打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("%s", "解码器无法打开\n");
        return;
    }

    //输出视频信息
    LOGI("视频的文件格式：%s", pFormatCtx->iformat->name);
    LOGI("视频时长：%ld", (pFormatCtx->duration) / 1000000);
    LOGI("视频的宽高：%d,%d", (pCodecCtx)->width, (pCodecCtx)->height);
    LOGI("解码器的名称：%s", (pCodec)->name);
    LOGI("音频相关信息打印，采样率：%d，采样格式：%d，通道数：%d", pCodecCtx->sample_rate, pCodecCtx->sample_fmt, pCodecCtx->channels);
    // 准备读取
    // AVPacket用于存储一帧一帧的压缩数据（H264）
    // 缓冲区，开辟空间
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    // AVFrame用于存储解码后的像素数据()
    // 内存分配
    AVFrame *frame = av_frame_alloc(); //分配一个AVFrame结构体,AVFrame结构体一般用于存储原始数据，指向解码后的原始帧

    // 计算 音频缓冲区大小
    size_t audioSize = av_samples_get_buffer_size(NULL, pCodecCtx->channels, pCodecCtx->sample_rate, pCodecCtx->sample_fmt, 1);
    //缓存区
    uint8_t *out_buffer = (uint8_t *) av_malloc(audioSize);

    //输出的声道布局（立体声）
    uint64_t  out_ch_layout= AV_CH_LAYOUT_STEREO;
    //输出采样位数  16位
    enum AVSampleFormat out_formart=AV_SAMPLE_FMT_S16;
    //输出的采样率必须与输入相同，TODO 为什么
    int out_sample_rate = pCodecCtx->sample_rate;
    //得到SwrContext ，进行重采样，具体参考http://blog.csdn.net/jammg/article/details/52688506
    SwrContext *swrContext = swr_alloc();
    //swr_alloc_set_opts将PCM源文件的采样格式转换为自己希望的采样格式
    swr_alloc_set_opts(swrContext, out_ch_layout, out_formart, out_sample_rate,
                       pCodecCtx->channel_layout, pCodecCtx->sample_fmt, pCodecCtx->sample_rate, 0,
                       NULL);
    swr_init(swrContext);

    int encodeResult = -1;
    AVStream *audioStream;
    // 创建编码格式上下文
    AVFormatContext *av_audio_format_context;
    encodeResult = avformat_alloc_output_context2(&av_audio_format_context, NULL, NULL, output_cstr);
    if(encodeResult < 0) {
        LOGI("编码器 avformat_alloc_output_context2 ret=%d", encodeResult);
        char errorBuf[AV_ERROR_MAX_STRING_SIZE];
        return;
    }
    // 打开编码文件
    encodeResult = avio_open(&av_audio_format_context->pb, output_cstr, AVIO_FLAG_READ_WRITE);
    if(encodeResult < 0) {
        LOGI("编码器 avio_open ret=%d", encodeResult);
        return;
    }

    // 确定文件输出格式，TODO确认oformat是什么时候赋值
    AVOutputFormat *audiOutputFormat = av_audio_format_context->oformat;
    AVCodec *audioCodec = avcodec_find_encoder(audiOutputFormat->audio_codec);
    if(audioCodec == NULL) {
        LOGI("编码器 audioCodec is null, %d", audiOutputFormat->audio_codec);
        return;
    }

    // 创建编码器对应上下文
    AVCodecContext *audioCodecContext = avcodec_alloc_context3(audioCodec);
    audioCodecContext->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    audioCodecContext->codec_id = AV_CODEC_ID_AAC;
    audioCodecContext->codec_type = AVMEDIA_TYPE_AUDIO;
    audioCodecContext->sample_fmt = pCodecCtx->sample_fmt;
    audioCodecContext->sample_rate = pCodecCtx->sample_rate;
    audioCodecContext->channel_layout = pCodecCtx->channel_layout;
    audioCodecContext->channels = pCodecCtx->channels;
    audioCodecContext->bit_rate = pCodecCtx->bit_rate;

    // 创建音频流AVStream
    audioStream = avformat_new_stream(av_audio_format_context, audioCodec);
    if(audioStream == NULL) {
        LOGI("编码器 audioStream is null");
        return;
    }

    LOGI("编码器 codec_id=%d, audio codec_i=%d", pCodecCtx->codec_id, audioCodecContext->codec_id);

    // 打开编码器
    encodeResult = avcodec_open2(audioCodecContext, audioCodec, NULL);
    if(encodeResult < 0) {
        LOGI("编码器 avcodec_open2 ret=%d", encodeResult);
        char errorBuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(encodeResult, errorBuf, sizeof(errorBuf));
        LOGI("%s%s", "编码器 avcodec_open2 errorBuf:", errorBuf);
        return;
    }

    //写入文件头
    avformat_write_header(av_audio_format_context, NULL);

    LOGI("编码器 11111");
    int got_audio, ret;
    int frame_count = 0;
    // 不初始化直接会挂的
    AVPacket *audioPacket = av_packet_alloc();
    int audioCnt = 0;
    // 一帧一帧的读取压缩数据
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        LOGI("编码器 222");
        //只要音频压缩数据（根据流的索引位置判断）
        if (packet->stream_index == audio_stream_idx) {
            LOGI("编码器 333");

            // 解码一帧音频压缩数据，得到视频像素数据
            ret = avcodec_send_packet(pCodecCtx, packet);
            if (ret < 0) {
                LOGE("%s", "解码错误");
                return;
            }
            got_audio = avcodec_receive_frame(pCodecCtx, frame);

            LOGE("%s%d", "解码结果 got_picture:", got_audio);
            //为0说明解码完成，非0正在解码
            if (!got_audio) {
                // 直接将解码以后的音频帧编码成mp3格式的
                int encode = avcodec_send_frame(audioCodecContext, frame);
                if (encode < 0) {
                    char errorBuf[AV_ERROR_MAX_STRING_SIZE];
                    av_strerror(encode, errorBuf, sizeof(errorBuf));
                    LOGI("%s%d", "编码 encode:", encode);
                    LOGI("%s%s", "编码 errorBuf:", errorBuf);
                }
                LOGE("解码结果 %d:", encode);

                while (!encode) {
                    LOGI( "audioCodecContext is %d ", audioCodecContext == NULL);
                    audioPacket = av_packet_alloc();
                    encode = avcodec_receive_packet(audioCodecContext, audioPacket);
                    LOGI( "avcodec_receive_packet encode:%d, index: %d", encode, audioPacket->stream_index);
                    if (encode < 0) {
                        char errorBuf[AV_ERROR_MAX_STRING_SIZE];
                        av_strerror(encode, errorBuf, sizeof(errorBuf));
                        LOGI("avcodec_receive_packet errorBuf: %s", errorBuf);
                        break;
                    }
//                    audioPacket->pts = audioCnt;
//                    audioPacket->dts = audioCnt;
//                    audioCnt++;
                    audioPacket->stream_index = 0;
                    LOGI( "avcodec_receive_packet 0000");
                    LOGI( "av_audio_format_context is %d, audioPacket is %d, av_audio_format_context->oformat is %d", av_audio_format_context == NULL, audioPacket == NULL, av_audio_format_context->oformat == NULL);
                    LOGI( "stream is %d", av_audio_format_context->streams[audioPacket->stream_index] == NULL);
                    LOGI("av_audio_format_context->oformat->name: %s", av_audio_format_context->oformat->name);
                    av_audio_format_context->debug = 1;
                    av_interleaved_write_frame(av_audio_format_context, audioPacket);
                    LOGI( "avcodec_receive_packet 1111");

                    av_packet_unref(audioPacket);
                    LOGI( "avcodec_receive_packet 222");
                }
                frame_count++;
            }
        }
        //释放资源
        av_packet_unref(packet);
    }

    LOGI( "success, frame_count: %d", frame_count);

    av_write_trailer(av_audio_format_context);
    av_frame_free(&frame);
    swr_free(&swrContext);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}

void extractAudioNoDecode(const char *srcPath, const char *dstPath) {
    int ret;
    AVFormatContext *in_fmt_ctx = NULL;
    int audio_index;
    AVStream *in_stream = NULL;
    AVCodecParameters *in_codecpar = NULL;
    AVFormatContext *out_fmt_ctx = NULL;
    AVOutputFormat *out_fmt = NULL;
    AVStream *out_stream = NULL;
    AVPacket pkt;

    //in_fmt_ctx
    ret = avformat_open_input(&in_fmt_ctx, srcPath, NULL, NULL);
    if (ret < 0) {
//        __android_log_print(ANDROID_LOG_ERROR, TAG, "avformat_open_input失败：%s",
//                            av_err2str(ret));
        goto end;
    }

    //audio_index
    audio_index = av_find_best_stream(in_fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audio_index < 0) {
//        __android_log_print(ANDROID_LOG_ERROR, TAG, "查找音频流失败：%s",
//                            av_err2str(audio_index));
        goto end;
    }

    //in_stream、in_codecpar
    in_stream = in_fmt_ctx->streams[audio_index];
    in_codecpar = in_stream->codecpar;
    if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO) {
//        __android_log_print(ANDROID_LOG_ERROR, TAG, "The Codec type is invalid!");
        goto end;
    }

    //out_fmt_ctx
    out_fmt_ctx = avformat_alloc_context();
    out_fmt = av_guess_format(NULL, dstPath, NULL);
    out_fmt_ctx->oformat = out_fmt;
    if (!out_fmt) {
//        __android_log_print(ANDROID_LOG_ERROR, TAG, "Cloud not guess file format");
        goto end;
    }

    //out_stream
    out_stream = avformat_new_stream(out_fmt_ctx, NULL);
    if (!out_stream) {
//        __android_log_print(ANDROID_LOG_ERROR, TAG, "Failed to create out stream");
        goto end;
    }

    //拷贝编解码器参数
    ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
    if (ret < 0) {
//        __android_log_print(ANDROID_LOG_ERROR, TAG, "avcodec_parameters_copy：%s",
//                            av_err2str(ret));
        goto end;
    }
    out_stream->codecpar->codec_tag = 0;


    //创建并初始化目标文件的AVIOContext
    if ((ret = avio_open(&out_fmt_ctx->pb, dstPath, AVIO_FLAG_WRITE)) < 0) {
//        __android_log_print(ANDROID_LOG_ERROR, TAG, "avio_open：%s",
//                            av_err2str(ret));
        goto end;
    }

    //initialize packet
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    //写文件头
    if ((ret = avformat_write_header(out_fmt_ctx, NULL)) < 0) {
//        __android_log_print(ANDROID_LOG_ERROR, TAG, "avformat_write_header：%s",
//                            av_err2str(ret));
        goto end;
    }

    while (av_read_frame(in_fmt_ctx, &pkt) == 0) {
        if (pkt.stream_index == audio_index) {
            //输入流和输出流的时间基可能不同，因此要根据时间基的不同对时间戳pts进行转换
            pkt.pts = av_rescale_q(pkt.pts, in_stream->time_base, out_stream->time_base);
            pkt.dts = pkt.pts;
            //根据时间基转换duration
            pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
            pkt.pos = -1;
            pkt.stream_index = 0;

            //写入
            av_interleaved_write_frame(out_fmt_ctx, &pkt);

            //释放packet
            av_packet_unref(&pkt);
        }
    }

    //写文件尾
    av_write_trailer(out_fmt_ctx);

    //释放资源
    end:
    if (in_fmt_ctx) avformat_close_input(&in_fmt_ctx);
    if (out_fmt_ctx) {
        if (out_fmt_ctx->pb) avio_close(out_fmt_ctx->pb);
        avformat_free_context(out_fmt_ctx);
    }
}