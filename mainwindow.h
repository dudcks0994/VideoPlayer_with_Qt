#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <queue>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QPushButton>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include "datainfo.h"

class Demuxer;

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class VideoRenderer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QImage* GetImage();
    AVFormatContext *GetFormatContext();
    AVCodecContext *GetVctx();
    AVCodecContext *GetActx();
    Resolution GetResolution();
    AVRational GetFrameRate();
    StreamIndex GetStreamIndex();
    PlayStatus *GetPlayStatus();
    void Play();

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void statusChange();
    void startDemuxing();

public slots:
    void onFrameReady();

private:
    Ui::MainWindow *ui;
    QPushButton *playButton;
    QString file;
    QSlider *playSlider, *soundSlider;
    QPainter *painter;
    QImage *image;
    QThread *demuxer_thread;
    Demuxer* demuxer;
    int init_video();
    Resolution res;
    AVFormatContext *fmtctx;
    AVStream *video_stream, *audio_stream;
    AVCodecContext *video_context, *audio_context;
    AVRational rate;
    StreamIndex stream_idx;
    PlayStatus* status;
};
#endif // MAINWINDOW_H
