#include "videoconverter.h"
#include "framemaker.h"
#include <windows.h>
#include <QDebug>

VideoConverter::VideoConverter(QObject *parent)
    : QObject{parent}
{
    is_sent = 0;
    sctx = 0;
}

void VideoConverter::Set(int w, int h, int format, FrameMaker *f)
{
    width = w;
    height = h;
    wanted_format = format;
    video_frame_q = f->GetFrameQueue();
    video_frame_mutex = f->GetFrameMutex();
}

void VideoConverter::NeedClean()
{
    is_sent = 1;
}

void VideoConverter::Convert()
{
    qDebug() << "VideoConverter start...\n";
    uchar*** image;
    image = (uchar ***)malloc(sizeof(uchar **) * 2);
    image[0] = (uchar **)calloc(60, sizeof(uchar *));
    image[1] = (uchar **)calloc(60, sizeof(uchar *));
    for (int i = 0; i < 60; ++i)
    {
        image[0][i] = (uchar *)malloc(width * height * 4);
        image[1][i] = (uchar *)malloc(width * height * 4);
    }
    int set_index = 0;
    int img_index = 0;
    int cur_set = 0;
    AVFrame *converted_frame = av_frame_alloc();
    while(1)
    {
        if (video_frame_q->empty() || (is_sent == 1 && img_index == 59))
        {
            qDebug() << "video converter is resting...";
            Sleep(8);
            continue;
        }
        video_frame_mutex->lock();
        AVFrame *frame;
        frame = video_frame_q->front();
        video_frame_q->pop();
        video_frame_mutex->unlock();
        qDebug() << "before scaling\n";
        if (!sctx)
        {
            sctx = sws_getContext(frame->width, frame->height, AVPixelFormat(frame->format), width, height, AV_PIX_FMT_RGB32, SWS_BICUBIC, 0, 0, 0);
            int buff_size = av_image_get_buffer_size(AV_PIX_FMT_RGB32, width, height, 1);
            buffer = (uint8_t* )av_malloc(buff_size);
            av_image_fill_arrays(converted_frame->data, converted_frame->linesize, buffer, AV_PIX_FMT_RGB32, width, height, 1);
        }
        qDebug() << "after scaling" ;
        int ret = sws_scale(sctx, frame->data, frame->linesize, 0, frame->height, converted_frame->data, converted_frame->linesize);
        qDebug() << "done real scale : " << ret;
        if (ret < 0)
            return;
        memcpy(image[cur_set][img_index], converted_frame->data[0], width * height * 4);
        ++img_index;
        qDebug() << "???";
        if (img_index == 60)
        {
            emit ImageSetReady(image[cur_set]);
            is_sent = 1;
            cur_set = (cur_set == 0) ? 1 : 0;
            img_index = 0;
        }
        qDebug() << "made image done!!!!!!!!!";
        av_frame_unref(frame);
    }
}
