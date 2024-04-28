#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QNetworkProxy>
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QtMath>
#include <QDateTimeAxis>
#include <QtGlobal>
#include <QEventLoop>
#include <QTextStream>
#include "CRC16.h"

#define CheckTime 1000
#define WAITIME 100

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTcpSocket *socket;
    QTcpSocket *Checksocket;

private slots:

    //socket相关处理
    void socketStatuChanged(QAbstractSocket::SocketState socketState);//检测socket状态变化
    void client_error(QAbstractSocket::SocketError error);//socket错误状态提示
    void CheckEquip();

    //交互操作相关处理
    void on_connectButton_clicked();
    void on_startButton_clicked();

    //以下是设备通讯相关
    void readHoldRegisters(QTcpSocket *socket,quint8 add,quint8 command,quint16 index1,quint16 index2);
    void onSocketReadyRead();

    //设备操作
    void on_upButton_pressed();
    void on_upButton_released();
    void on_stopButton_clicked();
    void on_downButton_pressed();
    void on_downButton_released();
    void on_clearButton_clicked();
    void testUpon();

    //输出文件相关操作
    void checkedFile();
    void outputData();

    //计时器相关，与采样频率高度相关
    void onTimerTimeout();
    void STimerTimeout();
    void waitForSignal();
    void onSignalReceived();

    //折线图相关操作
    void setupChar();
    void addValueToChart();
    void checkChart();

    //以下是输出数据到界面的方法
    void showDatas();

    void on_startButton_2_clicked();

private:
    Ui::MainWindow *ui;

    void setupSocket(QTcpSocket* &socket);

    quint8 CheckFlag=0x00;

    //设置两个Timer，一个快一个慢，快的实验计时，慢的每次到时间就检测flag，并把flag置零；
    QEventLoop loop;
    QTimer *Ftimer;
    QTimer *Stimer;

    //实时拉力值
    double tensile = 9999;
    int waitNums =0;
    int ExpTimes =0;

    //图表相关
    QChart* chart;
    QValueAxis* axisX;
    QValueAxis* axisY;
    QLineSeries* seriesY;
    qreal num=0;

};
#endif // MAINWINDOW_H
