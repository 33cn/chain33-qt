#ifndef MAINUI_H
#define MAINUI_H

/*
 * 主界面
 */

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <stdint.h>
#include <QNetworkAccessManager>
#include "basejsonconnector.h"
#include "enumtype.h"

class PlatformStyle;
class HomepageUI;
class AddressUI;
class Notificator;
class StatusBarUI;
class CliConsole;
class CToCtransactionUI;
//class AssetsManageUi;

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QTableView;
class QAbstractItemModel;
class QModelIndex;
class QProgressBar;
class QStackedWidget;
class QUrl;
QT_END_NAMESPACE

class MainUI : public JsonConnectorMainWindow
{
    Q_OBJECT

public:
    explicit MainUI(QString &stylesheet, QWidget *parent = 0);
    ~MainUI();

    void StopCommunicateChain33Thread();
    void ResumeCommunicateChain33Thread();
private:
    void initUI();

    /** Create the main UI actions. */
    void createActions();
    /** Create the menu bar and sub-menus. */
    void createMenuBar();
    /** Create the toolbars */
    void createToolBars();
    /** Create system tray (notification) icon */
    void createTrayIcon();

    void createStatusBar();

public:
    HomepageUI* m_lpHomepageUI;
    AddressUI*  m_lpAddressUI;
    StatusBarUI* m_lpStatusBarUI;

    QString     m_strPsd;
    QString     m_strVersion;
    EncryptionStatus m_nStatus; // 当前锁定状态
private:
    QAction *m_lpHomepageAction;
    QAction *m_lpAddressAction;

    QStackedWidget *centralWidget;
    QMenuBar *appMenuBar;

    QSystemTrayIcon *trayIcon;
    Notificator *notificator;

    QAction *quitAction;
    QAction *changePassphraseAction;
    QAction *unlockWalletAction;
    QAction *lockWalletAction;
    QAction *openminingAction;
    QAction *closeminingAction;
    QAction *openWalletDirAction;
    QAction *devolutionAction;
    QAction *changeDirAction;
    QAction *openRepairTimeAction;
    QAction *closeRepairTimeAction;
    QAction *veifySeedAction;
    QAction *openRPCConsoleAction;
    QAction *aboutAction;
    QAction *toggleHideAction;

    QNetworkAccessManager *m_lpaccessManager;

    CliConsole *rpcConsole;

    const PlatformStyle *m_platformStyle;
public:
    // 如果是导入钱包 默认创建 5 个地址
    bool GetOutOfSync();
    void SetTimeStatus(int nDiff);

signals:
    void UpdateWalletStatus(const QVariant &result);
public slots:
    void setEncryptionStatus(EncryptionStatus status);
    void setMiningStatus(bool bMining);

private slots:
    void gotoOverviewPage();
    void gotoAddressBookPage();

    /** Show window if hidden, unminimize when minimized, rise when obscured or show if hidden and fToggleHidden is true */
    void showNormalIfMinimized(bool fToggleHidden = false);

    void changePassphrase();
    void unlockWallet();
    void lockWallet();
    void openMining();
    void closeMining();
    void aboutClicked();
    void toggleHidden();
    void openWalletDir();
    void devolutionMining();
    void changeDir();
    void openRepairTime();
    void closeRepairTime();
    void veifySeed();
    void setRepairTimeStatus(bool bStatus);
    void iconIsActived(QSystemTrayIcon::ActivationReason reason);

    virtual void requestFinished(const QVariant &result, const QString &error);
};

#endif // MAINUI_H
