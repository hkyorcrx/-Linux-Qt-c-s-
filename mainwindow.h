#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include "client.h"
#include "Protocol.h"
#include "filesenddialog.h"

#include <QMainWindow>
#include <QPixmap>
#include <QLabel>
#include <QMessageBox>
#include <QToolButton>
#include <QIcon>
#include <QDateTime>

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

private:
    void init();
    void initButton();
    void autoConnectToServer();
    void chatMessageDisplay(QString senderName,QString timeStr,QByteArray contentBytes);

protected:
    void closeEvent(QCloseEvent* event)override;

private slots:
    void on_send_Button_clicked();

    void on_sendfile_Button_clicked();

    void on_reconnect_Button_clicked();

    void on_shift_Button_clicked();

    void onRecvServerMessage(QString message);

    void on_picture_Button_clicked();

    void on_rename_Button_clicked();

private:
    Ui::MainWindow *ui;
    Client *m_client;

    bool isOpen;
    QString m_name;

};
#endif // MAINWINDOW_H
