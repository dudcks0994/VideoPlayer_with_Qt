#include <windows.h>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "objectfactory.h"
#include <qpushbutton.h>
#include <QPainter>
#include <QImage>
#include <QMutex>
#include <QSlider>
#include <QScreen>

#pragma comment(lib, "winmm.lib")

void MainWindow::usleep(int msec)
{
    u_int64 start, cur, gap;
    typedef VOID (WINAPI *FuncQueryInterruptTime)(_Out_ PULONGLONG lpInterruptTime);
    HMODULE hModule = ::LoadLibrary(L"KernelBase.dll");
    FuncQueryInterruptTime func_QueryInterruptTime =
        (FuncQueryInterruptTime)::GetProcAddress(hModule, "QueryInterruptTime");
    func_QueryInterruptTime(&start);
    do{
        func_QueryInterruptTime(&cur);
        gap = (cur - start) / 10000.0;
        if (gap < msec / 2)
            Sleep(msec / 2);
    }while(gap < msec);
}

void MainWindow::onFrameReady(uchar* orig)
{
    // qDebug() << "frame is ready in main!\n";
    uchar* dst = image->bits();
    memcpy(dst, orig, width * height * 4);
    update();
}

void MainWindow::SetPixmap(Pool* p, AVRational rate)
{
    video_pool = p;
    image = new QImage(width, height, QImage::Format_RGB32);
    delay_msec = 1000.0 * 1.0 / (rate.den / rate.num);
}

void MainWindow::SetResolution(int w, int h)
{
    width = w;
    height = h;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    playButton = new QPushButton("Play", this);
    playButton->setGeometry(10, 10, 50, 30);
    image = 0;
}

void MainWindow::Rendering()
{
    int index = 0;
    video_pool = ObjectFactory::GetDemuxer()->GetVideoPool();
    image = new QImage(width, height, QImage::Format_RGB32);
    qDebug() << "start to rendering..." << video_pool;
    while (1)
    {
        qDebug() << "???";
        if (video_pool[index].status != 2)
        {
            Sleep(8);
            continue;
        }
        qDebug() << "before " << video_pool[index].img[1210];
        memcpy(image->bits(), video_pool[index].img, width * height * 4);
        qDebug() << "after";
        qDebug() << image->bits()[0] << " is first bit";
        // update();
        usleep(500);
        // free(video_pool[index].img);
        video_pool[index].status = 0;
        if (++index == 6)
            index = 0;
    }
}

MainWindow::~MainWindow() // should add signal, so when it called, call other class and thread to delete resource.
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawImage(10, 40, *image);
}
