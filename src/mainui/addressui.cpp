#include "addressui.h"
#include "ui_addressui.h"
#include "addresslistui.h"

AddressUI::AddressUI(QWidget *parent, const PlatformStyle *platformStyle) :
    QWidget(parent),
    ui(new Ui::AddressUI)
{
    ui->setupUi(this);

    m_lpMyAddressList = new AddressListUI(ForEditing, TabsReceiving,  this, platformStyle);
    m_lpFriendsAddressList = new AddressListUI(ForEditing, TabsSending, this, platformStyle);

    ui->horizontalStackedWidget1->addWidget(m_lpMyAddressList);
    ui->horizontalStackedWidget1->setCurrentWidget(m_lpMyAddressList);
    ui->horizontalStackedWidget1->setStyleSheet("border-radius:4px;");

    ui->horizontalStackedWidget2->addWidget(m_lpFriendsAddressList);
    ui->horizontalStackedWidget2->setCurrentWidget(m_lpFriendsAddressList);
    ui->horizontalStackedWidget2->setStyleSheet("border-radius:4px;");

    connect(m_lpMyAddressList, SIGNAL(updateWalletInfo(QList<QVariant>)), this, SLOT(slotupdateWalletInfo(QList<QVariant>)));
}

AddressUI::~AddressUI()
{
    if(NULL != m_lpMyAddressList)
    {
        delete m_lpMyAddressList;
        m_lpMyAddressList = NULL;
    }
    delete ui;
}

void AddressUI::slotupdateWalletInfo(const QList<QVariant> &walletsList)
{
    emit signalupdateWalletInfo(walletsList);
}

void AddressUI::StopMyAddressListThread()
{
    if(NULL != m_lpMyAddressList)
    {
        m_lpMyAddressList->StopAddressListThread();
    }
}

void AddressUI::ResumeMyAddressListThread()
{
    if(NULL != m_lpMyAddressList)
    {
        m_lpMyAddressList->ResumeMyAddressListThread();
    }
}
