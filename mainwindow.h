#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <queue>
#include <QTimer>
#include <QThread>
#include "videoworker.h"
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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int init_video(Demuxer *demuxer);
    // int update_image();
    int width, height;

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void statusChange();
    void startDemuxing();

public slots:
    void onFrameReady(uchar* orig);

private:

    Ui::MainWindow *ui;
    QImage image;
    QPushButton *playButton;
    QString filepath;
    QSlider *playSlider, *soundSlider;
    QPainter *painter;
};
#endif // MAINWINDOW_H
