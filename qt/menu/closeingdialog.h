#ifndef CLOSEINGDIALOG_H
#define CLOSEINGDIALOG_H

/*
 * 关闭界面
 */

#include <QDialog>
#include <QThread>
#include "basejsonconnector.h"

class CloseingThread : public QThread
{
    Q_OBJECT
public:
    CloseingThread();
    ~CloseingThread(){}

signals:
    void Chain33Closed();

protected:
    virtual void run();
};

enum CloseType{
    CloseUI = 0,
    RestartNewChain33
};

namespace Ui {
class CloseingDialog;
}

class CloseingDialog : public JsonConnectorDialog
{
    Q_OBJECT

public:
    explicit CloseingDialog(QWidget *parent = 0, CloseType = CloseUI);
    ~CloseingDialog();

    virtual void requestFinished(const QVariant &result, const QString &error);

public slots:
    void Chain33Closed();

private:
    Ui::CloseingDialog *ui;
    CloseingThread *m_lpCloseingThread;
    CloseType       m_Type;

    bool            m_isWalletLock;
    bool            m_isTicketLock;
};

#endif // CLOSEINGDIALOG_H
