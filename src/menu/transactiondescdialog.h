#ifndef TRANSACTIONDESCDIALOG_H
#define TRANSACTIONDESCDIALOG_H

/*
 * 双击交易详情后的 交易详情界面
 */

#include <QDialog>
#include "basejsonconnector.h"

namespace Ui {
    class TransactionDescDialog;
}
QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Dialog showing transaction details. */
class TransactionDescDialog : public JsonConnectorDialog
{
    Q_OBJECT

public:
    explicit TransactionDescDialog(const QModelIndex &idx, QWidget *parent = 0);
    ~TransactionDescDialog();

protected:
    virtual void requestFinished(const QVariant &result, const QString &error);

private slots:
    void on_CloseBtn_clicked();

private:
    QString getHtmlText(const QModelIndex &idx);
    QString getLabelName(const QString& strAddr);
    void contractAddr(QString& strAddr);

private:
    Ui::TransactionDescDialog *ui;
};

#endif // TRANSACTIONDESCDIALOG_H
