#ifndef VEIFYSEEDDIALOG_H
#define VEIFYSEEDDIALOG_H

/*
 * 验证助记词 界面
 */

#include <QDialog>
#include "basejsonconnector.h"
#include "enumtype.h"

namespace Ui {
class veifySeedDialog;
}

class veifySeedDialog : public JsonConnectorDialog
{
    Q_OBJECT

public:
    explicit veifySeedDialog(QWidget *parent = nullptr);
    ~veifySeedDialog();

protected:
    virtual void requestFinished(const QVariant &result, const QString &error);

private slots:
    void on_veifypushButton_clicked();

private:
    Ui::veifySeedDialog *ui;

    EncryptionStatus    m_nLockStatus;
    bool                m_bveify;   // true 正在验证  false 验证结束要锁定钱包
};

#endif // VEIFYSEEDDIALOG_H
