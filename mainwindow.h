#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <queue>
#include <QTimer>
#include <QThread>
#include "demuxer.h"
#include <QMutex>
#include <QPushButton>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>

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
    void SetPixmap(Pool* p,  AVRational rate);
    ~MainWindow();
    void SetResolution(int w, int h);
    int width, height;

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void statusChange();
    void startDemuxing();

public slots:
    void onFrameReady(uchar* orig);
    void Rendering();

private:
    void usleep(int msec);
    Ui::MainWindow *ui;
    QPushButton *playButton;
    QString filepath;
    QSlider *playSlider, *soundSlider;
    QPainter *painter;
    Pool*   video_pool;
    QImage *image;
    AVRational rate;
    int delay_msec;
};
#endif // MAINWINDOW_H
