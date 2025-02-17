#ifndef FRIENDMESSAGE_H
#define FRIENDMESSAGE_H

#include <QDialog>
#include <QMouseEvent>

namespace Ui {
class FriendMessage;
}


class FriendMessage : public QDialog
{
    Q_OBJECT

public:
    struct FriendInfo {//存储好友信息
        QString account;
        QString name;
        QString avator_base64;
        QString gender;
        QString signature;
    };

public:
    explicit FriendMessage(const FriendInfo &info, QWidget *parent = nullptr);
    ~FriendMessage();

    // 设置当前页面各信息
    void setMessage(const FriendInfo &info);

    // 将base64转换为图片
    QPixmap base64ToPixmap(const QString &base64Str);

    // 图片转换成base64格式
    QString pixmapToBase64(const QPixmap &pixmap);

    // 窗口背景
    void paintEvent(QPaintEvent *event);

    // 捕获鼠标按下事件
    void mousePressEvent(QMouseEvent *event);

    // 实现拖拽移动效果与调整大小效果
    void mouseMoveEvent(QMouseEvent *event);

    // 重置移动状态
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void on_CloseFriendMessageBtn_clicked();

private:
    Ui::FriendMessage *ui;
    // 存储好友信息
    FriendInfo friendInfo;
    int moveFlag = 0;
    QPoint dragPosition;
};

#endif // FRIENDMESSAGE_H
