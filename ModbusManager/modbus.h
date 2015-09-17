#ifndef MODBUS_H
#define MODBUS_H
#include<QList>
#include<QByteArray>
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

#endif // MODBUS_H
