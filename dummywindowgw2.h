#ifndef DUMMYWINDOWGW2_H
#define DUMMYWINDOWGW2_H

#include <QWidget>
#include <QtMqtt/QMqttClient>
#include <QStateMachine>

#include "datatypes.h"

namespace Ui {
class DummyWindowGW2;
}

class DummyWindowGW2 : public QWidget
{
    Q_OBJECT

public:
    explicit DummyWindowGW2(QWidget *parent = nullptr);
    ~DummyWindowGW2();

    void ConnectTo(QString ip, int port = 1883);
    void Disconnect();
    void PublishMqttMessage(QString topicStr, QString payloadStr);
    void SubscribeToTopic(QString topic);
    void GetSubsciptionPayload(const QMqttMessage &msg);
    void Unsubscribe();
    void InitStateMachine();
    void EnableButtons(bool enabled);

    void sendJob(Job job, int robotNo);
    void sendCheck(int robotNo);
    void sendStatus(int status, int stationId, int placeId, int robotNo);

private:
    Ui::DummyWindowGW2 *ui;

    QMqttClient *_client;
    QMqttSubscription *_subscription;

    bool _isConnected = false;
    bool _subscibed = false;

    QStateMachine _stateMachine;
    QState *_s1; //Initial state
    QState *_s2; //Job allocation
    QState *_s30, *_s40, *_s50, *_s60, *_s70, *_s80, *_s90; //Transport states
    QState *_s31, *_s41, *_s51; //Charging states
    QState *_s32, *_s42, *_s52; //Service states
    QState *_s33, *_s43; //Pause states

private slots:
    void UpdateConnectionState(QMqttClient::ClientState state);
    void UpdateSubscriberState(QMqttSubscription::SubscriptionState state);

    void sendCheckResponse();

    void pushButtonClicked();
    void pushButton3Clicked();
    void pushButton4Clicked();
    void pushButton5Clicked();
    void pushButton6Clicked();

signals:
    void transportJobStarted();
    void chargingJobStarted();
    void serviceJobStarted();
    void pauseJobStarted();

    void available();
    void assigned();
    void reserved();
    void inactive();
    void readyForReadingRfid();
    void readyForCharging();
};

#endif // DUMMYWINDOWGW2_H
