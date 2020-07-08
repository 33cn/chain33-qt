#ifndef HOMEPAGEUI_H
#define HOMEPAGEUI_H

/*
 * 我的主页 界面
 */

#include <QWidget>

class WalletSendUI;
class TransactionsListUI;
class PlatformStyle;

namespace Ui {
class HomepageUI;
}

class HomepageUI : public QWidget
{
    Q_OBJECT

public:
    explicit HomepageUI(QWidget *parent, const PlatformStyle *platformStyle);
    ~HomepageUI();

    void showOutOfSyncWarning(bool fShow);
    void StopWalletSendUpdateThread();
    void ResumeWalletSendUpdateThread();
signals:
    void signalupdateWalletInfo(const QList<QVariant> &walletsList);

public slots:
    void slotupdateWalletInfo(const QList<QVariant> &walletsList);

public:
    WalletSendUI*       m_lpWalletSendUI;

private:
    Ui::HomepageUI *ui;
    TransactionsListUI* m_lpTransactionsListUI;
};

#endif // HOMEPAGEUI_H
