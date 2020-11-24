#include "walletsendui.h"
#include "ui_walletsendui.h"
#include "addresslistui.h"
#include "addressui.h"
#include "guiutil.h"
#include "base58.h"
#include "basefuntion.h"
#include "mainui.h"
#include "basefuntion.h"
#include "base64.h"
#include "bitcoinunits.h"
#include "platformstyle.h"

extern MainUI*              g_lpMainUI;

GetTicketBalanceThread::GetTicketBalanceThread()
{
	m_mutex.lock();
	m_stop = false;
	m_mutex.unlock();
}

GetTicketBalanceThread::~GetTicketBalanceThread()
{
	Stop();
}

void GetTicketBalanceThread::run()
{
	QString cmd;
	while (true)
	{
		m_mutex.lock();
		if (m_stop) {
			m_mutex.unlock();
			return;
		}
		m_mutex.unlock();

		m_mutex.lock();
		if (m_cmd.isNull()) {
			m_mutex.unlock();
			msleep(100);
			continue;
		}
		else {
			cmd = m_cmd;
			m_cmd.clear();
			m_mutex.unlock();

			emit PostMsgGetTicketBalance(cmd);

			m_mutex.lock();
			m_condOK.wait(&m_mutex, MAX_TIMEOUT_WAIT_RESPONSE_RESULT);
			m_mutex.unlock();
		}
	}
}

void GetTicketBalanceThread::Stop()
{
	if (!isRunning())
	{
		return;
	}
	m_mutex.lock();
	m_stop = true;
	m_mutex.unlock();
	m_condOK.wakeOne();
	wait();
}

void GetTicketBalanceThread::Resume()
{
	if (isRunning()) {
		return;
	}
	m_mutex.lock();
	m_stop = false;
	m_mutex.unlock();
	start();
}

void GetTicketBalanceThread::SetTicketBalanceCmd(const QString &cmd)
{
	if (isRunning()) {
		m_mutex.lock();
		m_cmd = cmd;
		m_mutex.unlock();
	}
}

void GetTicketBalanceThread::Wakeup()
{
	if (isRunning()) {
		m_condOK.wakeOne();
	}
}

WalletSendUI::WalletSendUI(QWidget *parent, const PlatformStyle *platformStyle)
	: JsonConnectorWidget(parent)
	, ui(new Ui::WalletSendUI)
	, m_bFirst(true)
	, m_platformStyle(platformStyle)
{
	ui->setupUi(this);
	initUI();
}

WalletSendUI::~WalletSendUI()
{
	if (NULL != m_getTicketBalanceThread)
	{
		delete m_getTicketBalanceThread;
		m_getTicketBalanceThread = NULL;
	}
	delete ui;
}

void WalletSendUI::showOutOfSyncWarning(bool fShow)
{
	ui->labelWalletStatus->setVisible(fShow);
}

void WalletSendUI::SetTimeStatus(int nDiff)
{
	ui->labelErrorTime->setText(tr("时间误差: %1 秒").arg(QString::number(nDiff)));
	if (nDiff != 0)
	{
		ui->labelErrorTime->setVisible(true);
	}
	else
	{
		ui->labelErrorTime->setVisible(false);
	}
}

void WalletSendUI::initUI()
{
	this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());
	if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE) {
		this->setStyleSheet("QWidget {background-color:#FFFFFF;border:none;}" + CStyleConfig::GetInstance().GetStylesheet());
		ui->sendwidget1_3->setStyleSheet("QWidget {background-color:#F5F4F9;border-radius: 4px;}");
	}
	else {
		ui->sendwidget1_3->setStyleSheet("QWidget {background-color:#2c2c2c;border-radius: 4px;}");
	}

#ifndef MAC_OSX
	ui->labelTotal->setStyleSheet("QLabel { font: " + QString::number(GetBaseFontSize() + 4) + "pt;}");
	ui->labelBalance->setStyleSheet("QLabel { font: " + QString::number(GetBaseFontSize() + 2) + "pt;}");
	ui->labelFrozen->setStyleSheet("QLabel { font: " + QString::number(GetBaseFontSize() + 2) + "pt;}");
	ui->labelTotalText->setStyleSheet("QLabel { color: #969BA6; font: " + QString::number(GetBaseFontSize()) + "pt;}");
	ui->label_Balance_text->setStyleSheet("QLabel { color: #969BA6; font: " + QString::number(GetBaseFontSize()) + "pt;}");
	ui->label_Frozen_text->setStyleSheet("QLabel { color: #969BA6; font: " + QString::number(GetBaseFontSize()) + "pt;}");
#endif
	ui->verticalWidget0->setStyleSheet("border-radius:4px;");
	ui->verticalWidget1->setStyleSheet("border-radius:4px;");

#if QT_VERSION >= 0x040700
	ui->addAsLabel->setPlaceholderText(tr("输入发送交易的备注信息"));
	ui->payTo->setPlaceholderText(tr("输入%1地址(例如：1NS17iag9jJgTHD1VXjvLCEnZuQ3rJDE9L)").arg(CStyleConfig::GetInstance().GetAppName()));
#endif
	ui->addAsLabel->setToolTip(tr("输入发送交易的备注信息"));
	ui->payTo->setToolTip(tr("输入%1地址(例如：1NS17iag9jJgTHD1VXjvLCEnZuQ3rJDE9L)").arg(CStyleConfig::GetInstance().GetAppName()));

	GUIUtil::setupAddressWidget(ui->payTo, this);
	ui->fromAddr->setEnabled(false);
	ui->labelWalletStatus->setText("(" + tr("out of sync") + ")");
	showOutOfSyncWarning(true);

	ui->addAsLabel->setMaxLength(2000);
	ui->StakeWidget->setVisible(false);
	ui->UnconfirmedWidget->setVisible(false);

	UpDataUnitNameMinFee();

	ui->addAsLabel->setContextMenuPolicy(Qt::DefaultContextMenu);
	ui->payTo->setContextMenuPolicy(Qt::DefaultContextMenu);

	ui->labelErrorTime->setVisible(false);
	clear();
	m_getTicketBalanceThread = new GetTicketBalanceThread();
	connect(this, SIGNAL(SendGetTicketBalanceCmd(QString)), m_getTicketBalanceThread, SLOT(SetTicketBalanceCmd(QString)));
	connect(m_getTicketBalanceThread, SIGNAL(PostMsgGetTicketBalance(QString)), this, SLOT(DealPostMsgGetTicketBalance(QString)));
	m_getTicketBalanceThread->start();

	ui->addressFromButton->setIcon(m_platformStyle->SingleColorIcon(":/address_book"));
	ui->addressBookButton->setIcon(m_platformStyle->SingleColorIcon(":/address_book"));
}

void WalletSendUI::SetUpProperty(double dBalance, double dFrozen)
{
	QString strBalance = QString::number(dBalance, 'f', 4);
	ui->labelBalance->setText(strBalance);

	QString strFrozen = QString::number(dFrozen, 'f', 4);
	ui->labelFrozen->setText(strFrozen);

	double dAllCapital = dBalance + dFrozen;
	QString strAllCapital = QString::number(dAllCapital, 'f', 4);
	ui->labelTotal->setText(strAllCapital);
}

void WalletSendUI::SetMaxPropertyAddr(QString strAddr, double dBalance)
{
	ui->fromAddr->setText(strAddr);
	ui->payAmount->setAmountRange(dBalance);
}

void WalletSendUI::UpdateWalletInfo(const QList<QVariant> &walletsList)
{
	QString strMaxAddr;
	QString jsonCmd = "{\"addresses\":[";
	double dMaxBalance = 0.0;
	m_dBalance = 0.0;

	for (int i = 0; i < walletsList.size(); ++i) {
		QMap<QString, QVariant> addrMap = walletsList[i].toMap();

		jsonCmd.append("\"").append((addrMap["acc"].toMap())["addr"].toString().toStdString().c_str()).append("\"");
		if (i != walletsList.size() - 1) {
			jsonCmd.append(",");
		}
		else {
			jsonCmd.append("],");
		}

		m_dBalance += (addrMap["acc"].toMap())["balance"].toDouble();

		if (dMaxBalance < (addrMap["acc"].toMap())["balance"].toDouble()) {
			dMaxBalance = (addrMap["acc"].toMap())["balance"].toDouble();
			strMaxAddr = (addrMap["acc"].toMap())["addr"].toString();
		}

		if (ui->fromAddr->text() == (addrMap["acc"].toMap())["addr"].toString()) {
			ui->payAmount->setAmountRange(GetbalanceD((addrMap["acc"].toMap())["balance"].toDouble()));
		}
	}

	if (m_bFirst) {
		m_bFirst = false;
		SetMaxPropertyAddr(strMaxAddr, GetbalanceD(dMaxBalance));
	}

	if (CStyleConfig::GetInstance().GetCoinsType() == TOKEN_YCC) {
		jsonCmd.append("\"execer\":\"pos33\"}");
	}
	else {
		jsonCmd.append("\"execer\":\"ticket\"}");
	}
	emit SendGetTicketBalanceCmd(jsonCmd);
}

bool WalletSendUI::validate()
{
	// Check input validity
	bool retval = true;

	if (!ui->payAmount->validate()) {
		retval = false;
	}
	else {
		if (ui->payAmount->value() <= 0) {
			// Cannot send 0 coins or less
			ui->payAmount->setValid(false);
			retval = false;
		}
	}

	if (!ui->payTo->hasAcceptableInput() || !IsAddrValid(ui->payTo->text().toStdString().c_str())) {
		ui->payTo->setValid(false);
		retval = false;
	}

	if (ui->payTo->text().isEmpty()) {
		ui->payTo->setValid(false);
		retval = false;
	}

	return retval;
}

bool WalletSendUI::isClear()
{
	return ui->payTo->text().isEmpty();
}

void WalletSendUI::clear()
{
	ui->payTo->clear();
	ui->addAsLabel->clear();
	ui->payAmount->clear();
	ui->payTo->setFocus();
}

void WalletSendUI::on_addressFromButton_clicked()
{
	AddressListUI dlg(ForSending, TabsReceiving, this, m_platformStyle);
	if (dlg.exec()) {
		ui->fromAddr->setText(dlg.getReturnAddr());
		ui->payAmount->setAmountRange(dlg.getBalance());
	}
}

void WalletSendUI::on_addressBookButton_clicked()
{
	AddressListUI dlg(ForSending, TabsSending, this, m_platformStyle);
	if (dlg.exec()) {
		ui->payTo->setText(dlg.getReturnAddr());
		ui->payAmount->setFocus();
	}
	g_lpMainUI->m_lpAddressUI->m_lpFriendsAddressList->UpdateAddrList();
}

void WalletSendUI::on_clearButton_clicked()
{
	clear();
}

void WalletSendUI::on_sendButton_clicked()
{
    if(g_lpMainUI && !g_lpMainUI->GetOutOfSync()) {
        QMessageBox::information(this, tr("提示"), tr("钱包未同步成功，请稍等！"), tr("确定"));
        return;
    }

    // 1.验证地址,金额是否正确 2.弹框提醒 3.钱包如果加密锁定,要输入密码
    if(!validate())
        return;

    QString address = ui->payTo->text();
    qint64 amount = ui->payAmount->value();

    QString label;
    QStringList formatted;
    if(g_lpMainUI) {
        QMap<QString, QString> mapFriAddr = g_lpMainUI->m_lpAddressUI->m_lpFriendsAddressList->m_mapAddrLabel;
        label = mapFriAddr[address];
    }
    formatted.append(tr("<b>%1 %2</b> to %3 (%4)").arg(BitcoinUnits::format(BitcoinUnits::COIN, amount), CStyleConfig::GetInstance().GetUnitName(), label, address));

    int retval = QMessageBox::question(this, tr("Confirm send coins"), tr("Are you sure you want to send %1?").arg(formatted.join(tr(" and "))), tr("确定"), tr("取消"), QString(), 0, 1);
    if(retval != 0)
        return;


    EncryptionStatus nStatus = Wallet_Locked;
    if (g_lpMainUI) {
        nStatus = g_lpMainUI->m_nStatus;
    }

    if(Wallet_Unlocked != nStatus) {
        if(Wallet_Unlocked_MinerOnly == nStatus) {
            QMessageBox::information(this, tr("提示"), tr("钱包只解锁买票挖矿功能，请先解锁！"), tr("确定"));
        } else {
            QMessageBox::information(this, tr("提示"), tr("钱包当前是加密状态，请先解锁！"), tr("确定"));
        }
        return;
    }

    std::string strNode = ui->addAsLabel->text().toStdString();

    QJsonObject jsonParms;
    jsonParms.insert("from", ui->fromAddr->text());
    jsonParms.insert("to", address);
    jsonParms.insert("amount", amount);
    jsonParms.insert("note", strNode.c_str());
    QJsonArray params;
    params.insert(0, jsonParms);
    PostJsonMessage(ID_SendToAddress, params);
}

void WalletSendUI::contextualMenu(const QPoint &/*point*/)
{
	contextMenu->exec(QCursor::pos());
}

void WalletSendUI::requestFinished(const QVariant &result, const QString &error)
{
	QMap<QString, QVariant> resultMap = result.toMap();

	if (ID_SendToAddress == m_nID) {
		QString strHash = resultMap["hash"].toString();
		if (strHash.isEmpty()) {
			QMessageBox::information(this, tr("提示"), error, tr("确定"));
		}
		else {
			QMessageBox::information(this, tr("提示"), tr("发送成功，等区块链确认后，等待列表中刷新!"), tr("确定"));
			clear();

			PostJsonMessage(ID_GetAccounts);
		}
	}
	else if (ID_GetAccounts == m_nID) {
		QString strMaxAddr;
		double dMaxBalance = 0.0;
		double dBalance = 0.0;

		QMap<QString, QVariant> resultMap = result.toMap();
		QList<QVariant> walletsList = (resultMap["wallets"]).toList();
		for (int i = 0; i < walletsList.size(); ++i) {
			QMap<QString, QVariant> addrMap = walletsList[i].toMap();
			dBalance += (addrMap["acc"].toMap())["balance"].toDouble();

			if (dMaxBalance < (addrMap["acc"].toMap())["balance"].toDouble()) {
				dMaxBalance = (addrMap["acc"].toMap())["balance"].toDouble();
				strMaxAddr = (addrMap["acc"].toMap())["addr"].toString();
			}
		}

		SetMaxPropertyAddr(strMaxAddr, GetbalanceD(dMaxBalance));
		SetUpProperty(GetbalanceD(dBalance), GetbalanceD(m_dFrozen));
	}
	else if (ID_GetBalance_ticket == m_nID) {
		m_dFrozen = 0.0;

		QList<QVariant> BalanceList = result.toList();
		for (int i = 0; i < BalanceList.size(); ++i) {
			QMap<QString, QVariant> addrMap = BalanceList[i].toMap();
			m_dFrozen += addrMap["balance"].toDouble();
			m_dFrozen += addrMap["frozen"].toDouble();
		}

		SetUpProperty(GetbalanceD(m_dBalance), GetbalanceD(m_dFrozen));
		if (NULL != m_getTicketBalanceThread)
		{
			m_getTicketBalanceThread->Wakeup();
		}
	}
}

void WalletSendUI::DealPostMsgGetTicketBalance(const QString &cmd)
{
	PostJsonMessage(ID_GetBalance_ticket, cmd.toStdString().c_str());
}

void WalletSendUI::StopWalletSendUpdateThread()
{
	if (NULL != m_getTicketBalanceThread)
	{
		m_getTicketBalanceThread->Stop();
	}
}

void WalletSendUI::ResumeWalletSendUpdateThread()
{
	if (NULL != m_getTicketBalanceThread)
	{
		m_getTicketBalanceThread->Resume();
	}
}

void WalletSendUI::UpDataUnitNameMinFee()
{
	ui->labelTotalText->setText(tr("总额 (%1)").arg(CStyleConfig::GetInstance().GetUnitName()));
	ui->label_Balance_text->setText(tr("可用余额 (%1)").arg(CStyleConfig::GetInstance().GetUnitName()));
	ui->label_Frozen_text->setText(tr("挖矿冻结金额 (%1)").arg(CStyleConfig::GetInstance().GetUnitName()));
	ui->label_Stake_text->setText(tr("挖矿确认 (%1)").arg(CStyleConfig::GetInstance().GetUnitName()));
	QString strMinFeeText = tr("手续费仅 %1 ").arg(CStyleConfig::GetInstance().GetMinFee());
	strMinFeeText = strMinFeeText + CStyleConfig::GetInstance().GetUnitName();
	ui->label_fee_text->setText(strMinFeeText);
}
