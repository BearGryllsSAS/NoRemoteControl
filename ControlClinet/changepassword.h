#ifndef CHANGEPASSWORD_H
#define CHANGEPASSWORD_H

#include <QDialog>
#include <QLineEdit>
#include <QValidator>
#include <QMouseEvent>

namespace Ui {
class ChangePassword;
}

class ChangePassword : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePassword(QWidget *parent = nullptr);
    ~ChangePassword();

    // 设置输入验证器
    void setVal();

    // 窗口背景
    void paintEvent(QPaintEvent *event);

    // 捕获鼠标按下事件
    void mousePressEvent(QMouseEvent *event);

    // 实现拖拽移动效果与调整大小效果
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void on_pushButton_clicked();
public slots:
    void changePasswordAns1(const QJsonObject& json);//修改账号密码第一个申请的结果
    void changePasswordAns2(const QJsonObject& json);//修改账号密码第二个申请的结果

private:
    Ui::ChangePassword *ui;
    QValidator *validator;
    int moveFlag = 0;
    QPoint dragPosition;

signals:
    void customClose();//通知主窗体要关闭了
    void changePassword1(const QJsonObject &json);  // 告知主窗体发送修改申请
    void changePassword2(const QJsonObject &json);  // 告知主窗体发送最终修改申请
};

class LineChangePass:public QLineEdit//输入框类
{
    Q_OBJECT

public:
    LineChangePass(QWidget *parent = nullptr) : QLineEdit(parent) {}

public slots:
    void focusInEvent(QFocusEvent *event);//获得焦点处理事件
    void focusOutEvent(QFocusEvent *event);//失去焦点处理事件
};

#endif // CHANGEPASSWORD_H
