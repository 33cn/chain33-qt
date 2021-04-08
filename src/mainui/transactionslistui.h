#ifndef TRANSACTIONSLISTUI_H
#define TRANSACTIONSLISTUI_H

/*
 * 交易列表 界面
 */

#include <QWidget>
#include <QMenu>
#include <QThread>
#include <QStandardItemModel>
#include <QFont>
#include <QColor>
#include <QMutex>

#include "txviewitemdelegate.h"
#include "basejsonconnector.h"
#include "enumtype.h"

class TransactionsThread : public QThread
{
    Q_OBJECT
public:
    TransactionsThread() {m_bRunPostMsg = true;}
    void SetRunPostMsg(bool bRunPostMsg);

signals:
    void PostMsgWalletTxList();

protected:
    virtual void run();

private:
    bool    m_bRunPostMsg;
    QMutex m_mutex;
};

class TransactionFilterProxy;
class TransactionsListModel;

namespace Ui {
class TransactionsListUI;
}

enum DateEnum
{
    All,
    Today,
    ThisWeek,
    ThisMonth,
    LastMonth,
    ThisYear,
    Range
};

class TransactionsListUI : public JsonConnectorWidget
{
    Q_OBJECT

public:
    explicit TransactionsListUI(QWidget *parent = 0);
    ~TransactionsListUI();
    void showOutOfSyncWarning(bool fShow);

protected:
    virtual void requestFinished(const QVariant &result, const QString &error);

signals:
    void transactionClicked(const QModelIndex &index);

private:
    void initUI();  // 暂时不调用 过滤等功能不增加
    void createDateRangeWidget();

public slots:
    void PostMsgWalletTxList();
    void chooseDate(int idx);
    void chooseType(int idx);
    void changedPrefix(const QString &prefix);
    void changedAmount(const QString &amount);
    void focusTransaction(const QModelIndex&);

private slots:    
    void on_firstPageBtn_clicked();

    void on_prevPageBtn_clicked();

    void on_nextPageBtn_clicked();

    void SendAddrSucceedUpData();

    void contextualMenu(const QPoint &);
    void dateRangeChanged();
    void copyAddress();
    void copyLabel();
    void copyAmount();
    void copyTxID();

    void on_listTransactions_doubleClicked(const QModelIndex &index);

    void on_pushButton_clicked();

private:
    Ui::TransactionsListUI *ui;
    TxViewItemDelegate *txdelegate;
    TransactionFilterProxy *transactionProxyModel;
    QMenu *contextMenu;
    TransactionsListModel* model;

    TransactionsThread*     m_lpTransactionsThread;
    QString                 m_strFromTx;
    QString                 m_strFromTxPrev;
    QString                 m_strFromTxNext;
    int                     m_nPage;        // 当前页数 0表示首页 下一页++ 上一页--

    int                     m_ndirection;
    int                     m_nMaxHeight;
    int                     m_nMaxIndex;

    bool                    m_bFirstPost; // 数据返回第一条都是重复 除了第一次获取的结果，其他都过滤
    bool                    m_bSyncFinish;  // 第一次同步成功 刷新首页
};

#endif // TRANSACTIONSLISTUI_H
