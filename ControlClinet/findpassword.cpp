#include "findpassword.h"
#include "ui_findpassword.h"
#include "dialog.h"
#include <QRegularExpression>
#include <QPainter>

FindPassword::~FindPassword()
{
    delete validator;
    delete ui;
}

FindPassword::FindPassword(QTcpSocket* msocket, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FindPassword), socket(msocket)
{
    ui->setupUi(this);

    //初始化窗口
    setWindowFlags(Qt::Dialog);

    //给输入框设置验证器
    // setVal();

    // 将窗口居中显示
    QWidget *topLevelParent = parent ? parent->window() : nullptr;
    if (topLevelParent) {
        int x = topLevelParent->x() + (topLevelParent->width() - width()) / 2;
        int y = topLevelParent->y() + (topLevelParent->height() - height()) / 2;
        move(x, y);
    }
}

//给输入框设置验证器
void FindPassword::setVal()
{
    QRegularExpression regExp("^[a-zA-Z0-9]{1,12}$");
    validator = new QRegularExpressionValidator(regExp, this);
    ui->Pages1LineEdit->setValidator(validator);
    ui->Pages3_2LineEdit->setValidator(validator);
    ui->Pages3_1LineEdit->setValidator(validator);
}

// 取消第一个输入框的默认焦点设置
void FindPassword::showEvent(QShowEvent *event)
{
    // 调用父类的 showEvent 保证正常显示
    QMainWindow::showEvent(event);

    // 设置焦点到一个非输入框的控件，比如一个 QLabel
    ui->TextLabel->setFocus();  // 确保焦点在这个标签上，而不是输入框
}

void FindPassword::paintEvent(QPaintEvent *event)//初始化背景
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);//反锯齿
    QPixmap pixmap(":/Pictures/Resource/Pictures/FindPasswordBackground.png");
    painter.drawPixmap(0, 0, width(), height(), pixmap);
}


void FindPassword::closeEvent(QCloseEvent *event)//关闭窗口事件
{
    this->disconnect();
    //在关闭时调用 deleteLater
    this->deleteLater();
    //接受关闭事件
    event->accept();
}


void FindPassword::mousePressEvent(QMouseEvent *event)//点击窗口空白让所有输入框失去焦点
{
    ui->Pages1LineEdit->clearFocus();
    ui->Pages2LineEdit->clearFocus();
    ui->Pages3_1LineEdit->clearFocus();
    ui->Pages3_2LineEdit->clearFocus();
}

void FindPassword::on_Page1OkBtn_clicked()
{
    if(socket->state() != QAbstractSocket::ConnectedState){//没连接到服务器
        Dialog msgBox(this);
        msgBox.transText("请检查您的网络连接!");
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
        msgBox.exec();
        return;
    }
    jsonObj = QJsonObject();
    jsonObj["tag"] = "findpassword1";
    jsonObj["qq_number"] = ui->Pages1LineEdit->text();
    //发送消息
    jsonDoc = QJsonDocument(jsonObj);
    jsonData = jsonDoc.toJson();
    //添加标识符
    QByteArray messageWithSeparator = jsonData + "END";
    //发送JSON 数据
    socket->write(messageWithSeparator);
    socket->flush();
    jsonData.clear();
    qDebug() << "发送要找回的账号了";
}

void FindPassword::on_Page2OkBtn_clicked()
{
    if(socket->state() != QAbstractSocket::ConnectedState){//没连接到服务器
        Dialog msgBox(this);
        msgBox.transText("请检查您的网络连接!");
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
        msgBox.exec();
        return;
    }
    jsonObj = QJsonObject();
    jsonObj["tag"] = "findpassword2";
    jsonObj["qq_number"] = ui->Pages1LineEdit->text();
    jsonObj["theanswer"] = ui->Pages2LineEdit->text();
    //发送消息
    jsonDoc = QJsonDocument(jsonObj);
    jsonData = jsonDoc.toJson();
    //添加标识符
    QByteArray messageWithSeparator = jsonData + "END";
    //发送JSON 数据
    socket->write(messageWithSeparator);
    socket->flush();
    jsonData.clear();
    qDebug() << "发送密保答案了"<<messageWithSeparator;
}


void FindPassword::on_Page3OkBtn_clicked()
{
    if(socket->state() != QAbstractSocket::ConnectedState){//没连接到服务器
        Dialog msgBox(this);
        msgBox.transText("请检查您的网络连接!");
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
        msgBox.exec();
        return;
    }
    if(ui->Pages3_1LineEdit->text() != ui->Pages3_2LineEdit->text()){
        Dialog msgBox(this);
        msgBox.transText("您的两次密码不一致!");
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
        msgBox.exec();
        return;
    }
    jsonObj = QJsonObject();
    jsonObj["tag"] = "findpassword3";
    jsonObj["qq_number"]= ui->Pages1LineEdit->text();
    jsonObj["password"] = ui->Pages3_2LineEdit->text();
    //发送消息
    jsonDoc = QJsonDocument(jsonObj);
    jsonData = jsonDoc.toJson();
    //添加标识符
    QByteArray messageWithSeparator = jsonData + "END";
    //发送JSON 数据
    socket->write(messageWithSeparator);
    socket->flush();
    jsonData.clear();
    qDebug() << "发送修改后的密码了"<<messageWithSeparator;
}

void FindPassword::findPassword1(const QJsonObject &jsonObj)//看看有没有这个账号 对应切换页面或弹窗
{
    if(jsonObj["answer"] == "no"){
        Dialog msgBox(this);
        msgBox.transText("请检查您的账号!");
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
        msgBox.exec();
    }
    if(jsonObj["answer"] == "yes"){
        ui->PageFindStackedWidget->setCurrentIndex(1);
        ui->Page2QuestionLabel->setText(jsonObj["question"].toString());
    }
}


void FindPassword::findPassword2(const QJsonObject &jsonObj)//看看问题回答对了没有 对应切换页面或弹窗
{
    if(jsonObj["answer"] == "no"){
        Dialog msgBox(this);
        msgBox.transText("您的答案不正确!");
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
        msgBox.exec();
    }
    if(jsonObj["answer"] == "yes"){
        ui->PageFindStackedWidget->setCurrentIndex(2);
    }
}


void FindPassword::findPassword3(const QJsonObject &jsonObj)//修改密码成功没有 对应弹窗
{
    if(jsonObj["answer"] == "no"){
        Dialog msgBox(this);
        msgBox.transText("请求失败!");
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
        msgBox.exec();
    }
    if(jsonObj["answer"] == "yes"){
        QString text =  "找回密码成功";
        Dialog* dialog = new Dialog(this);
        dialog->transText(text);
        dialog->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog |Qt::FramelessWindowHint);
        dialog->exec();
        this->disconnect();
        this->close();
    }
}

void LineFind::focusInEvent(QFocusEvent *event)//获得焦点处理事件
{
    // 如果文本框显示的内容是默认提示文字 (placeholderText)，则清空文本框
    if (text().isEmpty()) {
        setPlaceholderText("");
    }

    // 调用父类的事件处理函数，确保正常的事件处理
    QLineEdit::focusInEvent(event);
}

void LineFind::focusOutEvent(QFocusEvent *event)//失去焦点处理事件
{
    if(text() == ""){
        if(objectName() == "Pages1LineEdit"){
            // setStyleSheet(
            //     "font: 13pt 'Microsoft YaHei UI'; "
            //     "border: 1px solid rgba(0, 0, 0, 0.3); "
            //     "border-radius: 8px; "
            //     "color: grey;"
            //     );
            setPlaceholderText("请输入你的账号");
        }
        else if(objectName() == "Pages2LineEdit"){
            // setStyleSheet(
            //     "font: 13pt 'Microsoft YaHei UI'; "
            //     "border: 1px solid rgba(0, 0, 0, 0.3); "
            //     "border-radius: 8px; "
            //     "color: grey;"
            //     );
            setPlaceholderText("请输入密保答案");
        }
        else if(objectName() == "Pages3_1LineEdit"){
            // setStyleSheet(
            //     "font: 13pt 'Microsoft YaHei UI'; "
            //     "border: 1px solid rgba(0, 0, 0, 0.3); "
            //     "border-radius: 8px; "
            //     "color: grey;"
            //     );
            setPlaceholderText("新的密码");
        }
        else if(objectName() == "Pages3_2LineEdit"){
            // setStyleSheet(
            //     "font: 13pt 'Microsoft YaHei UI'; "
            //     "border: 1px solid rgba(0, 0, 0, 0.3); "
            //     "border-radius: 8px; "
            //     "color: grey;"
            //     );
            setPlaceholderText("确认密码");
        }
    }
    QLineEdit::focusOutEvent(event);
}
