#include <windows.h>
#include "videoconverter.h"
#include <QDebug>
#include "objectfactory.h"
#include "mainwindow.h"

VideoConverter::VideoConverter(QObject *parent, uint8_t id)
    : QObject{parent}
{
    sctx = 0;
    Demuxer* tmp = ObjectFactory::GetDemuxer();
    video_pool = tmp->GetVideoPool();
    MainWindow* window = ObjectFactory::GetMainWindow();
    res = window->GetResolution();
    thread_id = id;
}

void VideoConverter::NeedClean()
{
}

void VideoConverter::Convert()
{
    qDebug() << "VideoConverter start..." << video_pool << " " << res.width << "," << res.height;
    AVFrame *converted_frame = av_frame_alloc();
    int rest_cnt = 0;
    while(1)
    {
        if (video_pool[thread_id].status != S_FRAME)
        {
            // qDebug() << "resting\n";
            // qDebug() << "VideoConverter : " << ++rest_cnt;
            Sleep(20);
            continue;
        }
        if (!sctx)
        {
            sctx = sws_getContext(video_pool[thread_id].frame->width, video_pool[thread_id].frame->height, AVPixelFormat(video_pool[thread_id].frame->format), res.width, res.height, AVPixelFormat(AV_PIX_FMT_RGB32), SWS_BICUBIC, 0, 0, 0);
            int buff_size = av_image_get_buffer_size(AVPixelFormat(AV_PIX_FMT_RGB32), res.width, res.height, 1);
            buffer = (uint8_t* )av_malloc(buff_size);
            av_image_fill_arrays(converted_frame->data, converted_frame->linesize, buffer, AVPixelFormat(AV_PIX_FMT_RGB32), res.width, res.height, 1);
        }
        int ret = sws_scale(sctx, video_pool[thread_id].frame->data, video_pool[thread_id].frame->linesize, 0, video_pool[thread_id].frame->height, converted_frame->data, converted_frame->linesize);
        if (ret < 0)
        {
            qDebug() << "error from scaling " << video_pool[thread_id].frame;
            return ;
        }
        video_pool[thread_id].img = (uchar *)malloc(res.width * res.height * 4);
        memcpy(video_pool[thread_id].img, converted_frame->data[0], res.width * res.height * 4);
        av_frame_unref(video_pool[thread_id].frame);
        video_pool[thread_id].status = S_IMAGE;
        // qDebug() << thread_id << "th thread : made image : " << video_pool[thread_id].img;
    }
}
