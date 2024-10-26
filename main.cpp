#include "mainwindow.h"
#include "demuxer.h"
#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QPainter>
#include <QImage>
#include <QThread>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow window;
    QThread *demuxer_thread = new QThread;
    Demuxer *demuxer = new Demuxer;
    window.init_video(demuxer);
    QObject::connect(demuxer_thread, SIGNAL(started()), demuxer, SLOT(Demuxing()));
    window.resize(window.width + 40, window.height + 40);
    window.setWindowTitle("FT 영상 플레이어");
    demuxer->moveToThread(demuxer_thread);
    demuxer_thread->start();
    window.show();
    return a.exec();
}
