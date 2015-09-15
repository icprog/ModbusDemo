#ifndef MODBUSMANAGER_H
#define MODBUSMANAGER_H

#include <QThread>
#include<QDateTime>
#include "qextserialport/src/qextserialport.h"
#include<QList>


/***********************
  usage
    ModbusManager* modbus = new ModbusManager("/dev/ttyUSB1",4);
    modbus->open();
    connect(modbus,SIGNAL(readyRead()),this,SLOT(fun()));


void MainWindow::fun()
{
    qDebug()<<"1111111111111111";
    Modbus *mb = modbus->getOneModbus();
    mb->print();
    delete mb;
}

  ********************/
class Modbus
{
public:

    int addr;
    int code;//功能码



    int regAddr;
    QList<int> datList;
    QByteArray rawData;
    int datCount;//数据数量
    bool isValid; //数据是否有效

private:
    int LRC;

    quint8 calCLR();//接收时候调用
    void load(QByteArray pkg);


    quint8 generateCLR();

public:
    Modbus();
    void generate();
    explicit Modbus(QByteArray pkg){load(pkg);}
    void addDat(int dat){datList.append(dat);}
    void print();

};


class ModbusManager : public QObject
{
    Q_OBJECT
        QextSerialPort *serial;
        QList<Modbus*> mbList;

signals:
        void readyRead();
public:
    enum QueryMode {
        Polling,
        EventDriven
    };
    enum ResultState
    {
        SUCCESS,
        TIMEOUT,
        FAILED
    };
    Modbus* getOneModbus();//返回动态生成对象 ，使用后要delete  失败返回NULL
    Modbus* WaitOneModbus(int timeout = 300);//failed return NULL
    void SendOneModbus(Modbus& ac,int ms = 0);

    ModbusManager(const QString& devName,int  dirIO = 0,QueryMode mode = EventDriven);
    bool open();
    void close(){serial->close();}
    ~ModbusManager();
private:
    //int bytesWrittenCnt;
    int _dirIO;
    void setDirection(int d); //485方向　0收1发
private slots:
    //void onByteWritten(){bytesWrittenCnt++;}
    void readyData();

};

#endif // MODBUSMANAGER_H

