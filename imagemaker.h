#ifndef IMAGEMAKER_H
#define IMAGEMAKER_H

#include <QObject>

class ImageMaker : public QObject
{
    Q_OBJECT
public:
    explicit ImageMaker(QObject *parent = nullptr);

private:
    int width, height;
    int wanted_format;
    QImage* image_set[2][60];
};

#endif // IMAGEMAKER_H
