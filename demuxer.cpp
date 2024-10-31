#include "demuxer.h"
#include "framemaker.h"
#include <QDebug>
#include <windows.h>
#include "videoconverter.h"
#include "objectfactory.h"
#include <QScreen>

Demuxer::Demuxer(QObject *parent, const QString& file)
    : QObject{parent}
{
    video_stream = 0;
    audio_stream = 0;
    video_packet_mutex = new QMutex;
    audio_packet_mutex = new QMutex;
    fmtctx = 0;
    video_stream = 0;
    audio_stream = 0;
    if (initVideo(file) != 0)
    {
        qDebug() << "Failed!";
        return ;
    }
    video_packet = (PacketBox*)calloc(60, sizeof(PacketBox));
    video_pool = (Pool *)calloc(6, sizeof(Pool));
}

AVCodecContext* Demuxer::GetVideoContext()
{
    return (video_context);
}

PacketBox* Demuxer::GetVideoPacket()
{
    return (video_packet);
}

Pool* Demuxer::GetVideoPool()
{
    return (video_pool);
}

int Demuxer::initVideo(const QString &file)
{
    AVCodec *audio_codec, *video_codec;
    qDebug() << "start initialize video\n";
    std::string path = file.toStdString();
    if (path == "")
        return (-1);
    const char *url = path.c_str();
    // const char *url = "https://demo.unified-streaming.com/k8s/features/stable/video/tears-of-steel/tears-of-steel.ism/.m3u8";
    int ret = avformat_open_input(&fmtctx, url, nullptr, nullptr);
    if (ret != 0)
        return (-1);
    avformat_find_stream_info(fmtctx, nullptr);
    vidx = av_find_best_stream(fmtctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    aidx = av_find_best_stream(fmtctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (vidx == -1 && aidx == -1)
        return (-1);
    if (vidx != -1)
    {
        video_stream = fmtctx->streams[vidx];
        video_codec = const_cast<AVCodec*>(avcodec_find_decoder(video_stream->codecpar->codec_id));
        video_context = avcodec_alloc_context3(video_codec);
        if (avcodec_parameters_to_context(video_context, video_stream->codecpar) != 0)
            return (-1);
        if (avcodec_open2(video_context, video_codec, nullptr) != 0)
            return (-1);
        width = video_context->width;
        height = video_context->height;
        rate = video_context->framerate;
    }
    if (aidx != -1)
    {
        audio_stream = fmtctx->streams[aidx];
        audio_codec = const_cast<AVCodec*>(avcodec_find_decoder(audio_stream->codecpar->codec_id));
        audio_context = avcodec_alloc_context3(audio_codec);
        if (avcodec_parameters_to_context(audio_context, audio_stream->codecpar) != 0)
            return (-1);
        if (avcodec_open2(audio_context, audio_codec, nullptr) != 0)
            return (-1);
    }

    QScreen *screen = QGuiApplication::primaryScreen();
    QSize resolution(0, 0);
    if (screen)
        resolution = screen->size();
    if (resolution.width() != 0 && (width + 200 > resolution.width() || height + 200 > resolution.height()))
    {
        double ratio;
        if (width >= resolution.width())
            ratio = (1.0 * (resolution.width())) / (width + 300);
        else
            ratio = (1.0 * (resolution.height())) / (height + 300);
        width = width * 1.0 * ratio;
        height = height * 1.0 * ratio;
    }
    MainWindow *window = ObjectFactory::GetMainWindow();
    window->resize(width + 40, height + 40);
    window->SetPixmap(video_pool, rate);
    window->SetResolution(width, height);
    UINT wavenum;
    char devname[128];
    wavenum = waveOutGetNumDevs();
    qDebug() << "장치 개수 : " << wavenum;
    WAVEOUTCAPS cap;
    for (UINT i = 0; i < wavenum; i++) {
        waveOutGetDevCaps(i, &cap, sizeof(WAVEOUTCAPS));
        WideCharToMultiByte(CP_ACP, 0, cap.szPname, -1, devname, 128, NULL, NULL);
        qDebug() << i << "번 : " << cap.wChannels << " 채널, 지원포맷=" << cap.dwFormats << ", 기능=" << cap.dwSupport << ", 이름= " << devname;
    }
    qDebug() << "Success initilize video..";
    return (0);
}

void Demuxer::Demuxing()
{
    qDebug() << "demuxing start...\n";

    AVPacket packet;

    memset(&packet, 0, sizeof(packet));
    packet_index = 0;
    int video_packet_index = 0;
    if (vidx >=0)
    {
        frameMaker_thread = new QThread(this);
        frameMaker = new FrameMaker();
        connect(frameMaker_thread, SIGNAL(started()), frameMaker, SLOT(Work()));
        frameMaker->moveToThread(frameMaker_thread);
        frameMaker_thread->start();
        for (int i = 0; i < 6; ++i)
        {
            videoConverter_thread[i] = new QThread();
            videoConverter[i] = new VideoConverter(0, i);
            videoConverter[i]->moveToThread(videoConverter_thread[i]);
            connect(videoConverter_thread[i], SIGNAL(started()), videoConverter[i], SLOT(Convert()));
            videoConverter_thread[i]->start();
        }
        // videoConverter = new VideoConverter();
        // videoConverter->Set(width, height, AV_PIX_FMT_RGB32, frameMaker);
        // videoConverter_thread = new QThread(this);
        // videoConverter->moveToThread(videoConverter_thread);
        // connect(videoConverter_thread, SIGNAL(started()), videoConverter, SLOT(Convert()));
        // videoConverter_thread->start();
    }
    if (aidx >= 0)
        ;
    while (1)
    {
        int ret = av_read_frame(fmtctx, &packet);
        if (ret != 0)
            break;
        if (packet.stream_index == vidx)
        {
            for (;;)
            {
                if (video_packet[video_packet_index].status)
                {
                    Sleep(8);
                    // qDebug() << "resting demuxer index : " << video_packet_index;
                    continue;
                }
                AVPacket *tmp = av_packet_clone(&packet);
                video_packet[video_packet_index].packet = tmp;
                video_packet[video_packet_index].status = 1;
                qDebug() << "pushed packet : " << video_packet_index << ", duration : " << tmp->duration;
                if (++video_packet_index == 60)
                    video_packet_index = 0;
                break;
            }
        }
        else if (packet.stream_index == aidx)
        {
            // AVPacket *tmp = av_packet_clone(&packet);

            // // 나중에 추가
        }
        ++packet_index;
        // av_packet_unref(&packet);
    }
    qDebug() << "error from read frame to packet";
}
