#include "mainwindow.h"
#include "demuxer.h"
#include "objectfactory.h"
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
    MainWindow *window;
    window = ObjectFactory::GetMainWindow();
    QString file = QFileDialog::getOpenFileName(window, "Open Video File", "", "Video Files (*.mp4 *.avi *.mkv *.m3u8)");
    QThread *demuxer_thread = new QThread;
    Demuxer *demuxer = ObjectFactory::GetDemuxer(file);
    QObject::connect(demuxer_thread, SIGNAL(started()), demuxer, SLOT(Demuxing()));
    // window->init_video(demuxer);
    window->setWindowTitle("FT 영상 플레이어");
    demuxer->moveToThread(demuxer_thread);
    demuxer_thread->start();
    window->show();
    window->Rendering();
    return a.exec();
}
