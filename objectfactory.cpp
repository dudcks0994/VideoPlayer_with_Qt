#include "objectfactory.h"
#include "demuxer.h"
#include "framemaker.h"
#include "imagemaker.h"
#include "videoconverter.h"
#include "videorenderer.h"

Demuxer* ObjectFactory::demuxer = nullptr;
MainWindow* ObjectFactory::mainWindow = nullptr;
QMutex* ObjectFactory::m = new QMutex();

ObjectFactory::ObjectFactory(QObject *parent)
    : QObject{parent}
{}

Demuxer* ObjectFactory::GetDemuxer(const QString& file)
{
    if (demuxer == 0)
        demuxer = new Demuxer(0, file);
    return (demuxer);
}

MainWindow* ObjectFactory::GetMainWindow()
{
    if (mainWindow == 0)
        mainWindow = new MainWindow();
    return (mainWindow);
}
