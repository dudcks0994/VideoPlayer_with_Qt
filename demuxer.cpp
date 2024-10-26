#include "demuxer.h"
#include <QDebug>
#include "framemaker.h"
#include <windows.h>
#include "videoconverter.h"

Demuxer::Demuxer(QObject *parent)
    : QObject{parent}
{
    video_packet = new std::queue<AVPacket *>;
    audio_packet = new std::queue<AVPacket *>;
    video_packet_n = new int;
    audio_packet_n = new int;
    *video_packet_n = 0;
    *audio_packet_n = 0;
    video_packet_mutex = new QMutex;
    audio_packet_mutex = new QMutex;


    // 패킷 큐, 패킷 뮤텍스, 패킷n, video_ctx
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

void Demuxer::SetProperty(int w, int h, AVRational rate_info)
{
    width = w;
    height = h;
    rate = rate_info;
}

void Demuxer::Demuxing()
{
    qDebug() << "demuxing start...\n";

    AVPacket packet;

    memset(&packet, 0, sizeof(packet));
    if (vidx >=0)
    {
        frameMaker_thread = new QThread(this);
        frameMaker = new FrameMaker();
        frameMaker->SetContext(video_packet, video_packet_mutex, video_packet_n, video_ctx);
        connect(frameMaker_thread, SIGNAL(started()), frameMaker, SLOT(Work()));
        videoConverter = new VideoConverter();
        videoConverter_thread = new QThread(this);
        videoConverter->Set(width, height, AV_PIX_FMT_RGB32, frameMaker);
        frameMaker->moveToThread(frameMaker_thread);
        videoConverter->moveToThread(videoConverter_thread);
        connect(videoConverter_thread, SIGNAL(started()), videoConverter, SLOT(Convert()));
        videoConverter_thread->start();
        frameMaker_thread->start();
    }
    if (aidx >= 0)
        ;
    while (1)
    {
        if (video_packet->size() >= 40)
        {
            Sleep(8);
            continue;
        }
        int ret = av_read_frame(fmtctx, &packet);
        if (ret != 0)
            break;
        if (packet.stream_index == vidx) {
            AVPacket *tmp = av_packet_clone(&packet);
            video_packet_mutex->lock();
            video_packet->push(tmp);
            ++(*video_packet_n);
            video_packet_mutex->unlock();
        }
        else if (packet.stream_index == aidx)
        {
            AVPacket *tmp = av_packet_clone(&packet);
            audio_packet_mutex->lock();
            audio_packet->push(tmp);
            ++(*audio_packet_n);
            audio_packet_mutex->unlock();
        }
        if (need_reset)
            return;
    }
}
