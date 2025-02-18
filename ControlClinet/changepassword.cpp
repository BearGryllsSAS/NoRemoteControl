#include "changepassword.h"
#include "ui_changepassword.h"
#include "dialog.h"
#include <QRegularExpression>
#include <QPainter>
#include <QPainterPath>
#include <QJsonObject>



ChangePassword::~ChangePassword()
{
    delete ui;
}

ChangePassword::ChangePassword(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChangePassword)
{
    ui->setupUi(this);

    //初始化窗口
    QIcon icon(":/Pictures/Resource/Pictures/ChangePasswordIcon.png");
    setWindowIcon(icon);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    setVal();

    // 将窗口居中显示
    QWidget *topLevelParent = parent ? parent->window() : nullptr;
    if (topLevelParent) {
        int x = topLevelParent->x() + (topLevelParent->width() - width()) / 2;
        int y = topLevelParent->y() + (topLevelParent->height() - height()) / 2;
        move(x, y);
    }
}

void ChangePassword::setVal()//设置输入验证器
{
    QRegularExpression regExp("^[a-zA-Z0-9]{1,12}$");//给输入框设置验证器
    validator = new QRegularExpressionValidator(regExp, this);
    ui->AccountLineEdit->setValidator(validator);
    ui->PasswordLineEdit->setValidator(validator);
    ui->NewPasswordLineEdit->setValidator(validator);
    ui->NewPassword2LineEdit->setValidator(validator);
}

void ChangePassword::paintEvent(QPaintEvent *event)//窗口背景
{
    //调用基类方法以处理父类的绘图
    QDialog::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);//反锯齿
    //加载背景图片
    QPixmap background(":/Pictures/Resource/Pictures/ChangePasswordBackground.png");
    //确保图片已加载
    if (background.isNull()) {
        qWarning("背景图像加载失败");
        return;
    }
    QSize newSize = QSize(this->width(), this->height());
    //将背景图缩放为新的大小
    QPixmap scaledBackground = background.scaled(newSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    //计算绘制的位置，保持居中
    int x = (this->width() - scaledBackground.width()) / 2;
    int y = (this->height() - scaledBackground.height()) / 2;
    //设置剪裁区域以适应圆角窗口
    int radius = 10;
    QPainterPath path;
    path.addRoundedRect(rect(), radius, radius);
    painter.setClipPath(path);
    //绘制背景图片
    painter.drawPixmap(x, y, scaledBackground);
    //绘制边框
    painter.setPen(QPen(Qt::gray, 3.5));
    painter.drawRoundedRect(rect(), radius, radius);
}

void ChangePassword::changePasswordAns1(const QJsonObject& json)//修改账号密码第一个申请的结果
{
    if(json ["answer"] == "fail"){
        ui->PasswordLineEdit->setEnabled(true);
        ui->AccountLineEdit->setEnabled(true);
        ui->OkBtn->setEnabled(true);
        QString text =  "请求失败。";
        Dialog* dialog = new Dialog(this);
        dialog->transText(text);
        dialog->exec();
        return;

    }
    else if(json ["answer"] == "user_not_found"){
        ui->PasswordLineEdit->setEnabled(true);
        ui->AccountLineEdit->setEnabled(true);
        ui->OkBtn->setEnabled(true);
        QString text =  "账号或密码错误，请检查。";
        Dialog* dialog = new Dialog(this);
        dialog->transText(text);
        dialog->exec();
        return;
    }
    else if(json ["answer"] == "succeed"){
        ui->stackedWidget->setCurrentIndex(1);
    }
}


void ChangePassword::changePasswordAns2(const QJsonObject& json)//修改账号密码第二个申请的结果
{
    if(json ["answer"] == "fail"){
        QString text =  "请求失败。";
        Dialog* dialog = new Dialog(this);
        dialog->transText(text);
        dialog->exec();
        this->close();
        return;

    }
    else if(json ["answer"] == "succeed"){
        QString text =  "修改成功!";
        Dialog* dialog = new Dialog(this);
        dialog->transText(text);
        dialog->exec();
        this->close();
        return;
    }
}

void ChangePassword::mousePressEvent(QMouseEvent *event)//捕获鼠标按下事件
{
    //清除焦点
    QList<QWidget*> widgets = this->findChildren<QWidget*>();
    for (QWidget* widget : widgets) {
        widget->clearFocus();
    }
    QPoint pos = event->pos();
    int margin = 30;
    if (pos.x() <= 30 || pos.x() >= width() - 30 ||
        pos.y() <= 30 || pos.y() >= height() - 30){//如果点击在边缘
        qDebug() << "点击在边缘";
        moveFlag = 1;
        dragPosition = event->globalPosition().toPoint() - this->frameGeometry().topLeft();//记录鼠标相对于窗口的初始偏移
        event->accept();
    }
}

void ChangePassword::mouseMoveEvent(QMouseEvent *event)//实现拖拽移动效果与调整大小效果
{
    qDebug()<<"鼠标位移了";
    QPoint globalPos = event->globalPosition().toPoint();//获取当前的鼠标位置
    QPoint delta = globalPos - (this->frameGeometry().topLeft() + dragPosition);//计算当前鼠标位置与窗口之间的相对偏移
    if (event->buttons() & Qt::LeftButton) {
        if (moveFlag == 1) {
            this->move(globalPos - dragPosition);
            event->accept();
        }
        //更新拖动位置
        dragPosition = globalPos - this->frameGeometry().topLeft();
    }
}


void LineChangePass::focusInEvent(QFocusEvent *event)//获得焦点处理事件
{
    if(text() == "请输入您的账号" || text() == "请输入您的密码" || text() == "请输入新的密码" || text() == "请确认您的密码"){
        setText("");
        setStyleSheet("font: 12pt 'Microsoft YaHei UI';"
                      "border: 1px solid rgba(0, 0, 0, 0.1);"
                      "border-radius: 8px;"
                      "padding: 5px;"
                      "color: black;");
    }
    if(objectName() == "line_newpassword" || objectName() == "line_newpassword2" || objectName() == "line_password"){
        this->setEchoMode(Password);
    }
    QLineEdit::focusInEvent(event);
}


void LineChangePass::focusOutEvent(QFocusEvent *event)//失去焦点处理事件
{
    if(text() == ""){
        if(objectName() == "line_accont")
        {
            setStyleSheet("font: 12pt 'Microsoft YaHei UI';"
                          "border: 1px solid rgba(0, 0, 0, 0.1);"
                          "border-radius: 8px;"
                          "padding: 5px;"
                          "color: grey;");
            setText("请输入您的账号");
        }
        if(objectName() == "line_password")
        {
            setStyleSheet("font: 12pt 'Microsoft YaHei UI';"
                          "border: 1px solid rgba(0, 0, 0, 0.1);"
                          "border-radius: 8px;"
                          "padding: 5px;"
                          "color: grey;");
            setText("请输入您的密码");
            setEchoMode(Normal);
        }
        if(objectName() == "line_newpassword")
        {
            setStyleSheet("font: 12pt 'Microsoft YaHei UI';"
                          "border: 1px solid rgba(0, 0, 0, 0.1);"
                          "border-radius: 8px;"
                          "padding: 5px;"
                          "color: grey;");
            setText("请输入新的密码");
            setEchoMode(Normal);
        }
        if(objectName() == "line_newpassword2")
        {
            setStyleSheet("font: 12pt 'Microsoft YaHei UI';"
                          "border: 1px solid rgba(0, 0, 0, 0.1);"
                          "border-radius: 8px;"
                          "padding: 5px;"
                          "color: grey;");
            setText("请确认您的密码");
            setEchoMode(Normal);
        }
    }
    QLineEdit::focusOutEvent(event);
}

void ChangePassword::on_pushButton_clicked()
{
    this->close();
}

