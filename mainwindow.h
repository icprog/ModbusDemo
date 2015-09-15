#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "modbusmanager.h"
#include"mythread.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    ModbusManager* modbus;
public:
    MyThread thread;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void fun();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
