#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include<QtCore>
#include "modbusmanager.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    modbus = new ModbusManager("/dev/ttyUSB1",4);
    modbus->open();
    connect(modbus,SIGNAL(readyRead()),this,SLOT(fun()));
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::fun()
{

    qDebug()<<"1111111111111111";
    Modbus *mb = modbus->getOneModbus();
    mb->print();
    delete mb;

    Modbus ac;
    ac.addr = 3;
    ac.code = 3;
    ac.regAddr = 0;
    ac.datCount = 1;
    ac.generate();
    modbus->SendOneModbus(ac,(int)(119200.0/11/1000*ac.rawData.length()));



}
