#ifndef DATAINFO_H
#define DATAINFO_H

#include <QMainWindow>
#define S_EMPTY 0
#define S_FRAME 1
#define S_CONVERT 2
#define S_IMAGE 3

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

struct Pool{
    uint8_t status;
    AVFrame* frame;
    AVFrame  converted_frame;
    uchar* img;
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
