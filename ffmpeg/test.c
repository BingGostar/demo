#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
#include <libavutil/log.h>
#include <libavutil/pixdesc.h>
#include <libavutil/time.h>

static const char* in_name = "rtsp://10.151.3.78:7777/vlc";
static const char* out_name = "rtmp://10.151.3.69/live/aaa";

int interrupt_callback(void* ptr) {
    return 1;
}


int media_trans(const char* in_name, const char* in_protocol, \
                const char* out_name, const char* out_protocol) {
    int ret = 0;
    AVPacket* pkt = NULL;
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;

    // input
    ifmt_ctx = avformat_alloc_context();
    if (!ifmt_ctx) {
        printf("avformat_alloc_context error\n");
        return -1;
    }
    const AVInputFormat* ifmt = av_find_input_format(in_protocol);
    printf("ifmt->name %s\n", ifmt->name);

    AVDictionary *format_opts = NULL;
    int scan_all_pmts_set = 0;
    
    av_dict_set_int(&format_opts, "timeout", 3000000, 0);
    av_dict_set_int(&format_opts, "rw_timeout", 1000000, 0);
    av_dict_set_int(&format_opts, "probesize", 32, 0); // not enough frames to estimate rate
    av_dict_set(&format_opts, "flvflags", "no_duration_filesize", 0);

    ret = avformat_open_input(&ifmt_ctx, in_name, ifmt, &format_opts);
    if (ret < 0) {
        printf("avformat_open_input error\n");
        return -1;
    }
    printf("find info \n");
    
    ret = avformat_find_stream_info(ifmt_ctx, NULL);
    if (ret < 0) {
        printf("avformat_find_stream_info error \n");
        return -1;
    }
    printf("format\n");
    av_dump_format(ifmt_ctx, 0, in_name, 0);

    for (size_t i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;
        AVCodecContext* codec_ctx = avcodec_alloc_context3(NULL);
        avcodec_parameters_to_context(codec_ctx, in_codecpar);
    
        if (in_codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            printf("extradata_size [%d]\n", in_codecpar->extradata_size);
            printf("codec_id %d \n", in_codecpar->codec_id);
            printf("time base {%d, %d}\n",in_stream->time_base.num, in_stream->time_base.den);
            printf("pix fmt [%d] [%s]\n", codec_ctx->pix_fmt, av_get_pix_fmt_name(codec_ctx->pix_fmt));
            printf("width [%d] height [%d]\n", codec_ctx->width, codec_ctx->height);
        }
        avcodec_free_context(&codec_ctx);
    }
    
    // output
    ret = avformat_alloc_output_context2(&ofmt_ctx, NULL, out_protocol, out_name);
    if (ret < 0) {
        printf("avformat_alloc_output_context2 error \n");
        return -1;
    }

    for(size_t i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream* in_stream = ifmt_ctx->streams[i];
        AVCodecParameters *in_par = in_stream->codecpar;
        const AVCodec* in_codec = avcodec_find_decoder(in_par->codec_id);
        AVCodecContext* codec_ctx = avcodec_alloc_context3(in_codec);
        AVStream* out_stream = avformat_new_stream(ofmt_ctx, NULL);
        if (!out_stream) {
            printf("avformat_new_stream error \n");
            goto end;
        }

        ret = avcodec_parameters_to_context(codec_ctx, in_par);
        if (ret < 0) {
            printf("avcodec_parameters_to_context error\n");
            goto end;
        }
        codec_ctx->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
            codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }
        ret = avcodec_parameters_from_context(out_stream->codecpar, codec_ctx);
        if (ret < 0) {
            printf("Failed to copy codec context to out_stream codecpar context\n");
            goto end;
        }

        // out_stream->time_base.den = out_stream->time_base.den * 2;

        // ret = avcodec_parameters_copy(out_stream->codecpar, in_par);
        // if (ret < 0) {
        //     printf("avcodec_parameters_copy error \n");
        //     goto end;
        // }
        // out_stream->codecpar->codec_tag = 0;
    }
    av_dump_format(ofmt_ctx, 0, out_name, 1);

    if (!(ofmt_ctx->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_name, AVIO_FLAG_WRITE);
        if (ret < 0) {
            printf("avio_open error\n");
            goto end;
        }
    }

    ret = avformat_write_header(ofmt_ctx, &format_opts);
    if (ret < 0) {
        printf("avformat_write_header error \n");
        goto end;
    }
    
    pkt = av_packet_alloc();
    if (!pkt) {
        printf("av_packet_alloc error\n");
        goto end;
    }

    AVRational out_time_base = {1, 500};
    

    while (1) {
        ret = av_read_frame(ifmt_ctx, pkt);
        if (ret < 0) {
            // printf("av_read_frame error [%s]\n", av_err2str(ret));

            // ret = av_seek_frame(ifmt_ctx, 0, 0, AVSEEK_FLAG_FRAME);
            // if (ret < 0) {
            //     printf("avio_seek error [%s]\n", av_err2str(ret));
            // }
            // continue;
            break;
        }
      
        
        if (pkt->pts == AV_NOPTS_VALUE) {
            printf("NOOOOOOOO!\n");
        }


        // if (pkt->flags != AV_PKT_FLAG_KEY) {
        //     continue;
        // }

        AVStream* in_stream = ifmt_ctx->streams[pkt->stream_index];
        AVStream* out_stream = ofmt_ctx->streams[pkt->stream_index];
        av_packet_rescale_ts(pkt, in_stream->time_base, out_time_base);
        pkt->pos = -1;
        if (av_interleaved_write_frame(ofmt_ctx, pkt) < 0) {
            printf("av_interleaved_write_frame error\n");
            break;
        }
        // av_packet_unref(pkt);
    }

    av_write_trailer(ofmt_ctx);

end:
    if (pkt) av_packet_unref(pkt);
    if (ifmt_ctx) avformat_close_input(&ifmt_ctx);
    if (ofmt_ctx){
        if (!(ofmt_ctx->flags & AVFMT_NOFILE)) avio_closep(&ofmt_ctx->pb);
        avformat_free_context(ofmt_ctx);
    }

    return 0;
}

int main() {
    // const char* err = av_err2str(-110);

    av_log_set_level(AV_LOG_DEBUG);

    media_trans(in_name, "rtsp", out_name, "flv");
    
    
}