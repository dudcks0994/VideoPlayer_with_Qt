#include <windows.h>
#include "videorenderer.h"
#include "objectfactory.h"
#include "datainfo.h"
#include "demuxer.h"
#include <realtimeapiset.h>

VideoRenderer::VideoRenderer(QObject *parent)
    : QObject{parent}
{
    Demuxer* tmp = ObjectFactory::GetDemuxer();
    MainWindow *window = ObjectFactory::GetMainWindow();
    AVRational rate = window->GetFrameRate();
    qDebug() << rate.num << " / " << rate.den;
    delay_msec = 1000.0 * 1.0 / (rate.num / rate.den);;
    video_pool = tmp->GetVideoPool();
    image = window->GetImage();
    res = window->GetResolution();
}

void uusleep(int msec, LARGE_INTEGER frequency)
{
    LARGE_INTEGER start, cur;
    int gap;
    QueryPerformanceCounter(&start);
    QueryPerformanceCounter(&cur);
    gap = (cur.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;

    while (gap < msec)
    {
        if (gap < msec / 8)
            Sleep(7 * msec / 8);
        else if (gap < 2 * msec / 8)
            Sleep(6 * msec / 8);
        else if (gap < 3 * msec / 8)
            Sleep(5 * msec / 8);
        else if (gap < 4 * msec / 8)
            Sleep(4 * msec / 8);
        else if (gap < 5 * msec / 8)
            Sleep(3 * msec / 8);
        else if (gap < 6 * msec / 8)
            Sleep(2 * msec / 8);
        else if (gap < 7 * msec / 8)
            Sleep(msec / 8);
        else
            Sleep(1);
        QueryPerformanceCounter(&cur);
        gap = (cur.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
    }
    // qDebug() << "end for sleep";
}

void VideoRenderer::usleep(int msec)
{
    ULONGLONG start, cur, gap;
    QueryInterruptTime(&start);
    QueryInterruptTime(&cur);
    gap = (cur - start) / 10000.0;
    while(gap < msec)
    {
        // qDebug() << "gap :" << gap;
        if (gap < 3 * msec / 4)
        {
            // qDebug() << "gap : " << gap << "msec : " << msec;
            Sleep(3 * msec / 4);
        }
        else if (gap < msec / 2)
        {
            // qDebug() << "gap : " << gap << "msec : " << msec;
            Sleep(msec / 2 );
        }
        else if (gap < msec / 4)
            Sleep(msec / 4);
        QueryInterruptTime(&cur);
        gap = (cur - start) / 10000.0;
    };

}

void VideoRenderer::Rendering()
{
    int index = 0;
    LARGE_INTEGER before, cur, frequency, nextRenderTime;
    QueryPerformanceFrequency(&frequency);
    before.QuadPart = 0;
    int delay = delay_msec;
    QueryPerformanceCounter(&nextRenderTime);
    nextRenderTime.QuadPart += frequency.QuadPart * delay_msec / 1000;
    qDebug() << "start to rendering..." << video_pool;
    while (1)
    {
        if (video_pool[index].status != S_IMAGE)
        {
            qDebug() << "waiting for image..";
            Sleep(8);
            continue;
        }
        memcpy(image->bits(), video_pool[index].converted_frame.data[0], res.width * res.height * 4);
        video_pool[index].status = S_EMPTY;
        emit OnImageReady();
        QueryPerformanceCounter(&cur);
        if (before.QuadPart != 0)
        {
            double gap = (cur.QuadPart - nextRenderTime.QuadPart) * 1000.0 / frequency.QuadPart;
            nextRenderTime.QuadPart += frequency.QuadPart * delay_msec / 1000;
            delay = delay_msec - gap;
            if (delay < 0)
                delay = 0;
        }
        before = cur;
        uusleep(delay, frequency);
        if (++index == MAX_POOL)
            index = 0;
    }
}

