#ifndef FRAMEMAKER_H
#define FRAMEMAKER_H

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

class FrameMaker : public QObject
{
    Q_OBJECT
public:
    explicit FrameMaker(QObject *parent = nullptr);
    void SetContext(std::queue<AVPacket *> *q, QMutex* mutex, int *n, AVCodecContext* vctx);
    std::queue<AVFrame *> *GetFrameQueue();
    QMutex *GetFrameMutex();
public slots:
    void Work();

signals:


private:
    std::queue<AVFrame *> *frame_q;
    std::queue<AVPacket *> *packet_q;
    AVCodecContext* video_ctx;
    QMutex *packet_mutex, *frame_mutex;
    int *packet_n, *frame_n;
};

#endif // FRAMEMAKER_H
