#include "videowidget.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QSlider>
#include <QSpinBox>

#include <QFileDialog>
#include <QInputDialog>

#include <vlc-qt/Common.h>

#include <QtWidgets>
#include <qvideowidget.h>
#include <qvideosurfaceformat.h>


#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;



VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
    , mediaPlayer(0, QMediaPlayer::VideoSurface)
    , playButton(0)
    , positionSlider(0)
    , errorLabel(0)

{




    QVideoWidget *videoWidget = new QVideoWidget;
    QAbstractButton *openButton = new QPushButton(tr("Open..."));
    connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));


    playButton = new QPushButton;
    playButton->setEnabled(false);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(playButton, SIGNAL(clicked()),
            this, SLOT(play()));

    positionSlider = new QSlider(Qt::Horizontal);
    positionSlider->setRange(0, 0);

    connect(positionSlider, SIGNAL(sliderMoved(int)),
            this, SLOT(setPosition(int)));

    errorLabel = new QLabel;
    errorLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    QAbstractButton *videostream = new QPushButton(tr("Get Video"));
    connect(videostream, SIGNAL(clicked()), this, SLOT(take_video()));


    QAbstractButton *trackingButton = new QPushButton(tr("Track"));
    connect(trackingButton, SIGNAL(clicked()), this, SLOT(tracking_status()));

    QAbstractButton *Actuation = new QPushButton(tr("Actuation Monitoring"));
    connect(Actuation, SIGNAL(clicked()), this, SLOT(actuation_monitoring()));

    QAbstractButton *Lighting = new QPushButton(tr("Lighting Monitoring"));
    connect(Lighting, SIGNAL(clicked()), this, SLOT(lighting_intensity()));

    QAbstractButton *streaming = new QPushButton(tr("Stream"));
    connect(streaming, SIGNAL(clicked()), this, SLOT(get_video()));

//    connect(ui->actionOpenUrl, SIGNAL(triggered()), this, SLOT(openURL()));

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
//    controlLayout->addWidget(openButton);
//    controlLayout->addWidget(playButton);
//    controlLayout->addWidget(positionSlider);
    controlLayout->addWidget(videostream);
//    controlLayout->addWidget(streaming);
    controlLayout->addWidget(trackingButton);
    controlLayout->addWidget(Actuation);
    controlLayout->addWidget(Lighting);


    QBoxLayout *layout = new QVBoxLayout;
//    layout->addWidget(videoWidget);
//    layout->addLayout(_player);
    layout->addLayout(controlLayout);
    layout->addWidget(errorLabel);

    setLayout(layout);

    mediaPlayer.setVideoOutput(videoWidget);
    connect(&mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(mediaStateChanged(QMediaPlayer::State)));
    connect(&mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(&mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(&mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));

    boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    Client = new TrackingClient(protocol);
    transport->open();

}

VideoPlayer::~VideoPlayer()
{
}

void VideoPlayer::openFile()
{
    errorLabel->setText("");

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"),QDir::homePath());

    if (!fileName.isEmpty()) {
        mediaPlayer.setMedia(QUrl::fromLocalFile(fileName));
        playButton->setEnabled(true);
    }
}


void VideoPlayer::play()
{
    switch(mediaPlayer.state()) {
    case QMediaPlayer::PlayingState:
        mediaPlayer.pause();
        break;
    default:
        mediaPlayer.play();
        break;
    }
}

void VideoPlayer::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state) {
    case QMediaPlayer::PlayingState:
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void VideoPlayer::positionChanged(qint64 position)
{
    positionSlider->setValue(position);
}

void VideoPlayer::durationChanged(qint64 duration)
{
    positionSlider->setRange(0, duration);
}

void VideoPlayer::setPosition(int position)
{
    mediaPlayer.setPosition(position);
}

void VideoPlayer::handleError()
{
    playButton->setEnabled(false);
    errorLabel->setText("Error: " + mediaPlayer.errorString());
}

// Actuation and Lighting Controls

void VideoPlayer::actuation_monitoring()
{
    QWidget*actuation = new QWidget;
    actuation->setWindowTitle("Actuation Manual Control");

    QSpinBox *spinner = new QSpinBox;
    QSlider *slider = new QSlider(Qt::Horizontal);
    QLabel *label = new QLabel;
    label->setText("panning X");
    spinner->setRange(0,10000);
    slider->setRange(0,10000);

    QObject::connect(spinner, SIGNAL(valueChanged(int)),
                     slider, SLOT(setValue(int)));
    QObject::connect(slider, SIGNAL(valueChanged(int)),
                     spinner, SLOT(setValue(int)));
    spinner->setValue(0);

    QSpinBox *spinner0 = new QSpinBox;
    QSlider *slider0 = new QSlider(Qt::Horizontal);
    QLabel *label0 = new QLabel;
    label0->setText("tilting Y");
    spinner0->setRange(0,10000);
    slider0->setRange(0,10000);

    QObject::connect(spinner0, SIGNAL(valueChanged(int)),
                     slider0, SLOT(setValue(int)));
    QObject::connect(slider0, SIGNAL(valueChanged(int)),
                     spinner0, SLOT(setValue(int)));
    spinner0->setValue(0);

    //Get actual position

    QAbstractButton *get_actual_position = new QPushButton(tr("Get Position"));
    QObject::connect(get_actual_position,SIGNAL(clicked()), SLOT(getActualPos()));






    QObject::connect(spinner,SIGNAL(valueChanged(int)), SLOT(setPos_X(int)));
    QObject::connect(slider,SIGNAL(valueChanged(int)), SLOT(setPos_X(int)));

    QObject::connect(spinner0,SIGNAL(valueChanged(int)), SLOT(setPos_Y(int)));
    QObject::connect(slider0,SIGNAL(valueChanged(int)), SLOT(setPos_Y(int)));


    //halt
    QAbstractButton *haltbutton = new QPushButton(tr("Halt"));
    QObject::connect(haltbutton, SIGNAL (clicked()), SLOT(halt()));

    //sleep
    QAbstractButton *sleepbutton = new QPushButton(tr("Sleep"));
    QObject::connect(sleepbutton, SIGNAL (clicked()), SLOT(sleep()));

    //wake
    QAbstractButton *wakebutton = new QPushButton(tr("Wake"));
    QObject::connect(wakebutton, SIGNAL (clicked()), SLOT(wake()));

    //calibration
    QAbstractButton *calibration = new QPushButton(tr("Calibrate"));
    connect(calibration, SIGNAL(clicked()), SLOT(calibrate()));

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(calibration);
    layout->addWidget(haltbutton);
    layout->addWidget(sleepbutton);
    layout->addWidget(wakebutton);
    layout->addWidget(get_actual_position);

//    layout->addWidget(slider);
//    layout->addWidget(spinner);
//    layout->addWidget(label);
//    layout->addWidget(slider0);
//    layout->addWidget(spinner0);
//    layout->addWidget(label0);

    actuation->setLayout(layout);

    actuation->resize(320,240);

    actuation->show();

}

void VideoPlayer::lighting_intensity()
{
    QWidget*lighting = new QWidget;
    lighting->setWindowTitle("Lighting Control");

    QLabel *label1 = new QLabel;
    label1->setText("Percentage set");

    QSpinBox *spinner1 = new QSpinBox;
    QSlider *slider1 = new QSlider(Qt::Horizontal);
    spinner1->setRange(0,100);
    slider1->setRange(0,100);


    QObject::connect(spinner1, SIGNAL(valueChanged(int)),
                     slider1, SLOT(setValue(int)));
    QObject::connect(slider1, SIGNAL(valueChanged(int)),
                     spinner1, SLOT(setValue(int)));

    QObject::connect(spinner1,SIGNAL(valueChanged(int)), SLOT(setIntensity(const int)));
    QObject::connect(slider1,SIGNAL(valueChanged(int)), SLOT(setIntensity(const int)));


    spinner1->setValue(100);


    QAbstractButton *light_toggle = new QPushButton(tr("Lighting Control"));

    QObject::connect(light_toggle, SIGNAL(clicked()), SLOT (lighting_toggle()));

    QLabel *label2 = new QLabel;
    label2->setText("On/Off Switch");

    QAbstractButton *setOff = new QPushButton(tr("OFF"));
    QObject::connect(setOff, SIGNAL(clicked()), SLOT(set_off()));

    QAbstractButton *setOn = new QPushButton(tr("ON"));
    QObject::connect(setOn, SIGNAL(clicked()), SLOT(set_on()));

    QVBoxLayout *layout1 = new QVBoxLayout;
//    layout1->addWidget(light_toggle);
    layout1->addWidget(label2);
    layout1->addWidget(setOff);
    layout1->addWidget(setOn);
    layout1->addWidget(slider1);
    layout1->addWidget(spinner1);
    layout1->addWidget(label1);
    lighting->setLayout(layout1);
    lighting->resize(320,240);
    lighting->show();

}

void VideoPlayer::lighting_toggle(){
//    QWidget *toggle = new QWidget;
//    toggle->setWindowTitle("Lighting Toggle");

//    QAbstractButton *setOff = new QPushButton(tr("OFF"));
//    QObject::connect(setOff, SIGNAL(clicked()), SLOT(set_off()));

//    QAbstractButton *setOn = new QPushButton(tr("ON"));
//    QObject::connect(setOn, SIGNAL(clicked()), SLOT(set_on()));
//    QSlider *on_off_slider = new QSlider;
//    on_off_slider->setRange(0,1);
//    QSpinBox *on_off_spinner = new QSpinBox;
//    on_off_spinner->setRange(0,1);

//    QObject::connect(on_off_slider, SIGNAL(valueChanged(int)),
//                     on_off_spinner, SLOT(setValue(int)));
//    QObject::connect(on_off_spinner, SIGNAL(valueChanged(int)),
//                     on_off_slider, SLOT(setValue(int)));
//    on_off_spinner->setValue(0);

//    QBoxLayout *layout3 = new QVBoxLayout;
//    layout3->addWidget(on_off_slider);
//    layout3->addWidget(on_off_spinner);
//    layout3->addWidget(setOn);
//    layout3->addWidget(setOff);
//    toggle->setLayout(layout3);
//    toggle->show();


}

void VideoPlayer::tracking_status(){
    QWidget *tracking_board = new QWidget;
    tracking_board->setWindowTitle("Tracking Status");



    QAbstractButton *set_auto_mode = new QPushButton(tr("Automatic"));
    QObject::connect(set_auto_mode,SIGNAL (clicked()), SLOT(set_auto_Mode()));// to be continued

    QAbstractButton *set_manual_mode = new QPushButton(tr("Manual"));
    QObject::connect(set_manual_mode,SIGNAL(clicked()), SLOT(set_manual_Mode()));

    QBoxLayout *layout4 = new QHBoxLayout;

    layout4->addWidget(set_auto_mode);
    layout4->addWidget(set_manual_mode);
//    layout4->addWidget(getActualPos);
    tracking_board->setLayout(layout4);
    tracking_board->show();

}



void VideoPlayer::set_off(){
    Client->setOnOff(false);
}

void VideoPlayer::set_on(){
    Client->setOnOff(true);
}

void VideoPlayer::getActualPos(){
    QWidget*actuation_position = new QWidget;
    actuation_position->setWindowTitle("Actuation Positioning");
    Coordinates _return;

    Client->getActualPos(_return);
    QLabel *position_label_x = new QLabel(tr("Actual Position X"));
    QString Qs = QString::number(_return.x);
    QLineEdit *position_display_x = new QLineEdit;
    position_display_x->setText(Qs);

    QLabel *position_label_y = new QLabel(tr("Actual Position Y"));
    QString Qs2 = QString::number(_return.y);
    QLineEdit *position_display_y = new QLineEdit;
    position_display_y->setText(Qs2);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(position_label_x);
    layout->addWidget(position_display_x);
    layout->addWidget(position_label_y);
    layout->addWidget(position_display_y);

    actuation_position->setLayout(layout);
    actuation_position->show();

}


void VideoPlayer::get_video(){
    system("avconv -f video4linux2 -i /dev/video0 -vcodec mpeg2video -r 25 -pix_fmt yuv420p -me_method epzs -b 2600k -bt 256k -f rtp rtp://153.106.112.178:8080");

}

void VideoPlayer::take_video(){ //done
   system("gst-launch-0.10 -e udpsrc port=5000 caps=\"application/x-rtp, "
                "media=video, clock-rate=90000, "
                "encoding-name=JPEG, payload=96\" "
                "! rtpjpegdepay ! jpegdec ! ffmpegcolorspace ! autovideosink &"
                );
}

int8_t VideoPlayer::getIntensity(){
    return Client->getIntensity();
}

void VideoPlayer::setIntensity(const int intens){//done
    Client->setIntensity(intens);
}


void VideoPlayer::calibrate(){ //done
    Client->calibrate();
}

void VideoPlayer::halt(){ //done
    Client->halt();
}

void VideoPlayer::sleep(){ //done
    Client->sleep();
}

void VideoPlayer::wake(){ //done
    Client->wake();
}

void VideoPlayer::set_auto_Mode(){ //done
    Client->setMode(PointMode::AUTOMATIC);
}

void VideoPlayer::set_manual_Mode(){ //done
    Client->setMode(PointMode::MANUAL);
}

void VideoPlayer::setPos_X(const int X){
//    Client->setPos(pos);
    coordinates->__set_x(X);

}

void VideoPlayer::setPos_Y(const int Y){
//    Client->setPos(Y);
    coordinates->__set_y(Y);
}
