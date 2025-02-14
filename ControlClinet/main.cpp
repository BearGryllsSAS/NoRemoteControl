#include "mainwindow.h"
#include "login.h"
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

    // 不显示 MainWindow
    // MainWindow w;
    // w.show();

    // 初始化整个项目的 QSS 样式
    initStyleSheet(a);

    // 创建登录窗口显示
    Login w;
    w.show();


    return a.exec();
}
