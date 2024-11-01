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
    frame_mutex = tmp->GetFrameMutex();
}

void VideoConverter::NeedClean()
{
}

void VideoConverter::Convert()
{
    qDebug() << "VideoConverter start..." << video_pool << " " << res.width << "," << res.height;
    int rest_cnt = 0;
    int index;
    int i = 0;
    while(1)
    {
        frame_mutex->lock();
        while (i < MAX_POOL)
        {
            if (video_pool[i].status == S_FRAME)
            {
                index = i;
                video_pool[i].status = S_CONVERT;
                break;
            }
            ++i;
            if (i == MAX_POOL)
            {
                i = 0;
                qDebug() << "from converter " << ++rest_cnt;
                Sleep(15);
            }
        }
        frame_mutex->unlock();
        int ret = sws_scale(video_pool[index].sctx, video_pool[index].frame->data, video_pool[index].frame->linesize, 0, video_pool[index].frame->height, video_pool[index].converted_frame.data, video_pool[index].converted_frame.linesize);
        if (ret < 0)
        {
            qDebug() << "error from scaling " << video_pool[index].frame;
            return ;
        }
        av_frame_unref(video_pool[index].frame);
        video_pool[index].status = S_IMAGE;
    }
}
