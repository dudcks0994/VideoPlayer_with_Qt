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
    res = window->GetResolution();
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
            // qDebug() << "wait for packet index " << packet_index;
            Sleep(20);
            continue;
        }
        AVPacket *packet = video_packet[packet_index].packet;
        int ret = avcodec_send_packet(video_ctx, packet);
        // qDebug() << "send packet returned " << ret;
        if (ret != 0)
        {
            if (ret == AVERROR(EAGAIN))
                continue;
            if (ret == AVERROR_EOF)
                break;
            else
                return ;
        }
        av_packet_unref(video_packet[packet_index].packet);
        video_packet[packet_index].status = 0;
        ret = avcodec_receive_frame(video_ctx, &frame);
        if (ret != 0)
        {
            // qDebug() << "receive frame returned " << ret;
            if (ret == AVERROR(EAGAIN))
            {
                if (++packet_index == 60)
                    packet_index = 0;
                continue;
            }
            if (ret == AVERROR_EOF)
                break;
        }
        AVFrame *tmp = av_frame_clone(&frame);
        for (;;)
        {
            if (video_pool[frame_index].status != S_EMPTY)
            {
                // qDebug() << "from frame maker " << ++rest_cnt;
                Sleep(20);
                continue;
            }
            video_pool[frame_index].frame = tmp;
            if (!video_pool[frame_index].sctx)
            {
                video_pool[frame_index].sctx = sws_getContext(video_pool[frame_index].frame->width, video_pool[frame_index].frame->height, AVPixelFormat(video_pool[frame_index].frame->format), res.width, res.height, AVPixelFormat(AV_PIX_FMT_RGB32), SWS_BICUBIC, 0, 0, 0);
                video_pool[frame_index].buff_size = av_image_get_buffer_size(AVPixelFormat(AV_PIX_FMT_RGB32), res.width, res.height, 1);
                video_pool[frame_index].buffer = (uint8_t* )av_malloc(video_pool[frame_index].buff_size);
                av_image_fill_arrays(video_pool[frame_index].converted_frame.data, video_pool[frame_index].converted_frame.linesize, video_pool[frame_index].buffer, AVPixelFormat(AV_PIX_FMT_RGB32), res.width, res.height, 1);
            }
            video_pool[frame_index].status = S_FRAME;
            // qDebug() << "pushed frame";
            if (++frame_index == MAX_POOL)
                frame_index = 0;
            break;
        }
        av_frame_unref(&frame);
        if (++packet_index == 60)
            packet_index = 0;
    }
}
