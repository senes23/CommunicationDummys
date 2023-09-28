#include "dummywindowgw2.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "ui_dummywindowgw2.h"

DummyWindowGW2::DummyWindowGW2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DummyWindowGW2)
{
    ui->setupUi(this);
    _client = new QMqttClient(this);
    connect(_client, &QMqttClient::stateChanged, this, &DummyWindowGW2::UpdateConnectionState);

    connect(ui->pushButton, &QPushButton::clicked, this, &DummyWindowGW2::pushButtonClicked);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &DummyWindowGW2::pushButton3Clicked);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &DummyWindowGW2::pushButton4Clicked);
    connect(ui->pushButton_5, &QPushButton::clicked, this, &DummyWindowGW2::pushButton5Clicked);
    connect(ui->pushButton_6, &QPushButton::clicked, this, &DummyWindowGW2::pushButton6Clicked);

    InitStateMachine();
    _stateMachine.start();
}

DummyWindowGW2::~DummyWindowGW2()
{
    Disconnect();
    delete ui;
}

void DummyWindowGW2::InitStateMachine()
{
    //States
    _s1 = new QState();
    _s2 = new QState();
    _s30 = new QState();
    _s31 = new QState();
    _s32 = new QState();
    _s33 = new QState();
    _s40 = new QState();
    _s41 = new QState();
    _s42 = new QState();
    _s43 = new QState();
    _s50 = new QState();
    _s51 = new QState();
    _s52 = new QState();
    _s60 = new QState();
    _s70 = new QState();
    _s80 = new QState();
    _s90 = new QState();

    //Initial Transition
    _s1->addTransition(ui->pushButton_2, &QPushButton::clicked, _s2);       //Starting sequence

    //Transitions for transport job
    _s2->addTransition(this, &DummyWindowGW2::transportJobStarted, _s30);   //Job: Transport
    _s30->addTransition(this, &DummyWindowGW2::reserved, _s40);
    _s40->addTransition(this, &DummyWindowGW2::assigned, _s50);
    _s50->addTransition(this, &DummyWindowGW2::readyForReadingRfid, _s60);
    _s60->addTransition(this, &DummyWindowGW2::assigned, _s70);
    _s70->addTransition(this, &DummyWindowGW2::readyForReadingRfid, _s80);
    _s80->addTransition(this, &DummyWindowGW2::assigned, _s90);
    _s90->addTransition(this, &DummyWindowGW2::available, _s2);

    //Transitions for charging job
    _s2->addTransition(this, &DummyWindowGW2::chargingJobStarted, _s31);    //Job: Charging
    _s31->addTransition(this, &DummyWindowGW2::reserved, _s41);
    _s41->addTransition(this, &DummyWindowGW2::readyForCharging, _s51);
    _s51->addTransition(ui->pushButton_2, &QPushButton::clicked, _s2);

    //Transitions for service job
    _s2->addTransition(this, &DummyWindowGW2::serviceJobStarted, _s32);     //Job: Service
    _s32->addTransition(this, &DummyWindowGW2::reserved, _s42);
    _s42->addTransition(this, &DummyWindowGW2::inactive, _s52);
    _s52->addTransition(ui->pushButton_2, &QPushButton::clicked, _s2);

    //Transitions for pause job
    _s2->addTransition(this, &DummyWindowGW2::pauseJobStarted, _s33);       //Job: Pause
    _s33->addTransition(this, &DummyWindowGW2::reserved, _s43);
    _s43->addTransition(ui->pushButton_2, &QPushButton::clicked, _s2);

    //Exit Transitions to stop the sequence
    _s2->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s30->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s31->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s32->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s33->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s40->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s41->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s42->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s43->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s50->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s51->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s52->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s60->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s70->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s80->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);
    _s90->addTransition(ui->pushButton_2, &QPushButton::clicked, _s1);

    //Assign property changes
    _s1->assignProperty(ui->plainTextEdit, "plainText", "Initial");
    _s1->assignProperty(ui->pushButton_2, "text", "Start");
    _s1->assignProperty(ui->spinBox_5, "enabled", true);
    _s2->assignProperty(ui->plainTextEdit, "plainText", "Job allocation");
    _s2->assignProperty(ui->pushButton_2, "text", "Exit");
    _s2->assignProperty(ui->spinBox_5, "enabled", false);
    _s30->assignProperty(ui->plainTextEdit, "plainText", "Ready for Transport");
    _s31->assignProperty(ui->plainTextEdit, "plainText", "Ready for Charging");
    _s32->assignProperty(ui->plainTextEdit, "plainText", "Ready for Service");
    _s33->assignProperty(ui->plainTextEdit, "plainText", "Ready for Pause");
    _s40->assignProperty(ui->plainTextEdit, "plainText", "Robot is driving to start place");
    _s41->assignProperty(ui->plainTextEdit, "plainText", "Robot is driving to charging station");
    _s42->assignProperty(ui->plainTextEdit, "plainText", "Robot is driving to service area");
    _s43->assignProperty(ui->plainTextEdit, "plainText", "Pause");
    _s43->assignProperty(ui->pushButton_2, "text", "Finish");

    _s50->assignProperty(ui->plainTextEdit, "plainText", "Robot is driving to the RFID Reader at start place");
    _s51->assignProperty(ui->plainTextEdit, "plainText", "Robot is charging");
    _s51->assignProperty(ui->pushButton_2, "text", "Finish");
    _s52->assignProperty(ui->plainTextEdit, "plainText", "Robot is in service");
    _s52->assignProperty(ui->pushButton_2, "text", "Finish");
    _s60->assignProperty(ui->plainTextEdit, "plainText", "Ready for reading RFID at start place");
    _s70->assignProperty(ui->plainTextEdit, "plainText", "Robot is driving to the RFID Reader at destination place");
    _s80->assignProperty(ui->plainTextEdit, "plainText", "Ready for reading RFID at destination place");
    _s90->assignProperty(ui->plainTextEdit, "plainText", "Robot is driving to destination place");

    //Build the state machine
    _stateMachine.addState(_s1);
    _stateMachine.addState(_s2);
    _stateMachine.addState(_s30);
    _stateMachine.addState(_s31);
    _stateMachine.addState(_s32);
    _stateMachine.addState(_s33);
    _stateMachine.addState(_s40);
    _stateMachine.addState(_s41);
    _stateMachine.addState(_s42);
    _stateMachine.addState(_s43);
    _stateMachine.addState(_s50);
    _stateMachine.addState(_s51);
    _stateMachine.addState(_s52);
    _stateMachine.addState(_s60);
    _stateMachine.addState(_s70);
    _stateMachine.addState(_s80);
    _stateMachine.addState(_s90);
    _stateMachine.setInitialState(_s1);

    connect(_s60, &QState::entered, this, &DummyWindowGW2::sendCheckResponse);
    connect(_s80, &QState::entered, this, &DummyWindowGW2::sendCheckResponse);
}

void DummyWindowGW2::EnableButtons(bool enabled)
{
    ui->pushButton_2->setEnabled(enabled);
    ui->pushButton_3->setEnabled(enabled);
    ui->pushButton_4->setEnabled(enabled);
    ui->pushButton_5->setEnabled(enabled);
    ui->pushButton_6->setEnabled(enabled);
    ui->comboBox->setEnabled(enabled);
    ui->comboBox_2->setEnabled(enabled);
    ui->spinBox->setEnabled(enabled);
    ui->spinBox_2->setEnabled(enabled);
    ui->spinBox_3->setEnabled(enabled);
    ui->spinBox_4->setEnabled(enabled);
    ui->spinBox_6->setEnabled(enabled);
    ui->spinBox_7->setEnabled(enabled);
}

void DummyWindowGW2::ConnectTo(QString ip, int port){
    _client->setHostname(ip);
    _client->setPort(port);
    _client->setUsername("VPJ");
    _client->setPassword("R462");
    _client->connectToHost();
}

void DummyWindowGW2::Disconnect()
{
    _client->disconnectFromHost();
}

void DummyWindowGW2::SubscribeToTopic(QString topic)
{
    _subscription = _client->subscribe(QMqttTopicFilter(topic));
    connect(_subscription, &QMqttSubscription::messageReceived, this, &DummyWindowGW2::GetSubsciptionPayload);
    connect(_subscription, &QMqttSubscription::stateChanged, this, &DummyWindowGW2::UpdateSubscriberState);
}

void DummyWindowGW2::Unsubscribe()
{
    _subscription->unsubscribe();
}

// Publish an MQTT message with with the given payload and topic
void DummyWindowGW2::PublishMqttMessage(QString topicStr, QString payloadStr)
{
    QMqttTopicName topic;
    topic.setName(topicStr);
    QByteArray message;
    message.append(payloadStr.toStdString());
    _client->publish(topic, message);
}

void DummyWindowGW2::UpdateConnectionState(QMqttClient::ClientState state)
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

void DummyWindowGW2::UpdateSubscriberState(QMqttSubscription::SubscriptionState state)
{
    switch (state)
    {
    case QMqttSubscription::SubscriptionPending:
    case QMqttSubscription::UnsubscriptionPending:
        ui->label_2->setText("Pending");
        break;
    case QMqttSubscription::Subscribed:
        _subscibed = true;
        ui->label_2->setText("Subscribed");
        ui->pushButton_3->setText("Unsubscribe");
        break;
    case QMqttSubscription::Unsubscribed:
        _subscibed = false;
        ui->label_2->setText("Unsubscribed");
        ui->pushButton_3->setText("Subscribe");
        break;
    case QMqttSubscription::Error:
        ui->label_2->setText("Unknown error");
        break;
    }
}

void DummyWindowGW2::sendCheckResponse()
{
    sendCheck(ui->spinBox_5->value());
}

void DummyWindowGW2::GetSubsciptionPayload(const QMqttMessage &msg)
{
    QJsonObject obj = QJsonDocument::fromJson(msg.payload()).object();
    int val = obj["status"].toInt(-1);
    qDebug() << "Value: " << val;

    switch (val) {
    case Available:
        emit available();
        break;
    case Assigned:
        emit assigned();
        break;
    case ReservedS:
        emit reserved();
        break;
    case Inactive:
        emit inactive();
        break;
    case Fault:
        break;
    case ReadyForCharging:
        emit readyForCharging();
        break;
    case ReadyForReading:
        emit readyForReadingRfid();
        break;
    case Charging:
        break;
    case ConnectedS:
        break;
    }
}

void DummyWindowGW2::sendJob(Job job, int robotNo)
{

    QJsonObject start
        {
            {"station_id", job.start.stationId},
            {"place_id", job.start.placeId}
        };
    QJsonObject destination
        {
            {"station_id", job.destination.stationId},
            {"place_id", job.destination.placeId}
        };
    QJsonObject object
        {
            {"jobtype", job.type},
            {"start", start},
            {"destination", destination}
        };

    QString payload = QJsonDocument(object).toJson(QJsonDocument::Compact);
    PublishMqttMessage("Robot/"+ QString::number(robotNo) + "/Move",
                       payload);
}

void DummyWindowGW2::sendCheck(int robotNo)
{
    QJsonObject object
        {
            {"read", 1}
        };

    QString payload = QJsonDocument(object).toJson(QJsonDocument::Compact);
    PublishMqttMessage("Robot/"+ QString::number(robotNo) + "/Checked",
                       payload);
}

void DummyWindowGW2::sendStatus(int status, int stationId, int placeId, int robotNo)
{
    QJsonObject object
        {
            {"status", status},
            {"station_id", stationId},
            {"place_id", placeId}
        };

    QString payload = QJsonDocument(object).toJson(QJsonDocument::Compact);
    PublishMqttMessage("Robot/"+ QString::number(robotNo) + "/Status",
                       payload);
}

void DummyWindowGW2::pushButtonClicked()
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

void DummyWindowGW2::pushButton3Clicked()
{
    if (_subscibed)
        Unsubscribe();
    else
        SubscribeToTopic("Robot/+/Status");
}

void DummyWindowGW2::pushButton4Clicked()
{
    Job test;
    test.type = ui->comboBox->currentIndex();
    test.start =
        {
            ui->spinBox->value(),
            ui->spinBox_2->value()
        };
    test.destination =
        {
            ui->spinBox_3->value(),
            ui->spinBox_4->value()
        };

    sendJob(test, ui->spinBox_5->value());

    switch (test.type)
    {
    case TransportJob:
        emit transportJobStarted();
        break;
    case ChargingJob:
        emit chargingJobStarted();
        break;
    case ServiceJob:
        emit serviceJobStarted();
        break;
    case PauseJob:
        emit pauseJobStarted();
        break;
    }
}

void DummyWindowGW2::pushButton5Clicked()
{
    sendCheck(ui->spinBox_5->value());
}

void DummyWindowGW2::pushButton6Clicked()
{
    sendStatus(ui->comboBox_2->currentIndex(), ui->spinBox_6->value(), ui->spinBox_7->value(), ui->spinBox_5->value());
}
