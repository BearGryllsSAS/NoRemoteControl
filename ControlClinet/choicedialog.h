#ifndef CHOICEDIALOG_H
#define CHOICEDIALOG_H

#include <QDialog>
#include <QString>
#include <QPainter>

namespace Ui {
class ChoiceDialog;
}

class ChoiceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChoiceDialog(QWidget *parent = nullptr);
    ~ChoiceDialog();
    void setEdit();//设置文本框
    void paintEvent(QPaintEvent *event);//窗口背景
    void transText(QString text);//改变文字
    void transButText(QString no, QString yes);//改变按钮文字

private slots:



    void on_OkBtn_clicked();

    void on_NoBtn_clicked();

private:
    Ui::ChoiceDialog *ui;
};

#endif // CHOICEDIALOG_H
