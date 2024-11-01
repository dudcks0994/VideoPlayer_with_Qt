#ifndef DEMUXER_H
#define DEMUXER_H
#include "mainwindow.h"
#include <QObject>
#include <QMutex>
#include <queue>
#include <QThread>
#include "datainfo.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

class FrameMaker;
class VideoConverter;
class ImageMaker;
class VideoRenderer;

struct PacketBox{
    uchar status;
    AVPacket* packet;
};



class Demuxer : public QObject
{
    Q_OBJECT
public:
    explicit Demuxer(QObject *parent = nullptr, const QString &filepath = "");
    PacketBox* GetVideoPacket();
    Pool* GetVideoPool();
    QMutex* GetFrameMutex();
signals:
    void Error();

public slots:
    void Demuxing();
private:
    void clean();
    int need_reset;
    AVFormatContext* fmtctx;
    Resolution res;
    AVRational rate;
    StreamIndex stream_idx;
    QThread *frameMaker_thread, *videoConverter_thread[6], *videoRenderer_thread;
    QMutex *frame_mutex;
    VideoConverter *videoConverter[6];
    VideoRenderer *videoRenderer;
    FrameMaker *frameMaker;
    ImageMaker *imageMaker;
    PacketBox* video_packet;
    Pool* video_pool;
    int packet_index;
};

#endif // DEMUXER_H
