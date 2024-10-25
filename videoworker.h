#ifndef VIDEOWORKER_H
#define VIDEOWORKER_H

#include <QObject>
#include <QImage>
#include <QThread>
#include <QDebug>
#include <QMutex>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

class VideoWorker : public QObject
{
    Q_OBJECT
public:
    explicit VideoWorker(QObject *parent = nullptr)
        : fmtCtx(nullptr), video_context(nullptr), scale_context(nullptr){
        memset(&convert_frame, 0, sizeof(AVFrame));
    }
    void setContext(AVFormatContext *a, AVCodecContext* b, int index, int w, int h, int den, int num){
        fmtCtx = a;
        video_context = b;
        vidx = index;
        width = w;
        height = h;
        image = QImage(width, height, QImage::Format_RGB32);
        delay = (1000.0 * den / num);
        // qDebug() << delay << Qt::endl;
    }
    enum PlayStatus{PLAY, STOP, END};
    PlayStatus status = STOP;

signals:
    void frameReady(uchar* orig);
public slots:
    void ButtonEvent();
    void run();
private:
    AVFormatContext *fmtCtx;
    AVCodecContext *video_context;
    int vidx;
    QImage image;
    SwsContext* scale_context;
    int width, height;
    int delay;
    void usleep(int s);
    AVFrame convert_frame;
};

#endif
