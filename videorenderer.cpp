#include <windows.h>
#include "videorenderer.h"
#include "objectfactory.h"
#include "datainfo.h"
#include "demuxer.h"

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

void VideoRenderer::usleep(int msec)
{
    u_int64 start, cur, gap;
    typedef VOID (WINAPI *FuncQueryInterruptTime)(_Out_ PULONGLONG lpInterruptTime);
    HMODULE hModule = ::LoadLibrary(L"KernelBase.dll");
    FuncQueryInterruptTime func_QueryInterruptTime =
        (FuncQueryInterruptTime)::GetProcAddress(hModule, "QueryInterruptTime");
    func_QueryInterruptTime(&start);
    do{
        func_QueryInterruptTime(&cur);
        gap = (cur - start) / 10000.0;
        if (gap < msec / 2)
            Sleep(msec / 2);
    }while(gap < msec);
}

void VideoRenderer::Rendering()
{
    int index = 0;
    qDebug() << "start to rendering..." << video_pool;
    while (1)
    {
        if (video_pool[index].status != S_IMAGE)
        {
            Sleep(8);
            continue;
        }
        memcpy(image->bits(), video_pool[index].img, res.width * res.height * 4);
        emit OnImageReady();
        usleep(delay_msec);
        free(video_pool[index].img);
        video_pool[index].status = S_EMPTY;
        if (++index == 6)
            index = 0;
    }
}

