#ifndef DEMUXER_H
#define DEMUXER_H

#include <QObject>
#include <QMutex>
#include <queue>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

class Demuxer : public QObject
{
    Q_OBJECT
public:
    explicit Demuxer(QObject *parent = nullptr);
    void SetFormatContext(AVFormatContext* ctx, int video_index, int audio_index);
    void SetCodecContext(AVCodecContext* vctx, AVCodecContext* actx);
    void SetMutex(QMutex* video, QMutex* audio);
    void SetProperty(int w, int h, int bitrate);
signals:

public slots:
    void Demuxing();

private:
    void clean();
    AVFormatContext *fmtctx;
    AVCodecContext *video_ctx, *audio_ctx;
    std::queue<AVPacket> *vq, *aq;
    QMutex* vq_lock, *aq_lock;
    int vidx, aidx;
    int need_reset;
    int width, height;
    int delay;
};

#endif // DEMUXER_H
