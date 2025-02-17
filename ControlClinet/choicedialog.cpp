#include "choicedialog.h"
#include "ui_choicedialog.h"

ChoiceDialog::ChoiceDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChoiceDialog)
{
    ui->setupUi(this);

    //初始化窗口
    QIcon icon(":/Pictures/Resource/Pictures/ChoiceDialogIcon.png");//初始化窗口
    setWindowIcon(icon);
    setWindowFlags( Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);//允许背景透明

    //设置文本框
    setEdit();

    // 连接按钮的点击信号
    connect(ui->OkBtn, &QPushButton::clicked, this, &ChoiceDialog::reject);
    connect(ui->NoBtn, &QPushButton::clicked, this, &ChoiceDialog::accept);

    // 将窗口居中显示
    QWidget *topLevelParent = parent ? parent->window() : nullptr;
    if (topLevelParent) {
        int x = topLevelParent->x() + (topLevelParent->width() - width()) / 2;
        int y = topLevelParent->y() + (topLevelParent->height() - height()) / 2;
        move(x, y);
    }
}

ChoiceDialog::~ChoiceDialog()
{
    delete ui;
}

void ChoiceDialog::setEdit()//设置文本框
{
    ui->NoticeTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//禁用垂直滚动条
    ui->NoticeTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//禁用水平滚动条
    ui->NoticeTextEdit->setReadOnly(true);//设置为只读
    ui->NoticeTextEdit->setAlignment(Qt::AlignCenter);
}


void ChoiceDialog::paintEvent(QPaintEvent *event)//窗口背景
{
    //调用基类方法以处理父类的绘图
    QDialog::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);//反锯齿
    //设置圆角半径
    int radius = 4;//圆角半径
    //设置背景颜色
    painter.setBrush(QBrush(QColor(255, 255, 255)));
    //绘制圆角矩形背景
    painter.drawRoundedRect(rect(), radius, radius);
}


void ChoiceDialog::transText(QString text)//改变文字
{
    //设置 textEdit_notice 的文本
    ui->NoticeTextEdit->setText(text);
    ui->NoticeTextEdit->setAlignment(Qt::AlignCenter);
}


void ChoiceDialog::transButText(QString no, QString yes)//改变按钮文字
{
    ui->NoBtn->setText(no);
    ui->OkBtn->setText(yes);
}



void ChoiceDialog::on_OkBtn_clicked()
{
    this->close();
}


void ChoiceDialog::on_NoBtn_clicked()
{
    this->close();
}

