#include "mainwindow.h"
#include "login.h"
#include "registerwindow.h"
#include "cutavator.h"
#include "findpassword.h"
#include "friendmessage.h"
#include "choicedialog.h"
#include "changepassword.h"
#include <QFile>
#include <QApplication>

void initStyleSheet(QApplication &a)
{
    QFile file(":/QSS/Resource/QSS/MacOS.qss");
    file.open(QFile::ReadOnly);

    QString qss;
    qss = file.readAll();
    file.close();

    a.setStyleSheet(qss);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 初始化整个项目的 QSS 样式
    initStyleSheet(a);

    ChangePassword w;
    w.show();

    // MainWindow w;
    // w.show();

    // ChoiceDialog w;
    // w.show();

    // FriendInfo s;
    // FriendMessage w(s, nullptr);
    // w.show();

    // 创建登录窗口显示
    // Login w;
    // w.show();

    // RegisterWindow w(nullptr);
    // w.show();

    // CutAvator w("C:\\Users\\haibara\\Pictures\\Camera Roll\\IMG_20240221_205217_844.jpg");
    // w.show();

    // FindPassword w(nullptr);
    // w.show();

    return a.exec();
}
