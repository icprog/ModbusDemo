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
    modbusManager = new ModbusManager("/dev/ttyUSB1",4);
    modbusManager->open();
    fun();
//    connect(modbusManager,SIGNAL(readyRead()),this,SLOT(fun()));
//    Modbus ac;
//    ac    qDebug("in the fun()");
//    ac.code = 3;
//    ac.regAddr = 4;
//    ac.datCount = 1;

//    ac.addr = 1;
//    ac.code = 0X10;
//    ac.regAddr = 4;
//    ac.addDat(1);
//    ac.addDat(0);
//    ac.addDat(0);
//    ac.addDat(0x3221);
//    modbusManager->SendOneModbus(ac);
//    ac.print();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::fun()
{
//    qDebug("in the fun()");
//    Modbus *mb = modbusManager->getOneModbus();
//    mb->print();
//    delete mb;

    Modbus ac;
    ac.addr = 3;
    ac.code = 3;
    ac.regAddr = 0;
    ac.datCount = 1;
    modbusManager->SendOneModbus(ac);
    if(true)
    {
        //disconnect(modbusManager,SIGNAL(readyRead()),this,SLOT(fun()));
        int tryCnt = 3;
        while(tryCnt--)
        {
            qDebug("tryCnt = %d",tryCnt);
            Modbus* _ac = modbusManager->WaitOneModbus(300);
            if(_ac == NULL)
            {
                qDebug("air ctrl error");
//            globalInfo->carNu = -1;
                //空调控制器异常
            }
            else
            {
//             globalInfo->carNu = _ac->datList[0];
                _ac->print();
                delete _ac;
                break;
            }
            //qDebug("tryCnt = %d",tryCnt);
        }
    }


}

