#include "videorenderer.h"

VideoRenderer::VideoRenderer(QObject *parent)
    : QObject{parent}
{
    cur_set = 0;
    is_ready = 0;
}

void VideoRenderer::SetPainter(QPainter *p, int w, int h)
{
    painter = p;
    width = w;
    height = h;
}

void VideoRenderer::ReceiveImageSet()
{
    // cur_set = (cur_set == 0) ? 1 : 0;
    is_ready = 1;
}

void VideoRenderer::Drawing()
{
}

