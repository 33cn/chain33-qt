#ifndef OFFLINEMININGDIALOG_H
#define OFFLINEMININGDIALOG_H

/*
 * 离线挖矿授权界面
 */

#include <QDialog>
#include "basejsonconnector.h"

class PlatformStyle;

namespace Ui {
class OfflineMiningDialog;
}

class OfflineMiningDialog : public JsonConnectorDialog
{
    Q_OBJECT

public:
    explicit OfflineMiningDialog(QWidget *parent, const PlatformStyle *platformStyle);

    ~OfflineMiningDialog();

    void accept();

    void PostMsgCreateBindMiner();

    void PostMsgCreateRawTransaction();

    void PostMsgSignRawTx();

    void PostMsgSignRawTx(IdType nID, const QString& strtxHex);

protected:
    virtual void requestFinished(const QVariant &result, const QString &error);

private slots:
    void on_BindCopyButton_clicked();

    void on_TransCopyButton_clicked();

    void SaveTxhex();

    void CreateTxhex();

    void on_toolButton_clicked();

private:
    Ui::OfflineMiningDialog *ui;
    QString m_strtxHex_CreateBindMiner;
    QString m_strtxHex_CreateRawTransaction;
    const PlatformStyle *m_platformStyle;
};

#endif // OFFLINEMININGDIALOG_H
