#include <windows.h>
#include <QDebug>
#include <QScreen>
#include "demuxer.h"
#include "videoconverter.h"
#include "objectfactory.h"
#include "framemaker.h"
#include "videorenderer.h"
#include "audiomaker.h"
#include "soundplayer.h"


Demuxer::Demuxer(QObject *parent, const QString& file)
    : QObject{parent}
{
    MainWindow *window = ObjectFactory::GetMainWindow();
    fmtctx = window->GetFormatContext();
    stream_idx = window->GetStreamIndex();
    video_packet = (PacketBox*)calloc(200, sizeof(PacketBox));
    audio_packet = (PacketBox*)calloc(200, sizeof(PacketBox));
    video_pool = (Pool *)calloc(MAX_POOL, sizeof(Pool));
    audio_pool = (AudioPool *)calloc(MAX_POOL, sizeof(AudioPool));
    frame_mutex = new QMutex;
}

PacketBox* Demuxer::GetVideoPacket()
{
    return (video_packet);
}

Pool* Demuxer::GetVideoPool()
{
    return (video_pool);
}

AudioPool* Demuxer::GetAudioPool()
{
    return (audio_pool);
}

PacketBox* Demuxer::GetaudioPacket()
{
    return (audio_packet);
}

QMutex* Demuxer::GetFrameMutex()
{
    return (frame_mutex);
}

void Demuxer::Demuxing()
{
    qDebug() << "demuxing start...\n";

    AVPacket packet;

    memset(&packet, 0, sizeof(packet));
    packet_index = 0;
    int video_packet_index = 0;
    int audio_packet_index = 0;
    if (stream_idx.vidx >=0)
    {
        frameMaker_thread = new QThread(this);
        frameMaker = new FrameMaker();
        connect(frameMaker_thread, SIGNAL(started()), frameMaker, SLOT(Work()));
        frameMaker->moveToThread(frameMaker_thread);
        frameMaker_thread->start();
        for (int i = 0; i < 4; ++i)
        {
            videoConverter_thread[i] = new QThread();
            videoConverter[i] = new VideoConverter(0, i);
            videoConverter[i]->moveToThread(videoConverter_thread[i]);
            connect(videoConverter_thread[i], SIGNAL(started()), videoConverter[i], SLOT(Convert()));
            videoConverter_thread[i]->start();
        }
        MainWindow *window = ObjectFactory::GetMainWindow();
        videoRenderer_thread = new QThread;
        videoRenderer = new VideoRenderer();
        connect(videoRenderer_thread, SIGNAL(started()), videoRenderer, SLOT(Rendering()));
        connect(videoRenderer, SIGNAL(OnImageReady()), window, SLOT(onFrameReady()), Qt::DirectConnection);
        videoRenderer->moveToThread(videoRenderer_thread);
        videoRenderer_thread->start();
    }
    if (stream_idx.aidx >= 0)
    {
        audioMaker_thread = new QThread(this);
        audioMaker = new AudioMaker;
        connect(audioMaker_thread, SIGNAL(started()), audioMaker, SLOT(Convert()));
        audioMaker->moveToThread(audioMaker_thread);
        audioMaker_thread->start();
        soundPlayer_thread = new QThread(this);
        soundPlayer = new SoundPlayer;
        soundPlayer->moveToThread(soundPlayer_thread);
        connect(soundPlayer_thread, SIGNAL(started()), soundPlayer, SLOT(Play()));
        soundPlayer_thread->start();
    }
    while (1)
    {
        int ret = av_read_frame(fmtctx, &packet);
        if (ret != 0)
            return ;
        AVPacket *tmp = av_packet_clone(&packet);
        if (packet.stream_index == stream_idx.vidx)
        {
            for (;;)
            {
                if (video_packet[video_packet_index].status)
                {
                    Sleep(8);
                    // qDebug() << "resting demuxer index : " << video_packet_index;
                    continue;
                }
                video_packet[video_packet_index].packet = tmp;
                video_packet[video_packet_index].status = 1;
                // qDebug() << "pushed video packet : " << video_packet_index << ", duration : " << tmp->duration;
                if (++video_packet_index == 60)
                    video_packet_index = 0;
                break;
            }
        }
        else if (packet.stream_index == stream_idx.aidx)
        {
            for(;;)
            {
                if (audio_packet[audio_packet_index].status != 0)
                {
                    Sleep(8);
                    // qDebug() << "wait for audio packet index " << audio_packet_index;
                    continue;
                }
                audio_packet[audio_packet_index].packet = tmp;
                audio_packet[audio_packet_index].status = 1;
                // qDebug() << "pushed audio packet : " << audio_packet_index << ", duration : " << tmp->duration;
                if (++audio_packet_index == 60)
                    audio_packet_index = 0;
                break;
            }
            // 나중에 추가
        }
        ++packet_index;
        av_packet_unref(&packet);
    }
    qDebug() << "error from read frame to packet";
}
