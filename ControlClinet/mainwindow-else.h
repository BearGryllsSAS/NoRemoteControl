#ifndef MAINWINDOW-ELSE_H
#define MAINWINDOW-ELSE_H


#include <QListView>
#include <QMouseEvent>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QPixmap>
#include <QSortFilterProxyModel>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>

// 好友列表类
class FriendList : public QListView
{
    Q_OBJECT

public:
    FriendList(QWidget *parent = nullptr) : QListView(parent) {}

    void mousePressEvent(QMouseEvent *event);//实现左右键都可选择
};

// 好友列表委托绘图类
class FriendDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    FriendDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;//好友列表项委托绘图
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;//返回尺寸
    QPixmap getRoundedPixmap(QPixmap srcPixmap, const int radius)const;//获得圆角图片
};

// 好友列表过滤代理模型
class FilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FilterProxyModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent) {
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent)const;//好友列表过滤代理模型判断是否接收
};

// 显示好友信息的edit类
class CustomTextEdit : public QTextEdit
{
public:
    CustomTextEdit(QWidget *parent = nullptr) : QTextEdit(parent) , background(":/Pictures/Resource/Pictures/FriendsShowBackground.jpg"){}
    QPixmap background;//背景图

protected:
    void paintEvent(QPaintEvent *event);//右边edit的背景
};

// 实现点击头像弹出好友信息窗口(好友申请内的)
class LabelFriendAva:public QLabel
{
    Q_OBJECT

public:
    LabelFriendAva(QWidget *parent=nullptr):QLabel(parent){}
    void mousePressEvent(QMouseEvent *event);//点击头像弹出好友个人信息
};

class LineSerach :public QLineEdit//搜索框类
{
    Q_OBJECT

public:
    LineSerach(QWidget *parent = nullptr) : QLineEdit(parent) {}

public slots:
    void focusInEvent(QFocusEvent *event);//获得焦点处理事件
    void focusOutEvent(QFocusEvent *event);//失去焦点处理事件

};

// 聊天列表过滤代理模型
class TalkFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit TalkFilterProxyModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent) {
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent)const;//聊天列表过滤代理模型判断是否接收
};

// 消息列表类
class TalkList : public QListView
{
    Q_OBJECT

public:
    TalkList(QWidget *parent = nullptr) : QListView(parent) {
    }

protected:
    void contextMenuEvent(QContextMenuEvent *event);//菜单

signals:
    void choiceDone(const QJsonObject &json);
};

class TalkDelegate : public QStyledItemDelegate//聊天列表委托绘图类
{
    Q_OBJECT
public:
    TalkDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;//聊天列表项委托绘图
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;//返回尺寸
    QPixmap getRoundedPixmap(QPixmap srcPixmap, const int radius)const;//获得圆角图片
    QString displayTimeComparison(const QString &timestampStr)const;//解析时间戳
};

class EnterTextEdit : public QTextEdit {
    Q_OBJECT

public:
    EnterTextEdit(QWidget *parent = nullptr) : QTextEdit(parent) {}

protected:
    void keyPressEvent(QKeyEvent *event) override;//按下判断是不是回车加ctrl

signals:
    void enterKey();//回车和ctrl键被按下
};

class TalkStacked : public QStackedWidget//显示聊天信息的类
{
public:
    TalkStacked(QWidget *parent = nullptr): QStackedWidget(parent), background(":/Pictures/Resource/Pictures/TalkStackWidgetBackground.jpg"){};
    QPixmap background;//背景图

protected:
    void paintEvent(QPaintEvent *event);//右边edit的背景

};


#endif // MAINWINDOW-ELSE_H
