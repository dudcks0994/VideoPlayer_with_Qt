#ifndef VIDEOCONVERTER_H
#define VIDEOCONVERTER_H

#include <QObject>
#include <QMutex>
#include <queue>
#include "demuxer.h"

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
    explicit VideoConverter(QObject *parent = nullptr, uint8_t id = 0);

public slots:
    void Convert();
    void NeedClean();

signals:
    void ImageSetReady();
private:
    SwsContext *sctx;
    uint8_t* buffer;
    uint8_t thread_id;
    Pool* video_pool;
    Resolution res;
    int wanted_format;
};

#endif // VIDEOCONVERTER_H
