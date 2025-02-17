#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "choicedialog.h"
#include "autocleartextedit.h"
#include "dialog.h"
#include <QStandardPaths>
#include <QDir>
#include <QBuffer>
#include <QPainterPath>
#include <QSqlQuery>

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //初始化窗口
    setWindowIcon(QIcon(":/Pictures/Resource/Pictures/MainWindowIcon.png"));
    // setWindowFlags(Qt::FramelessWindowHint);
    // setAttribute(Qt::WA_TranslucentBackground);

    // 初始化头像
    initAvatar();

    // 设置好友列表模型
    setupFriendList();


}

void MainWindow::initAvatar()//初始化头像
{
    //获取应用程序的专用目录
    QString appDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString saveLoginAvatorPath = dir.filePath("laseloginavator.png");
    QPixmap pixmap(saveLoginAvatorPath);
    if (pixmap.isNull()) {
        pixmap = QPixmap(":/Pictures/Resource/Pictures/DefaultAvatarMainWindow.jpg");
    } else {}
    ui->AvatorLabel->setPixmap(pixmap);
    ui->AvatorLabel->setScaledContents(true);
    ui->AvatorLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

void MainWindow::setupFriendList()//设置好友列表模型
{
    //设置列表
    ui->FriendsList->setViewMode(QListView::ListMode);//设置视图模式为列表模式
    ui->FriendsList->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选择行为为选择整行
    ui->FriendsList->setSelectionMode(QAbstractItemView::SingleSelection);//设置选择模式为单选
    ui->FriendsList->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止编辑
    ui->FriendsList->setItemDelegate(new FriendDelegate(this));//设置自定义的委托用于绘制列表项
    //设置项
    friendModel = new QStandardItemModel(this);
    ui->FriendsList->setModel(friendModel);
    QStandardItem *addFriendItem0 = new QStandardItem("新的朋友");
    QStandardItem *addFriendItem = new QStandardItem("新的朋友");
    QStandardItem *friendListItem = new QStandardItem("好友列表");
    //设置不可选标志
    addFriendItem0->setFlags(Qt::NoItemFlags);
    addFriendItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    friendListItem->setFlags(Qt::NoItemFlags);
    //获取代表添加好友的图标
    //转换为 QPixmap，指定图标的大小
    QPixmap addFriendPixmap(":/pictures/addfriends.jpg"); // 32x32 像素的图标
    //将 QPixmap 转换为 Base64
    QImage image = addFriendPixmap.toImage();
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG"); // 将图像保存为 PNG 格式
    QByteArray byteArray = buffer.data();
    QString base64String = byteArray.toBase64();
    //将 Base64 字符串设置到 addFriendItem 中
    addFriendItem->setData(base64String, Qt::UserRole + 4);
    //将项添加到模型中
    friendModel->appendRow(addFriendItem0);
    friendModel->appendRow(addFriendItem);
    friendModel->appendRow(friendListItem);
    //初始化模型
    filterProxyModel = new FilterProxyModel(this);
    filterProxyModel->setSourceModel(friendModel);
    //设置模型到列表视图
    ui->FriendsList->setModel(filterProxyModel);
    //连接信号与槽
    connect(ui->FriendsList->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::onFriendItemCurrentChanged);
}

void MainWindow::onFriendItemCurrentChanged()//好友列表页面项切换项更新右边视图
{
    auto index = ui->FriendsList->currentIndex();
    if (!index.isValid()) {
        clearEditShow2();
        QPixmap pix(":/Pictures/Resource/Pictures/FriendsShowBackground.jpg");
        ui->FriendsShowTextEdit->background = pix;
        ui->FriendsShowTextEdit->repaint();
        return;
    }
    int row = index.row();
    if (row == 0 || row == 2) return;
    else if (row == 1) {//选择新的好友的操作 查看未处理的好友申请
        updateEditShow2New();
        return;
    }
    updateEditShow2Normal(index);//普通好友
}

void MainWindow::clearEditShow2()//清理右边好友信息视图
{
    //清理旧的布局和控件
    QLayout *existingLayout = ui->FriendsShowTextEdit->layout();
    if (existingLayout) {
        QLayoutItem *item;
        while ((item = existingLayout->takeAt(0)) != nullptr) {
            QWidget *widget = item->widget();
            if (widget) {
                widget->deleteLater();//删除控件
            }
            delete item;//删除布局项
        }
    }
    for (QWidget *widget : ui->FriendsShowTextEdit->findChildren<QWidget*>()) {
        if (qobject_cast<QPushButton*>(widget) || qobject_cast<QLabel*>(widget)) {
            //检查是否是 QPushButton 或 QLabel
            widget->deleteLater();//删除按钮或标签
        }
    }
}

// 更新右边好友信息视图(好友申请)
void MainWindow::updateEditShow2New()
{
    if(ui->FriendsList->currentIndex().row() == 1){
        QModelIndex secondItemIndex = ui->FriendsList->model()->index(1, 0);
        ui->FriendsList->model()->setData(secondItemIndex, false, Qt::UserRole + 20);
        ui->FriendsList->model()->dataChanged(secondItemIndex, secondItemIndex, {Qt::UserRole + 20});
    }
    clearEditShow2();//清理旧控件
    // 把右边的edit背景重绘为默认
    QPixmap pix(":/Pictures/Resource/Pictures/FriendsShowBackground.jpg");
    ui->FriendsShowTextEdit->background = pix;
    ui->FriendsShowTextEdit->repaint();
    if(newFriendArray.empty()){
        QLabel *label = new QLabel(this);
        label->setText("目前没有好友申请哦。");
        label->setStyleSheet("font-size: 22px; color: #333333; font-weight: bold;");
        newLayout->addWidget(label, Qt::AlignCenter);
    }
    for (const auto &friendRequest : newFriendArray) {//遍历添加好友申请用户
        QString friendNickname = friendRequest.name;
        QString friendavatorBase64 = friendRequest.avator_base64;
        QByteArray avatarByteArray = QByteArray::fromBase64(friendavatorBase64.toUtf8());
        QImage avatarImage;
        avatarImage.loadFromData(avatarByteArray);
        QPixmap avatarPixmap = QPixmap::fromImage(avatarImage);
        avatarPixmap = getRoundedPixmap(avatarPixmap, 5);
        QHBoxLayout *requestLayout = new QHBoxLayout();
        requestLayout->setContentsMargins(0, 0, 0, 0);
        requestLayout->setSpacing(10);
        requestLayout->setSizeConstraint(QLayout::SetMinimumSize);
        //添加头像
        LabelFriendAva *avatorLabel = new LabelFriendAva(this);
        avatorLabel->setPixmap(avatarPixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        avatorLabel->setFixedSize(60, 60);
        avatorLabel->setScaledContents(true);
        avatorLabel->setCursor(Qt::PointingHandCursor);
        requestLayout->addWidget(avatorLabel);
        requestLayout->setAlignment(avatorLabel, Qt::AlignLeft);
        //给头像添加申请人信息
        avatorLabel->setProperty("avator",QVariant(friendRequest.avator_base64));
        avatorLabel->setProperty("account",QVariant(friendRequest.account));
        avatorLabel->setProperty("nickname",QVariant(friendRequest.name));
        avatorLabel->setProperty("signature",QVariant(friendRequest.signature));
        avatorLabel->setProperty("gender",QVariant(friendRequest.gender));
        //添加名称
        QLabel *nicknameLabel = new QLabel(friendNickname);
        nicknameLabel->setStyleSheet("font-size: 16px; color: #333333;");
        requestLayout->addWidget(nicknameLabel);
        requestLayout->setAlignment(nicknameLabel, Qt::AlignLeft);
        QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        requestLayout->addItem(spacer);
        //接受按钮
        QPushButton *acceptButton = new QPushButton("接受");
        acceptButton->setCursor(Qt::PointingHandCursor);
        // 存储数据到按钮
        acceptButton->setProperty("sender", friendRequest.account);
        acceptButton->setStyleSheet(
            "QPushButton {"
            "    font: 14pt 'Microsoft YaHei UI';"
            "    background-color: rgb(5, 186, 251);"
            "    color: white;"
            "    border-radius: 5px;"
            "    min-width: 80px;"
            "    min-height: 30px;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(5, 186, 251, 0.7);"
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(255, 0, 0, 0.5);"
            "}"
            );
        connect(acceptButton, &QPushButton::clicked, this, [this, acceptButton]() {
            ChoiceDialog dialog(this);
            dialog.transText("你确定要接受好友申请吗？");
            dialog.transButText("确定", "取消");
            int result = dialog.exec();
            if (result == QDialog::Accepted) {
                QString senderId = acceptButton->property("sender").toString();
                if (!senderId.isEmpty()) {
                    acceptAddFriends(myInfo.account, senderId);
                }
            }
        });
        requestLayout->addWidget(acceptButton);
        requestLayout->setAlignment(acceptButton, Qt::AlignRight);
        //拒绝按钮
        QPushButton *rejectButton = new QPushButton("拒绝");
        rejectButton->setCursor(Qt::PointingHandCursor);
        // 存储数据到按钮
        rejectButton->setProperty("sender", friendRequest.account);
        rejectButton->setStyleSheet(
            "QPushButton {"
            "    font: 14pt 'Microsoft YaHei UI';"
            "    background-color: rgb(255, 0, 0);"
            "    color: white;"
            "    border-radius: 5px;"
            "    min-width: 80px;"
            "    min-height: 30px;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(255, 0, 0, 0.7);"
            "}"
            "QPushButton:pressed {"
            "    background-color: rgba(255, 0, 0, 0.5);"
            "}"
            );
        connect(rejectButton, &QPushButton::clicked, this, [this, rejectButton]() {
            ChoiceDialog dialog(this);
            dialog.transText("你确定要拒绝好友申请吗？");
            dialog.transButText("确定", "取消");
            int result = dialog.exec();
            if (result == QDialog::Accepted) {
                QString senderId = rejectButton->property("sender").toString();
                if (!senderId.isEmpty()) {
                    rejectAddFriends(myInfo.account, senderId);
                }
            }
        });
        requestLayout->addWidget(rejectButton);
        requestLayout->setAlignment(rejectButton, Qt::AlignRight);
        newLayout->addLayout(requestLayout, Qt::AlignTop);
    }
    //最下方添加一个伸缩器
    QSpacerItem *bottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    newLayout->addItem(bottomSpacer);
}

void MainWindow::acceptAddFriends(const QString &account, const QString &sender)//接受好友申请
{
    if(!checkSocket()){
        return;
    }
    QJsonObject json;
    json["tag"] = "newfriends";
    json["answer"] = "accept";
    json["account"] = account;
    json["sender"] = sender;
    QJsonDocument jsonDoc(json);
    QByteArray jsonData = jsonDoc.toJson() + "END";
    socket->write(jsonData);
    socket->flush();
}

void MainWindow::rejectAddFriends(const QString &account, const QString &sender)//拒绝好友申请
{
    if(!checkSocket()){
        return;
    }
    QJsonObject json;
    json["tag"] = "newfriends";
    json["answer"] = "reject";
    json["account"] = account;
    json["sender"] = sender;
    QJsonDocument jsonDoc(json);
    QByteArray jsonData = jsonDoc.toJson() + "END";
    socket->write(jsonData);
    socket->flush();
    for (int i = newFriendArray.size() - 1; i >= 0; --i) {
        if (newFriendArray[i].account == json["sender"].toString()) {
            newFriendArray.removeAt(i);
        }
    }
    updateEditShow2New();
}

bool MainWindow::checkSocket()//检查是不是连接上服务器了
{
    if (socket->state() != QAbstractSocket::ConnectedState) {
        QMessageBox msgBox;
        QIcon icon = QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowIcon(icon);
        msgBox.setWindowTitle("请求失败");
        msgBox.setText("请检查您的网络连接!");
        msgBox.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog);
        msgBox.exec();
        return false;
    }
    return true;
}

void MainWindow::updateEditShow2Normal(const QModelIndex &index)//更新右边好友信息视图
{
    clearEditShow2();//清理旧控件
    //从模型中获取好友的信息
    deleteFriendNum = ui->FriendsList->currentIndex().data(Qt::UserRole+1).toString();
    QString nickname = index.data(Qt::DisplayRole).toString();
    QString accountNum = index.data(Qt::UserRole + 1).toString();
    QString gender = index.data(Qt::UserRole + 2).toString();
    QString signature = index.data(Qt::UserRole + 3).toString();
    QString avator = index.data(Qt::UserRole + 4).toString();//base64
    QByteArray byteArray = QByteArray::fromBase64(avator.toUtf8());
    QImage image;
    image.loadFromData(byteArray);
    QPixmap pixmap = QPixmap::fromImage(image);
    //把右边的edit背景重绘
    ui->FriendsShowTextEdit->background = pixmap;
    ui->FriendsShowTextEdit->repaint();
    //添加间隔器
    QSpacerItem *spacertop = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    newLayout->addItem(spacertop);
    //显示头像
    QLabel *avatorLabel = new QLabel;
    avatorLabel->setPixmap(pixmap);
    avatorLabel->setScaledContents(true);
    avatorLabel->setFixedSize(120, 120);
    avatorLabel->setAlignment(Qt::AlignCenter);
    avatorLabel->setStyleSheet("border: 3px solid black; border-radius: 8px;");
    newLayout->addWidget(avatorLabel, 0, Qt::AlignCenter);
    //添加信息标签
    auto addTextEditWithStyle = [&](const QString &prefix, const QString &text) {
        AutoClearTextEdit *textEdit = new AutoClearTextEdit(prefix + text);
        textEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        textEdit->setFixedSize(300, 50);
        textEdit->setStyleSheet(
            "font-size: 16px; "
            "font-weight: bold; "
            "color: #333333; "
            "padding: 5px; "
            "background: rgba(255, 255, 255, 0); "
            "border: 2px solid black; "
            "border-radius: 5px;");
        textEdit->setAlignment(Qt::AlignCenter);
        textEdit->setReadOnly(true);//设置为只读
        textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//禁用垂直滚动条
        textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//禁用水平滚动条
        return textEdit;
    };
    //添加间隔器
    QSpacerItem *spacer0 = new QSpacerItem(0, 15, QSizePolicy::Fixed, QSizePolicy::Minimum);
    newLayout->addItem(spacer0);
    //添加信息
    if (!nickname.isEmpty()) newLayout->addWidget(addTextEditWithStyle("昵称: ", nickname));
    if (!accountNum.isEmpty()) newLayout->addWidget(addTextEditWithStyle("账号: ", accountNum));
    if (!gender.isEmpty()) newLayout->addWidget(addTextEditWithStyle("性别: ", gender));
    if (!signature.isEmpty()) newLayout->addWidget(addTextEditWithStyle("签名: ", signature));
    //添加间隔器
    QSpacerItem *spacer1 = new QSpacerItem(0, 5, QSizePolicy::Fixed, QSizePolicy::Minimum);
    newLayout->addItem(spacer1);
    //创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignCenter);//按钮布局居中
    //添加发送信息按钮
    QPushButton *sendButton = new QPushButton("发送信息");
    sendButton->setCursor(Qt::PointingHandCursor);
    sendButton->setFixedSize(140, 55);
    sendButton->setStyleSheet(
        "QPushButton {"
        "    font: 12pt 'Microsoft YaHei UI';"
        "    background-color: rgb(5, 186, 251);"
        "    color: white;"
        "    font-weight: bold; "
        "    border-radius: 15px;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(5, 186, 251, 0.7);"
        "    font-weight: bold; "
        "    color: white;"
        "    border-radius: 15px;"
        "}"
        "QPushButton:pressed {"
        "    background-color: rgba(0, 123, 255, 0.8);"
        "    font-weight: bold; "
        "    color: rgba(255, 255, 255, 0.9);"
        "    border-radius: 15px;"
        "}"
        );
    buttonLayout->addWidget(sendButton);
    //添加间隔器
    QSpacerItem *spacer = new QSpacerItem(25, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);
    buttonLayout->addItem(spacer);
    //添加删除好友按钮
    QPushButton *deleteButton = new QPushButton("删除好友");
    deleteButton->setCursor(Qt::PointingHandCursor);
    deleteButton->setFixedSize(140, 55);
    deleteButton->setStyleSheet(
        "QPushButton {"
        "    font: 12pt 'Microsoft YaHei UI';"
        "    background-color: rgb(255, 0, 0);"
        "    color: white;"
        "    font-weight: bold; "
        "    border-radius: 15px;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(255, 0, 0, 0.7);"
        "    font-weight: bold; "
        "    color: white;"
        "    border-radius: 15px;"
        "}"
        "QPushButton:pressed {"
        "    background-color: rgba(200, 0, 0, 0.8);"
        "    font-weight: bold; "
        "    color: rgba(255, 255, 255, 0.9);"
        "    border-radius: 15px;"
        "}"
        );
    buttonLayout->addWidget(deleteButton);
    //将按钮布局添加到主布局中
    newLayout->addLayout(buttonLayout);
    //添加间隔器
    QSpacerItem *spacer2 = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    newLayout->addItem(spacer2);
    //增加间距
    newLayout->setSpacing(3);
    newLayout->setContentsMargins(5, 5, 5, 5);
    //为按钮添加点击事件
    connect(sendButton, &QPushButton::clicked, this, [=]() {
        qDebug() << "发送信息按钮被点击";
        sendMessageToFriend(accountNum);
    });
    connect(deleteButton, &QPushButton::clicked, this, [&]() {
        qDebug() << "删除好友按钮被点击";
        if(friendHash.find(deleteFriendNum) == friendHash.end()){
            Dialog dialog(this);
            dialog.transText("他不在您的好友列表当中!");
            dialog.exec();
            return;
        }
        ChoiceDialog dialog(this);
        dialog.transText("你确定要删除好友吗?");
        dialog.transButText("确认","取消");
        int result = dialog.exec();
        if(result == QDialog::Accepted){
            deleteFriend();
        }
    });
    //设置布局
    ui->FriendsShowTextEdit->setLayout(newLayout);
}

void MainWindow::sendMessageToFriend(const QString &account)//给某个好友发送消息
{
    if(friendHash.find(account) != friendHash.end()){//确定有这个好友
        AccountInfo info = friendHash[account];
        addSomeoneToTalkList(info, NULL, NULL, NULL);
        selectSomeoneInTalkList(account);
        ui->ChatBtn->click();
    }
    else{//没有这个好友
        Dialog dialog(this);
        dialog.transText("他不在您的好友列表当中!");
        dialog.exec();
        return;
    }
}

void MainWindow::deleteFriend()//删除好友
{
    if(!checkSocket()){
        return;
    }
    QJsonObject jsonObj;
    jsonObj["tag"] = "deletefriend";
    jsonObj["account"] = myInfo.account;
    jsonObj["friend"] = deleteFriendNum;
    QJsonDocument jsonDoc(jsonObj);
    QByteArray jsonData = jsonDoc.toJson() + "END";
    socket->write(jsonData);
    socket->flush();
    qDebug()<<"发送了删除好友请求";
}

//向聊天列表加入某个好友
int MainWindow::addSomeoneToTalkList(const AccountInfo &friendMessage, QString message, QString date, QString unread)
{
    //检查好友是否已经存在
    for (int row = 0; row < talkModel->rowCount(); ++row) {
        QStandardItem *item = talkModel->item(row);
        if (item->data(Qt::UserRole + 1).toString() == friendMessage.account) {
            //好友已经存在，返回其索引
            return row;//返回已有项的索引
        }
    }
    QSqlQuery qry(db);
    qry.prepare("SELECT * FROM talks WHERE friend_id = :friend_id");
    qry.bindValue(":friend_id", friendMessage.account);
    qry.exec();
    if(!qry.next()){//如果没有记录则插入
        qry.prepare("INSERT INTO talks (friend_id) VALUES (:friend_id)");
        qry.bindValue(":friend_id", friendMessage.account);
        qry.exec();
    }
    QStandardItem *item = new QStandardItem();
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    //设置标准项的显示文本为好友的昵称
    item->setText(friendMessage.name);
    //设置标准项为不可编辑状态
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);//设置为不可编辑
    //将好友的信息存储在标准项的用户角色数据中
    item->setData(friendMessage.account, Qt::UserRole + 1);//存储账号
    item->setData(friendMessage.gender, Qt::UserRole + 2);//存储性别
    item->setData(friendMessage.signature, Qt::UserRole + 3);//存储个性签名
    item->setData(friendMessage.avator_base64, Qt::UserRole + 4);//存储头像的Base64字符串
    item->setData(message, Qt::UserRole + 5);//存储上一条聊天信息
    item->setData(date, Qt::UserRole + 6);//存储上次沟通日期
    item->setData(unread, Qt::UserRole + 10);//存储未读消息个数
    //将创建的标准项添加到好友模型中
    talkModel->appendRow(item);
    //添加到消息列表的hash中
    messageListHash.insert(friendMessage.account, friendMessage);
    //返回新添加项的索引
    return talkModel->rowCount() - 1; //返回最后一项的索引
}

int MainWindow::selectSomeoneInTalkList(const QString &account)//打开某个人的聊天框
{
    ui->LineSearchTextEdit->setText("搜索");
    ui->LineSearchTextEdit->clearFocus();
    //获取源模型
    QAbstractItemModel *sourceModel = talkFilterProxyModel->sourceModel();
    if (sourceModel) {
        for (int row = 0; row < sourceModel->rowCount(); ++row) {
            QModelIndex index = sourceModel->index(row, 0);
            QVariant data = sourceModel->data(index, Qt::UserRole + 1);
            QString string = data.toString();
            //匹配上则返回
            if (string == account) {
                //获取过滤后的索引
                QModelIndex proxyIndex = talkFilterProxyModel->mapFromSource(index);
                if (proxyIndex.isValid()) {
                    ui->TalkListView->setCurrentIndex(proxyIndex);
                }
                return row;
            }
        }
    }
    return -1;
}

QPixmap MainWindow::getRoundedPixmap(const QPixmap srcPixmap, const int radius)//获得圆角图片
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
    painter.setRenderHint(QPainter::Antialiasing);//反锯齿
    //将图片绘制到desPixmap中，IgnoreAspectRatio忽视图片比例
    painter.drawPixmap(0, 0, desSize.width(), desSize.height(), srcPixmap.scaled(desSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    painter.setClipping(false);//关闭裁剪
    return desPixMap;
}

void MainWindow::on_CloseMainWindowBtn_clicked()
{
    this->close();
}

