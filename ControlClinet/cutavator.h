#ifndef CUTAVATOR_H
#define CUTAVATOR_H

#include <QDialog>
#include <QLabel>
#include <QMouseEvent>


namespace Ui {
class CutAvator;
}

class CutAvator : public QDialog
{
    Q_OBJECT

public:
    // explicit CutAvator(QWidget *parent = nullptr);
    explicit CutAvator(const QString &mypath, QWidget *parent = nullptr);
    ~CutAvator();

    // 处理传入的图片，确保它是有效的
    void dealImage(QString path);

    // 设置头像
    void setBackground(QString avapath);

    // 移动明亮矩形更新头像
    void moveRect(const QPoint &newPosition);

    // 初始化整个窗口背景
    void paintEvent(QPaintEvent *event);

private slots:
    void on_CutBtn_clicked();

    void on_OkBtn_clicked();

signals:
    // 裁剪完了发送信号
    void cutOk(const QPixmap &pixmap);

private:
    Ui::CutAvator *ui;
    QString path;
    int flag = 0;
    QRect *labelRect;
};

// 用于裁剪图片
class DraggableResizableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit DraggableResizableLabel(QWidget *parent = nullptr) : QLabel(parent)
    {}

signals:
    void labelMoved(const QPoint &newPosition); //标签发生任何变化时发送信号

protected:
    void mousePressEvent(QMouseEvent *event) override;//点击判断是要干什么
    void mouseMoveEvent(QMouseEvent *event) override;//移动时判断是移动还是变动大小
    void mouseReleaseEvent(QMouseEvent *event) override;//释放重置状态

private:
    QPoint myOffset;
    bool myResizing = false;//标志是否正在调整大小
};

#endif // CUTAVATOR_H
