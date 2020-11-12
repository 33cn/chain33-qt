#ifndef STATUSBARUI_H
#define STATUSBARUI_H

/*
 * 界面最下面 同步滚动条 界面
 */

#include <QWidget>
#include <QMovie>
#include <QDateTime>
#include <QThread>
#include <QSemaphore>
#include <QMutex>
#include <QWaitCondition>

#include "basejsonconnector.h"

class PlatformStyle;

class StatusBarThread : public QThread
{
    Q_OBJECT
public:
    StatusBarThread();
    ~StatusBarThread();
    void SetOutOfSync(bool bOutOfSync);

    void ReleaseOneSem(){m_sem.release();}
    void Stop();
    void Resume();
signals:
    void PostMsgGetPeerInfo();
    void PostMsgIsSync();
    void PostMsgGetLastHeader();
    void PostMsgGetTicketCount();
    void PostMsgIsNtpClockSync();
    void PostMsgGetNetInfo();
    void PostMsgGetFatalFailure();
    void PostMsgGetWalletStatus();
    void PostMsgGetTimeStatus();
    void JudgefreeBytesAvailable();

protected:
    virtual void run();

private:
    bool m_bOutOfSync;
    QSemaphore m_sem;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_stop;
};

namespace Ui {
class StatusBarUI;
}

class StatusBarUI
        : public JsonConnectorDialog
{
    Q_OBJECT

public:
    explicit StatusBarUI(QWidget *parent, const PlatformStyle *platformStyle);
    ~StatusBarUI();

    bool eventFilter(QObject * watched, QEvent * event);

    void RestartChain33Init();

    void StopUpdateStatusBar();
    void ResumeUpdateStatusBar();
protected:
    virtual void requestFinished(const QVariant &result, const QString &error);

public slots:
    void PostMsgGetPeerInfo();
    void PostMsgIsSync();
    void PostMsgGetLastHeader();
    void PostMsgGetTicketCount();
    void PostMsgIsNtpClockSync();
    void PostMsgGetNetInfo();
    void PostMsgGetFatalFailure();
    void setEncryptionStatus(int status);
    void setNumConnections(int count);
    void setNumBlocks();
    void setAutoMiningStatus();
    void PostMsgGetWalletStatus();
    void PostMsgGetTimeStatus();
    void JudgefreeBytesAvailable();
    void UpdateWalletStatus(const QVariant &result);

public:
    bool                m_bOutOfSync;      // true 同步成功

private:
    Ui::StatusBarUI*    ui;

    StatusBarThread*    m_lpStatusBarThread;
    int                 m_nPeerHeight;
    int                 m_nLastHeight;
    int                 m_LockStatus;       // 加锁状态
    bool                m_AutoMining;       // 是否开启自动挖矿 false 没有  true 开启
    int                 m_spinnerFrame;     // 同步时转动的图片
    int                 m_NumConnections;   // 连接种子数
    int                 m_npeersList;
    int                 m_nMiningTicket;

    int                 m_bShowMsgBox0;
    int                 m_bShowMsgBox1;

    const PlatformStyle *m_platformStyle;
    bool                m_bFirst;            // 第一次完全解锁后 判断一下合约里面是否还有币 如果有的话转出
};

#endif // STATUSBARUI_H
