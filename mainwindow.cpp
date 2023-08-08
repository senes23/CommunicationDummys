#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dummywindowgw2.h"
#include "dummywindowgw4.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::Dialog);
    this->setFixedSize(this->size());

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::pushButtonClicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::pushButton2Clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pushButtonClicked()
{
    auto dummyWindow = new DummyWindowGW2();
    dummyWindow->setWindowTitle("Communication Dummy - Gewerk 2");
    dummyWindow->setWindowFlags(Qt::Dialog);
    dummyWindow->setFixedSize(dummyWindow->size());
    dummyWindow->show();
    this->close();
}

void MainWindow::pushButton2Clicked()
{
    auto dummyWindow = new DummyWindowGW4();
    dummyWindow->setWindowTitle("Communication Dummy - Gewerk 4");
    dummyWindow->setWindowFlags(Qt::Dialog);
    dummyWindow->setFixedSize(dummyWindow->size());
    dummyWindow->show();
    this->close();
}
