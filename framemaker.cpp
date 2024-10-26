#include "framemaker.h"
#include <windows.h>
#include <QDebug>

FrameMaker::FrameMaker(QObject *parent)
    : QObject{parent}
{
    frame_q = new std::queue<AVFrame *>;
    frame_mutex = new QMutex;
}

void FrameMaker::SetContext(std::queue<AVPacket *> *q, QMutex* mutex, int *n, AVCodecContext* vctx)
{
    packet_q = q;
    packet_mutex = mutex;
    packet_n = n;
    video_ctx = vctx;
}

std::queue<AVFrame* >* FrameMaker::GetFrameQueue()
{
    return frame_q;
}

QMutex* FrameMaker::GetFrameMutex()
{
    return frame_mutex;
}

void FrameMaker::Work()
{
    qDebug() << "frame maker start...\n";
    AVFrame frame;
    memset(&frame, 0, sizeof(AVFrame));
    while (1)
    {
        if (packet_q->empty() || frame_q->size() > 10)
        {
            qDebug() << "frame maker is resting...";
            Sleep(1000);
            continue;
        }
        packet_mutex->lock();
        AVPacket *packet = packet_q->front();
        packet_q->pop();
        packet_mutex->unlock();
        int ret = avcodec_send_packet(video_ctx, packet);
        if (ret != 0)
        {
            if (ret == AVERROR(EAGAIN))
                continue;
            if (ret == AVERROR_EOF)
                break;
            else
                return ;
        }
        ret = avcodec_receive_frame(video_ctx, &frame);
        if (ret != 0)
        {
            if (ret == AVERROR(EAGAIN))
                continue;
            if (ret == AVERROR_EOF)
                break;
        }
        AVFrame *tmp = av_frame_clone(&frame);
        frame_mutex->lock();
        frame_q->push(tmp);
        frame_mutex->unlock();
        av_packet_unref(packet);
        qDebug() << "pushed frame";
    }
}
