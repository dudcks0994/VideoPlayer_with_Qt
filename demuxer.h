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

class Demuxer : public QObject
{
    Q_OBJECT
public:
    explicit Demuxer(QObject *parent = nullptr);
    void SetFormatContext(AVFormatContext* ctx, int video_index, int audio_index);
    void SetCodecContext(AVCodecContext* vctx, AVCodecContext* actx);
    void SetProperty(int w, int h, AVRational rate_info);
signals:

public slots:
    void Demuxing();

private:
    void clean();
    AVFormatContext *fmtctx;
    AVCodecContext *video_ctx, *audio_ctx;
    std::queue<AVPacket *> *video_packet, *audio_packet;
    int *video_packet_n, *audio_packet_n;
    QMutex* video_packet_mutex, *audio_packet_mutex;
    int vidx, aidx;
    int need_reset;
    int width, height;
    AVRational rate;
    QThread *frameMaker_thread, *videoConverter_thread, *imageMaker_thread;
    VideoConverter *videoConverter;
    FrameMaker *frameMaker;
};

#endif // DEMUXER_H
