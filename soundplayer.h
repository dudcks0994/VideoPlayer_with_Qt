#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

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


class SoundPlayer : public QObject
{
    Q_OBJECT
public:
    explicit SoundPlayer(QObject *parent = nullptr);

public slots:
    void Play();

private:
    HWAVEOUT hWaveDev;
    WAVEFORMATEX wf;
    AudioPool* audio_pool;
signals:
};

#endif // SOUNDPLAYER_H
