#include "modbusmanager.h"
#include<iostream>
#include<stdio.h>
//#include "ztool.h"
#include<QTimer>
#include<QFile>
#include <QEventLoop>
#include<QThread>
#include <unistd.h>
#include<QDebug>
#include<stdio.h>
using namespace std;

quint8 Modbus::calCLR()
{
    quint8 sum = 0;
    sum = addr + code;
    if(code == 0x10)
    {
        sum += (regAddr >> 8)+regAddr + datCount;
    }
    else if(code == 0x03)
    {
        sum += datCount << 1;
        for(int i = 0;i<datCount;i++)
            sum += (datList[i] >> 8) +datList[i];
    }

    return quint8(~sum + 1);
}
Modbus::Modbus()
{
    regAddr = 0;
    isValid = false;
}

static char* hex2ascii(int hex,char *ascii,int len)
{
    char* _ascii = ascii;
    while(len--)
        *ascii++ = ((hex >> (len << 2))&0x0f) > 9?((hex >> (len << 2))&0x0f) - 10 +'A':((hex >> (len << 2))&0x0f) + '0';
    return _ascii;

}

static quint64 ascii2hex(const char *ascii,int len)
{
    quint64 hex = 0;
    while(len--)
        hex |=  isdigit(*ascii)?quint64(*ascii++ -'0')<<(len<<2):isalpha(*ascii)?quint64(tolower(*ascii++)-'a'+10)<<(len<<2):0<<(len<<2);
    return hex;
}
void msleep(int ms)
{
    QEventLoop q;
    QTimer::singleShot(ms,&q,SLOT(quit()));
    q.exec();
}
void Modbus::load(QByteArray pkg)
{
    regAddr = 0;
    isValid = false;
    datList.clear();
    char* off = pkg.data();
    addr = ascii2hex(off+1,2);
    code = ascii2hex(off+3,2);

    if(code == 0x03)
    {
        datCount = ascii2hex(off+5,2) >> 1;
        for(int i = 0;i < datCount;i++)
            datList.append(ascii2hex(off+7+(i<<2),4));
        LRC = ascii2hex(off+7+(datCount<<2),2);
    }
    else if(code == 0x10)
    {
        regAddr = ascii2hex(off+5,4);
        datCount = ascii2hex(off+9,4);
        LRC = ascii2hex(off+13,2);
    }
    if(datCount != datList.length())
        return;
    isValid = LRC == calCLR();

}

quint8 Modbus::generateCLR()
{
    quint8 sum = 0;
    sum = addr +code+(regAddr>>8) + regAddr + (datCount>>8)+datCount;
    if(code == 0x10)
    {
        sum += (datCount<<1);
        for(int i = 0;i < datCount;i++)
            sum+=(datList[i]>>8)+datList[i];
    }
    sum = ~sum + 1;
    return sum;
}

void Modbus::generate()
{
    isValid = false;
    char tmp[10];
    rawData.clear();
    rawData.append(':');
    rawData.append(hex2ascii(addr,tmp,2),2);
    rawData.append(hex2ascii(code,tmp,2),2);
    rawData.append(hex2ascii(regAddr,tmp,4),4);
    rawData.append(hex2ascii(datCount,tmp,4),4);
    if(code == 0x10)
    {
        rawData.append(hex2ascii(datCount<<1,tmp,2),2);
        for(int i = 0;i < datCount;i++)
            rawData.append(hex2ascii(datList[i],tmp,4),4);
    }
    LRC = generateCLR();
    rawData.append(hex2ascii(LRC,tmp,2),2);
    rawData.append(0x0d);
    rawData.append(0x0a);
    isValid = true;

}
void Modbus::print()
{
    qDebug()<<"addr     : "<<addr;
    qDebug()<<"code     : "<<code;
    qDebug()<<"regAddr  : "<<regAddr;
    qDebug("DataList :");
    for(int i = 0;i < datList.length();i++)
    {
        qDebug(" %x ",datList[i]);
    }
    qDebug("\n");
    qDebug("rawData  :");
    for(int i = 0;i < rawData.length();i++)
    {
        qDebug(" %2x ",(char)rawData[i]);
    }
    qDebug("\n");
}


static void QextSerialPort_init(QextSerialPort *port)
{
    port->setDtr(false);
    port->setBaudRate(BAUD19200);
    port->setFlowControl(FLOW_OFF);
    port->setParity(PAR_NONE);
    port->setDataBits(DATA_8);
    port->setStopBits(STOP_1);
    port->setTimeout(300);

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
void ModbusManager::SendOneModbus(Modbus& mb,int ms)
{
    setDirection(1);
    mb.generate();
    serial->write(mb.rawData);
    msleep(ms);
    setDirection(0);

}
bool ModbusManager::open()
{
    bool res = serial->open(QIODevice::ReadWrite);
    if(!res)
        qDebug()<<serial->errorString();
    return res;
}
