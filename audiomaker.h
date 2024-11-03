#ifndef AUDIOMAKER_H
#define AUDIOMAKER_H

#include <windows.h>
#include <QObject>
#include "datainfo.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#define BUFFER_SIZE 17640
#define PACKET_SIZE 20000

class AudioMaker : public QObject
{
    Q_OBJECT
public:
    explicit AudioMaker(QObject *parent = nullptr);
public slots:
    void Convert();
private:
    AVCodecContext* actx;
    SwrContext* sctx;

    AVChannelLayout out;
    AVSampleFormat out_format;
    PacketBox* audio_packet;
    AudioPool *audio_pool;
    char *read_buffer;
    char *send_buffer;
signals:
};

#endif // AUDIOMAKER_H
