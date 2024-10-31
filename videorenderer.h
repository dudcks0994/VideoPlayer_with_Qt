#ifndef VIDEORENDERER_H
#define VIDEORENDERER_H

#include <QObject>
#include <QPainter>

class VideoRenderer : public QObject
{
    Q_OBJECT
public:
    explicit VideoRenderer(QObject *parent = nullptr);
    void SetPainter(QPainter *p, int w, int h);
    void Drawing();

signals:
    void DoneImageSet();
public slots:
    void ReceiveImageSet();


private:
    int is_ready;
    int cur_set;
    QPainter *painter;
    int delay;
    int width, height;
};

#endif // VIDEORENDERER_H
