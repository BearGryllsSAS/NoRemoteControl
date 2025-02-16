#ifndef FINDPASSWORD_H
#define FINDPASSWORD_H

#include <QMainWindow>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QCloseEvent>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class FindPassword;
}

class FindPassword : public QMainWindow
{
    Q_OBJECT

public:
    explicit FindPassword(QTcpSocket* msocket, QWidget *parent = nullptr);
    ~FindPassword();

    // 取消第一个输入框的默认焦点设置
    void showEvent(QShowEvent *event);

    // 给输入框设置验证器
    void setVal();

    // 初始化背景
    void paintEvent(QPaintEvent *event);

    // 关闭窗口事件
    void closeEvent(QCloseEvent *event);

    // 点击窗口空白让所有输入框失去焦点
    void mousePressEvent(QMouseEvent *event);

private slots:
    void on_Page1OkBtn_clicked();

    void on_Page2OkBtn_clicked();

    void on_Page3OkBtn_clicked();

public slots:
    void findPassword1(const QJsonObject &jsonObj); // 看看有没有这个账号 对应切换页面或弹窗
    void findPassword2(const QJsonObject &jsonObj); // 看看问题回答对了没有 对应切换页面或弹窗
    void findPassword3(const QJsonObject &jsonObj); // 看看修改密码的结果 对应弹窗

private:
    Ui::FindPassword *ui;
    QRegularExpressionValidator *validator;
    QTcpSocket *socket;
    QJsonObject jsonObj;
    QJsonDocument jsonDoc;
    QByteArray jsonData;
};

class LineFind :public QLineEdit//输入文本框
{
    Q_OBJECT

public:
    LineFind(QWidget *parent = nullptr) : QLineEdit(parent) {}

public slots:
    void focusInEvent(QFocusEvent *event);//获得焦点处理事件
    void focusOutEvent(QFocusEvent *event);//失去焦点处理事件
};

#endif // FINDPASSWORD_H
