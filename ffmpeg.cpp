#include "ffmpeg.h"
#include <QDebug>
int sw_conversion(const char *src_filename,
                   const char* src_size,
                   const char * dst_filename,
                   const char* dst_size,
                   enum AVPixelFormat src_pix_fmt,
                   enum AVPixelFormat dst_pix_fmt)
{
    struct SwsContext *sws_ctx;

    uint8_t *src_data[4];
    uint8_t *dst_data[4];
    int src_linesize[4];
    int dst_linesize[4];
    int src_bufsize;
    int dst_bufsize;
    int src_w ;
    int src_h ;
    int dst_w ;
    int dst_h ;
    int ret;

    FILE *src_file;
    FILE *dst_file;

    /*source file parse*/
    if (av_parse_video_size(&src_w, &src_h, src_size) < 0) {
        qDebug() << "Invalid size '" << src_size << "', must be in the form WxH or a valid size abbreviation";
        return -1;
    }
    src_file = fopen(src_filename, "rb");
    if (!src_file) {
        qDebug() << "Could not open source file" << src_filename;
        return -1;
    }

    /*destination file parse*/
    if (av_parse_video_size(&dst_w, &dst_h, dst_size) < 0) {
        qDebug() << "Invalid size '" << dst_size << "', must be in the form WxH or a valid size abbreviation";
        return -1;
    }
    dst_file = fopen(dst_filename, "wb");
    if (!dst_file) {
        qDebug() << "Could not open destination file" << dst_filename;
        return -1;
    }

    /* create scaling context */
    sws_ctx = sws_getContext(src_w, src_h, src_pix_fmt, dst_w, dst_h, dst_pix_fmt,
                             SWS_BILINEAR, NULL, NULL, NULL);
    if (!sws_ctx) {
        qDebug() << "Impossible to create scale context for the conversion"
                        << "fmt:" << av_get_pix_fmt_name(src_pix_fmt) << "s:" << src_w << "x" << src_h
                        << "-> fmt:" << av_get_pix_fmt_name(dst_pix_fmt) << "s:" << dst_w << "x" << dst_h;
        ret = AVERROR(EINVAL);
        goto end;
    }

    /* allocate source and destination image buffers */
    if ((ret = av_image_alloc(src_data, src_linesize, src_w, src_h, src_pix_fmt, 16)) < 0) {
        qDebug() << "Could not allocate source image";
        goto end;
    }
    src_bufsize = ret;

    /* buffer is going to be written to rawvideo file, no alignment */
    if ((ret = av_image_alloc(dst_data, dst_linesize, dst_w, dst_h, dst_pix_fmt, 1)) < 0) {
        qDebug() << "Could not allocate destination image";
        goto end;
    }
    dst_bufsize = ret;

    fread(src_data[0], 1, src_bufsize, src_file);
    sws_scale(sws_ctx, (const uint8_t * const*)src_data,  src_linesize, 0, src_h, dst_data, dst_linesize);

    /* write scaled image to file */
    fwrite(dst_data[0], 1, dst_bufsize, dst_file);

    end:
    fclose(dst_file);
    fclose(src_file);
    av_freep(&src_data[0]);
    av_freep(&dst_data[0]);
    sws_freeContext(sws_ctx);
    return 0;
}

