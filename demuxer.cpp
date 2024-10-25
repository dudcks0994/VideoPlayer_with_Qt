#include "demuxer.h"
#include <QDebug>

Demuxer::Demuxer(QObject *parent)
    : QObject{parent}
{
    vq = new std::queue<AVPacket>;
    aq = new std::queue<AVPacket>;
}

void Demuxer::SetFormatContext(AVFormatContext* ctx, int video_index, int audio_index)
{
    need_reset = 0;
    fmtctx = ctx;
    vidx = video_index;
    aidx = audio_index;
}

void Demuxer::SetCodecContext(AVCodecContext* vctx, AVCodecContext* actx)
{
    video_ctx = vctx;
    audio_ctx = actx;
}

void Demuxer::SetMutex(QMutex* video, QMutex* audio)
{
    vq_lock = video;
    aq_lock = audio;
}

void Demuxer::SetProperty(int w, int h, int bitrate)
{

}

void Demuxer::Demuxing()
{
    AVPacket packet;
    memset(&packet, 0, sizeof(packet));
    if (vidx >=0)
        ;
    if (aidx >= 0)
        ;
    qDebug() << "demuxing start...c\n";
    while (av_read_frame(fmtctx, &packet) == 0) {
        if (packet.stream_index == vidx) {
            vq_lock->lock();
            vq->push(packet);
            vq_lock->unlock();
        }
        else if (packet.stream_index == aidx)
        {
            aq_lock->lock();
            aq->push(packet);
            aq_lock->unlock();
        }
        if (need_reset)
            return;
    }
}
