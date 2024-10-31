#ifndef DEMUXER_H
#define DEMUXER_H

#include <QObject>
#include <QMutex>
#include <queue>
#include <QThread>

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

struct PacketBox{
    uchar status;
    AVPacket* packet;
};

struct Pool{
    uint8_t status;
    AVFrame* frame;
    uchar* img;
};


class Demuxer : public QObject
{
    Q_OBJECT
public:
    explicit Demuxer(QObject *parent = nullptr, const QString &filepath = "");
    void SetProperty(int w, int h, AVRational rate_info);
    AVCodecContext* GetVideoContext();
    AVCodecContext* GetAudioContext();
    PacketBox* GetVideoPacket();
    Pool* GetVideoPool();
signals:
    void Error();

public slots:
    void Demuxing();
private:
    void clean();
    int initVideo(const QString &file);
    AVFormatContext *fmtctx;
    AVStream *video_stream, *audio_stream;
    AVCodecContext *video_context, *audio_context;
    int *video_packet_n, *audio_packet_n;
    QMutex* video_packet_mutex, *audio_packet_mutex;
    int vidx, aidx;
    int need_reset;
    int width, height;
    AVRational rate;
    QThread *frameMaker_thread, *videoConverter_thread[6], *imageMaker_thread;
    VideoConverter *videoConverter[6];
    FrameMaker *frameMaker;
    ImageMaker *imageMaker;
    PacketBox* video_packet;
    Pool* video_pool;
    int packet_index;
};

#endif // DEMUXER_H
