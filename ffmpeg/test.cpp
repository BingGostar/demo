#include <iostream>
#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
#include <libavutil/log.h>
}

using namespace std;

static const std::string protocol = "flv";
static const std::string in_name = "rtmp://10.151.3.69/live/aaa";
static const std::string out_name = "rtmp://10.151.3.69/live/aaa";

int interrupt_callback(void* ptr) {
    return 1;
}


int open_in_file(const std::string & in_name, const std::string & protocol) {
    AVDictionary *inOpt = NULL;
    int ret = 0;

    AVFormatContext *ifmt_ctx = avformat_alloc_context();
    if (!ifmt_ctx) {
        cout << "avformat_alloc_context error" << endl;
        return -1;
    }
    cout << "find in format "<< endl;
    const AVInputFormat* ifmt = av_find_input_format(protocol.c_str());
    cout << "ifmt->name " << ifmt->name << endl;

    AVDictionary *format_opts;
    int scan_all_pmts_set = 0;
    // av_dict_set(&format_opts, "scan_all_pmts", "1", 16);

    ret = avformat_open_input(&ifmt_ctx, in_name.c_str(), ifmt, NULL);
    if (ret < 0) {
        cout << "avformat_open_input error " << endl;
        return -1;
    }

    cout << "find info " << endl;
    
    ret = avformat_find_stream_info(ifmt_ctx, NULL);
    if (ret < 0) {
        cout << "avformat_find_stream_info error " << endl;
        return -1;
    }
    cout << "format " << endl;
    av_dump_format(ifmt_ctx, 0, in_name.c_str(), 0);

    for(size_t i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream* stream = ifmt_ctx->streams[i];
        const AVCodec* codec = avcodec_find_decoder(stream->codecpar->codec_id);
        AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    }

    return 0;
}


int main() {
    av_log_set_level(AV_LOG_DEBUG);

    open_in_file(in_name, protocol);
    
    
}