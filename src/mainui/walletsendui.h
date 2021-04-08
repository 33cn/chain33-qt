#ifndef WALLETSENDUI_H
#define WALLETSENDUI_H

/*
 * 总额和发送交易 界面
 */

class SendCoinsRecipient;
class PlatformStyle;

#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "basejsonconnector.h"

namespace Ui {
	class WalletSendUI;
}

class GetTicketBalanceThread :public QThread
{
	Q_OBJECT
public:
	GetTicketBalanceThread();
	~GetTicketBalanceThread();
	void Stop();
	void Resume();
public slots:
	void SetTicketBalanceCmd(const QString &cmd);

	void Wakeup();

signals:
	void PostMsgGetTicketBalance(const QString &cmd);
protected:
	virtual void run();
private:
	bool m_stop;
	QString m_cmd;
	QMutex m_mutex;
	QWaitCondition m_condOK;
};

class WalletSendUI : public JsonConnectorWidget
{
	Q_OBJECT

public:
	explicit WalletSendUI(QWidget *parent, const PlatformStyle *platformStyle);
	~WalletSendUI();

	bool validate();
	bool isClear();
	void showOutOfSyncWarning(bool fShow);

	void SetTimeStatus(int nDiff);
	void UpdateWalletInfo(const QList<QVariant> &walletsList);
	void StopWalletSendUpdateThread();
	void ResumeWalletSendUpdateThread();
	void UpDataUnitNameMinFee();

signals:
	void SendAddrSucceedUpData();
	void SendGetTicketBalanceCmd(const QString &cmd);

private:
	void initUI();
	void SetTicketBalanceCmd(const QString &cmd);

public slots:
	void SetUpProperty(double dBalance, double dFrozen);
	void SetMaxPropertyAddr(QString strAddr, double dBalance);
	void clear();

private slots:
	void on_addressFromButton_clicked();
	void on_addressBookButton_clicked();
	void on_clearButton_clicked();
	void on_sendButton_clicked();
	void contextualMenu(const QPoint &point);
	void DealPostMsgGetTicketBalance(const QString &cmd);

protected:
	virtual void requestFinished(const QVariant &result, const QString &error);

private:
	Ui::WalletSendUI *ui;

	qint64 currentBalance;
	qint64 currentStake;
	qint64 currentUnconfirmedBalance;
	qint64 currentImmatureBalance;

	bool    m_bFirst;   // 是否是第一次登录，如果是第一次，要 add 列表，以后是刷新，同时显示最多金额的地址

	double m_dBalance;
	double m_dFrozen;

	QMenu* contextMenu;
	GetTicketBalanceThread *m_getTicketBalanceThread;
	const PlatformStyle *m_platformStyle;
};

#endif // WALLETSENDUI_H
