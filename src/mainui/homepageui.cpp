#include "homepageui.h"
#include "ui_homepageui.h"
#include "walletsendui.h"
#include "transactionslistui.h"
#include <QStandardItemModel>

HomepageUI::HomepageUI(QWidget *parent, const PlatformStyle *platformStyle) :
    QWidget(parent),
    ui(new Ui::HomepageUI)
{
    ui->setupUi(this);

    m_lpWalletSendUI = new WalletSendUI(this, platformStyle);
    m_lpTransactionsListUI = new TransactionsListUI(this);

    ui->horizontalStackedWidget1->addWidget(m_lpWalletSendUI);
    ui->horizontalStackedWidget1->setCurrentWidget(m_lpWalletSendUI);

    ui->horizontalStackedWidget2->addWidget(m_lpTransactionsListUI);
    ui->horizontalStackedWidget2->setCurrentWidget(m_lpTransactionsListUI);

    connect(m_lpWalletSendUI, SIGNAL(SendAddrSucceedUpData()), m_lpTransactionsListUI, SLOT(SendAddrSucceedUpData()));
}

HomepageUI::~HomepageUI()
{
    delete ui;
}

void HomepageUI::showOutOfSyncWarning(bool fShow)
{
    m_lpWalletSendUI->showOutOfSyncWarning(fShow);
    m_lpTransactionsListUI->showOutOfSyncWarning(fShow);
}

void HomepageUI::slotupdateWalletInfo(const QList<QVariant> &walletsList)
{
    m_lpWalletSendUI->UpdateWalletInfo(walletsList);
}

void HomepageUI::StopWalletSendUpdateThread()
{
    m_lpWalletSendUI->StopWalletSendUpdateThread();
}

void HomepageUI::ResumeWalletSendUpdateThread()
{
    m_lpWalletSendUI->ResumeWalletSendUpdateThread();
}
