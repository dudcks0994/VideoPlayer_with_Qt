#include <windows.h>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "videoworker.h"
#include "demuxer.h"
#include <qpushbutton.h>
#include <QPainter>
#include <QImage>
#include <QMutex>
#include <QSlider>
#include <QScreen>

#pragma comment(lib, "winmm.lib")

#define WIDTH 720
#define HEIGHT 480

void MainWindow::onFrameReady(uchar* orig)
{
    // qDebug() << "frame is ready in main!\n";
    uchar* dst = image.bits();
    memcpy(dst, orig, width * height * 4);
    update();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    playButton = new QPushButton("Play", this);
    playButton->setGeometry(10, 10, 50, 30);
    filepath = QFileDialog::getOpenFileName(this, "Open Video File", "", "Video Files (*.mp4 *.avi *.mkv *.m3u8)");
    painter = new QPainter(this);
    // connect(playButton, &QPushButton::clicked, [this]() {
    //     emit statusChange();
    // });
}

MainWindow::~MainWindow() // should add signal, so when it called, call other class and thread to delete resource.
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawImage(10, 40, image);
}

int MainWindow::init_video(Demuxer *demuxer)
{
    qDebug() << "start initialize video\n";
    std::string path = filepath.toStdString();
    // if (path == "")
    //     this->close();
    const char *url = path.c_str();
    // const char *url = "https://demo.unified-streaming.com/k8s/features/stable/video/tears-of-steel/tears-of-steel.ism/.m3u8";
    AVFormatContext *fmtCtx = 0;
    int ret = avformat_open_input(&fmtCtx, url, nullptr, nullptr);
    if (ret != 0)
        return -1;
    avformat_find_stream_info(fmtCtx, nullptr);
    int vidx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    int aidx = av_find_best_stream(fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (vidx == -1 || aidx == -1)
        return (-1);
    AVStream *video_stream = fmtCtx->streams[vidx];
    AVStream *audio_stream = fmtCtx->streams[aidx];

    const AVCodec *audio_codec = avcodec_find_decoder(audio_stream->codecpar->codec_id);
    const AVCodec *video_codec = avcodec_find_decoder(video_stream->codecpar->codec_id);

    AVCodecContext *video_context = avcodec_alloc_context3(video_codec);
    AVCodecContext *audio_context = avcodec_alloc_context3(audio_codec);

    if (avcodec_parameters_to_context(video_context, video_stream->codecpar) != 0)
        return -1;
    if (avcodec_parameters_to_context(audio_context, audio_stream->codecpar) != 0)
        return -1;
    if (avcodec_open2(video_context, video_codec, nullptr) != 0)
        return -1;
    if (avcodec_open2(audio_context, audio_codec, nullptr) != 0)
        return -1;
    width = video_context->width;
    height = video_context->height;
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
    image = QImage(width, height, QImage::Format_RGB32);
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
    demuxer->SetFormatContext(fmtCtx, vidx, aidx);
    demuxer->SetCodecContext(video_context, audio_context);
    demuxer->SetProperty(width, height, video_stream->r_frame_rate);
    qDebug() << "success to initialize video!\n";
    return 0;
}
