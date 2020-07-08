#include "mainui.h"
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QIcon>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLocale>
#include <QMessageBox>
#include <QMimeData>
#include <QProgressBar>
#include <QStackedWidget>
#include <QDateTime>
#include <QMovie>
#include <QFileDialog>
#include <QDesktopServices>
#include <QTimer>
#include <QDragEnterEvent>
#include <QUrl>
#include <QStyle>
#include <QFile>
#include <iostream>
#ifdef Q_OS_MAC
#include "macdockiconhandler.h"
#endif
#include "homepageui.h"
#include "addressui.h"
#include "notificator.h"
#include "guiconstants.h"
#include "statusbarui.h"
#include "aboutdialog.h"
#include "askpassphrasedialog.h"
#include "addresslistui.h"
#include "cliconsole.h"
#include "basefuntion.h"
#include "manageui.h"
#include "offlineminingdialog.h"
#include "changedirdialog.h"
#include "walletsendui.h"
#include "closeingdialog.h"
#include "veifyseeddialog.h"
#include "platformstyle.h"

MainUI*             g_lpMainUI = NULL;
EncryptionStatus    g_nStatus;          // 当前锁定状态
QString             g_strVersion;
extern ManageUI*    g_lpManageUI;
QString             g_strPsd;

MainUI::MainUI(QString &/*stylesheet*/, QWidget *parent)
    : JsonConnectorMainWindow(parent)
    , trayIcon(0)
    , notificator(0)
    , rpcConsole (NULL)
{
    g_lpMainUI = this;
    initUI();
}

MainUI::~MainUI()
{
    if(trayIcon) // Hide tray icon, as deleting will let it linger until quit (on Ubuntu)
        trayIcon->hide();
#ifdef Q_OS_MAC
    delete appMenuBar;
#endif

    if (g_lpaccessManager)
    {
        delete g_lpaccessManager;
        g_lpaccessManager = NULL;
    }

    if(m_platformStyle) {
        delete m_platformStyle;
        m_platformStyle = NULL;
    }
}

void MainUI::initUI()
{
#ifdef Q_OS_MAC
    resize(1000, 495);
#else
    resize(1000, 520);
#endif    
    PostJsonMessage(ID_GetVersion);

    setWindowTitle(tr("%1钱包-正式版 %2").arg(CStyleConfig::GetInstance().GetAppName(), g_strVersion));

    m_platformStyle = PlatformStyle::instantiate("other");
#ifndef Q_OS_MAC
    QString strMainIcon;
    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE){
        strMainIcon = ":/icons/yccIcon";
    } else {
        strMainIcon = ":/icons/bitcoin";
    }
    qApp->setWindowIcon(QIcon(strMainIcon));
    setWindowIcon(QIcon(strMainIcon));
#else
    setUnifiedTitleAndToolBarOnMac(true);
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif
    setAcceptDrops(true);

    createActions();
    createMenuBar();
    createToolBars();
    createTrayIcon();

    statusBar()->setSizeGripEnabled(false);

    m_lpHomepageUI = new HomepageUI(this, m_platformStyle);
    m_lpAddressUI = new AddressUI(this, m_platformStyle);
    connect(m_lpAddressUI, SIGNAL(signalupdateWalletInfo(QList<QVariant>)), m_lpHomepageUI, SLOT(slotupdateWalletInfo(QList<QVariant>)));

    centralWidget = new QStackedWidget(this);
    centralWidget->addWidget(m_lpHomepageUI);
    centralWidget->addWidget(m_lpAddressUI);

  //  centralWidget->addWidget(m_lpAssetsManageUi);
    setCentralWidget(centralWidget);

    m_lpHomepageAction->setChecked(true);
    centralWidget->setCurrentWidget(m_lpHomepageUI);

    createStatusBar();

    // 启动的时候 钱包默认为加锁状态
    setEncryptionStatus(Wallet_Locked);
    setMiningStatus(false);
    setRepairTimeStatus(GetRepairTimeConfig());
}

void MainUI::createActions()
{
    QActionGroup *tabGroup = new QActionGroup(this);

    m_lpHomepageAction = new QAction(tr("我的主页"), this);
    m_lpHomepageAction->setToolTip(tr("显示钱包的概况"));
    m_lpHomepageAction->setCheckable(true);
    tabGroup->addAction(m_lpHomepageAction);

    m_lpAddressAction = new QAction(tr("地址簿"), this);
    m_lpAddressAction->setToolTip(tr("编辑存储地址和标签的列表"));
    m_lpAddressAction->setCheckable(true);
    tabGroup->addAction(m_lpAddressAction);

    connect(m_lpHomepageAction, SIGNAL(triggered()), this, SLOT(showNormalIfMinimized()));
    connect(m_lpHomepageAction, SIGNAL(triggered()), this, SLOT(gotoOverviewPage()));
    connect(m_lpAddressAction, SIGNAL(triggered()), this, SLOT(showNormalIfMinimized()));
    connect(m_lpAddressAction, SIGNAL(triggered()), this, SLOT(gotoAddressBookPage()));

    quitAction = new QAction(QIcon(":/icons/quit"), tr("E&xit"), this);
    quitAction->setToolTip(tr("Quit application"));
    quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    quitAction->setMenuRole(QAction::QuitRole);

    changePassphraseAction = new QAction(m_platformStyle->SingleColorIcon(":/icons/key"), tr("更改密码(&C)"), this);
    unlockWalletAction = new QAction(QIcon(":/icons/lock_closed"), tr("解锁钱包(&U)"), this);
    lockWalletAction = new QAction(m_platformStyle->SingleColorIcon(":/icons/lock_open"), tr("锁定钱包(&L)"), this);
    openminingAction = new QAction(QIcon(":/icons/mining_closed"), tr("开启自动购票(&O)"), this);
    closeminingAction = new QAction(m_platformStyle->SingleColorIcon(":/icons/mining_open"), tr("关闭自动购票(&S)"), this);
    openRepairTimeAction = new QAction(QIcon(":/icons/RepairTime_closed"), tr("开启自动修复时间(&R)"), this);
    closeRepairTimeAction = new QAction(m_platformStyle->SingleColorIcon(":/icons/RepairTime_open"), tr("关闭自动修复时间(&T)"), this);
    devolutionAction = new QAction(m_platformStyle->SingleColorIcon(":/icons/offline_mining"), tr("离线挖矿授权(&A)"), this);
    openWalletDirAction = new QAction(m_platformStyle->SingleColorIcon(":/icons/open_walletdir"), tr("打开数据目录(&D)"), this);
    changeDirAction = new QAction(m_platformStyle->SingleColorIcon(":/icons/change_datadir"), tr("修改数据目录(&M)"), this);
    veifySeedAction = new QAction(m_platformStyle->SingleColorIcon(":/icons/veify_seed"), tr("验证助记词(&V)"), this);
    openRPCConsoleAction = new QAction(QIcon(":/icons/debugwindow"), tr("调试窗口(&D)"), this);

    QString strMainIcon;
    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE){
        strMainIcon = ":/icons/yccIcon";
    } else {
        strMainIcon = ":/icons/bitcoin";
    }
    aboutAction = new QAction(m_platformStyle->SingleColorIcon(strMainIcon), tr("关于%1(&A)").arg(CStyleConfig::GetInstance().GetAppName()), this);

    connect(quitAction, SIGNAL(triggered()), g_lpManageUI, SLOT(CloseChain33()));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(changePassphraseAction, SIGNAL(triggered()), this, SLOT(changePassphrase()));
    connect(unlockWalletAction, SIGNAL(triggered()), this, SLOT(unlockWallet()));
    connect(lockWalletAction, SIGNAL(triggered()), this, SLOT(lockWallet()));
    connect(openminingAction, SIGNAL(triggered()), this, SLOT(openMining()));
    connect(openWalletDirAction, SIGNAL(triggered()), this, SLOT(openWalletDir()));
    connect(devolutionAction, SIGNAL(triggered()), this, SLOT(devolutionMining()));
    connect(changeDirAction, SIGNAL(triggered()), this, SLOT(changeDir()));
    connect(closeminingAction, SIGNAL(triggered()), this, SLOT(closeMining()));
    connect(openRepairTimeAction, SIGNAL(triggered()), this, SLOT(openRepairTime()));
    connect(closeRepairTimeAction, SIGNAL(triggered()), this, SLOT(closeRepairTime()));
    connect(veifySeedAction, SIGNAL(triggered()), this, SLOT(veifySeed()));

    rpcConsole = new CliConsole(this);
    connect(openRPCConsoleAction, SIGNAL(triggered()), rpcConsole, SLOT(show()));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(aboutClicked()));
    toggleHideAction = new QAction(QIcon(strMainIcon), tr("&Show / Hide"), this);
    connect(toggleHideAction, SIGNAL(triggered()), this, SLOT(toggleHidden()));
}

void MainUI::createMenuBar()
{
#ifdef Q_OS_MAC
    // Create a decoupled menu bar on Mac which stays even if the window is closed
    appMenuBar = new QMenuBar();
#else
    // Get the main window's menu bar on other platforms
    appMenuBar = menuBar();
#endif

    QMenu *settings = appMenuBar->addMenu(tr("&Settings"));
    settings->addAction(changePassphraseAction);
    settings->addAction(unlockWalletAction);
    settings->addAction(lockWalletAction);
    settings->addAction(openminingAction);
    settings->addAction(closeminingAction);
    settings->addAction(openRepairTimeAction);
    settings->addAction(closeRepairTimeAction);
    settings->addAction(openWalletDirAction);
#ifdef WIN32
    settings->addAction(changeDirAction);
#endif
    settings->addAction(devolutionAction);
    settings->addAction(veifySeedAction);

    QMenu *help = appMenuBar->addMenu(tr("&Help"));
    help->addAction(openRPCConsoleAction);
    help->addAction(aboutAction);
}

void MainUI::createToolBars()
{
    QToolBar *toolbar = addToolBar(tr("Tabs toolbar"));
    toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE){
        toolbar->setStyleSheet("QWidget {background-color:#9A9FB6;border:none;} QToolButton { background-color: transparent; width: 100px; color: #DDDEE3;} QToolButton:hover { color: #EEEFF4; } QToolButton:checked, QToolButton:pressed { color: #ffffff; }");
    } else {
        toolbar->setStyleSheet("QToolButton { background-color: transparent; width: 100px; color: #ffffff;} QToolButton:hover { background-color: #2c2c2c; } QToolButton:checked, QToolButton:pressed { background-color: transparent; color: #ffba26; }");
    }
    toolbar->addAction(m_lpHomepageAction);
    toolbar->addAction(m_lpAddressAction);
    toolbar->setMovable(false); // 不可拖动
    toolbar->resize(100, 50);
}

void MainUI::createTrayIcon()
{
    QMenu *trayIconMenu;
#ifndef Q_OS_MAC
    trayIcon = new QSystemTrayIcon(this);
    trayIconMenu = new QMenu(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip(tr("%1 客户端").arg(CStyleConfig::GetInstance().GetAppName()));
    QString strIcon;
    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE){
        strIcon = ":/icons/yccSmallIcon";
    } else {
        strIcon = ":/icons/toolbar";
    }
    trayIcon->setIcon(QIcon(strIcon));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();
#else
    // Note: On Mac, the dock icon is used to provide the tray's functionality.
    MacDockIconHandler *dockIconHandler = MacDockIconHandler::instance();
    dockIconHandler->setMainWindow((QMainWindow *)this);
    trayIconMenu = dockIconHandler->dockMenu();
#endif

    trayIconMenu->addAction(toggleHideAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(openRPCConsoleAction);
#ifndef Q_OS_MAC // This is built-in on Mac
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
#endif

    notificator = new Notificator(qApp->applicationName(), trayIcon);
}

void MainUI::createStatusBar()
{
    // Create status bar
    statusBar();
    m_lpStatusBarUI = new StatusBarUI(this, m_platformStyle);
    connect(this, SIGNAL(UpdateWalletStatus(QVariant)), m_lpStatusBarUI, SLOT(UpdateWalletStatus(QVariant)));
    statusBar()->addPermanentWidget(m_lpStatusBarUI, 1);
}

bool MainUI::GetOutOfSync()
{
    return m_lpStatusBarUI->m_bOutOfSync;
}

void MainUI::SetTimeStatus(int nDiff)
{
    m_lpHomepageUI->m_lpWalletSendUI->SetTimeStatus(nDiff);
}

void MainUI::gotoOverviewPage()
{
    m_lpHomepageAction->setChecked(true);
    centralWidget->setCurrentWidget(m_lpHomepageUI);
}

void MainUI::gotoAddressBookPage()
{
    m_lpAddressAction->setChecked(true);
    centralWidget->setCurrentWidget(m_lpAddressUI);
    m_lpAddressUI->m_lpMyAddressList->PostMsgGetAccounts();
}

void MainUI::showNormalIfMinimized(bool fToggleHidden)
{
    if (isHidden())
    {
        show();
        activateWindow();
    }
    else if (isMinimized())
    {
        showNormal();
        activateWindow();
    }
    else if(fToggleHidden)
        hide();
}

void MainUI::setEncryptionStatus(EncryptionStatus status)
{
    g_nStatus = status;
    switch(status)
    {
    case Wallet_Unlocked_MinerOnly:
        changePassphraseAction->setEnabled(true);
        unlockWalletAction->setVisible(false);
        lockWalletAction->setVisible(true);
        lockWalletAction->setIcon(QIcon(":/icons/lock_open_mineronly"));
        break;
    case Wallet_Unlocked:
        changePassphraseAction->setEnabled(true);
        unlockWalletAction->setVisible(false);
        lockWalletAction->setVisible(true);
        lockWalletAction->setIcon(m_platformStyle->SingleColorIcon(":/icons/lock_open"));
        break;
    case Wallet_Locked:
        changePassphraseAction->setEnabled(true);
        lockWalletAction->setVisible(false);
        unlockWalletAction->setVisible(true);
        break;
    }

    m_lpStatusBarUI->setEncryptionStatus(status);
}

void MainUI::setMiningStatus(bool bMining)
{
    openminingAction->setVisible(!bMining);
    closeminingAction->setVisible(bMining);
}

void MainUI::changePassphrase()
{
    AskPassphraseDialog dlg(AskPassphraseDialog::ChangePass, this);
    dlg.exec();
}

void MainUI::unlockWallet()
{
    AskPassphraseDialog dlg(AskPassphraseDialog::Unlock, this);
    if (dlg.exec())
    {
        setEncryptionStatus(dlg.m_nStatus);
    }
}

void MainUI::lockWallet()
{
    PostJsonMessage(ID_Lock);
}

void MainUI::openMining()
{
    PostJsonMessage(ID_SetAutoMining, "{\"flag\":1}");
}

void MainUI::closeMining()
{
    PostJsonMessage(ID_SetAutoMining, "{\"flag\":0}");
}

void MainUI::aboutClicked()
{
    AboutDialog dlg;
    dlg.exec();
}

void MainUI::toggleHidden()
{
    showNormalIfMinimized(true);
}

void MainUI::openWalletDir()
{
#ifndef WIN32
    QDesktopServices::openUrl(QUrl::fromLocalFile(GetDefaultDataDir()));
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile("file:" + GetDefaultDataDir()));
#endif
}

void MainUI::devolutionMining()
{
    OfflineMiningDialog dlg(this, m_platformStyle);
    dlg.exec();
}

void MainUI::changeDir()
{
    ChangeDirDialog dlg;
    int r = dlg.exec();
    m_lpStatusBarUI->RestartChain33Init();
    if(r == 2)
    {
        ResumeCommunicateChain33Thread();
    }
}

void MainUI::openRepairTime()
{
    setRepairTimeStatus(true);
    CloseingDialog dlg(this, RestartNewChain33);
    dlg.exec();
}

void MainUI::closeRepairTime()
{
    setRepairTimeStatus(false);
    CloseingDialog dlg(this, RestartNewChain33);
    dlg.exec();
}

void MainUI::veifySeed()
{
    veifySeedDialog dlg(this);
    dlg.exec();
}

void MainUI::setRepairTimeStatus(bool bStatus)
{
    openRepairTimeAction->setVisible(!bStatus);
    closeRepairTimeAction->setVisible(bStatus);
    SetRepairTimeConfig(bStatus);
}

void MainUI::iconIsActived(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    //点击托盘显示窗口
    case QSystemTrayIcon::Trigger:
    {
        showNormal();
        break;
    }
    //双击托盘显示窗口
    case QSystemTrayIcon::DoubleClick:
    {
        showNormal();
        break;
    }
    default:
        break;
    }
}

void MainUI::requestFinished(const QVariant &result, const QString &/*error*/)
{
    QMap<QString, QVariant> resultMap = result.toMap();
    if (m_nID == ID_Lock)
    {
        bool isOK = resultMap["isOK"].toBool();
        if(isOK)
        {
            setEncryptionStatus(Wallet_Locked);
        }
        else
        {
            // error
            QMessageBox::critical(this, tr("Wallet lock failed"), g_mapErrorCode[resultMap["msg"].toString()], tr("确定"));
        }
    }
    else if (m_nID == ID_SetAutoMining)
    {
        bool isOK = resultMap["isOK"].toBool();
        if(isOK)
        {
            PostJsonMessage(ID_GetWalletStatus);
        }
        else
        {
            QMessageBox::critical(this, tr("AutoMining failed"), g_mapErrorCode[resultMap["msg"].toString()], tr("确定"));
        }
    }
    else if(ID_ImportPrivKey == m_nID)
    {
        // "result":{"label":"string","acc":{"currency":int32,"balance":int64,"frozen":int64,"addr":"string"}}
        QMap<QString, QVariant> accMap = resultMap["acc"].toMap();
        return;
    }
    else if (ID_GetVersion == m_nID)
    {
        if(!resultMap["chain33"].toString().isEmpty())
        {
            g_strVersion = "chain33:" + resultMap["chain33"].toString() + " app:" + resultMap["app"].toString() + " localDb:" + resultMap["localDb"].toString();
            setWindowTitle(tr("%1钱包-正式版 %2").arg(CStyleConfig::GetInstance().GetAppName(), g_strVersion));
#ifdef QT_DEBUG
            setWindowTitle(tr("%1钱包-test %2").arg(CStyleConfig::GetInstance().GetAppName(), g_strVersion));
#endif
        }
        else
        {
            PostJsonMessage(ID_GetVersion);
        }
    }
    else if (ID_GetWalletStatus == m_nID)
    {
        emit UpdateWalletStatus(result);
    }
}

void MainUI::StopCommunicateChain33Thread()
{
    m_lpStatusBarUI->StopUpdateStatusBar();
    m_lpAddressUI->StopMyAddressListThread();
    m_lpHomepageUI->StopWalletSendUpdateThread();
}

void MainUI::ResumeCommunicateChain33Thread()
{
    m_lpStatusBarUI->ResumeUpdateStatusBar();
    m_lpAddressUI->ResumeMyAddressListThread();
    m_lpHomepageUI->ResumeWalletSendUpdateThread();
}
