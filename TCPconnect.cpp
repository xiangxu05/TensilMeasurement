#include "TCPconnect.h"

void setupSocket(QTcpSocket *&socket)
{
    socket = new QTcpSocket();//初始化socket
    socket->setProxy(QNetworkProxy::NoProxy);//不走代理，防止部分设备开了代理无法内网连接。

    //connect(socket, &QTcpSocket::stateChanged, this, &MainWindow::socketStatuChanged);
    //connect(socket,&QTcpSocket::errorOccurred,this,&MainWindow::client_error);//socket相关状态连接
}

void connectServer(QTcpSocket *socket, QString IP, int port)
{
    socket->connectToHost(IP,port);
}

void readHoldRegisters(QTcpSocket *socket, quint8 add, quint8 command, quint16 index)
{
    QByteArray requestData;
    QDataStream requestStream(&requestData, QIODevice::WriteOnly);
    requestStream.setByteOrder(QDataStream::BigEndian);

    requestStream << add << command << index;
    requestStream << calculateCRC16(requestData);

    socket->write(requestData);
    socket->flush();
}

QString onSocketReadyRead(QTcpSocket *socket)
{
    QByteArray responseData = socket->readAll();
    qDebug()<<QString(responseData);
    return QString(responseData);

}

void disconnectServer(QTcpSocket *socket)
{
    socket->close();
}


