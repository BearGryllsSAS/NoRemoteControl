#include "login.h"
#include "ui_login.h"
#include "dialog.h"
#include <QIcon>
#include <QPixmap>
#include <QStandardPaths>
#include <QDir>
#include <QRegularExpression>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QBuffer>
#include <QCloseEvent>

Login::Login(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);

    // 设置窗口图标 ------> 不需要，因为已经设置无边框了，只用更改 lable 的样式为图片就行
    // QIcon icon(":/Pictures/Resource/Pictures/LoginIcon.png");
    // setWindowIcon(icon);

    // 设置无边框
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    // 初始化头像
    setAvatar();

    // 初始化左上角图标
    setIcon();

    // 设置输入验证器，确保只能在输入框输入字母和数字
    setValidator();

    // 设置定时器，以实现动态背景、动态按钮文本更新、以及登录请求的定时发送
    setTimer();

    // 连接socket的信号槽，读取从服务器传输的数据，并根据数据内容执行不同的操作
    connect(&socket, &QTcpSocket::readyRead, this, &Login::onReadyRead);

    // 判断是否需要自动登录以及记住密码
    ifAutoLogin();
}

// 初始化头像
void Login::setAvatar()
{
    QPixmap pixmap(":/Pictures/Resource/Pictures/DefaultAvatar.jpg");  //默认头像
    pixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio);
    ui->AvatarLab->setPixmap(pixmap);
    //获取应用程序的专用目录
    QString appDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDir);
    //创建目录（如果不存在）
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    saveLoginAvatorPath = dir.filePath("laseloginavator.png");
    if (QFile::exists(saveLoginAvatorPath)) {   //如果有上次登录成功的人就加载头像
        QPixmap newpixmap(saveLoginAvatorPath);
        newpixmap = newpixmap.scaled(100, 100, Qt::KeepAspectRatio);
        ui->AvatarLab->setPixmap(newpixmap);
    }
}

// 初始化左上角图标
void Login::setIcon()
{
    QPixmap pixmapIcon(":/Pictures/Resource/Pictures/LoginIcon.png");
    ui->LoginIconLab->setPixmap(pixmapIcon);
    ui->LoginIconLab->setScaledContents(true);
}

// 设置输入验证器，确保只能在输入框输入字母和数字
void Login::setValidator()
{
    QRegularExpression regExp("^[a-zA-Z0-9]{1,12}$");   //给输入框设置验证器
    Login::validator = new QRegularExpressionValidator(regExp, this);
    ui->AccountLineEdit->setValidator(validator);
    ui->PasswordLineEdit->setValidator(validator);
}

// 绘制渐变背景，当窗口的 update() 函数被调用后，该函数会自动调用
void Login::paintEvent(QPaintEvent *event)//绘画渐变背景
{
    QPainter painter(this); // 创建一个QPainter对象，绘制到当前控件
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿
    QLinearGradient gradient(0, 0, width(), height());  // 创建一个从(0, 0)到控件右下角的线性渐变
    // 计算颜色变化
    int r1 = 255;   // 固定红色分量（白色）
    int g1 = 220 + (sin(offset * 0.06) * 30);   // 增加绿色分量变化
    int r2 = 172;   // 固定红色分量（淡蓝色）
    int g2 = 224;   // 固定绿色分量（淡蓝色）
    int b2 = 249;   // 固定蓝色分量（淡蓝色）
    gradient.setColorAt(0.0, QColor(r1, g1, 235));  // 动态渐变开始颜色
    gradient.setColorAt(1.0, QColor(r2, g2, b2));   // 动态渐变结束颜色
    painter.setBrush(gradient); // 将之前设置好的渐变 gradient 作为画刷，使得后续的绘制操作都采用这个渐变填充效果。
    painter.drawRect(rect());   // 绘制一个矩形，rect() 返回当前控件的矩形区域，drawRect() 用渐变填充这个矩形区域，也就是填充整个控件背景。
}

// 设置定时器，以实现动态背景、动态按钮文本更新、以及登录请求的定时发送
void Login::setTimer()
{
    // 建立并启动计时器 来更新动态背景
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Login::updateGradient);
    timer->start(50);

    // 建立计时器（不启动） 来发送登录请求
    cancelTimer = new QTimer(this);
    connect(cancelTimer, &QTimer::timeout, this, &Login::sendLogin);
    cancelRequested = false;

    //建立计时器（不启动） 来更新取消登录按钮文本 ---》 点击登录后的按钮样子？
    textUpdateTimer = new QTimer(this);
    connect(textUpdateTimer, &QTimer::timeout, this, &Login::updateLoginText);
}

// 更新渐变背景
void Login::updateGradient()
{
    offset++;
    update();
}

void Login::on_LoginBtn_clicked()
{
    // tcpConnect();
    // if (socket.state() != QAbstractSocket::ConnectedState) {

    //     // 更新按钮的状态
    //     ui->LoginBtn->setText("登录");

    //     Dialog msgBox(this);
    //     msgBox.transText("请检查您的网络连接!");
    //     msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
    //     msgBox.exec();
    //     return;
    // }

    // ui->LoginBtn->setStyleSheet(// 设置取消登录红色风格
    //     "QPushButton {"
    //     "    font: 12pt 'Microsoft YaHei UI';"
    //     "    background-color: rgb(255, 0, 0);"
    //     "    color: white;"
    //     "    border-radius: 15px;"
    //     "}"
    //     "QPushButton:hover {"
    //     "    background-color: rgba(255, 0, 0, 0.7);"
    //     "    color: white;"
    //     "    border-radius: 15px;"
    //     "}"
    //     "QPushButton:pressed {"
    //     "    background-color: rgba(200, 0, 0, 0.8);"
    //     "    color: rgba(255, 255, 255, 0.9);"
    //     "    border-radius: 15px;"
    //     "}"
    //     );

    if (cancelRequested) {
        //如果请求取消，重置状态并返回
        ui->PasswordLineEdit->setEnabled(true);
        ui->AccountLineEdit->setEnabled(true);
        // ui->LoginBtn->setStyleSheet(
        //     "QPushButton {"
        //     "    font: 12pt 'Microsoft YaHei UI';"
        //     "    background-color: rgb(5, 186, 251);"
        //     "    color: white;"
        //     "    border-radius: 15px;"
        //     "}"
        //     "QPushButton:hover {"
        //     "    background-color: rgba(5, 186, 251, 0.7);"
        //     "    color: white;"
        //     "    border-radius: 15px;"
        //     "}"
        //     "QPushButton:pressed {"
        //     "    background-color: rgba(0, 123, 255, 0.8);"
        //     "    color: rgba(255, 255, 255, 0.9);"
        //     "    border-radius: 15px;"
        //     "}"
        //     );
        cancelRequested = false;
        dotCount = 1;
        cancelTimer->stop();
        textUpdateTimer->stop();
        ui->LoginBtn->setText("登录");//重置按钮文本
        return;
    }

    // 判断账号和密码是否为空，空的话直接报错，不会进行于服务器的通信
    if (ui->AccountLineEdit->text().isEmpty() || ui->PasswordLineEdit->text().isEmpty())
    {
        // 更新按钮的状态
        // ui->LoginBtn->setText("登录");

        Dialog msgBox(this);
        msgBox.transText("账号或密码不能为空!");
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
        msgBox.exec();
        return;
    }

    ui->PasswordLineEdit->setEnabled(false);
    ui->AccountLineEdit->setEnabled(false);

    //更改按钮文本为“正在登录.”
    ui->LoginBtn->setText("正在登录.");
    cancelRequested = true;
    //启动定时器，4.5秒后登录(通过信号和槽)
    cancelTimer->start(4500);
    //启动定时器更新文本
    textUpdateTimer->start(350);
    //如果计时器还在运行，直接返回
    if (cancelTimer->isActive()) {
        return;
    }
}

// 发送登录请求
void Login::sendLogin()
{
    // ui->LoginBtn->setText("登录");
    // ui->LoginBtn->setStyleSheet(
    //     "QPushButton {"
    //     "    font: 12pt 'Microsoft YaHei UI';"
    //     "    background-color: rgb(5, 186, 251);"
    //     "    color: white;"
    //     "    border-radius: 15px;"
    //     "}"
    //     "QPushButton:hover {"
    //     "    background-color: rgba(5, 186, 251, 0.7);"
    //     "    color: white;"
    //     "    border-radius: 15px;"
    //     "}"
    //     "QPushButton:pressed {"
    //     "    background-color: rgba(0, 123, 255, 0.8);"
    //     "    color: rgba(255, 255, 255, 0.9);"
    //     "    border-radius: 15px;"
    //     "}"
    //     );
    cancelRequested = false;
    dotCount = 1;
    cancelTimer->stop();
    textUpdateTimer->stop();
    ui->PasswordLineEdit->setEnabled(true);
    ui->AccountLineEdit->setEnabled(true);

    tcpConnect();
    if (socket.state() != QAbstractSocket::ConnectedState) {

        // 更新按钮的状态
        ui->LoginBtn->setText("登录");

        Dialog msgBox(this);
        msgBox.transText("请检查您的网络连接!");
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
        msgBox.exec();
        return;
    }

    QJsonObject jsonObj;
    QJsonDocument jsonDoc;
    jsonObj["tag"] = "login";
    jsonObj["qq_number"] = ui->AccountLineEdit->text();
    jsonObj["password"] = ui->PasswordLineEdit->text();
    QJsonDocument jsondoc(jsonObj);
    QByteArray jsonData = jsondoc.toJson() + "END";
    //发送消息
    socket.write(jsonData);
    socket.flush();
}

// 连接服务器
bool Login::tcpConnect()
{
    if (socket.state() != QAbstractSocket::ConnectedState){
        socket.connectToHost("127.0.0.1", 8888);//修改这个地方
        if (socket.waitForConnected(1000)) {
            qDebug() << "socket初始化成功";
            return true;
        } else {
            qDebug() << "socket初始化失败"<<socket.errorString();
            return false;
        }
    }
    return true;
}

// 更新取消按钮的文本 模拟动态效果
void Login::updateLoginText()
{
    QString text = "正在登录.";
    if (dotCount < 5) {
        text.append(QString(".").repeated(dotCount));
        dotCount++;
    } else {
        dotCount = 1;//重置点计数
    }
    ui->LoginBtn->setText(text);
}

// 读取并解析服务器中发送过来的消息，然后做出响应
void Login::onReadyRead()
{
    jsonData += socket.readAll();//使用 += 追加接收的数据
    //检查数据是否以END结尾
    while (jsonData.endsWith("END")) {
        jsonData.chop(3);//移除结尾的"END"
        //确保 jsonData 完整，尝试将其解析为 JSON 文档
        jsonDoc = QJsonDocument::fromJson(jsonData);
        jsonData.clear();
        if (jsonDoc.isNull()) {
            qDebug() << "未能解析 JSON 数据，当前数据是:" << jsonData;
            return;//有可能数据未完整，提前返回
        }
        //处理解析成功的 JSON 对象
        jsonObj = jsonDoc.object();
        qDebug()<<jsonObj["tag"];
        //根据 "answer" 处理不同的请求
        if (jsonObj["answer"] == "dengluchenggong") {//登录成功
            dengLuChengGong();
        }
        else if (jsonObj["answer"] == "denglushibai") {//登录失败
            qDebug() << "登录失败";
            dengLuShiBai();
        }
        else if (jsonObj["answer"] == "askforavator") {
            qDebug() << "要发送头像了";
            // emit sendAvator();//发送头像请求
        }
        else if (jsonObj["answer"] == "zhucechenggong") {
            qDebug() << "注册成功了";
            // emit zhuCeChengGong(jsonObj["qq_number"].toString());
        }
        else if (jsonObj["answer"] == "zhuAccountInfo") {
            qDebug() << "注册失败了";
            // emit zhuCeShiBai();
        }
        else if (jsonObj["tag"] == "youravator") {
            qDebug() << "要保存登录成功的头像了";
            saveAvator(jsonObj["avator"].toString());
        }
        else if (jsonObj["tag"] == "findpassword1_answer") {
            qDebug() << "收到找回密码有没有这个账号了";
            // emit findPass1(jsonObj);
        }
        else if (jsonObj["tag"] == "findpassword2_answer") {
            qDebug() << "收到回发的密保问题结果了";
            // emit findPass2(jsonObj);
        }
        else if (jsonObj["tag"] == "findpassword3_answer") {
            qDebug() << "收到修改密码结果了";
            // emit findPass3(jsonObj);
        }
        //清空 jsonData，准备接收下一个数据块
        jsonData.clear();
    }
}

// 判断是否需要自动登录以及记住密码
void Login::ifAutoLogin()
{
    if (set.value("autologin", false).toBool()) {//获取自动登录用户名并设置到输入框
        qDebug()<<"要自动登录了";
        ui->AutoLoginCheckBox->setChecked(true);
        QString tmp = set.value("autologinuser", "").toString();
        ui->AccountLineEdit->setText(tmp);
        ui->PasswordLineEdit->setText(settings.value(tmp+"/password").toString());
        setSty();
        on_LoginBtn_clicked();//登录
    }
    else
    {
        qDebug()<<"没有自动登录";
        ifRememberPassword();//没有自动登录 判断是不是有人登录成功记住密码了 加载账号
    }
}

// 初始设置输入框格式
void Login::setSty()
{
    if(ui->AccountLineEdit->text() != "请输入你的账号"){
        ui->AccountLineEdit->setStyleSheet("font: 12pt 'Microsoft YaHei UI'; "
                                      "border: 1px solid rgba(0, 0, 0, 0.3); "
                                      "border-radius: 10px; "
                                      "color: black;");}
    if(ui->PasswordLineEdit->text() != "请输入你的密码"){
        ui->PasswordLineEdit->setStyleSheet("font: 12pt 'Microsoft YaHei UI'; "
                                         "border: 1px solid rgba(0, 0, 0, 0.3); "
                                         "border-radius: 10px; "
                                         "color: black;");
        ui->PasswordLineEdit->setEchoMode(QLineEdit::Password);}
}

// 判断上次登录的时候是否记住了密码
void Login::ifRememberPassword()
{
    QString tmp = settings.value("lastlogin",false).toString();
    if(settings.value("lastlogin",false).toBool()){//上次有人记住密码 加载账号
        ui->AccountLineEdit->setText(settings.value("lastlogin").toString());
        if(settings.value(tmp+"/password").toBool()){//判断这个账号有没有记录的密码
            ui->PasswordLineEdit->setText(settings.value(tmp+"/password").toString());
            ui->AutoLoginCheckBox->setChecked(true);
        }
        setSty();
    }
}

Login::~Login()
{
    delete ui;
    delete validator;
    delete timer;
    delete cancelTimer;
}

void Login::on_CloseTheWindowBtn_clicked()
{
    this->close();
}

void Login::dengLuShiBai()//登录失败
{
    settings.remove(ui->AccountLineEdit->text());
    QFile::remove(saveLoginAvatorPath);
    settings.setValue("lastlogin", "");
    set.setValue("autologin", false);
    set.setValue("autologinuser", "");
    qDebug() << "移除记住的账号与密码与头像了";
    // if (socket.state() == QAbstractSocket::ConnectedState) {

    //     // 更新按钮的状态
    //     ui->LoginBtn->setText("登录");

    //     Dialog msgBox(this);
    //     msgBox.transText("请检查您的账号密码!");
    //     msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
    //     msgBox.exec();
    // }

    // 更新按钮的状态
    ui->LoginBtn->setText("登录");

    Dialog msgBox(this);
    msgBox.transText("请检查您的账号密码!");
    msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
    msgBox.exec();
}

void Login::dengLuChengGong()//登录成功
{
    qDebug() << "登录成功";
    loginFlag = true;
    settings.setValue("lastlogin", ui->AccountLineEdit->text());//记录上次登录账号
    if (ui->RememberPasswordCheckBox->isChecked() && ui->AutoLoginCheckBox->isChecked()) {
        remeberPassword();//记住密码
        rememberAvator();//记住头像
        set.setValue("autologin", true);
        set.setValue("autologinuser", ui->AccountLineEdit->text());
        qDebug() << "记住密码和头像了,下次自动登录";
    } else if (ui->RememberPasswordCheckBox->isChecked()) {
        remeberPassword();//记住密码
        rememberAvator();//记住头像
        set.setValue("autologin", false);
        set.setValue("autologinuser", "");
        qDebug() << "记住密码和头像了,下次不自动登录";
    } else {
        rememberAvator();//记住头像
        settings.remove(ui->AccountLineEdit->text());
        QFile::remove(saveLoginAvatorPath);
        settings.setValue("lastlogin", "");
        set.setValue("autologin", false);
        set.setValue("autologinuser", "");
        qDebug() << "移除记住的账号与密码与头像了";
    }

    // Dialog msgBox(this);
    // msgBox.transText("登录成功!");
    // msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
    // msgBox.exec();
}

void Login::rememberAvator()//记住头像
{
    qDebug()<<"要发送索要头像的请求了";
    QJsonObject jsonObj;
    QJsonDocument jsonDoc;
    QByteArray jsonData;
    jsonObj["tag"] = "askforavator";//向服务器索要头像
    jsonObj["qq_number"] = ui->AccountLineEdit->text();
    jsonDoc = QJsonDocument(jsonObj);
    jsonData = jsonDoc.toJson();
    //发送消息
    jsonDoc = QJsonDocument(jsonObj);
    jsonData = jsonDoc.toJson();
    //添加标识符
    QByteArray messageWithSeparator = jsonData + "END";
    //发送JSON 数据
    socket.write(messageWithSeparator);
    socket.flush();
    jsonData.clear();
}

void Login::saveAvator(QString avator)//保存登录成功的头像
{
    qDebug() << "正在保存登录成功的用户的头像";
    avator = avator.simplified();
    QByteArray byteArray = QByteArray::fromBase64(avator.toUtf8());
    QImage image;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::ReadOnly);
    if (!image.load(&buffer, "PNG")) {
        qDebug() << "无法从数据加载图像";
        return;
    }
    //保存到用户的文档目录
    if (image.save(saveLoginAvatorPath)) {
        qDebug() << "头像已成功保存到" << saveLoginAvatorPath;
    } else {
        qDebug() << "保存头像失败";
    }

    // Dialog msgBox(this);
    // msgBox.transText("保存登录成功的头像!");
    // msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
    // msgBox.exec();

    this->close();
}

void Login::closeEvent(QCloseEvent *event)//关闭窗口事件
{
    if (loginFlag == true) {
        qDebug() << "登录成功 弹出新窗口";
        socket.disconnectFromHost();
        qDebug() << "登录窗口已断开和服务器的连接";
        //发送登录成功的信号
        // emit loginSucceed(ui->AccountLineEdit->text());
    } else {
        //如果没有登录，直接关闭窗口
        qDebug()<<"正常关闭";
        socket.disconnectFromHost();
        event->accept();
    }
}

void Login::remeberPassword()//记住密码
{
    settings.beginGroup(ui->AccountLineEdit->text());
    settings.setValue("qq_num", ui->AccountLineEdit->text());
    settings.setValue("password", ui->PasswordLineEdit->text());
    settings.endGroup();
}


void Login::on_pushButton_clicked()
{
    tcpConnect();
    if(ui->LoginBtn->text() != "登录"){
        on_LoginBtn_clicked();
    }
    if (socket.state() != QAbstractSocket::ConnectedState) {
        Dialog msgBox(this);
        msgBox.transText("请检查您的网络连接!");
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
        msgBox.exec();
        return;
    }
    // regis = new RegisterWindow(&socket,this);
    // connect(this,&Login::sendAvator,regis,&RegisterWindow::sendAva);
    // connect(this,&Login::zhuCeChengGong,regis,&RegisterWindow::chengGong);
    // connect(this,&Login::zhuCeShiBai,regis,&RegisterWindow::shiBai);
    // regis->show();
}

