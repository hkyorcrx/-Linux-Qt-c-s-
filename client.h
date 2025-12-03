#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QString>


class Client: public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject* parent = nullptr);
    Client(QString ip,quint16 port,QObject* parent = nullptr);
    ~Client();

public:
    void connectToServer(const QString& ip,quint16 port);
    void sendData(const QString& data);
    // void disconnectFromServer();
    bool isConnected()const;
    QString getIp(){
        return m_ip;
    }
    quint16 getPort(){
        return m_port;
    }
    void flush(){
        m_socket->flush();
    }

signals:
    // void sig_connected();
    // void sig_disconnected();
    void sig_error(QString str);
    void sig_recvData(QString str);

private slots:
    void onConnected();
    void onDisconnected(); // 断开连接
    void onReadyRead();   // 接收数据
    void onErrorOccurred(QAbstractSocket::SocketError err);
    // void onReadySend();

private:
    QTcpSocket *m_socket;  // 套接字对象
    QString m_ip;
    quint16 m_port;
    QString m_recvData;

};

#endif // CLIENT_H
