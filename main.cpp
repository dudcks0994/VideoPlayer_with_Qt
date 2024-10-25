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
    window.resize(window.width + 40, window.height + 40);
    window.setWindowTitle("FT 영상 플레이어");
    window.show();
    return a.exec();
}
