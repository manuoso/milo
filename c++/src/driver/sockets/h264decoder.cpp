extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/mem.h>
#include <libswscale/swscale.h>
}

#ifndef PIX_FMT_RGB24
#define PIX_FMT_RGB24 AV_PIX_FMT_RGB24
#endif

#ifndef CODEC_CAP_TRUNCATED
#define CODEC_CAP_TRUNCATED AV_CODEC_CAP_TRUNCATED
#endif

#ifndef CODEC_FLAG_TRUNCATED
#define CODEC_FLAG_TRUNCATED AV_CODEC_FLAG_TRUNCATED
#endif

#include "milo/driver/sockets/h264decoder.hpp"
#include <utility>

typedef unsigned char ubyte;

/* For backward compatibility with release 9 or so of libav */
#if (LIBAVCODEC_VERSION_MAJOR <= 54) 
#  define av_frame_alloc avcodec_alloc_frame
#  define av_frame_free  avcodec_free_frame  
#endif


H264Decoder::H264Decoder()
{
    avcodec_register_all();

    codec_ = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec_)
        throw H264InitFailure("[H264_DECODER] cannot find decoder");
    
    context_ = avcodec_alloc_context3(codec_);
    if (!context_)
        throw H264InitFailure("[H264_DECODER] cannot allocate context");

    if(codec_->capabilities & CODEC_CAP_TRUNCATED) {
        context_->flags |= CODEC_FLAG_TRUNCATED;
    }  

    int err = avcodec_open2(context_, codec_, nullptr);
    if (err < 0)
        throw H264InitFailure("[H264_DECODER] cannot open context");

    parser_ = av_parser_init(AV_CODEC_ID_H264);
    if (!parser_)
        throw H264InitFailure("[H264_DECODER] cannot init parser");
    
    frame_ = av_frame_alloc();
    if (!frame_)
        throw H264InitFailure("[H264_DECODER] cannot allocate frame");

    #if 1
    pkt_ = new AVPacket;
    if (!pkt_)
        throw H264InitFailure("[H264_DECODER] cannot allocate packet");
    av_init_packet(pkt_);
    #endif

    disable_logging();
}

H264Decoder::~H264Decoder()
{
    av_parser_close(parser_);
    avcodec_close(context_);
    av_free(context_);
    av_frame_free(&frame_);

    #if 1
    delete pkt_;
    #endif
}

ssize_t H264Decoder::parse(const ubyte* in_data, ssize_t in_size)
{
    auto nread = av_parser_parse2(parser_, context_, &pkt_->data, &pkt_->size, 
                                    in_data, in_size, 
                                    0, 0, AV_NOPTS_VALUE);
    return nread;
}

bool H264Decoder::is_frame_available() const
{
    return pkt_->size > 0;
}

const AVFrame& H264Decoder::decode_frame()
{
    int got_picture = 0;
    int nread = avcodec_decode_video2(context_, frame_, &got_picture, pkt_);

    if (nread < 0 || got_picture == 0)
        throw H264DecodeFailure("[H264_DECODER] error decoding frame");

    return *frame_;
}

ConverterRGB24::ConverterRGB24()
{
    framergb_ = av_frame_alloc();

    if (!framergb_)
        throw H264DecodeFailure("[H264_DECODER] cannot allocate frame");

    context_ = nullptr;
}

ConverterRGB24::~ConverterRGB24()
{
    sws_freeContext(context_);
    av_frame_free(&framergb_);
}

const AVFrame& ConverterRGB24::convert(const AVFrame &frame, ubyte* out_rgb)
{
    int w = frame.width;
    int h = frame.height;
    int pix_fmt = frame.format;
    
    context_ = sws_getCachedContext(context_, 
                                    w, h, (AVPixelFormat)pix_fmt, 
                                    w, h, PIX_FMT_RGB24, SWS_BILINEAR, 
                                    nullptr, nullptr, nullptr);
    if (!context_)
        throw H264DecodeFailure("[H264_DECODER] cannot allocate context");
    
    // Setup framergb with out_rgb as external buffer. Also say that we want RGB24 output.
    avpicture_fill((AVPicture*)framergb_, out_rgb, PIX_FMT_RGB24, w, h);
    // Do the conversion.
    sws_scale(context_, frame.data, frame.linesize, 0, h,
                framergb_->data, framergb_->linesize);
    framergb_->width = w;
    framergb_->height = h;
    return *framergb_;
}

/*
Determine required size of framebuffer.

avpicture_get_size is used in http://dranger.com/ffmpeg/tutorial01.html 
to do this. However, avpicture_get_size returns the size of a compact 
representation, without padding bytes. Since we use avpicture_fill to 
fill the buffer we should also use it to determine the required size.
*/
int ConverterRGB24::predict_size(int w, int h)
{
    return avpicture_fill((AVPicture*)framergb_, nullptr, PIX_FMT_RGB24, w, h);  
}

std::pair<int, int> width_height(const AVFrame& f)
{
    return std::make_pair(f.width, f.height);
}

int row_size(const AVFrame& f)
{
    return f.linesize[0];
}

void disable_logging()
{
    av_log_set_level(AV_LOG_QUIET);
}
