#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include <QCloseEvent>
#include <QScrollArea>
#include <QFileDialog>
#include <QTimer>
#include <QInputDialog>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // m_client = new Client("192.168.88.132",9090,this);
    m_client = new Client(this);
    autoConnectToServer();

    //初始化一些基本内容
    init();
    initButton();

    connect(m_client, &Client::sig_error, this, [=](QString errMsg){
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("发送失败");
        msgBox.setText(errMsg);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.addButton(tr("确定"), QMessageBox::AcceptRole);
        msgBox.exec();
        ui->input_Edit->setStyleSheet("border: 1px solid red;");
        QTimer::singleShot(3000, [=](){
            ui->input_Edit->setStyleSheet("");
        });
    });

    connect(m_client, &Client::sig_recvData, this, &MainWindow::onRecvServerMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    // 设置用户头像
    QLabel *imgLabel = ui->img_label;
    QPixmap pixmap(":/images/Profile_picture.png");
    if(pixmap.isNull()){
        qDebug()<<"Profile picture is wrong!";
        imgLabel->setText("Profile picture error!");
        return;
    }
    // 设置比例
    pixmap = pixmap.scaled(imgLabel->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    imgLabel->setPixmap(pixmap);

    isOpen = true;

    ui->name_label->setText("胡子鱼");
    ui->person_signature_label->setText("人要自由自在，一切置身事外");

    m_name = ui->name_label->text() ;
    qDebug()<<"m_name = "<<m_name;

    // 开启自动换行
    ui->person_signature_label->setWordWrap(true);
    // 设置尺寸策略（水平可扩展，垂直自适应）
    ui->person_signature_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // 限制最大宽度（避免文本过长时过度拉伸，保持界面整洁）
    ui->person_signature_label->setMaximumWidth(200);
    ui->person_signature_label->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // 初始化toolbutton
    QToolButton *toolbtn = ui->sendfile_Button;
    toolbtn->setIcon(QIcon(":/images/addMessage.png"));
    toolbtn->setIconSize(QSize(34,34));
    toolbtn->setFixedSize(34,34);
    toolbtn->setCursor(Qt::PointingHandCursor);

    toolbtn = ui->saying_Button;
    toolbtn->setIcon(QIcon(":/images/saying.png"));
    toolbtn->setIconSize(QSize(34,34));
    toolbtn->setFixedSize(34,34);
    toolbtn->setCursor(Qt::PointingHandCursor);

    toolbtn = ui->remote_Button;
    toolbtn->setIcon(QIcon(":/images/remoteControle.png"));
    toolbtn->setIconSize(QSize(34,34));
    toolbtn->setFixedSize(34,34);
    toolbtn->setCursor(Qt::PointingHandCursor);

    toolbtn = ui->design_Button;
    toolbtn->setIcon(QIcon(":/images/design.png"));
    toolbtn->setIconSize(QSize(34,34));
    toolbtn->setFixedSize(34,34);
    toolbtn->setCursor(Qt::PointingHandCursor);

    toolbtn = ui->addmessage_Button;
    toolbtn->setIcon(QIcon(":/images/addMessage.png"));
    toolbtn->setIconSize(QSize(34,34));
    toolbtn->setFixedSize(34,34);
    toolbtn->setCursor(Qt::PointingHandCursor);

    toolbtn = ui->rename_Button;
    toolbtn->setIcon(QIcon(":/images/rename.png"));
    toolbtn->setIconSize(QSize(26,26));
    toolbtn->setFixedSize(26,26);
    toolbtn->setCursor(Qt::PointingHandCursor);

    //
    QScrollArea *chatScrollArea = ui->scrollArea;
    chatScrollArea->setWidgetResizable(true); // 内容Widget自动适应高度
    chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏水平滚动条

    // chatScrollArea->setWidget(ui->chat_widget);

    QVBoxLayout *chatLayout = ui->chat_VLayout;
    chatLayout->setSpacing(10); // 聊天项之间的间距
    chatLayout->setContentsMargins(10, 10, 10, 10); // 容器内边距
    // chatLayout->addStretch(1); // 底部留白，新消息从上方追加
    chatLayout->setAlignment(Qt::AlignTop);


    // ChatItemWidget* chat = new ChatItemWidget(this);

}

void MainWindow::initButton()
{
    // 发送消息按钮
    ui->send_Button->setShortcut(Qt::CTRL + Qt::Key_Return);
    ui->send_Button->setCursor(Qt::PointingHandCursor);
    ui->shift_Button->setCursor(Qt::PointingHandCursor);

    QToolButton *toolbtn = ui->font_Button;
    toolbtn->setIcon(QIcon(":/images/font.png"));
    toolbtn->setIconSize(QSize(28,28));
    toolbtn->setFixedSize(28,28);
    toolbtn->setCursor(Qt::PointingHandCursor);

    toolbtn = ui->face_Button;
    toolbtn->setIcon(QIcon(":/images/face.png"));
    toolbtn->setIconSize(QSize(28,28));
    toolbtn->setFixedSize(28,28);
    toolbtn->setCursor(Qt::PointingHandCursor);

    toolbtn = ui->picture_Button;
    toolbtn->setIcon(QIcon(":/images/picture.png"));
    toolbtn->setIconSize(QSize(28,28));
    toolbtn->setFixedSize(28,28);
    toolbtn->setCursor(Qt::PointingHandCursor);

    toolbtn = ui->message_Button;
    toolbtn->setIcon(QIcon(":/images/message.png"));
    toolbtn->setIconSize(QSize(28,28));
    toolbtn->setFixedSize(28,28);
    toolbtn->setCursor(Qt::PointingHandCursor);

    toolbtn = ui->reconnect_Button;
    toolbtn->setIcon(QIcon(":/images/reConnect.png"));
    toolbtn->setIconSize(QSize(28,28));
    toolbtn->setFixedSize(28,28);
    toolbtn->setCursor(Qt::PointingHandCursor);
}

void MainWindow::autoConnectToServer()
{
    m_client->connectToServer(m_client->getIp(),m_client->getPort());

}

void MainWindow::chatMessageDisplay(QString senderName, QString timeStr, QByteArray contentBytes)
{
    QString headerText = QString("%1 %2").arg(senderName).arg(timeStr);

    QWidget *msgWidget = new QWidget(this);

    msgWidget->setStyleSheet("background-color: transparent;");

    QGridLayout *gridLayout = new QGridLayout(msgWidget);
    gridLayout->setContentsMargins(10, 5, 10, 5); // 设置一点边距
    gridLayout->setVerticalSpacing(2); // 名字和内容之间的间距紧凑一点

    QLabel *nameTimeLabel = new QLabel(headerText);
    nameTimeLabel->setStyleSheet("color: blue; font-size: 12px; font-weight: bold;");
    nameTimeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *contentLabel = new QLabel(contentBytes);
    contentLabel->setStyleSheet("color: black; font-size: 14px;");
    contentLabel->setWordWrap(true);
    contentLabel->setTextInteractionFlags(Qt::TextSelectableByMouse); // 允许鼠标复制文字
    contentLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    gridLayout->addWidget(nameTimeLabel, 0, 0);
    gridLayout->addWidget(contentLabel, 1, 0);

    ui->chat_VLayout->addWidget(msgWidget);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // QMessageBox::StandardButton reply;
    // reply = QMessageBox::question(this,"确认退出","你确定要关闭窗口嘛？",
    //                               QMessageBox::Yes | QMessageBox::No);

    // if(reply == QMessageBox::Yes){
    //     event->accept();
    // }else{
    //     event->ignore();
    // }
}

void MainWindow::on_send_Button_clicked()
{
    QString sendMessage = ui->input_Edit->toPlainText();
    QString senderName = ui->name_label->text();
    if(sendMessage.isEmpty()){
        QMessageBox::warning(this,"warning","输入框为空，无法发送数据",QMessageBox::Yes);
        return;
    }
    if(senderName.isEmpty()){
        QMessageBox::warning(this,"warning","昵称不能为空",QMessageBox::Yes);
        return;
    }
    if(m_client){
        TextBody tb;
        tb.timestamp = QDateTime::currentSecsSinceEpoch(); // 获取当前时间戳

        memset(tb.sender, 0, sizeof(tb.sender));
        strncpy(tb.sender, senderName.toStdString().c_str(), sizeof(tb.sender) - 1);

        QByteArray contentBytes = sendMessage.toUtf8();

        QByteArray packetBody;
        packetBody.append((char*)&tb, sizeof(TextBody));
        packetBody.append(contentBytes);

        PacketHeader header;
        header.type = MSG_TEXT;
        header.length = packetBody.size();

        m_client->sendData((char*)&header);
        m_client->sendData(packetBody);

        m_client->flush();

        // 消息只转发给了其他人
        QString timeStr = QDateTime::fromSecsSinceEpoch(tb.timestamp).toString("HH:mm:ss");

        // 显示到聊天框
        // 显示顺序，名称 时间
        // 换行：输入内容
        chatMessageDisplay(senderName,timeStr,contentBytes);
        // 清空输入框
        ui->input_Edit->clear();
    }
    else{
        QMessageBox::warning(this, "错误", "未连接到服务器，发送失败", QMessageBox::Yes);
        qDebug() << "客户端对象未初始化或未连接";
    }
}


void MainWindow::on_sendfile_Button_clicked()
{
    QStringList filepaths = QFileDialog::getOpenFileNames(this,"选择文件","D://");
    if (!filepaths.isEmpty()) {
        // 转换为QFileInfo列表（方便获取文件详情）
        QList<QFileInfo> fileInfoList;
        for (const QString& path : filepaths) {
            fileInfoList.append(QFileInfo(path));
        }

        // 3. 弹出自定义对话框，传入多文件信息
        FileSendDialog sendDialog(this);
        sendDialog.setFileDataList(fileInfoList); // 传入列表

        // 4. 点击发送后处理多文件发送
        if (sendDialog.exec() == QDialog::Accepted) {

        }
    }
}


void MainWindow::on_reconnect_Button_clicked()
{
    autoConnectToServer();
}


void MainWindow::on_shift_Button_clicked()
{
    if(isOpen){
        ui->shift_Button->setText("打开");
        isOpen=false;
    }else{
        ui->shift_Button->setText("关闭");
        isOpen=true;
    }
}

void MainWindow::onRecvServerMessage(QString message)
{
    ui->input_Edit->setText(message);

}


void MainWindow::on_picture_Button_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"选择文件","D://", tr("Images (*.png *.xpm *.jpg)"));

}


void MainWindow::on_rename_Button_clicked()
{
    bool isOk; // 记录用户是否点击“确定”
    QString newName = QInputDialog::getText(
        this,                  // 父窗口
        "修改用户名",           // 对话框标题
        "请输入新的用户名：",    // 提示文字
        QLineEdit::Normal,     // 输入框类型（普通文本）
        ui->name_label->text(), // 默认值（当前显示的用户名）
        &isOk                  // 输出参数：用户是否确认
        );

    // 2. 处理输入结果：用户点击“确定”且输入不为空
    if (isOk && !newName.trimmed().isEmpty()) {
        ui->name_label->setText(newName.trimmed());
        qDebug() << "用户名已修改为：" << newName.trimmed();
    }

}

