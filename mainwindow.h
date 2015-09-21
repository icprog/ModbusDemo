#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "modbusmanager.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    ModbusManager* modbusManager;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void fun();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
