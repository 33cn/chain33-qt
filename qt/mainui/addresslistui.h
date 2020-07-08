#ifndef ADDRESSLISTUI_H
#define ADDRESSLISTUI_H

/*
 * 单个地址界面
 */

#include <QDialog>
#include <QAction>
#include <QMenu>
#include <QApplication>
#include <QSortFilterProxyModel>
#include <QMap>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "txviewitemdelegate.h"
#include "basejsonconnector.h"
#include "enumtype.h"
#include "friendsaddrlistxml.h"

class AddressListThread : public QThread
{
    Q_OBJECT
public:
    AddressListThread();
    ~AddressListThread();
    void Wakeup();
    void Stop();
    void Resume();
signals:
    void PostMsgGetAccounts();

protected:
    virtual void run();
private:
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_stop;
};

class AddressTableModel;
class PlatformStyle;

namespace Ui {
class AddressListUI;
}

class AddressListUI : public JsonConnectorDialog
{
    Q_OBJECT

public:
    explicit AddressListUI(AddrModeType mode, AddrTypeTabs tab, QWidget *parent, const PlatformStyle *platformStyle);
    ~AddressListUI();

    const QString &getReturnAddr() const { return m_strReturnAddr; }
    const QString &getReturnLabel() const { return m_strReturnLabel; }
    const double &getBalance() const { return m_dBalance; }

    // 新建好友地址 同步
    void UpdateAddrList();

    void StopAddressListThread();
    void ResumeMyAddressListThread();
signals:
    void updateWalletInfo(const QList<QVariant> &walletsList);

public slots:
    void PostMsgGetAccounts();

private slots:
    void on_outputButton_clicked();

    void on_addButton_clicked();

    void on_deleteButton_clicked();

    void on_copyToClipboard_clicked();

    void onCopyLabelAction();

    void onEditAction();

    void contextualMenu(const QPoint &point);    

public slots:
    void done(int retval);

protected:
    virtual void requestFinished(const QVariant &result, const QString &error);

public:
    QMap<QString, QString> m_mapAddrLabel;

private:
    Ui::AddressListUI *ui;

    AddrModeType mode;
    AddrTypeTabs tab;

    QString newAddressToSelect;
    TxViewItemDelegate *txdelegate;
    QAction* deleteAction;
    QMenu* contextMenu;

    QSortFilterProxyModel* proxyModel;
    AddressTableModel* model;

    QString m_strReturnAddr;
    QString m_strReturnLabel;
    double  m_dBalance;

    FriendsAddrListXml* m_lpFriendsAddrListXml;
    AddressListThread*  m_lpAddressListThread;

    bool    m_bFirst;   // 是否是第一次登录，如果是第一次，要 add 列表，以后是刷新，同时显示最多金额的地址
};

#endif // ADDRESSLISTUI_H
