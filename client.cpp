#include "client.h"

#include <QDebug>

Client::Client(QObject *parent)
    : QObject(parent)
{
    // 初始化套接字
    m_socket = new QTcpSocket(this);
    m_ip="192.168.88.132";
    m_port=9090;

    // 绑定信号与内部槽函数
    // 连接服务器成功，会发射connected信号
    connect(m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(m_socket,&QTcpSocket::errorOccurred,this,&Client::onErrorOccurred);

}

Client::Client(QString ip, quint16 port, QObject *parent) : QObject(parent)
{
    m_socket = new QTcpSocket(this); // 必须初始化socket！
    m_ip = ip;
    m_port = port;

    // 绑定信号（和默认构造一致）
    connect(m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(m_socket,&QTcpSocket::errorOccurred,this,&Client::onErrorOccurred);

}

Client::~Client()
{
    if(m_socket== nullptr){
        return;
    }else{
        m_socket->close();
    }
}



void Client::connectToServer(const QString &ip, quint16 port)
{
    if(isConnected()){
        // disconnectFromServer();
    }
    // 发起连接(异步,不会阻塞界面）
    m_socket->connectToHost(QHostAddress(ip),port);
}

void Client::sendData(const QString &data)
{
    if(m_socket==nullptr){
        qDebug() << "[Client] 发送失败：套接字未初始化";
        emit sig_error("套接字未初始化，发送失败"); // 对外抛错误信号
        return;
    }
    if (!isConnected()) {
        qDebug() << "[Client] 发送失败：未连接到服务器";
        emit sig_error("未连接到服务器，无法发送数据");
        return;
    }
    QString sendData = data.trimmed(); // 去除首尾空格
    if (sendData.isEmpty()) {
        qDebug() << "[Client] 发送失败：数据为空";
        emit sig_error("发送数据为空，取消发送");
        return;
    }
    QByteArray byteData = sendData.toUtf8();

    qint64 writeLen = m_socket->write(byteData);
    if (writeLen == -1) {
        QString errMsg = QString("数据发送失败：%1").arg(m_socket->errorString());
        qDebug() << "[Client] " << errMsg;
        emit sig_error(errMsg);
    } else {
        m_socket->flush();
        qDebug() << "[Client] 数据发送成功 | 长度：" << writeLen << "字节 | 内容：" << sendData;
    }

}

bool Client::isConnected() const
{
    return m_socket!=nullptr && (m_socket->state() == QAbstractSocket::ConnectedState);
}


void Client::onConnected()
{
    qDebug()<<"服务器连接成功";
}

void Client::onDisconnected()
{

}

void Client::onReadyRead()
{
    QByteArray recvDataByte = m_socket->readAll();
    if (recvDataByte.isEmpty()) {
        qWarning() << "读取到空数据";
        return;
    }
    m_recvData = QString::fromUtf8(recvDataByte);
    qInfo() << "收到服务器数据：" << m_recvData;
    emit sig_recvData(m_recvData);

}

void Client::onErrorOccurred(QAbstractSocket::SocketError err)
{
    if (err == QAbstractSocket::UnknownSocketError) return;
    QString errMsg;
    switch(err) {
    case QAbstractSocket::ConnectionRefusedError:
        errMsg = "服务器拒绝连接（可能未启动/端口错误）";
        break;
    case QAbstractSocket::HostNotFoundError:
        errMsg = "未找到服务器（IP地址错误）";
        break;
    default:
        errMsg = m_socket->errorString(); // 兜底描述
    }
    qDebug() << "[Client 错误]：" << errMsg;
    emit sig_error(errMsg); // 通知UI层提示用户

}

