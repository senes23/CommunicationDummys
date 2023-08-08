#ifndef DUMMYWINDOWGW4_H
#define DUMMYWINDOWGW4_H

#include <QWidget>
#include <QtMqtt/QMqttClient>
#include <QStateMachine>

namespace Ui {
class DummyWindowGW4;
}

class DummyWindowGW4 : public QWidget
{
    Q_OBJECT

public:
    explicit DummyWindowGW4(QWidget *parent = nullptr);
    ~DummyWindowGW4();

    void ConnectTo(QString ip, int port = 1883);
    void Disconnect();
    void PublishMqttMessage(QString topicStr, QString payloadStr);
    void SubscribeToTopic(QString topic);
    void GetSubsciptionPayload(const QMqttMessage &msg);
    void Unsubscribe();
    void InitStateMachine();
    void EnableButtons(bool enabled);

private:
    Ui::DummyWindowGW4 *ui;
    QMqttClient *_client;
    QMqttSubscription *_subscription;

    bool _isConnected = false;
    bool _subscibed = false;

    QStateMachine _stateMachine;
    QState *_s1; //Initial state
    QState *_s2;
    QState *_s3;
    QState *_s4;
    QState *_s5;
    QState *_s6;

    void sendBatteryPower(int value);

private slots:
    void UpdateConnectionState(QMqttClient::ClientState state);
    void UpdateSubscriberState(QMqttSubscription::SubscriptionState state);

    void sendStartCharging();
    void sendAbortCharging();

    void pushButtonClicked();
    void pushButton3Clicked();
    void pushButton4Clicked();
    void pushButton5Clicked();
    void pushButton6Clicked();
    void pushButton7Clicked();
    void pushButton9Clicked();
    void pushButton10Clicked();

signals:
    void robotConnected();
    void robotCharging();
    void chargingResponse();
};

#endif // DUMMYWINDOWGW4_H
