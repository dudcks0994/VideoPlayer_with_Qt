#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H

#include <QObject>
#include <QPainter>

class VideoRenderer : public QObject
{
    Q_OBJECT
public:
    explicit VideoRenderer(QObject *parent = nullptr);
    void SetPainter(QPainter *p, int msec, int w, int h, int format);

signals:
    void DoneImageSet();
public slots:
    ;


private:
    QPainter *painter;
    int delay;
    int width, height;
};

#endif // VIDEORENDERER_H
