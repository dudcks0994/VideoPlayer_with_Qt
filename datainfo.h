#ifndef DATAINFO_H
#define DATAINFO_H

#include <windows.h>
#include <QMainWindow>
#define S_EMPTY 0
#define S_FRAME 1
#define S_CONVERT 2
#define S_IMAGE 3

#define MAX_POOL 10
#define MAX_HDR 10

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

struct PacketBox{
    uchar status;
    AVPacket* packet;
};

struct Pool{
    uint8_t status;
    AVFrame* frame;
    AVFrame  converted_frame;
    SwsContext *sctx;
    int buff_size;
    uint8_t* buffer;
    uchar* img;
};
struct AudioPool{
    int status;
    WAVEHDR hdr;
};

struct Resolution{
    int width;
    int height;
};

struct StreamIndex{
    int vidx;
    int aidx;
};

enum PlayStatus{
    P_STOP,
    P_PLAY,
    P_END
};

#endif // DATAINFO_H
