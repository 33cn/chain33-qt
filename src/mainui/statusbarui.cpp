#include "statusbarui.h"
#include "ui_statusbarui.h"
#include "guiconstants.h"
#include "enumtype.h"
#include "basejsonconnector.h"
#include "askpassphrasedialog.h"
#include <QMessageBox>
#include "basefuntion.h"
#include "homepageui.h"
#include "mainui.h"
#include "platformstyle.h"
#include "manageui.h"

#define TOTAL_SEMAPHORE_NUM 6

extern ManageUI*   g_lpManageUI;
extern MainUI*     g_lpMainUI;

StatusBarThread::StatusBarThread()
    : m_bOutOfSync (false)
    , m_stop (false)
{
}

StatusBarThread::~StatusBarThread()
{
    Stop();
}

void StatusBarThread::SetOutOfSync(bool bOutOfSync)
{
    m_mutex.lock();
    m_bOutOfSync = bOutOfSync;
    m_mutex.unlock();
}

void StatusBarThread::Stop()
{
    if(!isRunning()) {
        return;
    }
    m_mutex.lock();
    m_stop = true;
    m_mutex.unlock();

    qDebug() << "Stop semaphore available " << m_sem.available();
    m_sem.release(TOTAL_SEMAPHORE_NUM - m_sem.available());
    qDebug() << "Stop release semaphore";
    m_cond.wakeOne();
    wait();
}

void StatusBarThread::Resume()
{
    if(isRunning())
    {
        return;
    }
    m_mutex.lock();
    m_stop = false;
    m_mutex.unlock();
    start();
}

void StatusBarThread::run()
{
    int nNo = 0;
    int nNetInfo = 0;
#ifdef WIN32
    int nNtpClockSync = 0;
#endif
    while (true)
    {
        m_mutex.lock();
        if(m_stop) {
            m_mutex.unlock();
            break;
        }
        m_mutex.unlock();

        //warning:sem must refer to TOTAL_SEMAPHORE_NUM, max number of sem must equal to TOTAL_SEMAPHORE_NUM
        int sem = 0;
        while(m_sem.available() > 0)
        {
            qDebug() << "thread acquire available:" << m_sem.available();
            m_sem.acquire(1);
        }
        // 一直同步数据 同步完成后 获取当前节点的信号 会出现又同步不成功的情况 需要继续获取
        emit PostMsgGetPeerInfo();
        emit PostMsgIsSync();
        sem += 2;

        m_mutex.lock();
        if(m_bOutOfSync) {
            m_mutex.unlock();
            ++nNo;
            if(nNo > 2) {
                nNo = 0;
                emit PostMsgGetTicketCount();
                sem += 1;
            }
        } else {
            m_mutex.unlock();
        }

        ++nNetInfo;
        if(nNetInfo > 5)
        {
            nNetInfo = 0;
            emit PostMsgGetNetInfo();
            emit PostMsgGetWalletStatus();
            emit PostMsgGetTimeStatus();
            //    emit PostMsgIsNtpClockSync(); // 暂时先去掉这个判读时间同步的功能
            //    emit PostMsgGetFatalFailure();    // 暂时不开放
            sem += 3;
        }        

#ifdef WIN32
        ++nNtpClockSync;
       if(nNtpClockSync > 60*60*24)
       {
           nNtpClockSync = 0;
           // 数据目录磁盘空间少于 5G 给出提醒
           emit JudgefreeBytesAvailable();
       }
#endif
       //qDebug() << "thread acquire " << sem << " semaphore";
       m_sem.tryAcquire(sem, MAX_TIMEOUT_WAIT_RESPONSE_RESULT);
       //qDebug() << "thread acquire semaphore suc:" << m_sem.available();

       m_mutex.lock();
       m_cond.wait(&m_mutex, 2000);
       m_mutex.unlock();
    }
}



StatusBarUI::StatusBarUI(QWidget *parent, const PlatformStyle *platformStyle)
    : JsonConnectorDialog(parent)
    , ui(new Ui::StatusBarUI)
    , m_platformStyle (platformStyle)
    , m_bFirst (true)
{
    ui->setupUi(this);
    if(CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE)
    {
        QString stylesheet = "QWidget {background-color:#ffffff;border:none;}" + CStyleConfig::GetInstance().GetStylesheet();
        this->setStyleSheet(stylesheet);
    }
    else
    {
        this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());
    }
    RestartChain33Init();

 //   connect(this, SIGNAL(setNumConnectionsSignal(int)), this, SLOT(setNumConnections(int)));

    m_lpStatusBarThread = new StatusBarThread();
    m_lpStatusBarThread->SetOutOfSync(true);
    m_lpStatusBarThread->start();
    connect(m_lpStatusBarThread, SIGNAL(PostMsgGetPeerInfo()), this, SLOT(PostMsgGetPeerInfo()));
    connect(m_lpStatusBarThread, SIGNAL(PostMsgIsSync()), this, SLOT(PostMsgIsSync()));
    connect(m_lpStatusBarThread, SIGNAL(PostMsgGetLastHeader()), this, SLOT(PostMsgGetLastHeader()));
    connect(m_lpStatusBarThread, SIGNAL(PostMsgGetTicketCount()), this, SLOT(PostMsgGetTicketCount()));
    connect(m_lpStatusBarThread, SIGNAL(PostMsgIsNtpClockSync()), this, SLOT(PostMsgIsNtpClockSync()));
    connect(m_lpStatusBarThread, SIGNAL(PostMsgGetNetInfo()), this, SLOT(PostMsgGetNetInfo()));
    connect(m_lpStatusBarThread, SIGNAL(PostMsgGetFatalFailure()), this, SLOT(PostMsgGetFatalFailure()));
    connect(m_lpStatusBarThread, SIGNAL(PostMsgGetWalletStatus()), this, SLOT(PostMsgGetWalletStatus()));
    connect(m_lpStatusBarThread, SIGNAL(PostMsgGetTimeStatus()), this, SLOT(PostMsgGetTimeStatus()));
    connect(m_lpStatusBarThread, SIGNAL(JudgefreeBytesAvailable()), this, SLOT(JudgefreeBytesAvailable()));

    ui->labelEncryptionIcon->installEventFilter(this);
    ui->labelStakingIcon->installEventFilter(this);
}

StatusBarUI::~StatusBarUI()
{
    if(NULL != m_lpStatusBarThread)
    {
        m_lpStatusBarThread->Stop();
        delete m_lpStatusBarThread;
        m_lpStatusBarThread = NULL;
    }
    delete ui;
}

bool StatusBarUI::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->labelEncryptionIcon) {
        if(event->type() == QEvent::MouseButtonPress) {
            switch(m_LockStatus)
            {
            case Wallet_Unlocked:
            case Wallet_Unlocked_MinerOnly:
                PostJsonMessage(ID_Lock);
                break;
            case Wallet_Locked: {
                AskPassphraseDialog dlg(AskPassphraseDialog::Unlock, this);
                if (dlg.exec() && g_lpMainUI)
                    g_lpMainUI->setEncryptionStatus(dlg.m_nStatus);
            }
                break;
            }
        }
    }

    return QDialog::eventFilter(watched, event);
}

void StatusBarUI::RestartChain33Init()
{
    m_nPeerHeight = 0;
    m_nLastHeight = 0;
    m_bOutOfSync = false;
    m_spinnerFrame = 0;
    m_AutoMining = false;
    m_NumConnections = 0;
    m_npeersList = 0;
    m_nMiningTicket = 0;
    m_bShowMsgBox0 = 0;
    m_bShowMsgBox1 = 0;

    ui->progressBar->setTextVisible(true);
    ui->progressBar->setValue(1);
    ui->labelStakingIcon->setPixmap(QIcon(":/icons/staking_off").pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
    ui->labelStakingIcon->setToolTip(tr("Not staking"));
    setNumConnections(0);
    QString strIcon = QString(":/movies/res/movies/spinner-%1.png").arg(m_spinnerFrame, 3, 10, QChar('0'));
    ui->labelBlocksIcon->setPixmap(QIcon(strIcon).pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
    ui->labelNetInfoIcon->setPixmap(QIcon(":/icons/netinfo_not").pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));
    ui->labelNetInfoIcon->setToolTip(tr("不可以对其他节点提供服务"));
    JudgefreeBytesAvailable();
}

void StatusBarUI::PostMsgGetWalletStatus()
{
    PostJsonMessage(ID_GetWalletStatus);
}

void StatusBarUI::PostMsgGetTimeStatus()
{
    PostJsonMessage(ID_GetTimeStatus);
}

void StatusBarUI::JudgefreeBytesAvailable()
{
    ui->labelfreeBytes->setVisible(false);

#ifdef WIN32
    uint64_t freeBytesAvailable = GetfreeBytesAvailable(GetDefaultDataDir());
    if(freeBytesAvailable < 5 * GB_BYTES)
    {
        ui->labelfreeBytes->setText(tr("数据目录磁盘空间少于 5G，请更改数据目录选择更大空闲盘符存储数据！"));
        ui->labelfreeBytes->setVisible(true);
    }
#endif
}

void StatusBarUI::requestFinished(const QVariant &result, const QString &error)
{
    QMap<QString, QVariant> resultMap = result.toMap();
    if (m_nID == ID_GetPeerInfo)
    {
        QList<QVariant> peersList = (resultMap["peers"]).toList();

        setNumConnections(peersList.size()-1);
        m_npeersList = peersList.size();

        for (int i=0; i<peersList.size(); ++i) {
            QMap<QString, QVariant> headerMap = peersList[i].toMap();
            int nPeerHeight = (headerMap["header"].toMap())["height"].toInt();
            if(nPeerHeight > m_nPeerHeight)
                m_nPeerHeight = nPeerHeight;

            if(headerMap["self"].toBool()){
                m_nLastHeight = nPeerHeight;
            }
        }

        ui->height_label->setText(tr("当前/最新高度: ") + QString::number(m_nLastHeight, 10) + "/" + QString::number(m_nPeerHeight, 10));

        setNumBlocks();
        if(NULL != m_lpStatusBarThread)
        {
            m_lpStatusBarThread->ReleaseOneSem();
        }
    }
    else if (m_nID == ID_GetWalletStatus)
    {
        UpdateWalletStatus(result);
        if(NULL != m_lpStatusBarThread)
        {
            m_lpStatusBarThread->ReleaseOneSem();
           // qDebug()<<"WalletStatus released semaphore";
        }
    }
    else if (m_nID == ID_IsSync)
    {
        m_bOutOfSync = result.toBool();
        if(NULL != m_lpStatusBarThread)
        {
            m_lpStatusBarThread->SetOutOfSync(m_bOutOfSync);
            m_lpStatusBarThread->ReleaseOneSem();
        }
    }
    else if (m_nID == ID_Lock)
    {
        bool isOK = resultMap["isOK"].toBool();
        if(isOK) {
            g_lpMainUI->setEncryptionStatus(Wallet_Locked);
        } else {
            QMessageBox::critical(this, tr("Wallet lock failed"), tr("Wallet lock failed error."), tr("确定"));
        }
    }
    else if (m_nID == ID_SetAutoMining)
    {
        bool isOK = resultMap["isOK"].toBool();
        if(isOK) {
            PostJsonMessage(ID_GetWalletStatus);
        } else {
            QMessageBox::critical(this, tr("AutoMining failed"), g_mapErrorCode[resultMap["msg"].toString()], tr("确定"));
        }
    }
    else if (m_nID == ID_GetTicketCount)
    {
        m_nMiningTicket = result.toInt();
        if(NULL != m_lpStatusBarThread)
        {
            m_lpStatusBarThread->ReleaseOneSem();
        }
    }
    else if (m_nID == ID_IsNtpClockSync)
    {
        if(!result.toBool() && m_bShowMsgBox0 == 0)
        {
            m_bShowMsgBox0 = 1;
            m_bShowMsgBox0 = QMessageBox::warning(this, tr("提示"), tr("请设置当前时间与网络时间同步!"), tr("确定"));
        }
    }
    else if (m_nID == ID_GetNetInfo)
    {
        if(resultMap["service"].toBool())
        {
            ui->labelNetInfoIcon->setPixmap(m_platformStyle->SingleColorIcon(":/icons/netinfo_ok").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
            ui->labelNetInfoIcon->setToolTip(tr("可以对其他节点提供服务"));
        }
        else
        {
            ui->labelNetInfoIcon->setPixmap(QIcon(":/icons/netinfo_not").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
            ui->labelNetInfoIcon->setToolTip(tr("不可以对其他节点提供服务"));
        }
        if(NULL != m_lpStatusBarThread)
        {
            m_lpStatusBarThread->ReleaseOneSem();
           // qDebug()<<"NetInfo released semaphore";
        }
    }
    else if(m_nID == ID_GetFatalFailure)
    {
        if(result.toInt() == 1 && m_bShowMsgBox1 == 0)
        {
            m_bShowMsgBox1 = 1;
            m_bShowMsgBox1 = QMessageBox::warning(this, tr("提示"), tr("数据库损坏，请删除数据库重新同步数据!"), tr("确定"));
        }
    }
    else if(m_nID == ID_GetTimeStatus)
    {
        g_lpMainUI->SetTimeStatus(resultMap["diff"].toInt());
        if(NULL != m_lpStatusBarThread)
        {
            m_lpStatusBarThread->ReleaseOneSem();
          //  qDebug()<<"TimeStatus released semaphore";
        }
    }
    else if (m_nID == ID_GetAccounts)
    {
        QList<QVariant> walletsList = (resultMap["wallets"]).toList();
        for (int i=0; i<walletsList.size(); ++i)
        {
            QMap<QString, QVariant> addrMap = walletsList[i].toMap();
            QString strLabel = addrMap["label"].toString();
            if(strLabel.indexOf("Deleted") == -1)
            {
                QString strAddr = addrMap["acc"].toMap()["addr"].toString();
                QJsonObject jsonParms;
                jsonParms.insert("execer", "token");
                jsonParms.insert("funcName", "GetAccountTokenAssets");
                QJsonObject jsonPayload;
                jsonPayload.insert("address", strAddr);
                jsonPayload.insert("execer", "trade");
                jsonParms.insert("payload", jsonPayload);
                QJsonArray params;
                params.insert(0, jsonParms);
                PostJsonMessage(ID_GetAccountTokenAssets_trade, params);
            }
        }
    }
    else if(ID_GetAccountTokenAssets_trade == m_nID)
    {
        QList<QVariant> tokensList = (resultMap["tokenAssets"]).toList();
        for(int i=0; i<tokensList.size(); ++i)
        {
            qint64 nBalance = qint64(tokensList[i].toMap()["account"].toMap()["balance"].toDouble());
            if(nBalance > 0)
            {
                QString strSymbol = tokensList[i].toMap()["symbol"].toString();
                QString strAddr = tokensList[i].toMap()["account"].toMap()["addr"].toString();
                SendToTrade(strAddr, strSymbol, 0-nBalance);
            }
        }
    }
    else if(m_nID == ID_SendToAddress)
    {
        QString strHash = resultMap["hash"].toString();
        if(strHash.isEmpty())
        {
            qWarning() << ("ID_SendToAddress Post error: ") << error;
        }
    }
}

void StatusBarUI::PostMsgGetPeerInfo()
{
    PostJsonMessage(ID_GetPeerInfo);
}

void StatusBarUI::PostMsgIsSync()
{
    PostJsonMessage(ID_IsSync);
}

void StatusBarUI::PostMsgGetLastHeader()
{
    PostJsonMessage(ID_GetLastHeader);
}

void StatusBarUI::PostMsgGetTicketCount()
{
    PostJsonMessage(ID_GetTicketCount);
}

void StatusBarUI::PostMsgIsNtpClockSync()
{
    PostJsonMessage(ID_IsNtpClockSync);
}

void StatusBarUI::PostMsgGetNetInfo()
{
    PostJsonMessage(ID_GetNetInfo);
}

void StatusBarUI::PostMsgGetFatalFailure()
{
    PostJsonMessage(ID_GetFatalFailure);
}

void StatusBarUI::setEncryptionStatus(int status)
{
    m_LockStatus = status;
    switch(status)
    {
    case Wallet_Unlocked_MinerOnly:
        ui->labelEncryptionIcon->show();
        ui->labelEncryptionIcon->setPixmap(QIcon(":/icons/lock_open_mineronly").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        ui->labelEncryptionIcon->setToolTip(tr("Wallet is <b>encrypted</b> and currently <b>unlocked</b>"));
        break;
    case Wallet_Unlocked:
        ui->labelEncryptionIcon->show();
        ui->labelEncryptionIcon->setPixmap(m_platformStyle->SingleColorIcon(":/icons/lock_open").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        ui->labelEncryptionIcon->setToolTip(tr("Wallet is <b>encrypted</b> and currently <b>unlocked</b>"));
        break;
    case Wallet_Locked:
        ui->labelEncryptionIcon->show();
        ui->labelEncryptionIcon->setPixmap(QIcon(":/icons/lock_closed").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        ui->labelEncryptionIcon->setToolTip(tr("Wallet is <b>encrypted</b> and currently <b>locked</b>"));
        break;
    }
}

void StatusBarUI::setNumConnections(int count)
{
    m_NumConnections = count;
    QString icon;
    if(count <= 0) {
        icon = ":/icons/connect_0";
    } else if (count <= 1) {
        icon = ":/icons/connect_1";
    } else if (count <= 5) {
        icon = ":/icons/connect_2";
    } else if (count <= 15) {
        icon = ":/icons/connect_3";
    } else {
        icon = ":/icons/connect_4";
    }

    ui->labelConnectionsIcon->setPixmap(QIcon(icon).pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
    ui->labelConnectionsIcon->setToolTip(tr("与%1网络建立了 %n 个连接", "", count).arg(CStyleConfig::GetInstance().GetAppName()));
}

void StatusBarUI::setNumBlocks()
{
    QString tooltip;

    if(!m_bOutOfSync) {
        int nMax = 0;
        int nMin = 0;
        if(m_nLastHeight > m_nPeerHeight) {
            nMax = m_nLastHeight;
            nMin = m_nPeerHeight;
        } else {
            nMax = m_nPeerHeight;
            nMin = m_nLastHeight;
        }

        int nRemainingBlocks = nMax - nMin;
        float nPercentageDone = 1;
        if (nMax != 0)
            nPercentageDone = 100 - nRemainingBlocks / (nMax * 0.01f);

        ui->progressBarLabel->setText(tr("Synchronizing with network..."));
        ui->progressBarLabel->setVisible(true);
     //   ui->progressBar->setFormat(tr("~%n block(s) remaining", "", nRemainingBlocks));
        ui->progressBar->setMaximum(nMax);
        if(m_npeersList >= 2) {
            ui->progressBar->setValue(nMin);
        } else {
            ui->progressBar->setValue(1);
        }
        ui->progressBar->setVisible(true);

        tooltip = tr("Downloaded %1 of %2 blocks of transaction history (%3% done).").arg(nMin).arg(nMax).arg(nPercentageDone, 0, 'f', 2);

        tooltip = tr("Catching up...") + QString("<br>") + tooltip;

        QString strIcon = QString(":/movies/res/movies/spinner-%1.png").arg(m_spinnerFrame, 3, 10, QChar('0'));
        ui->labelBlocksIcon->setPixmap(m_platformStyle->SingleColorIcon(strIcon).pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));

        m_spinnerFrame = (m_spinnerFrame + 1) % 36;

        if(g_lpMainUI && g_lpMainUI->m_lpHomepageUI)
            g_lpMainUI->m_lpHomepageUI->showOutOfSyncWarning(true);
    } else {
        ui->progressBarLabel->setVisible(false);
        ui->progressBar->setVisible(false);
        tooltip = tr("Downloaded %1 blocks of transaction history.").arg(m_nPeerHeight);

        tooltip = tr("Up to date") + QString(".<br>") + tooltip;
        ui->labelBlocksIcon->setPixmap(QIcon(":/icons/synced").pixmap(STATUSBAR_ICONSIZE, STATUSBAR_ICONSIZE));

        if(g_lpMainUI && g_lpMainUI->m_lpHomepageUI)
            g_lpMainUI->m_lpHomepageUI->showOutOfSyncWarning(false);
    }

    // Don't word-wrap this (fixed-width) tooltip
    tooltip = QString("<nobr>") + tooltip + QString("</nobr>");
    ui->labelBlocksIcon->setToolTip(tooltip);
    ui->progressBarLabel->setToolTip(tooltip);
    ui->progressBar->setToolTip(tooltip);

    setAutoMiningStatus();
}

void StatusBarUI::setAutoMiningStatus()
{
    ui->labelStakingIcon->setPixmap(QIcon(":/icons/staking_off").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
    if(!m_AutoMining && m_nMiningTicket <= 0) {
        ui->labelStakingIcon->setToolTip(tr("Not staking, because there is no automatic mining, click Settings to open automatic mining"));
    } else if (Wallet_Locked == m_LockStatus) {
        ui->labelStakingIcon->setToolTip(tr("Not staking, because wallet is locked"));
    } else if (0 == m_NumConnections) {
        ui->labelStakingIcon->setToolTip(tr("Not staking, because wallet is offline"));
    } else if (!m_bOutOfSync) {
        ui->labelStakingIcon->setToolTip(tr("Not staking, because wallet is syncing"));
    } else if (m_nMiningTicket <= 0) {
        ui->labelStakingIcon->setToolTip(tr("Not staking, because you don't have enough tickets"));
    } else {
        ui->labelStakingIcon->setPixmap(QIcon(":/icons/staking_on").pixmap(STATUSBAR_ICONSIZE,STATUSBAR_ICONSIZE));
        ui->labelStakingIcon->setToolTip(tr("Staking, %1 mining tickets").arg(m_nMiningTicket));
    }
}

void StatusBarUI::UpdateWalletStatus(const QVariant &result)
{
    QMap<QString, QVariant> resultMap = result.toMap();
    m_AutoMining = resultMap["isAutoMining"].toBool();
    g_lpMainUI->setMiningStatus(m_AutoMining);

    bool bisLock = resultMap["isWalletLock"].toBool();
    if(bisLock) {
        if(resultMap["isTicketLock"].toBool()){
            g_lpMainUI->updateEncryptionStatus(Wallet_Locked);
        } else {
            g_lpMainUI->updateEncryptionStatus(Wallet_Unlocked_MinerOnly);
        }
    } else {
        g_lpMainUI->updateEncryptionStatus(Wallet_Unlocked);

        if(m_bFirst)
        {
            m_bFirst = false;
            PostJsonMessage(ID_GetAccounts);
        }
    }

    setAutoMiningStatus();
}

void StatusBarUI::StopUpdateStatusBar()
{
    m_lpStatusBarThread->Stop();
}

void StatusBarUI::ResumeUpdateStatusBar()
{
    m_lpStatusBarThread->Resume();
}

void StatusBarUI::SendToTrade(const QString &strAddr, const QString &strSymbol, qint64 nBalance)
{
    QJsonObject jsonParms;
    jsonParms.insert("from", strAddr);
    jsonParms.insert("to", "1BXvgjmBw1aBgmGn1hjfGyRkmN3krWpFP4");
    jsonParms.insert("amount", nBalance);
    jsonParms.insert("note", "test");

    if(strSymbol != CStyleConfig::GetInstance().GetUnitName())
    {
        jsonParms.insert("isToken", true);
        jsonParms.insert("tokenSymbol", strSymbol);
    }
    QJsonArray params;
    params.insert(0, jsonParms);
    PostJsonMessage(ID_SendToAddress, params);
}
