
#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <qmediaplayer.h>
#include "gen-cpp/Tracking.h"
#include <QtGui/QMovie>
#include <QtWidgets/QWidget>
//#include <opencv2/opencv.hpp>
#include <vlc/vlc.h>
#include <vlc-qt/Instance.h>
#include <vlc-qt/Media.h>
#include <vlc-qt/MediaPlayer.h>


#include <QLabel>
#include <QLCDNumber>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QMainWindow>
//using namespace cv;

//namespace Ui{
//    class VideoPlayer;
//}
class QAbstractButton;
class QSlider;
class QLabel;

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = 0);
    ~VideoPlayer();

public slots:
    void openFile();
    void play();
    void actuation_monitoring();
    void lighting_intensity();


    void lighting_toggle();
    void tracking_status();

    //Video Stream
    void get_video();
    void take_video();

    void set_off();
    void set_on();


    void set_auto_Mode();
    void set_manual_Mode();
//    void recv_setIntensity();
    void setIntensity(const int intens);

    //Lighting Thrift
    int8_t getIntensity();
    //Actuation Thrift

    void calibrate();
    void halt();
    void sleep();
    void wake();
    void setPos(int coord);
    void getActualPos();
private slots:
    void mediaStateChanged(QMediaPlayer::State state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void setPosition(int position);
    void handleError();


private:
    QMediaPlayer mediaPlayer;
    QAbstractButton *playButton;
    QSlider *positionSlider;
    QLabel *errorLabel;
    TrackingClient *Client;
    Coordinates *coordinates;
//    Ui::VideoPlayer *ui;

    VlcInstance *_instance;
    VlcMedia *_media;
    VlcMediaPlayer *_player;
};

#endif
