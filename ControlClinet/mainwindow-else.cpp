#include "mainwindow-else.h"
#include "friendmessage.h"
#include <QPainterPath>
#include <QMenu>
#include <QJsonObject>

// 实现左右键都可选择
void FriendList::mousePressEvent(QMouseEvent *event)
{
    QListView::mousePressEvent(event);
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
            setCurrentIndex(index);
            clicked(index);
        }
    }
}


// 好友列表项委托绘图
void FriendDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) return;
    //保存绘图状态
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);//反锯齿
    painter->setRenderHint(QPainter::TextAntialiasing);//文本反锯齿
    //获取名称
    QString name = index.data(Qt::DisplayRole).toString();
    //不同条件下绘制背景
    QColor backgroundColor;
    if (option.state & QStyle::State_Selected) {
        backgroundColor = QColor(255, 220, 240);//选择则粉色
    } else if (option.state & QStyle::State_MouseOver &&index.row() != 0 && index.row() != 2) {
        backgroundColor = QColor(240, 240, 240);//鼠标悬浮则灰色
    } else {
        backgroundColor = option.palette.window().color();
    }
    if(index.row() == 1 && index.data(Qt::UserRole + 20) == true){//表示有未读好友申请
        backgroundColor = QColor(255, 105, 180);
    }
    //绘制带圆角的背景
    painter->setBrush(backgroundColor);
    painter->setPen(Qt::NoPen);//不绘制边框
    painter->drawRoundedRect(option.rect, 10, 10);//10是圆角半径
    //获取图标
    QString base64avator = index.data(Qt::UserRole + 4).toString();
    QByteArray imageData = QByteArray::fromBase64(base64avator.toUtf8());
    QImage image;
    bool imageLoaded = image.loadFromData(imageData);
    if (imageLoaded) {
        image = image.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPixmap pixmap = QPixmap::fromImage(image);
        pixmap = getRoundedPixmap(pixmap, 5);
        //绘制图标
        int y = option.rect.y() + (option.rect.height() - 40) / 2;//计算Y坐标
        QRect iconRect(option.rect.x() + 5, y, 40, 40);//调整图标矩形的大小
        painter->drawPixmap(iconRect, pixmap);
    }
    //设置字体和颜色
    QColor textColor = option.palette.text().color();//默认文字颜色
    QFont font = painter->font();//获取默认字体
    //检查索引
    if (index.row() == 0 || index.row() == 2) {
        textColor = Qt::gray;//设置为灰色
        font.setPointSize(10);//设置字号
    }
    painter->setFont(font);//应用字体设置
    painter->setPen(textColor);//应用文字颜色
    //绘制文字，留出图标空间
    if (imageLoaded) {
        painter->drawText(option.rect.adjusted(50, 0, 0, 0), Qt::AlignVCenter | Qt::TextSingleLine, name);//有头像
    } else {
        painter->drawText(option.rect.adjusted(5, 0, 0, 0), Qt::AlignVCenter | Qt::TextSingleLine, name);//无头像
    }
    //恢复绘图状态
    painter->restore();
}


// 返回尺寸
QSize FriendDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int row = index.row();
    if(row == 2 || row == 0) {
        return QSize(170, 20);
    }
    else
        return QSize(170, 56);
}

// 获得圆角图片
QPixmap FriendDelegate::getRoundedPixmap(QPixmap srcPixmap, const int radius)const
{
    //目标图片尺寸
    QSize desSize = srcPixmap.size();
    //新建一个目标大小的画布Qpixmap
    QPixmap desPixMap(desSize);
    //填充透明色作为背景
    desPixMap.fill(Qt::transparent);
    //以QPixmap 为绘画背景进行画笔绘制
    QPainter painter(&desPixMap);
    painter.setRenderHints(QPainter::Antialiasing);//抗锯齿
    painter.setRenderHints(QPainter::SmoothPixmapTransform);//平滑像素图变换
    QPainterPath path;//绘制路径
    //绘制圆角矩形，其中最后两个参数值的范围为（0-99），就是圆角的px值
    path.addRoundedRect(0, 0, desSize.width(), desSize.height(), radius, radius);
    //将绘制的圆角矩形路径中内容进行裁剪
    painter.setClipPath(path);
    //将图片绘制到desPixmap中，IgnoreAspectRatio忽视图片比例
    painter.drawPixmap(0, 0, desSize.width(), desSize.height(), srcPixmap.scaled(desSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    painter.setClipping(false);//关闭裁剪
    return desPixMap;
}

// 好友列表过滤代理模型判断是否接收
bool FilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (sourceRow < 3) {
        return true;
    }
    const QString filter = filterRegularExpression().pattern();
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QString nickname = index.data(Qt::DisplayRole).toString();
    QString account = index.data(Qt::UserRole + 1).toString();
    bool accepts = nickname.contains(filter, Qt::CaseInsensitive) ||
                   account.contains(filter, Qt::CaseInsensitive);
    return accepts;
}

// 右边edit的背景
void CustomTextEdit::paintEvent(QPaintEvent *event)
{
    //调用基类方法
    QTextEdit::paintEvent(event);
    //创建QPainter对象
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);//反锯齿
    //计算窗口的最大边长和背景的最小边长
    int maxSize = qMax(viewport()->width(), viewport()->height());
    //将背景图缩放为正方形的大小
    QPixmap scaledBackground = background.scaled(maxSize, maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    //计算将背景图居中的位置
    int x = (viewport()->width() - scaledBackground.width()) / 2;
    int y = (viewport()->height() - scaledBackground.height()) / 2;
    //绘制背景图片
    painter.drawPixmap(x, y, scaledBackground);
    //创建一个蒙版，设置透明度
    painter.setPen(Qt::transparent);
    painter.setBrush(QColor(255, 255, 255, 150));
    painter.drawRect(0, 0, viewport()->width(), viewport()->height());
}

// 点击头像弹出好友个人信息
void LabelFriendAva::mousePressEvent(QMouseEvent *event)
{
    FriendMessage::FriendInfo info;
    info.avator_base64 = property("avator").toString();
    info.account = property("account").toString();
    info.name = property("nickname").toString();
    info.signature = property("signature").toString();
    info.gender = property("gender").toString();
    FriendMessage *dialog = new FriendMessage(info,this);
    dialog->exec();
}

void LineSerach::focusInEvent(QFocusEvent *event)//获得焦点处理事件
{
    setStyleSheet("font: 10pt 'Microsoft YaHei UI'; "
                  "border-radius: 3px; "
                  "background:white;"
                  "color: black;"
                  "padding: 5px;");
    if(text() == "搜索"){
        setText("");
    }
    QLineEdit::focusInEvent(event);
}


void LineSerach::focusOutEvent(QFocusEvent *event)//失去焦点处理事件
{
    setStyleSheet("font: 10pt 'Microsoft YaHei UI'; "
                  "border-radius: 3px; "
                  "background:rgb(245, 245, 245);"
                  "color: grey;"
                  "padding: 5px;");
    if (text().isEmpty()) {
        setText("搜索");
    }
    QLineEdit::focusOutEvent(event);
}

bool TalkFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const//聊天列表过滤代理模型判断是否接收
{
    const QString filter = filterRegularExpression().pattern();
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    QString nickname = index.data(Qt::DisplayRole).toString();
    QString account = index.data(Qt::UserRole + 1).toString();
    bool accepts = nickname.contains(filter, Qt::CaseInsensitive) ||
                   account.contains(filter, Qt::CaseInsensitive);
    return accepts;
}

void TalkList::contextMenuEvent(QContextMenuEvent *event)//菜单
{
    //获取当前点击的索引
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        //设置当前选中的项
        setCurrentIndex(index);
    }
    //创建上下文菜单
    QMenu menu(this);
    QJsonObject json;
    emit choiceDone(json);//发送空 先触发点击事件
    menu.setStyleSheet(
        "QMenu {"
        "   background-color: rgb(240, 240, 240);"
        "   border-radius: 3px;"
        "   border: 0.5px solid rgb(0, 0, 0);"
        "}"
        "QMenu::item {"
        "   padding: 10px;"
        "   text-align: left;"
        "}"
        "QMenu::item:selected {"
        "   background-color: rgb(200, 200, 200);"
        "}"
        "QMenu::item:pressed {"
        "   background-color: rgb(180, 180, 180);"
        "}");
    //添加菜单项
    QAction *action1 = menu.addAction("发送消息");
    QAction *action2 = menu.addAction("查看资料");
    QAction *action3 = menu.addAction("删除好友");
    QAction *action4 = menu.addAction("移除消息列表");
    QAction *action5 = menu.addAction("清空聊天记录");
    // 执行菜单并获取用户选择
    QAction *selectedAction = menu.exec(event->globalPos());
    if (selectedAction) {
        if (selectedAction == action1) {
            //处理发送消息的逻辑
            json["tag"] = "sendmessage";
        } else if (selectedAction == action2) {
            //处理查看资料的逻辑
            json["tag"] = "message";
        } else if (selectedAction == action3) {
            //处理删除好友的逻辑
            json["tag"] = "deletefriend";
        } else if (selectedAction == action4) {
            //处理移除消息列表的逻辑
            json["tag"] = "deletetalk";
        } else if (selectedAction == action5) {
            //处理清空聊天记录的逻辑
            json["tag"] = "clearmessage";
        }
        emit choiceDone(json);
    }
}
