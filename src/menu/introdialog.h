#ifndef INTRODIALOG_H
#define INTRODIALOG_H

/*
 * 欢迎 选择数据库路径 界面
 */

#include <QDialog>

enum Status {
    ST_OK,
    ST_ERROR
};

namespace Ui {
class IntroDialog;
}

class IntroDialog : public QDialog
{
    Q_OBJECT
public:
    explicit IntroDialog(QWidget *parent = 0);
    ~IntroDialog();

    void accept();

    int CheckDir();
private slots:
    void on_ellipsisButton_clicked();

    void on_dataDirDefault_clicked();

    void on_dataDirCustom_clicked();

private:
    Ui::IntroDialog *ui;

};

#endif // INTRODIALOG_H
