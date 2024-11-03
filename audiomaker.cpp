#include "audiomaker.h"
#include "objectfactory.h"
#include "demuxer.h"

AudioMaker::AudioMaker(QObject *parent)
    : QObject{parent}
{
    read_buffer = (char *)malloc(PACKET_SIZE);
    send_buffer = (char *)malloc(BUFFER_SIZE);
    Demuxer* demuxer = ObjectFactory::GetDemuxer();
    audio_packet = demuxer->GetaudioPacket();
    audio_pool = demuxer->GetAudioPool();
    MainWindow* window = ObjectFactory::GetMainWindow();
    actx = window->GetActx();
    sctx = 0;
}

void AudioMaker::Convert()
{
    int packet_index = 0;
    int pool_index = 0;
    AVFrame* frame = av_frame_alloc();
    char *send_location = send_buffer;
    char *read_location = read_buffer;
    while(1)
    {
        if (audio_packet[packet_index].status != 1)
        {
            Sleep(15);
            continue;
        }
        int ret = avcodec_send_packet(actx, audio_packet[packet_index].packet);
        if (ret != 0)
            break; // 사실상 코덱이 flush되든 더이상 불가능
        av_packet_unref(audio_packet[packet_index].packet);
        audio_packet[packet_index].status = 0;
        ret = avcodec_receive_frame(actx, frame);
        if (ret != 0)
        {
            if (ret == AVERROR_EOF)
            {
                qDebug() << "EOF from audio frame";
                return ;
            }
            else
            {
                if (++packet_index == 60)
                    packet_index = 0;
                continue;
            }
        }
        if (!sctx)
        {
            int srate = frame->sample_rate;
            av_channel_layout_default(&out, 2);
            out_format = (AVSampleFormat)frame->format;
            int ret = swr_alloc_set_opts2(&sctx, &out, AV_SAMPLE_FMT_S16, 44100, &frame->ch_layout, out_format, srate, 0, NULL);
            if (ret != 0)
            {
                qDebug() << "error from swr alloc";
                return ;
            }
            ret = swr_init(sctx);
            if (ret != 0)
            {
                qDebug() << "error from swr init";
                return ;
            }
        }
        int sample_num = 0;
        sample_num = swr_convert(sctx, (uint8_t **)(&read_buffer), PACKET_SIZE / 4, frame->extended_data, frame->nb_samples);
        if (sample_num < 0)
        {
            char *tmp = (char *)malloc(1000);
            av_strerror(sample_num, tmp, 1000);
            qDebug() << "error from swr_convert" << sample_num;
            return ;
        }
        int read_bytes = sample_num * 4;
        // qDebug() << "read bytes is " << read_bytes;
        read_location = read_buffer;
        for (;;)
        {
            int remain_buffer_size = BUFFER_SIZE - (send_location - send_buffer);
            // qDebug() << "remain buffer size is " << remain_buffer_size;
            if (read_bytes <= remain_buffer_size)
            {
                memcpy(send_location, read_location, read_bytes);
                send_location += read_bytes;
                break;
            }
            memcpy(send_location, read_location, remain_buffer_size);
            read_bytes -= remain_buffer_size;
            read_location += remain_buffer_size;
            while (audio_pool[pool_index].status != 0)
            {
                // qDebug() << "wait for sound play... " << pool_index;
                Sleep(10);
            }
            audio_pool[pool_index].hdr.lpData = send_buffer;
            audio_pool[pool_index].hdr.dwBufferLength = BUFFER_SIZE;
            audio_pool[pool_index].status = 1;
            // qDebug() << "made 1 audio pool";
            send_buffer = (char *)malloc(BUFFER_SIZE);
            send_location = send_buffer;
            if (++pool_index == 10)
                pool_index = 0;
        }
        if(++packet_index == 60)
            packet_index = 0;
    }
}
