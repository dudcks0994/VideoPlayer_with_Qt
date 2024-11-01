#ifndef FRAMEMAKER_H
#define FRAMEMAKER_H

#include <QObject>
#include <QMutex>
#include <queue>
#include "datainfo.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

struct PacketBox;

struct Pool;

class FrameMaker : public QObject
{
    Q_OBJECT
public:
    explicit FrameMaker(QObject *parent = nullptr);
public slots:
    void Work();

signals:


private:
    AVCodecContext* video_ctx;
    PacketBox* video_packet;
    Pool* video_pool;
    Resolution res;
    int packet_index, frame_index;
};

#endif // FRAMEMAKER_H
