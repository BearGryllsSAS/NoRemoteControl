#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QRegularExpressionValidator>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>


namespace Ui {
class Login;
}

class Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

    // 初始化头像
    void setAvatar();

    // 初始化左上角图标
    void setIcon();

    // 设置输入验证器，确保只能在输入框输入字母和数字
    void setValidator();

    // 绘制渐变背景，当窗口的 update() 函数被调用后，该函数会自动调用
    void paintEvent(QPaintEvent *event);

    // 设置定时器，以实现动态背景、动态按钮文本更新、以及登录请求的定时发送
    void setTimer();

    // 连接服务器
    bool tcpConnect();

    // 判断是否需要自动登录以及记住密码
    void ifAutoLogin();

    // 初始设置输入框格式
    void setSty();

    // 判断上次登录的时候是否记住了密码
    void ifRememberPassword();

    // 登录失败
    void dengLuShiBai();

    // 登录成功
    void dengLuChengGong();

    // 记住头像
    void rememberAvator();

    // 保存登录成功的头像
    void saveAvator(QString avator);

    // 窗口关闭事件
    void closeEvent(QCloseEvent *event);

public:
    QString saveLoginAvatorPath;    //保存上次登录成功的人的头像 用来初始化界面

    QRegularExpressionValidator *validator;     // 正则表达式验证器

    int offset = 0;     // 渐变尺度

    QTimer *cancelTimer;    // 登录请求定时器

    bool cancelRequested;   // 登录请求的状态

    int dotCount = 0;       // 用于更新文本的点计数

    QTimer *textUpdateTimer;    // 登录时用于更新按钮文本的计时器

    /*存储
    /  autologin: 是否启用自动登录的布尔值。
    /  autologinuser: 自动登录的用户名。加载后去setting中找密码
    */
    QSettings set;

    /*存储
    /  lastlogin: 上次成功登录的用户名。如果有说明则有人记住密码了 没有则说明没人记住密码 不必理会
    /  账号/password: 与特定用户相关联的密码。分组存储 组名是账号
    */
    QSettings settings;



private slots:
    void on_CloseTheWindowBtn_clicked();

    // 更新渐变背景
    void updateGradient();

    // 发送登录请求
    void sendLogin();

    // 更新取消按钮的文本 模拟动态效果
    void updateLoginText();

    // 读取并解析服务器中发送过来的消息，然后做出响应
    void onReadyRead();


    void on_LoginBtn_clicked();

private:
    Ui::Login *ui;
    // tcp 连接套接字
    QTcpSocket socket;
    QJsonObject jsonObj;
    QJsonDocument jsonDoc;
    // 存储从服务器中读取到的数据
    QByteArray jsonData;
    //是不是正在登录
    bool loginFlag = false;
};

#endif // LOGIN_H
