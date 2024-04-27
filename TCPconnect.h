#ifndef TCPCONNECT_H
#define TCPCONNECT_H
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>
#include <QNetworkProxy>
#include "CRC16.h"

void setupSocket(QTcpSocket* &socket);

void connectServer(QTcpSocket *socket,QString IP,int port); //连接服务器

void disconnectServer(QTcpSocket *socket);

void readHoldRegisters(QTcpSocket *socket,quint8 add,quint8 command,quint16 index); //读寄存器

QString onSocketReadyRead(QTcpSocket *socket); //收到信息时，调用

#endif // TCPCONNECT_H
