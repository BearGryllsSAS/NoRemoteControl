#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindow-else.h"
#include "changepassword.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QTcpSocket>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


// 好友列表过滤模型
class FilterProxyModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    struct AccountInfo {//存储好友信息
        QString account;//账号
        QString name;//名称
        QString avator_base64;//头像的 Base64 编码字符串
        QString gender;//性别
        QString signature;//签名
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 初始化头像
    void initAvatar();

    // 设置好友列表模型
    void setupFriendList();

    // 设置聊天列表模型
    void setupTalkList();

    // 清理右边好友信息视图
    void clearEditShow2();

    // 获得圆角图片
    QPixmap getRoundedPixmap(const QPixmap srcPixmap, const int radius);

    // 接受好友申请
    void acceptAddFriends(const QString &account, const QString &sender);

    // 拒绝好友申请
    void rejectAddFriends(const QString &account, const QString &sender);

    // 检查是不是连接上服务器了
    bool checkSocket();

    // 给某个好友发送消息
    void sendMessageToFriend(const QString &account);

    // 删除好友
    void deleteFriend();

    // 向聊天列表加入某个好友
    int addSomeoneToTalkList(const AccountInfo &friendMessage, QString message, QString date, QString unread);

    // 打开某个人的聊天框
    int selectSomeoneInTalkList(const QString &account);

    // 清空某人的未读消息并更新左边消息列表视图
    void clearUnread(const QString& friendId);

    // 将聊天页面切换到账号为account的页面
    bool switchPageTo(const QString &friendId);

    // 设置左下角按钮菜单
    void setupMenu();

private slots:
    void on_CloseMainWindowBtn_clicked();
    // 好友列表页面项切换项更新右边视图
    void onFriendItemCurrentChanged();
    // 更新右边好友信息视图(好友申请)
    void updateEditShow2New();
    // 更新右边好友信息视图
    void updateEditShow2Normal(const QModelIndex &index);
    // 聊天页面项切换项更新消息框视图
    void onTalkItemCurrentChanged();
    // 收到修改密码窗口的信号 发送信息
    void changePassword1(const QJsonObject &json);
    // 收到修改密码窗口的信号 发送最终的信息
    void changePassword2(const QJsonObject &json);

private:
    Ui::MainWindow *ui;
    // 好友列表项模型
    QStandardItemModel *friendModel;
    // 好友列表筛选模型
    FilterProxyModel *filterProxyModel;
    //好友申请列表
    QVector<AccountInfo> newFriendArray;
    // 好友列表主布局
    QVBoxLayout *newLayout;
    // 用户自己的信息
    AccountInfo myInfo;

    QTcpSocket *socket;

    QString deleteFriendNum;

    // 以账号为索引存储好友列表
    QHash<QString, AccountInfo> friendHash;

    // 聊天列表项模型
    QStandardItemModel *talkModel;

    // 用户的数据库
    QSqlDatabase db;

    // 以账号为索引存储消息列表
    QHash<QString, AccountInfo> messageListHash;

    // 聊天列表筛选模型
    TalkFilterProxyModel *talkFilterProxyModel;

    // 左下角菜单栏
    QMenu *toolMenu;

    // 是否已经打开修改密码窗体了
    int changePassFlag = 0;

    // 修改密码窗口
    ChangePassword *dialogChangePass;

signals:
    void changePasswordAnswer1(const QJsonObject& json);//修改账号密码第一个申请的结果
    void changePasswordAnswer2(const QJsonObject& json);//修改账号密码第一个申请的结果
};
#endif // MAINWINDOW_H
