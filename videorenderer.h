#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H


#define _HAS_STD_BYTE 0
#include <QObject>
#include <QPainter>
#include "datainfo.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
struct Pool;

class VideoRenderer : public QObject
{
    Q_OBJECT
public:
    explicit VideoRenderer(QObject *parent = nullptr);

signals:
    void OnImageReady();
public slots:
    void Rendering();


private:
    void usleep(int msec);
    Pool* video_pool;
    QImage* image;
    int delay_msec;
    Resolution res;
};

#endif // VIDEORENDERER_H
