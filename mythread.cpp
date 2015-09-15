#include "mythread.h"
#include<QDebug>
MyThread::MyThread(QObject *parent) :
    QThread(parent)
{
}
void MyThread::run()
{
    modbus = new ModbusManager("/dev/ttyUSB1",4);
    modbus->open();
    connect(modbus,SIGNAL(readyRead()),this,SLOT(fun()));
    exec();
}

void MyThread::fun()
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
