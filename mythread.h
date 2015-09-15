#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include"modbusmanager.h"
class MyThread : public QThread
{
    Q_OBJECT
public:
    ModbusManager* modbus;
    explicit MyThread(QObject *parent = 0);
    
signals:
    
public slots:
    void run();
    void fun();
};

#endif // MYTHREAD_H
