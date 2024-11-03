#include <windows.h>
#include <realtimeapiset.h>
#include "soundplayer.h"
#include "objectfactory.h"
#include "demuxer.h"

SoundPlayer::SoundPlayer(QObject *parent)
    : QObject{parent}
{
    wf.cbSize = sizeof(WAVEFORMATEX);
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.nSamplesPerSec = 44100;
    wf.wBitsPerSample = 16;
    wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
    waveOutOpen(&hWaveDev, WAVE_MAPPER, &wf, 0, 0, 0);
    Demuxer *demuxer = ObjectFactory::GetDemuxer();
    audio_pool = demuxer->GetAudioPool();
}

static void uusleep(int msec, LARGE_INTEGER frequency)
{
    LARGE_INTEGER start, cur;
    int gap;
    QueryPerformanceCounter(&start);
    QueryPerformanceCounter(&cur);
    gap = (cur.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;

    while (gap < msec)
    {
        if (gap < msec / 8)
            Sleep(7 * msec / 8);
        else if (gap < 2 * msec / 8)
            Sleep(6 * msec / 8);
        else if (gap < 3 * msec / 8)
            Sleep(5 * msec / 8);
        else if (gap < 4 * msec / 8)
            Sleep(4 * msec / 8);
        else if (gap < 5 * msec / 8)
            Sleep(3 * msec / 8);
        else if (gap < 6 * msec / 8)
            Sleep(2 * msec / 8);
        else if (gap < 7 * msec / 8)
            Sleep(msec / 8);
        else
            Sleep(1);
        QueryPerformanceCounter(&cur);
        gap = (cur.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
    }
    // qDebug() << "end for sleep";
};

void SoundPlayer::Play()
{
    int pool_index = 0;
    int delay = 100;
    LARGE_INTEGER before, cur, frequency, nextPlayTime;
    QueryPerformanceFrequency(&frequency);
    before.QuadPart = 0;
    QueryPerformanceCounter(&nextPlayTime);
    nextPlayTime.QuadPart += frequency.QuadPart * 100 / 1000;
    while (1)
    {
        if (audio_pool[pool_index].status != 1)
        {
            Sleep(15);
            continue;
        }
        uint ret = waveOutPrepareHeader(hWaveDev, &(audio_pool[pool_index].hdr), sizeof(WAVEHDR));
        waveOutWrite(hWaveDev, &(audio_pool[pool_index].hdr), sizeof(WAVEHDR));
        QueryPerformanceCounter(&cur);
        if (before.QuadPart != 0)
        {
            double gap = (cur.QuadPart - nextPlayTime.QuadPart) * 1000.0 / frequency.QuadPart;
            nextPlayTime.QuadPart += frequency.QuadPart * 100 / 1000;
            delay = 100 - gap;
            if (delay < 0)
                delay = 0;
        }
        before = cur;
        uusleep(delay, frequency);
        free(audio_pool[pool_index].hdr.lpData);
        audio_pool[pool_index].status = 0;
        if (++pool_index == MAX_POOL)
            pool_index = 0;
    }
}
