﻿#ifndef MANAGEUI_H
#define MANAGEUI_H

/*
 * 正在加载请稍等 界面
 * 管理运行 mainUI
 * 启动 chain33
 */

#include <QMainWindow>
#include <QThread>
#include <QProcess>
#include "basejsonconnector.h"
#include <mutex>

#ifdef WIN32
#include <windows.h>
#include <tlhelp32.h>
#include "clearthread.h"
#endif

// 判断启动 chain33 是否成功
class RuningThread : public QThread
{
    Q_OBJECT
public:
    RuningThread();
    ~RuningThread(){}
    void SetFinish();
    void SetQuit(bool bQuit);

 //   bool GetProcessidFromName();

signals:
    void PostMsgGetWalletstatus();

    void startChain33();

protected:
    virtual void run();

private:
    bool        m_bFinish;
    bool        m_bQuit;
    QMutex      m_mutexFinish;
    QMutex      m_mutexQuit;
};



namespace Ui {
class ManageUI;
}

class ManageUI : public JsonConnectorMainWindow
{
    Q_OBJECT

public:
    explicit ManageUI(QWidget *parent = 0, const char* lpstylesheet = NULL);
    ~ManageUI();

    virtual void requestFinished(const QVariant &result, const QString &error);

    void StartThread();

    void QuitThread();

    void CloseQueueChain33();

    void ShowHide();

    void UnlockWallet(bool isWalletLock, bool isTicketLock);

public slots:
    void PostMsgGetWalletstatus();

    void startChain33();    

    void CloseChain33Temp();

    void CloseChain33();

private:
    Ui::ManageUI *ui;
    QString         m_stylesheet;
    RuningThread*   m_lpThread;
    QProcess*       m_lpQProcess;
    bool            m_bFirst;
    int             m_nCloseChain33;
#ifdef WIN32
    DWORD           m_dwChain33ProcessId;
#else
    qint64          m_dwChain33ProcessId;
#endif
#ifdef WIN32
    ClearThread*    m_clearThread;
#endif
};

#endif // MANAGEUI_H
