#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifndef STYLESHEET
#define STYLESHEET
QString redSignal ="background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                    " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(255, 0, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                    " border-radius: 12px;";
QString greenSignal ="background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                      " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(0, 255, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                      " border-radius: 12px;";;
QString yellowSignal ="background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5,"
                       " radius:0.5, fx:0.5, fy:0.5, stop:0 rgba(255, 255, 0, 255), stop:1 rgba(255, 255, 255, 255));"
                       " border-radius: 12px;";
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupSocket(socket);
    setupChar();
    Ftimer = new QTimer(this);
    Stimer = new QTimer(this);
    setWindowTitle("拉力监测 V1.02");
    setWindowIcon(QIcon(":/new/prefix1/1.ico"));
    ui->preData->setChecked(true);
    ui->checkCurve->setChecked(true);
    connect(ui->checkFile,&QCheckBox::toggled,this,&MainWindow::checkedFile);
    connect(Ftimer, &QTimer::timeout, this, &MainWindow::onTimerTimeout);//关联一个计时器
    connect(Stimer, &QTimer::timeout, this, &MainWindow::STimerTimeout);//关联一个计时器

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupSocket(QTcpSocket* &socket){
    socket = new QTcpSocket(this);//初始化socket
    socket->setProxy(QNetworkProxy::NoProxy);//不走代理，防止部分设备开了代理无法内网连接。
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);
    connect(socket, &QTcpSocket::stateChanged, this, &MainWindow::socketStatuChanged);
    connect(socket, &QTcpSocket::errorOccurred,this,&MainWindow::client_error);//socket相关状态连接
    connect(socket, &QTcpSocket::connected,this,&MainWindow::CheckEquip);
}

void MainWindow::socketStatuChanged(QAbstractSocket::SocketState socketState){
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    QString hostName = socket->peerName();
    switch(socketState) {
    case QAbstractSocket::UnconnectedState:{
        // 未连接状态下的操作
        ui->statuLable2->setText(hostName+"未连接");
        ui->statuLabel1->setStyleSheet(redSignal);
        ui->infoLabel->setText("未连接");
        ui->connectButton->setText("连接");
        ui->startButton->setText("开始监测");
        break;
    }
    case QAbstractSocket::HostLookupState:{
        // 主机查找状态下的操作
        ui->statuLable2->setText("查找主机"+hostName+"……");
        break;
    }
    case QAbstractSocket::ConnectingState:{
        // 连接状态下的操作
        ui->statuLable2->setText("连接"+hostName+"中……");
        ui->statuLabel1->setStyleSheet(yellowSignal);
        ui->infoLabel->setText("连接中");
        break;
    }
    case QAbstractSocket::ConnectedState:{
        // 已连接状态下的操作
        ui->statuLable2->setText("连接"+hostName+"成功！");
        ui->statuLabel1->setStyleSheet(greenSignal);
        ui->infoLabel->setText("已连接");
        ui->connectButton->setText("取消连接");
        break;
    }
    case QAbstractSocket::BoundState:
        // 已绑定状态下的操作
        ui->statuLable2->setText(hostName+":已经绑定");
        break;
    case QAbstractSocket::ClosingState:{
        // 关闭状态下的操作
        ui->statuLable2->setText(hostName+"未连接");
        ui->statuLabel1->setStyleSheet(redSignal);
        ui->infoLabel->setText("未连接");
        ui->connectButton->setText("连接");
        ui->statuLabel2->setStyleSheet(redSignal);
        ui->infoLabel_2->setText("未在线");
        ui->statuLabel3->setStyleSheet(redSignal);
        ui->infoLabel_3->setText("未在线");
        break;
    }
    default:
        // 其他状态下的操作
        ui->statuLable2->setText(hostName+":Unknown State");
        break;
    }
}

void MainWindow::client_error(QAbstractSocket::SocketError error){
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    QString hostName = socket->peerName();
    if (socket) {
        switch (error) {
        case QAbstractSocket::ConnectionRefusedError:
            ui->statuLable2->setText("连接"+hostName+"被拒绝");
            break;
        case QAbstractSocket::RemoteHostClosedError:
            ui->statuLable2->setText("远程主机"+hostName+"关闭了连接");
            break;
        case QAbstractSocket::HostNotFoundError:
            ui->statuLable2->setText("找不到主机名:"+hostName);
            break;
        case QAbstractSocket::SocketAccessError:
            ui->statuLable2->setText("试图访问套接字的方式无效:"+hostName);
            break;
        case QAbstractSocket::SocketResourceError:
            ui->statuLable2->setText("本地系统资源不足，无法完成所请求的操作:"+hostName);
            break;
        case QAbstractSocket::SocketTimeoutError:
            ui->statuLable2->setText("操作超时:"+hostName);
            break;
        case QAbstractSocket::DatagramTooLargeError:
            ui->statuLable2->setText("数据报文太大，无法发送:"+hostName);
            break;
        case QAbstractSocket::NetworkError:
            ui->statuLable2->setText("网络错误:"+hostName);
            break;
        case QAbstractSocket::AddressInUseError:
            ui->statuLable2->setText("地址已在使用中:"+hostName);
            break;
        case QAbstractSocket::SocketAddressNotAvailableError:
            ui->statuLable2->setText("套接字地址不可用:"+hostName);
            break;
        case QAbstractSocket::UnsupportedSocketOperationError:
            ui->statuLable2->setText("不支持请求的操作:"+hostName);
            break;
        case QAbstractSocket::UnfinishedSocketOperationError:
            ui->statuLable2->setText("上次操作没有完成:"+hostName);
            break;
        case QAbstractSocket::ProxyAuthenticationRequiredError:
            ui->statuLable2->setText("代理服务器要求身份验证:"+hostName);
            break;
        case QAbstractSocket::SslHandshakeFailedError:
            ui->statuLable2->setText("SSL/TLS 握手失败:"+hostName);
            break;
        case QAbstractSocket::ProxyConnectionRefusedError:
            ui->statuLable2->setText("代理服务器拒绝连接:"+hostName);
            break;
        case QAbstractSocket::ProxyConnectionClosedError:
            ui->statuLable2->setText("代理服务器关闭连接:"+hostName);
            break;
        case QAbstractSocket::ProxyConnectionTimeoutError:
            ui->statuLable2->setText("代理服务器连接超时:"+hostName);
            break;
        case QAbstractSocket::ProxyNotFoundError:
            ui->statuLable2->setText("无法找到代理服务器:"+hostName);
            break;
        case QAbstractSocket::ProxyProtocolError:
            ui->statuLable2->setText("代理服务器返回的响应与预期的不符:"+hostName);
            break;
        case QAbstractSocket::UnknownSocketError:
            ui->statuLable2->setText("未知的套接字错误:"+hostName);
            break;
        default:
            ui->statuLable2->setText("发生未知错误:"+hostName+" "+socket->errorString());
            break;
        }
    }
}


void MainWindow::on_connectButton_clicked()
{
    if(ui->connectButton->text()=="连接"){
        if(socket->state() != QAbstractSocket::ConnectedState){
            // 如果套接字状态不是已连接状态，就尝试连接到主机
            socket->connectToHost(ui->IP->text(),ui->port->text().toInt());
        }
    }
    else if(ui->connectButton->text()=="取消连接"){
        if(ui->startButton->text()=="暂停监测"){
            ui->startButton->text()="开始监测";
            on_startButton_clicked();
        }
        socket->close();
    }
}
void MainWindow::CheckEquip(){
    QThread::msleep(100);
    readHoldRegisters(socket,0x02,0x03,0x1000,0x0001);
    QThread::msleep(100);
    readHoldRegisters(socket,0x01,0x03,0x07D0,0x0002);
}
void MainWindow::on_startButton_clicked()
{
    if(ui->connectButton->text()=="取消连接"){
        if(ui->infoLabel_2->text()=="已在线" && ui->infoLabel_3->text()=="已在线"){
            if(ui->startButton->text()=="开始监测"){
                ui->startButton->setText("暂停监测");
                int tryTime = 0;
                while(tensile==9999){
                    readHoldRegisters(socket,0x01,0x03,0x07D0,0x0002);
                    waitForSignal();
                    tryTime++;
                    if(tryTime >20){
                        break;
                        QMessageBox::warning(this,"警告","设备通讯错误！");
                        on_startButton_clicked();
                    }
                }
                if(!ExpTimes){
                    ExpTimes=ui->TimeEdit->text().toInt();
                    QString fileName = ui->FilePath->text();
                    if(fileName.isEmpty())
                    {
                        QMessageBox::warning(this,"警告","未指定存储文件路径！");
                        ui->startButton->setText("开始监测");
                        return;
                    }
                    else{
                        QFile file(fileName);
                        if (!file.open(QIODevice::Append | QIODevice::Text)) {
                            QMessageBox::warning(this,"警告","无法保存为该文件，请检查！");
                        }
                        QTextStream out(&file);
                        out<<"Nums"<<","<<"Tensile"<<","<<"\n";

                        file.close();
                    }
                }
                Ftimer->start(WAITIME);
                Stimer->start(CheckTime);
                ui->upButton->setDisabled(true);
                ui->downButton->setEnabled(false);
                ui->checkFile->setEnabled(false);
                testUpon();
            }
            else{
                ui->startButton->setText("开始监测");
                on_stopButton_clicked();
                Ftimer->stop();
                Stimer->stop();
                ui->recButton->setText("记录数据");
                ui->upButton->setEnabled(true);
                ui->downButton->setEnabled(true);
                ui->checkFile->setEnabled(true);
                if(!ExpTimes){
                    num=0;
                    seriesY->clear();
                }
            }
        }else{
            QMessageBox::warning(this,"警告","设备未在线，请检查！");
            ui->startButton->text()="开始监测";
            on_stopButton_clicked();
            Ftimer->stop();
            Stimer->stop();
            if(!ExpTimes){
                num=0;
                seriesY->clear();
            }
            ui->upButton->setEnabled(true);
            ui->downButton->setEnabled(true);
            ui->checkFile->setEnabled(true);
            //停止时钟
        }
    }
    else if(ui->connectButton->text()=="连接"){
        QMessageBox::warning(this,"警告","请先连接设备！");
    }
}

void MainWindow::readHoldRegisters(QTcpSocket *socket, quint8 add, quint8 command, quint16 index1,quint16 index2)
{
    QByteArray requestData;
    QDataStream requestStream(&requestData, QIODevice::WriteOnly);
    requestStream.setByteOrder(QDataStream::BigEndian);
    requestStream << add << command << index1 <<index2;
    requestStream << calculateCRC16(requestData);
    socket->write(requestData);
    socket->flush();
}

void MainWindow::onSocketReadyRead()
{
    QByteArray responseData = socket->readAll();
    if(verifyCRC16(responseData)){
        if(responseData[0] == 0x01){
            CheckFlag |= 0x0F;
            ui->statuLabel3->setStyleSheet(greenSignal);
            ui->infoLabel_3->setText("已在线");
            QByteArray byteArray=responseData.mid(3, 4);
            QDataStream stream(byteArray);
            int decimalValue;
            stream >> decimalValue;
            if (decimalValue > 61440){
                decimalValue = decimalValue -65536;
            }//由于量程一般不会达到六百多，因此大于F000的直接用补码表示就是负数了。
            tensile = decimalValue;
            tensile = tensile /100* 9.8;
        }
        else if(responseData[0] == 0x02){
            if(responseData[2] == 0x10 && responseData[3] == 0x00 && responseData[4] == 0x00 && responseData[5] == 0x00){
                Vflag=false;
            }else{
                Vflag=true;
            }
            CheckFlag |= 0xF0;
            ui->statuLabel2->setStyleSheet(greenSignal);
            ui->infoLabel_2->setText("已在线");
        }
    }

}

void MainWindow::testUpon()
{

    readHoldRegisters(socket,0x02,0x06,0x0305,static_cast<quint16>(ui->testSpeed->text().toInt()/5));
    QThread::msleep(100);
    readHoldRegisters(socket,0x02,0x06,0x051B,0x0000);
    QThread::msleep(100);
    readHoldRegisters(socket,0x02,0x06,0x051C,0x0000);
    QThread::msleep(100);
    readHoldRegisters(socket,0x02,0x06,0x2105,0x0001);
}

void MainWindow::on_upButton_pressed()
{
    readHoldRegisters(socket,0x02,0x06,0x0306,static_cast<quint16>(ui->speed->text().toInt()));
    QThread::msleep(100);
    readHoldRegisters(socket,0x02,0x06,0x051B,0x0000);
    QThread::msleep(100);
    readHoldRegisters(socket,0x02,0x06,0x051C,0x0010);
    QThread::msleep(100);
    readHoldRegisters(socket,0x02,0x06,0x2105,0x0001);
}


void MainWindow::on_upButton_released()
{
    readHoldRegisters(socket,0x02,0x06,0x2105,0x0000);
}


void MainWindow::on_stopButton_clicked()
{
    readHoldRegisters(socket,0x02,0x06,0x2105,0x0000);
}


void MainWindow::on_downButton_pressed()
{
    readHoldRegisters(socket,0x02,0x06,0x0306,static_cast<quint16>(ui->speed->text().toInt()));
    QThread::msleep(100);
    readHoldRegisters(socket,0x02,0x06,0x051B,0x0010);
    QThread::msleep(100);
    readHoldRegisters(socket,0x02,0x06,0x051C,0x0010);
    QThread::msleep(100);
    readHoldRegisters(socket,0x02,0x06,0x2105,0x0001);
}


void MainWindow::on_downButton_released()
{
    readHoldRegisters(socket,0x02,0x06,0x2105,0x0000);
}

void MainWindow::on_clearButton_clicked()
{
    readHoldRegisters(socket,0x02,0x06,0x2000,0x0001);
}

//输出文件相关操作
void MainWindow::checkedFile(){
    if(ui->checkFile->isChecked()){
        QString defaultFileFormat = "csv";
        QString defaultFileName = "output";
        QString defaultFileFullName = defaultFileName + "." + defaultFileFormat;

        QString fileName = QFileDialog::getSaveFileName(
            nullptr,
            "选择一个文件",
            defaultFileFullName,
            "CSV Files (*.csv);;All Files (*)"
            );

        if(fileName.isEmpty())
        {
            ui->checkFile->setChecked(false);
            QMessageBox::warning(this,"警告","请选择一个文件");
        }
        else
        {
            ui->FilePath->setText(fileName);
        }
    }
    else{
        ui->FilePath->setText("");
    }
}

void MainWindow::outputData(){

    QString fileName = ui->FilePath->text();

    if(fileName.isEmpty())
    {
        return;
    }
    else{
        QFile file(fileName);
        if (!file.open(QIODevice::Append | QIODevice::Text)) {
            QMessageBox::warning(this,"警告","无法保存为该文件，请检查！");
        }
        QTextStream out(&file);
        out<<num<<","<<tensile<<","<<"\n";

        file.close();
        return;
    }

}

//以下是计时器相关
void MainWindow::onTimerTimeout(){
    // 每次定时器超时时调用readHoldRegisters方法
    //readHoldRegisters(socket,0x02,0x03,0x1000,0x0001);
    waitNums++;
    if(waitNums==8){
        readHoldRegisters(socket,0x02,0x03,0x1000,0x0001);
        waitNums=0;
        QThread::msleep(50);
    }
    readHoldRegisters(socket,0x01,0x03,0x07D0,0x0002);

    showDatas();
    if(ui->recButton->text()=="停止记录"){
        outputData();
    }
    addValueToChart();
}

void MainWindow::STimerTimeout(){
    ExpTimes--;
    int Min=ExpTimes/60;
    int Sec=ExpTimes%60;
    ui->TimeLabel->setText(QString::number(Min)+"分"+QString::number(Sec)+"秒");
    QString styleSheet = "color: black; font-size: 24px; font-weight: bold;text-align: center;";
    ui->TimeLabel->setStyleSheet(styleSheet);
    ui->TimeLabel->setAlignment(Qt::AlignCenter);
    if(!ExpTimes || Vflag==false){
        on_startButton_clicked();
        ui->recButton->setText("记录数据");
    }
    if((CheckFlag & 0x0F) == 0x00){
        ui->statuLabel3->setStyleSheet(redSignal);
        ui->infoLabel_3->setText("未在线");
    }
    if((CheckFlag & 0xF0) == 0x00){
        ui->statuLabel2->setStyleSheet(redSignal);
        ui->infoLabel_2->setText("未在线");
    }

}

void MainWindow::waitForSignal(){
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onSignalReceived);
    loop.exec();
}

void MainWindow::onSignalReceived(){
    disconnect(socket, &QTcpSocket::readyRead, this, &MainWindow::onSignalReceived);
    loop.quit();
}

void MainWindow::showDatas(){
    QString styleSheet = "color: black; font-size: 24px; font-weight: bold;text-align: center;";
    if(ui->preData->isChecked()){
        ui->TensileLabel->setText(QString::number(tensile,'f', 3));
        ui->TensileLabel->setStyleSheet(styleSheet);
        ui->TensileLabel->setAlignment(Qt::AlignCenter);
    }
    else{
        ui->TensileLabel->setText("--");
        ui->TensileLabel->setStyleSheet(styleSheet);
        ui->TensileLabel->setAlignment(Qt::AlignCenter);
    }
}

void MainWindow::on_startButton_2_clicked()
{
    ExpTimes=0;
    int Min=ExpTimes/60;
    int Sec=ExpTimes%60;
    ui->TimeLabel->setText(QString::number(Min)+"分"+QString::number(Sec)+"秒");
    QString styleSheet = "color: black; font-size: 24px; font-weight: bold;text-align: center;";
    ui->TimeLabel->setStyleSheet(styleSheet);
    ui->TimeLabel->setAlignment(Qt::AlignCenter);
    if(ui->startButton->text()=="暂停监测"){
        on_startButton_clicked();
    }
    tensile = 9999;
    ui->TensileLabel->setText("--");
    ui->TensileLabel->setStyleSheet(styleSheet);
    ui->TensileLabel->setAlignment(Qt::AlignCenter);

    num=0;
    seriesY->clear();
    //关闭所有功能，重置所有数据
}

void MainWindow::setupChar(){

    //初始化图表框架
    chart = new QChart();
    chart->setTitle("实时磁感应强度曲线");
    ui->curve->setChart(chart);

    //初始化折线序列
    seriesY = new QLineSeries;
    seriesY->setName("拉力值/N");

    chart->addSeries(seriesY);
    axisX = new QValueAxis;
    axisX->setTickCount(10);

    chart->setAxisX(axisX,seriesY);

    axisX->setRange(0,num+10);


    axisY = new QValueAxis;
    axisY->setRange(-10,300);
    chart->setAxisY(axisY,seriesY);

}

void MainWindow::addValueToChart(){
    if(ui->checkCurve->isChecked()){
        seriesY->append(num, tensile);
        axisX->setMin(num-((30000/WAITIME)));
        axisX->setMax(num);
        num++;
        qreal maxValue = std::numeric_limits<qreal>::lowest();
        qreal minValue = std::numeric_limits<qreal>::max();

        // 遍历系列中的数据点，找到最大值
        foreach (const QPointF &point, seriesY->points()) {
            minValue = std::min(minValue, point.y());
            maxValue = std::max(maxValue, point.y());
        }
        int intMax = static_cast<int>(maxValue) + 10;
        int intMin = static_cast<int>(minValue) - 10;
        axisY->setMin(intMin);
        axisY->setMax(intMax);
    }
    else{
        return;
    }
}

void MainWindow::checkChart(){
    if(!ui->checkCurve->isChecked()){
        seriesY->clear();
    }
}

void MainWindow::on_recButton_clicked()
{
    if(ui->recButton->text()=="记录数据"){
        ui->recButton->setText("停止记录");
    }else{
        ui->recButton->setText("记录数据");
    }
}

