#include "videorenderer.h"

VideoRenderer::VideoRenderer(QObject *parent)
    : QObject{parent}
{}

void VideoRenderer::SetPainter(QPainter *p, int msec)
{
    painter = p;
}
