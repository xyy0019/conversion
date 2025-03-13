#ifndef FFMPEG_H
#define FFMPEG_H

extern "C" {
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavutil/parseutils.h>
    // 包含其他需要的 FFmpeg 头文件
}

int sw_conversion(const char *src_filename,
                   const char* src_size,
                   const char * dst_filename,
                   const char* dst_size,
                   enum AVPixelFormat src_pix_fmt,
                   enum AVPixelFormat dst_pix_fmt);
#endif // FFMPEG_H
