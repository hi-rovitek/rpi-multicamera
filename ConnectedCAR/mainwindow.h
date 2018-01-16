#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <opencv2/opencv.hpp>
#include <wiringPi.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>

using namespace cv;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTimer *m_pTimer;

    VideoCapture cap;

    Mat inputImg[4];
    Mat inputImg1[4];
    Mat outputImg;

    u_int8_t driving_signal_flag;
    u_int8_t pre_signal;

    String can_Info[6];
    String can_Info_txt[6];


    int roi_x[4],roi_y[4],roi_width[4],roi_height[4];

    void multiCapture(u_int8_t index);
    void initCamera(void);
    void setCameraPort(u_int8_t cam_index,u_int8_t pin_msdelay);


private slots:
    void onTimer();
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
