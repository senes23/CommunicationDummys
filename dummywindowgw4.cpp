#include "dummywindowgw4.h"
#include "ui_dummywindowgw4.h"

#include <QtStateMachine>

DummyWindowGW4::DummyWindowGW4(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DummyWindowGW4)
{
    ui->setupUi(this);
    _client = new QMqttClient(this);
    connect(_client, &QMqttClient::stateChanged, this, &DummyWindowGW4::UpdateConnectionState);

    connect(ui->pushButton, &QPushButton::clicked, this, &DummyWindowGW4::pushButtonClicked);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &DummyWindowGW4::pushButton3Clicked);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &DummyWindowGW4::pushButton4Clicked);
    connect(ui->pushButton_5, &QPushButton::clicked, this, &DummyWindowGW4::pushButton5Clicked);
    connect(ui->pushButton_6, &QPushButton::clicked, this, &DummyWindowGW4::pushButton6Clicked);
    connect(ui->pushButton_7, &QPushButton::clicked, this, &DummyWindowGW4::pushButton7Clicked);
    connect(ui->pushButton_9, &QPushButton::clicked, this, &DummyWindowGW4::pushButton9Clicked);
    connect(ui->pushButton_10, &QPushButton::clicked, this, &DummyWindowGW4::pushButton10Clicked);

    InitStateMachine();
    _stateMachine.start();
}

DummyWindowGW4::~DummyWindowGW4()
{
    Disconnect();
    delete ui;
}

void DummyWindowGW4::InitStateMachine()
{
    //States
    _s1 = new QState();
    _s2 = new QState();
    _s3 = new QState();
    _s4 = new QState();
    _s5 = new QState();
    _s6 = new QState();

    //Transitions
    _s1->addTransition(ui->pushButton_2, &QPushButton::clicked, _s2);   //Starting sequence
    _s2->addTransition(this, &DummyWindowGW4::robotConnected, _s3);        //Robot is connected with the charging station
    _s3->addTransition(this, &DummyWindowGW4::robotCharging, _s4);         //Charging process is running
    _s4->addTransition(this, &DummyWindowGW4::chargingResponse, _s5);      //Response after completed charging process
    _s4->addTransition(ui->pushButton_8, &QPushButton::clicked, _s6);   //Abort charging process
    _s5->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);   //Finish Charging process
    _s6->addTransition(this, &DummyWindowGW4::chargingResponse, _s5);      //Response after aborted charging process

    //Exit Transitions to stop the sequence
    _s2->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s3->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s4->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s6->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);

    //Assign property changes
    _s1->assignProperty(ui->plainTextEdit, "plainText", "Initial");
    _s1->assignProperty(ui->pushButton_2, "text", "Start");
    _s1->assignProperty(ui->spinBox, "enabled", true);
    _s2->assignProperty(ui->plainTextEdit, "plainText", "Driving to the charging station");
    _s2->assignProperty(ui->pushButton_2, "text", "Exit");
    _s2->assignProperty(ui->spinBox, "enabled", false);
    _s3->assignProperty(ui->plainTextEdit, "plainText", "Ready for charging");
    _s3->assignProperty(ui->pushButton_2, "text", "Exit");
    _s4->assignProperty(ui->plainTextEdit, "plainText", "Charging process running");
    _s4->assignProperty(ui->pushButton_2, "text", "Exit");
    _s5->assignProperty(ui->plainTextEdit, "plainText", "Charging process finished");
    _s5->assignProperty(ui->pushButton_2, "text", "Finish");
    _s6->assignProperty(ui->plainTextEdit, "plainText", "Charging process aborted");
    _s6->assignProperty(ui->pushButton_2, "text", "Exit");

    //Build the state machine
    _stateMachine.addState(_s1);
    _stateMachine.addState(_s2);
    _stateMachine.addState(_s3);
    _stateMachine.addState(_s4);
    _stateMachine.addState(_s5);
    _stateMachine.addState(_s6);
    _stateMachine.setInitialState(_s1);

    connect(_s3, &QState::entered, this, &DummyWindowGW4::sendStartCharging);
    connect(_s6, &QState::entered, this, &DummyWindowGW4::sendAbortCharging);
}

void DummyWindowGW4::EnableButtons(bool enabled)
{
    ui->pushButton_2->setEnabled(enabled);
    ui->pushButton_3->setEnabled(enabled);
    ui->pushButton_4->setEnabled(enabled);
    ui->pushButton_5->setEnabled(enabled);
    ui->pushButton_6->setEnabled(enabled);
    ui->pushButton_7->setEnabled(enabled);
    ui->pushButton_8->setEnabled(enabled);
    ui->pushButton_9->setEnabled(enabled);
    ui->pushButton_10->setEnabled(enabled);
    ui->horizontalSlider->setEnabled(enabled);
}

void DummyWindowGW4::ConnectTo(QString ip, int port){
    _client->setHostname(ip);
    _client->setPort(port);
    _client->setUsername("VPJ");
    _client->setPassword("R462");
    _client->connectToHost();
}

void DummyWindowGW4::Disconnect()
{
    _client->disconnectFromHost();
}

void DummyWindowGW4::SubscribeToTopic(QString topic)
{
    _subscription = _client->subscribe(QMqttTopicFilter(topic));
    connect(_subscription, &QMqttSubscription::messageReceived, this, &DummyWindowGW4::GetSubsciptionPayload);
    connect(_subscription, &QMqttSubscription::stateChanged, this, &DummyWindowGW4::UpdateSubscriberState);
}

void DummyWindowGW4::Unsubscribe()
{
    _subscription->unsubscribe();
}

void DummyWindowGW4::GetSubsciptionPayload(const QMqttMessage &msg)
{
    QJsonObject obj = QJsonDocument::fromJson(msg.payload()).object();
    int val = obj["status"].toInt();
    qDebug() << "Value: " << val;

    switch (val) {
    case 8:
        emit robotConnected();
        break;
    case 7:
        emit robotCharging();
        break;
    case 0:
        emit chargingResponse();
        break;
    default:
        break;
    }
}

// Publish an MQTT message with with the given payload and topic
void DummyWindowGW4::PublishMqttMessage(QString topicStr, QString payloadStr)
{
    QMqttTopicName topic;
    topic.setName(topicStr);
    QByteArray message;
    message.append(payloadStr.toStdString());
    _client->publish(topic, message);
}

void DummyWindowGW4::UpdateConnectionState(QMqttClient::ClientState state)
{
    switch(state)
    {
    case QMqttClient::Connected:
        _isConnected = true;
        ui->label->setText("Connected");
        ui->pushButton->setText("Disconnect");
        EnableButtons(true);
        break;
    case QMqttClient::Disconnected:
        _isConnected = false;
        ui->label->setText("Disconnected");
        ui->pushButton->setText("Connect");
        EnableButtons(false);
        break;
    case QMqttClient::Connecting:
        _isConnected = false;
        ui->label->setText("Connecting");
        ui->pushButton->setText("Disconnect");
        EnableButtons(false);
        break;
    }
}

void DummyWindowGW4::UpdateSubscriberState(QMqttSubscription::SubscriptionState state)
{
    switch (state)
    {
    case QMqttSubscription::SubscriptionPending:
    case QMqttSubscription::UnsubscriptionPending:
        ui->label_5->setText("Pending");
        break;
    case QMqttSubscription::Subscribed:
        _subscibed = true;
        ui->label_5->setText("Subscribed");
        ui->pushButton_7->setText("Unsubscribe");
        break;
    case QMqttSubscription::Unsubscribed:
        _subscibed = false;
        ui->label_5->setText("Unsubscribed");
        ui->pushButton_7->setText("Subscribe");
        break;
    case QMqttSubscription::Error:
        ui->label_5->setText("Unknown error");
        break;
    }
}

void DummyWindowGW4::sendStartCharging()
{
    PublishMqttMessage("Charging/" + QString::number(ui->spinBox->value()) + "/Load",
                       "{\"charge\": true}");
}

void DummyWindowGW4::sendAbortCharging()
{
    PublishMqttMessage("Charging/" + QString::number(ui->spinBox->value()) + "/Load",
                       "{\"charge\": false}");
}

void DummyWindowGW4::sendBatteryPower(int value)
{
    PublishMqttMessage("Robot/" + QString::number(ui->spinBox->value()) + "/Energy",
                       "{\"battery_power\": " + QString::number(value) + "}");
}

void DummyWindowGW4::pushButtonClicked()
{
    if (_isConnected)
    {
        if (_subscibed)
            Unsubscribe();
        Disconnect();
    }
    else
        ConnectTo(ui->lineEdit->text(), 1883);
}

void DummyWindowGW4::pushButton3Clicked()
{
    //Roboter verbunden
    PublishMqttMessage("Charging/" + QString::number(ui->spinBox->value()) + "/Connected",
                       "{\"status\": 8}");
}

void DummyWindowGW4::pushButton4Clicked()
{
    //Roboter lädt
    PublishMqttMessage("Charging/" + QString::number(ui->spinBox->value()) + "/Connected",
                       "{\"status\": 7}");
}

void DummyWindowGW4::pushButton5Clicked()
{
    //Laderückmeldung frei
    PublishMqttMessage("Charging/" + QString::number(ui->spinBox->value()) + "/Connected",
                       "{\"status\": 0}");
}

void DummyWindowGW4::pushButton6Clicked()
{
    //Ladezustand
    sendBatteryPower(ui->horizontalSlider->value());
}

void DummyWindowGW4::pushButton7Clicked()
{
    if (_subscibed)
        Unsubscribe();
    else
    {
        SubscribeToTopic("Charging/+/Loaded");
        SubscribeToTopic("Charging/+/Connected");
    }
}

void DummyWindowGW4::pushButton9Clicked()
{
    sendStartCharging();
}

void DummyWindowGW4::pushButton10Clicked()
{
    sendAbortCharging();
}
