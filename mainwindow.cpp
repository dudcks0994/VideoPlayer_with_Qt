#include <windows.h>
#include "demuxer.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "objectfactory.h"
#include <qpushbutton.h>
#include <QPainter>
#include <QImage>
#include <QMutex>
#include <QSlider>
#include <QScreen>

#pragma comment(lib, "winmm.lib")



void MainWindow::onFrameReady()
{
    // qDebug() << "frame is ready in main!\n";
    update();
}

QImage *MainWindow::GetImage()
{
    return (image);
}

AVFormatContext *MainWindow::GetFormatContext()
{
    return (fmtctx);
}

AVCodecContext *MainWindow::GetVctx()
{
    return (video_context);
}

AVCodecContext *MainWindow::GetActx()
{
    return (audio_context);
}

Resolution MainWindow::GetResolution()
{
    return (res);
}

AVRational MainWindow::GetFrameRate()
{
    return (rate);
}

StreamIndex MainWindow::GetStreamIndex()
{
    return (stream_idx);
}

PlayStatus *MainWindow::GetPlayStatus()
{
    return (status);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    video_stream = 0;
    audio_stream = 0;
    video_context = 0;
    audio_context = 0;
    fmtctx = 0;
    file = QFileDialog::getOpenFileName(this, "Open Video File", "", "Video Files (*.mp4 *.avi *.mkv *.m3u8)");
    init_video();
    image = new QImage(res.width, res.height, QImage::Format_RGB32);
    status = (PlayStatus *)malloc(sizeof(PlayStatus));
    *status = P_STOP;
}

void MainWindow::Play()
{
    demuxer_thread = new QThread;
    demuxer = ObjectFactory::GetDemuxer(file);
    this->move(QPoint(0, 0));
    this->resize(res.width + 40, res.height + 40);
    playButton = new QPushButton("Play", this);
    playButton->setGeometry(10, 10, 50, 30);
    connect(playButton, &QPushButton::clicked, [this]() {
        emit statusChange();
    });
    demuxer->moveToThread(demuxer_thread);
    QObject::connect(demuxer_thread, SIGNAL(started()), demuxer, SLOT(Demuxing()));
    demuxer_thread->start();
}

MainWindow::~MainWindow() // should add signal, so when it called, call other class and thread to delete resource.
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawImage(10, 40, *image);
}

int MainWindow::init_video()
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
    stream_idx.vidx = av_find_best_stream(fmtctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    stream_idx.aidx = av_find_best_stream(fmtctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (stream_idx.vidx == -1 && stream_idx.aidx == -1)
        return (-1);
    if (stream_idx.vidx != -1)
    {
        video_stream = fmtctx->streams[stream_idx.vidx];
        video_codec = const_cast<AVCodec*>(avcodec_find_decoder(video_stream->codecpar->codec_id));
        video_context = avcodec_alloc_context3(video_codec);
        if (avcodec_parameters_to_context(video_context, video_stream->codecpar) != 0)
            return (-1);
        if (avcodec_open2(video_context, video_codec, nullptr) != 0)
            return (-1);
        res.width = video_context->width;
        res.height = video_context->height;
        rate = video_context->framerate;
        if (!rate.num || !rate.den)
            rate = video_stream->avg_frame_rate;
        // qDebug() << "rate is " << video_stream->avg_frame_rate.num / video_stream->avg_frame_rate.den;
    }
    if (stream_idx.aidx != -1)
    {
        audio_stream = fmtctx->streams[stream_idx.aidx];
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
    if (resolution.width() != 0 && (res.width + 200 > resolution.width() || res.height + 200 > resolution.height()))
    {
        double ratio;
        if (res.width >= resolution.width())
            ratio = (1.0 * (resolution.width())) / (res.width + 300);
        else
            ratio = (1.0 * (resolution.height())) / (res.height + 300);
        res.width = res.width * 1.0 * ratio;
        res.height = res.height * 1.0 * ratio;
    }
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
