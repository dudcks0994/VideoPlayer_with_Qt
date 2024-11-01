#include "framemaker.h"
#include <windows.h>
#include <QDebug>
#include "objectfactory.h"
#include "demuxer.h"

FrameMaker::FrameMaker(QObject *parent)
    : QObject{parent}
{
    Demuxer *tmp = ObjectFactory::GetDemuxer();
    MainWindow *window = ObjectFactory::GetMainWindow();
    video_ctx = window->GetVctx();
    video_packet = tmp->GetVideoPacket();
    video_pool = tmp->GetVideoPool();
    packet_index = 0;
}

void FrameMaker::Work()
{
    qDebug() << "frame maker start..." << video_pool;
    AVFrame frame;
    memset(&frame, 0, sizeof(AVFrame));
    frame_index = 0;
    int rest_cnt = 0;
    while (1)
    {
        if (!video_packet[packet_index].status)
        {
            Sleep(20);
            continue;
        }
        AVPacket *packet = video_packet[packet_index].packet;
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
        for (;;)
        {
            if (video_pool[frame_index].status != S_EMPTY)
            {
                // qDebug() << "frameMaker : " << ++rest_cnt;
                Sleep(10);
                continue;
            }
            video_pool[frame_index].frame = tmp;
            video_pool[frame_index].status = S_FRAME;
            if (++frame_index == 6)
                frame_index = 0;
            break;
        }
        av_frame_unref(&frame);
        av_packet_unref(video_packet[packet_index].packet);
        video_packet[packet_index].status = 0;
        if (++packet_index == 30)
            packet_index = 0;
    }
}
