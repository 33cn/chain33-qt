#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

/*
 * 关于界面
 */
#include "basejsonconnector.h"
#include <QDialog>
#include <QTextStream>

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public JsonConnectorDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

protected:
    virtual void requestFinished(const QVariant &result, const QString &error);

private:
    Ui::AboutDialog *ui;

private slots:
    void on_buttonBox_accepted();
};

#endif // ABOUTDIALOG_H
