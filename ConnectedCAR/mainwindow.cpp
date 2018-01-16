#include "mainwindow.h"
#include "ui_mainwindow.h"

#define _FRAME_WIDTH            640
#define _FRAME_HEIGHT           480
#define _FRAME_PER_SECOND       90
#define _FRAME_WIDTH_SMALL      320
#define _FRAME_HEIGHT_SMALL     240

#define _SIGNAL_NOMAL           0
#define _SIGNAL_LEFT            1
#define _SIGNAL_RIGHT           2

#define _CAN_RPM                0
#define _CAN_GEAR               1
#define _CAN_BREAK              2
#define _CAN_ACCEL_PEDAL        3
#define _CAN_SPEED              4
#define _CAN_THIROTTLE          5

#define _TIMER_MS               20


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    driving_signal_flag = _NOMAL_SIGNAL;
    pre_signal = _NOMAL_SIGNAL;
    can_Info[_CAN_RPM] = "RPM: 1700";
    can_Info[_CAN_GEAR] = "GEAR: D";
    can_Info[_CAN_BREAK] = "BREAK: Break OFF";
    can_Info[_CAN_ACCEL_PEDAL] = "ACCEL PEDAL: 000";
    can_Info[_CAN_SPEED] = "SPEED: 52";
    can_Info[_CAN_THIROTTLE] = "THIROTTLE: 19";

    wiringPiSetup(); //Rraspi pin for switching multi camera

    initCamera();
    outputImg = Mat(570,800,CV_8UC3,Scalar(255,255,255));
    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onTimer()));
    m_pTimer->start(_TIMER_MS);
}

MainWindow::~MainWindow()
{
    if (m_pTimer->isActive())
        m_pTimer->stop();

    delete ui;
}

void MainWindow::onTimer()
{
    static int nCount=0;

    multiCapture(nCount+1);
    if(driving_signal_flag != pre_signal)
    {
        outputImg = Mat(570,800,CV_8UC3,Scalar(255,255,255));
    }
    //driving_signal_flag = _SIGNAL_LEFT;
    if(nCount == 2)
    {
        // Mat imageROI4 = inputImg1[2](cv::Rect(0,80,640,320));
        switch(driving_signal_flag)
        {
        case _SIGNAL_NOMAL:
            roi_x[0] = 80; roi_y[0] = 10; roi_width[0] = _FRAME_WIDTH_SMALL; roi_height[0]=_FRAME_HEIGHT_SMALL;
            roi_x[1] = 400; roi_y[1] = 10; roi_width[1] = _FRAME_WIDTH_SMALL; roi_height[1]=_FRAME_HEIGHT_SMALL;
            roi_x[2] = 80; roi_y[2] = 250; roi_width[2] = _FRAME_WIDTH; roi_height[2]=320;

            cv::resize(inputImg1[0],inputImg1[0],Size(_FRAME_WIDTH_SMALL,_FRAME_HEIGHT_SMALL),0,0,INTER_AREA);
            cv::resize(inputImg1[1],inputImg1[1],Size(_FRAME_WIDTH_SMALL,_FRAME_HEIGHT_SMALL),0,0,INTER_AREA);
            break;
        case _SIGNAL_LEFT:

            roi_x[0] = 80; roi_y[0] = 10; roi_width[0] = _FRAME_WIDTH; roi_height[0]=_FRAME_HEIGHT-80;
            roi_x[1] = 520; roi_y[1] = 410; roi_width[1] = 200; roi_height[1]=160;
            roi_x[2] = 200; roi_y[2] = 410; roi_width[2] = 320; roi_height[2]=160;

            cv::resize(inputImg1[0],inputImg1[0],Size(roi_width[0],roi_height[0]),0,0,INTER_AREA);
            cv::resize(inputImg1[1],inputImg1[1],Size(roi_width[1],roi_height[1]),0,0,INTER_AREA);
            //cv::resize(imageROI4,imageROI4,Size(roi_width[2],roi_height[2]),0,0,INTER_AREA);
            cv::resize(inputImg1[2],inputImg1[2],Size(roi_width[2],roi_height[2]),0,0,INTER_AREA);



            break;
        case _SIGNAL_RIGHT:
            roi_x[0] = 80; roi_y[0] = 410; roi_width[0] = 200; roi_height[0]=160;
            roi_x[1] = 80; roi_y[1] = 10; roi_width[1] = _FRAME_WIDTH; roi_height[1]=_FRAME_HEIGHT-80;
            roi_x[2] = 280; roi_y[2] = 410; roi_width[2] = 320; roi_height[2]=160;


            cv::resize(inputImg1[0],inputImg1[0],Size(roi_width[0],roi_height[0]),0,0,INTER_AREA);
            cv::resize(inputImg1[1],inputImg1[1],Size(roi_width[1],roi_height[1]),0,0,INTER_AREA);
            cv::resize(inputImg1[2],inputImg1[2],Size(roi_width[2],roi_height[2]),0,0,INTER_AREA);
            break;
        default:
            break;
        }


        Mat imageROI1 = outputImg(cv::Rect(roi_x[0],roi_y[0],roi_width[0],roi_height[0]));
        Mat imageROI2 = outputImg(cv::Rect(roi_x[1],roi_y[1],roi_width[1],roi_height[1]));
        Mat imageROI3 = outputImg(cv::Rect(roi_x[2],roi_y[2],roi_width[2],roi_height[2]));




        inputImg1[0].copyTo(imageROI1,inputImg1[0]);
        inputImg1[1].copyTo(imageROI2,inputImg1[1]);
        inputImg1[2].copyTo(imageROI3,inputImg1[2]);

        if(driving_signal_flag == _NOMAL_SIGNAL)
        {
            cv::Point txtPoint;
            for(int i = 0 ; i < 6 ; i++)
            {
                txtPoint.x = 90;
                txtPoint.y = 270+i*15;
                cv::putText(outputImg,can_Info[i],txtPoint,FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0));
            }
        }

        QImage image1= QImage((uchar*) outputImg.data,outputImg.cols, outputImg.rows, outputImg.step, QImage::Format_RGB888);
        //  QImage image1= QImage((uchar*) inputImg[1].data,inputImg[1].cols, inputImg[1].rows,inputImg[1].step, QImage::Format_RGB888);
        ui->SCREEN_VIEW->setPixmap(QPixmap::fromImage(image1));
        pre_signal = driving_signal_flag;


    }

    if(nCount<2)
        nCount++;
    else
        nCount=0;
}

void MainWindow::multiCapture(u_int8_t index)
{
    //13 , 7
    uint8_t gpio_delay = 12;
    uint8_t cap_delay = 7;
    if(index == 1)
    {

        if(cap.grab())
        {
            cap.grab();
            cap.retrieve(inputImg[0]);
            usleep(1000*cap_delay);
            cvtColor(inputImg[0],inputImg1[0],CV_BGR2RGB);
        }
        setCameraPort(1,gpio_delay);

        // ui->SCREEN_VIEW->setScaledContents( true );
        // ui->SCREEN_VIEW->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    }
    else if(index == 2)
    {
        if(cap.grab())
        {
            cap.grab();
            cap.retrieve(inputImg[1]);
            usleep(1000*cap_delay);
            cvtColor(inputImg[1],inputImg1[1],CV_BGR2RGB);
        }
        setCameraPort(2,gpio_delay);

    }
    else if(index == 3)
    {

        if(cap.grab())
        {
            cap.grab();
            cap.retrieve(inputImg[2]);
            usleep(1000*cap_delay);
            Mat temp;
            cvtColor(inputImg[2],temp,CV_BGR2RGB);
            inputImg1[2] = temp(cv::Rect(0,80,640,320)).clone();
        }
        setCameraPort(3,gpio_delay);

    }
    else if(index == 4)
    {
 //        setCameraPort(4,22);
        cap >> inputImg[3];
        cvtColor(inputImg[3], inputImg1[3],CV_BGR2GRAY);
        setCameraPort(4,0);

    }
}

void MainWindow::initCamera(void)
{
    pinMode(7,OUTPUT);   //Set PIN MODE
    pinMode(0,OUTPUT);   //this pin can enable to use camera module stack1
    pinMode(1,OUTPUT);

    setCameraPort(3,100);
    cap.open(0);
    cap.set(CV_CAP_PROP_FRAME_WIDTH , _FRAME_WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT , _FRAME_HEIGHT);
    cap.set(CV_CAP_PROP_FPS,_FRAME_PER_SECOND);
    cap.set(CV_CAP_PROP_EXPOSURE,0.5);
//    cap.release();
    //cap.set(CV_CAP_PROP_GAIN,0.1);
    usleep(1000 * 500);

/*
    setCameraPort(2,15);
    cap.open(0);
    cap.set(CV_CAP_PROP_FRAME_WIDTH , _FRAME_WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT , _FRAME_HEIGHT);
    cap.set(CV_CAP_PROP_FPS,_FRAME_PER_RATE);
    cap.set(CV_CAP_PROP_EXPOSURE,0.4);
    //cap.set(CV_CAP_PROP_GAIN,0.1);
    cap.release();

    usleep(1000* 500);

    setCameraPort(1,15);
    cap.open(0);
    cap.set(CV_CAP_PROP_FRAME_WIDTH , _FRAME_WIDTH);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT , _FRAME_HEIGHT);
    cap.set(CV_CAP_PROP_FPS , _FRAME_PER_RATE);
    cap.set(CV_CAP_PROP_EXPOSURE , 0.4);


    //cap.set(CV_CAP_PROP_GAIN,0.1);

    usleep(1000* 500);
    */
}

void MainWindow::setCameraPort(u_int8_t cam_index,u_int8_t pin_msdelay)    //switch 3 camera
{
    switch(cam_index)
    {
    case 1:

        digitalWrite (7,0) ; // off    turn on Camera A
        digitalWrite (0, 0) ; // off
        digitalWrite (1, 1) ; // on
        usleep(pin_msdelay*1000);
        break;
    case 2:

        digitalWrite (7,1) ; // off    turn on Camera B
        digitalWrite (0, 0) ; // off
        digitalWrite (1, 1) ; // on
        usleep(pin_msdelay*1000);
        break;
    case 3:
        digitalWrite (7,0) ; // off    turn on Camera C
        digitalWrite (0, 1) ; // off
        digitalWrite (1, 0) ; // on
        usleep(pin_msdelay*1000);
        break;
    case 4:

        digitalWrite (7,1) ; // off    turn on Camera D
        digitalWrite (0, 1) ; // off
        digitalWrite (1, 0) ; // on
        usleep(pin_msdelay*1000);
        break;
    default:
        break;
    }
    
}



void MainWindow::on_pushButton_clicked()
{
    int signal_flag = ui->lineEdit->text().toInt();
    driving_signal_flag = signal_flag;
}
