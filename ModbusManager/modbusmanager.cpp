#include "modbusmanager.h"
#include "modbus.h"
#include "qextserialport/src/qextserialport.h"
#include<iostream>
#include<stdio.h>
#include "ztools.h"
#include<QTimer>
#include<QFile>
#include <QEventLoop>
#include<QThread>
#include <unistd.h>
#include<QDebug>
#include<stdio.h>
using namespace std;

static void QextSerialPort_init(QextSerialPort *port)
{
    port->setDtr(false);
    port->setBaudRate(BAUD19200);
    port->setFlowControl(FLOW_OFF);
    port->setParity(PAR_EVEN);
    port->setDataBits(DATA_8);
    port->setStopBits(STOP_1);
    port->setTimeout(300);

}
bool ModbusManager::open()
{
    bool res = serial->open(QIODevice::ReadWrite);
    if(!res)
        qDebug()<<serial->errorString();
    return res;
}
void ModbusManager::close()
{
    serial->close();
}

void ModbusManager::setDirection(int d)
{
    QString io = QString::number(_dirIO);
    if(_dirIO == 0)
        return;

    QFile f("/sys/class/gpio/gpio"+io+"/value");
    if(!f.open(QFile::ReadWrite))
        qDebug("open file value failed!!");
	f.write(QString::number(d).toAscii());
    f.close();
    sync();
}

ModbusManager::ModbusManager(const QString& devName,int dirIO,QueryMode mode)
{
    _dirIO = dirIO;
    QString io = QString::number(dirIO);
    if(0 != _dirIO)
    {
        if(!QFile::exists("/sys/class/gpio/gpio"+io))
        {
            QString cmd = "echo "+io+" > /sys/class/gpio/export";
            int res = system(cmd.toAscii().data());
            if(res == -1)
            {
                qDebug("echo io > /sys/class/gpio/export failed!!");
            }
            QFile f;
            if(!QFile::exists("/sys/class/gpio/gpio"+io))
                qDebug()<<"not exits gpio"+io;
            f.setFileName("/sys/class/gpio/gpio"+io+"/direction");
            f.open(QFile::ReadWrite);
            f.write("out");
            f.close();
            sync();
            setDirection(0);
        }
    }
    if(mode == EventDriven)
    {
        serial = new QextSerialPort(devName,QextSerialPort::EventDriven);
        connect(serial,SIGNAL(readyRead()),this,SLOT(readyData()));
    }
    else if(mode == Polling)
        serial = new QextSerialPort(devName,QextSerialPort::Polling);
    QextSerialPort_init(serial);

}

ModbusManager::~ModbusManager()
{
    delete serial;
}
void ModbusManager::readyData()
{

    static QString buff;
    QByteArray tmp;
    int start = -1,end = -1;
    while((tmp = serial->readAll()).length() != 0)
        buff.append(tmp);
    qDebug()<<"recv :"<<buff;

    if(start == -1)
    {
        start = buff.indexOf(':');
        if(start == -1)
            return;
    }
    if(end == -1)
    {
        end = buff.indexOf("\x0d\x0a",start);
        if(end == -1)
            return;
    }
    Modbus* mb = new Modbus(buff.mid(start,end-start+2).toAscii());
    buff.clear();
    mbList.append(mb);
    start = -1;
    end = -1;

    emit readyRead();

}
Modbus* ModbusManager::getOneModbus()
{
    if(mbList.isEmpty())
        return NULL;
    Modbus* mb = mbList.front();
    mbList.pop_front();
    return mb;
}//返回动态生成对象 ，使用后要delete  失败返回NULL

Modbus* ModbusManager::WaitOneModbus(int msecs)
{
    QEventLoop q;
    Modbus* resMb = NULL;
    QTimer::singleShot(msecs,&q,SLOT(quit()));
    connect(this,SIGNAL(readyRead()),&q,SLOT(quit()));
    q.exec();
    resMb = getOneModbus();
    return resMb;
}
void ModbusManager::SendOneModbus(Modbus& mb)
{
    mb.generate();
    setDirection(1);

    serial->write(mb.rawData);
    int bpf = 0;//bit per byte frame
    int startBits = 1;
    bpf = serial->dataBits() + startBits+ (serial->stopBits() == STOP_1?1:2) + (serial->parity() == PAR_NONE ? 0:1);
    int delay = bpf * 1000 * mb.rawData.length() /serial->baudRate() + 2;
    ZTools::msleep(delay);
    setDirection(0);

}

