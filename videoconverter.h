#ifndef VIDEOCONVERTER_H
#define VIDEOCONVERTER_H

#include <QObject>
#include <QMutex>
#include <queue>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

class FrameMaker;

class VideoConverter : public QObject
{
    Q_OBJECT
public:
    explicit VideoConverter(QObject *parent = nullptr);
    void Set(int w, int h, int format, FrameMaker *f);

public slots:
    void Convert();
    void NeedClean();

signals:
    void ImageSetReady(uchar **p);
private:
    std::queue<AVFrame *> *video_frame_q;
    QMutex* video_frame_mutex;
    SwsContext *sctx;
    uint8_t* buffer;
    int width, height;
    int wanted_format;
    int is_sent;
};

#endif // VIDEOCONVERTER_H
