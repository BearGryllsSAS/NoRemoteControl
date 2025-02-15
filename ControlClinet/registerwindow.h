#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include "cutavator.h"
#include <QMainWindow>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QCloseEvent>
#include <QTcpSocket>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QLineEdit>
#include <QLabel>

namespace Ui {
class RegisterWindow;
}

class RegisterWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RegisterWindow(QTcpSocket *msocket, QWidget *parent = nullptr);
    ~RegisterWindow();

    // 给输入框设置验证器
    void setVal();

    // 设置默认头像
    void setAva();

    // 给图像裁剪成圆形
    QPixmap createCircularPixmap(const QString &imagePath);

    // 初始化背景
    void paintEvent(QPaintEvent *event);

    // 切割圆角图片
    QPixmap getRoundedPixmap(const QPixmap srcPixmap, const float radiusRatio);

    // 关闭事件
    void closeEvent(QCloseEvent *event);

    // 点击窗口空白让所有输入框失去焦点
    void mousePressEvent(QMouseEvent *event);

    // 收到用户选择的头像然后做相应处理
    void receiveAvator(const QPixmap &pixmap);

    // 发送头像给服务器
    void sendAva();

    // 注册成功
    void chengGong(QString qqnum);

    // 注册失败
    void shiBai();

    // 判断是否可以点击提交按钮提交注册申请
    void isCanSubmit();

    // 取消第一个输入框的默认焦点设置
    void showEvent(QShowEvent *event);

private slots:
    void on_AnswerLineEdit_textChanged(const QString &arg1);

    void on_SexComboBox_currentIndexChanged(int index);

    void on_NicknameLineEdit_textChanged(const QString &arg1);

    void on_PasswordLineEdit_1_textChanged(const QString &arg1);

    void on_PasswordLineEdit_2_textChanged(const QString &arg1);

    void on_QuestionLineEdit_textChanged(const QString &arg1);

    void on_SubmitBtn_clicked();

private:
    Ui::RegisterWindow *ui;
    QRegularExpressionValidator *validator;
    QTcpSocket *socket;
    QJsonObject jsonObj;
    QJsonDocument jsonDoc;
    QByteArray jsonData;
};

class LineRegis :public QLineEdit {
    Q_OBJECT

public:
    LineRegis(QWidget *parent = nullptr) : QLineEdit(parent) {}

public slots:
    void focusInEvent(QFocusEvent *event);//获得焦点处理事件
    void focusOutEvent(QFocusEvent *event);//失去焦点处理事件
};

class LabelRegis:public QLabel//点击头像更换头像
{
    Q_OBJECT

public:
    LabelRegis(QWidget *parent=nullptr):QLabel(parent){}
    void mousePressEvent(QMouseEvent *event);//点击头像更换头像
};

#endif // REGISTERWINDOW_H
