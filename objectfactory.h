#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#include <QObject>
#include <QMutex>
#include "mainwindow.h"

class Demuxer;
class MainWindow;

class ObjectFactory : public QObject
{
    Q_OBJECT
public:
    explicit ObjectFactory(QObject *parent = nullptr);
    static Demuxer* GetDemuxer(const QString& file = "");
    static MainWindow* GetMainWindow();
    static Demuxer* demuxer;
    static MainWindow* mainWindow;
    static QMutex *m;
signals:

private:

};

#endif // OBJECTFACTORY_H
