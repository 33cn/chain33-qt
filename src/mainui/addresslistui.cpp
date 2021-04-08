#include "addresslistui.h"
#include "ui_addresslistui.h"
#include "editaddressdialog.h"
#include "addresstablemodel.h"
#include "csvmodelwriter.h"
#include "mainui.h"
#include "basefuntion.h"

#include <QStandardItemModel>

AddressListThread::AddressListThread()
{
    m_mutex.lock();
    m_stop = false;
    m_mutex.unlock();
}

AddressListThread::~AddressListThread()
{
    Stop();
}

void AddressListThread::run()
{
    while (true)
    {
        m_mutex.lock();
        if(m_stop) {
            m_mutex.unlock();
            return;
        }
        m_mutex.unlock();

        emit PostMsgGetAccounts();
        m_mutex.lock();
        m_cond.wait(&m_mutex, MAX_TIMEOUT_WAIT_RESPONSE_RESULT);
        m_mutex.unlock();
    }
}

void AddressListThread::Wakeup()
{
    if(!isRunning()) {
        return;
    }
    m_cond.wakeOne();
}

void AddressListThread::Stop()
{
    if(!isRunning()) {
        return;
    }
    m_mutex.lock();
    m_stop = true;
    m_mutex.unlock();

    m_cond.wakeOne();
    wait();
}

void AddressListThread::Resume()
{
    if(isRunning()) {
        return;
    }
    m_mutex.lock();
    m_stop = false;
    m_mutex.unlock();

    start();
}

AddressListUI::AddressListUI(AddrModeType mode, AddrTypeTabs tab, QWidget *parent, const PlatformStyle *platformStyle)
    : JsonConnectorDialog(parent)
    , ui(new Ui::AddressListUI)
    , mode(mode)
    , tab(tab)
    , txdelegate(new TxViewItemDelegate(AddressList, platformStyle))
    , m_dBalance (0)
    , m_lpFriendsAddrListXml (NULL)
    , m_lpAddressListThread (NULL)
    , m_bFirst (true)
{
    ui->setupUi(this);
    this->resize(600, 400);

    this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());
    if(CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE) {
        this->setStyleSheet("QWidget {background-color:#FFFFFF;border:none;}" + CStyleConfig::GetInstance().GetStylesheet());
    }

    ui->deleteButton->setVisible(true);
    switch(tab)
    {
    case TabsSending:
        setWindowTitle(tr("Friends address"));
        ui->addressTitleLabel->setText(tr("Friends address"));
        break;
    case TabsReceiving:
        setWindowTitle(tr("My address"));
     //   ui->deleteButton->setVisible(true);
        ui->addressTitleLabel->setText(tr("My address"));
        break;
    }

    switch(mode)
    {
    case ForSending:
        connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
     //   ui->horizontalWidget1->setVisible(false);
        break;
    case ForEditing:
        break;
    }

    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  //  connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged()));
    ui->tableView->setItemDelegate(txdelegate);
    ui->tableView->horizontalHeader()->setVisible(false);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setShowGrid(false);
    ui->tableView->setFrameShape(QFrame::NoFrame);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->verticalHeader()->setDefaultSectionSize(51);

 //   QListView::setAllColumnsShowFocus(true);

    QAction *copyLabelAction = new QAction(tr("Copy &Label"), this);
    QAction *copyAddressAction = new QAction(tr("&Copy Address"), this);
    QAction *editAction = new QAction(tr("&Edit"), this);
    if(tab == TabsReceiving)
        ui->deleteButton->setText(tr("隐藏"));
    deleteAction = new QAction(ui->deleteButton->text(), this);

    contextMenu = new QMenu();
    contextMenu->addAction(copyAddressAction);
    contextMenu->addAction(copyLabelAction);
    contextMenu->addAction(editAction);
    if (tab == TabsReceiving && CStyleConfig::GetInstance().GetCoinsType() == TOKEN_YCC) {
        QAction *editMiningAction = new QAction(tr("&Set Minging"), this);
        contextMenu->addAction(editMiningAction);
        connect(editMiningAction, SIGNAL(triggered()), this, SLOT(onSetMiningAction()));
    }
    contextMenu->addAction(deleteAction);
    contextMenu->setStyleSheet("QMenu {background-color:#2c2c2c;} QMenu::item:selected { background-color:#454545; }");
    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE)
        contextMenu->setStyleSheet("QMenu {background-color:#ffffff;color: #1B2740;} QMenu::item:selected {background-color:#5282DB;color: #ffffff;}");

    // Connect signals for context menu actions
    connect(copyAddressAction, SIGNAL(triggered()), this, SLOT(on_copyToClipboard_clicked()));
    connect(copyLabelAction, SIGNAL(triggered()), this, SLOT(onCopyLabelAction()));
    connect(editAction, SIGNAL(triggered()), this, SLOT(onEditAction()));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(on_deleteButton_clicked()));
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualMenu(QPoint)));

    model = new AddressTableModel(this);
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->tableView->setModel(proxyModel);
    ui->tableView->sortByColumn(0, Qt::AscendingOrder);
    switch(tab)
    {
    case TabsReceiving:
        proxyModel->setFilterRole(TypeRole);
        proxyModel->setFilterFixedString(TYPERECEIVEING);
        break;
    case TabsSending:
        proxyModel->setFilterRole(TypeRole);
        proxyModel->setFilterFixedString(TYPESENDING);
        break;
    }

    if (TabsReceiving == tab) {
        m_lpAddressListThread = new AddressListThread();
        connect(m_lpAddressListThread, SIGNAL(PostMsgGetAccounts()), this, SLOT(PostMsgGetAccounts()));
        m_lpAddressListThread->start();
    } else {
        m_lpFriendsAddrListXml = new FriendsAddrListXml();
        QList<FriendsAddrData> AddrDataList;
        m_lpFriendsAddrListXml->ReadXml(AddrDataList);
        for (int i=0; i<AddrDataList.size(); ++i)
        {
            model->updateEntry(AddressTableEntry(TabsSending, AddrDataList[i].strLable, AddrDataList[i].strAddr), CT_NEW);
            m_mapAddrLabel.insert(AddrDataList[i].strAddr, AddrDataList[i].strLable);
        }
    }
}

AddressListUI::~AddressListUI()
{
    if(NULL != m_lpAddressListThread)
    {
        delete m_lpAddressListThread;
        m_lpAddressListThread = NULL;
    }

    delete ui;
}

void AddressListUI::UpdateAddrList()
{
    model->RemoveALLEntry();
    m_mapAddrLabel.clear();
    QList<FriendsAddrData> AddrDataList;
 //   m_lpFriendsAddrListXml->GetFriendsAddrData(AddrDataList);
    m_lpFriendsAddrListXml->ReadXml(AddrDataList);
    for (int i=0; i<AddrDataList.size(); ++i)
    {
        model->updateEntry(AddressTableEntry(TabsSending, AddrDataList[i].strLable, AddrDataList[i].strAddr), CT_NEW);
        m_mapAddrLabel.insert(AddrDataList[i].strAddr, AddrDataList[i].strLable);
    }
}

void AddressListUI::PostMsgGetAccounts()
{
    PostJsonMessage(ID_GetAccounts);
}

void AddressListUI::on_outputButton_clicked()
{
    // CSV is currently the only supported format
    QString filename = GUIUtil::getSaveFileName( this, tr("Export Address Book Data"), QString(), tr("Comma separated file (*.csv)"));

    if (filename.isNull()) return;

    CSVModelWriter writer(filename);

    // name, column, role
    writer.setModel(model);
    writer.addColumn(tr("标签"), 1, Item_Label);
    writer.addColumn(tr("地址"), 1, Item_Address);
    if(TabsReceiving == tab) {
        writer.addColumn(tr("可用金额 (单位：%1)").arg(CStyleConfig::GetInstance().GetUnitName()), 1, Item_Balance);
        writer.addColumn(tr("冻结金额 (单位：%1)").arg(CStyleConfig::GetInstance().GetUnitName()), 1, Item_Frozen);
    }

    if(!writer.write()) {
        QMessageBox::critical(this, tr("Error exporting"), tr("Could not write to file %1.").arg(filename), QMessageBox::Abort, QMessageBox::Abort);
    }
}

void AddressListUI::on_addButton_clicked()
{
    if(!model)
        return;
    EditAddressDialog dlg( tab == TabsSending ? NewSendingAddress : NewReceivingAddress, this);
    dlg.setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());
    dlg.setModel(model);
    if(dlg.exec()) {
        newAddressToSelect = dlg.getAddress();
        if (TabsSending == tab) {
            m_lpFriendsAddrListXml->AddNewData(dlg.getlabel(), dlg.getAddress());
            m_mapAddrLabel.insert(dlg.getAddress(), dlg.getlabel());
        }
    }
}

void AddressListUI::on_deleteButton_clicked()
{
    QTableView *table = ui->tableView;
    if(!table->selectionModel())
        return;
    FriendsAddrData AddrData;
    QModelIndexList indexes = table->selectionModel()->selectedRows();
    if(!indexes.isEmpty())
    {
        foreach (QModelIndex index, indexes)
        {
            AddrData.strLable = table->model()->data(index, Item_Label).toString();
            AddrData.strAddr = table->model()->data(index, Item_Address).toString();
        }
    }

    if(TabsSending == tab)
    {
        if(m_lpFriendsAddrListXml)
        {
            m_lpFriendsAddrListXml->RemoveData(AddrData);
            model->updateEntry(AddressTableEntry(TabsSending, AddrData.strLable, AddrData.strAddr), CT_DELETED);
            m_mapAddrLabel.remove(AddrData.strAddr);
        }
    }
    else
    {
        std::stringstream ostr;
        ostr << "{\"addr\":\"" << AddrData.strAddr.toStdString().c_str() << "\",\"label\":\"Deleted" << AddrData.strLable.toStdString().c_str() << QDateTime::currentDateTime().toString().toStdString().c_str() << "\"}";
        PostJsonMessage(ID_SetLabl, ostr.str().c_str());

        model->updateEntry(AddressTableEntry(TabsReceiving, AddrData.strLable, AddrData.strAddr), CT_DELETED);
        m_mapAddrLabel.remove(AddrData.strAddr);
    }
}

void AddressListUI::on_copyToClipboard_clicked()
{
    //add prompt for node award in my address
    if(TabsReceiving == tab && ui->tableView && ui->tableView->selectionModel())
    {
        QModelIndexList selection = ui->tableView->selectionModel()->selectedRows(0);

        if(!selection.isEmpty() && selection.at(0).data(Item_Label).toString().startsWith("node award"))
        {
            QMessageBox::warning(this, tr("提示"), tr("空投地址不可用于转账、挖矿等操作!"));
            return;
        }
    }
    GUIUtil::copyEntryData(ui->tableView, 0, Item_Address);
}

void AddressListUI::onCopyLabelAction()
{
    GUIUtil::copyEntryData(ui->tableView, 0, Item_Label);
}

void AddressListUI::onEditAction()
{
    if(!ui->tableView->selectionModel())
        return;
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
    if(indexes.isEmpty())
        return;

    EditAddressDialog dlg( tab == TabsSending ? EditSendingAddress : EditReceivingAddress, this);
    dlg.setModel(model);
    QModelIndex origIndex = proxyModel->mapToSource(indexes.at(0));
    QString strLabel = origIndex.data(Item_Label).toString();
    QString strAddr = origIndex.data(Item_Address).toString();
    dlg.loadRow(origIndex.row(), strLabel, strAddr);
    if(dlg.exec()) {
        switch(tab)
        {
        case TabsReceiving:
            m_mapAddrLabel[strAddr] = dlg.getlabel();
            break;
        case TabsSending:
        {
            if(m_lpFriendsAddrListXml) {
                m_lpFriendsAddrListXml->UpDataXml(strLabel, strAddr, dlg.getlabel(), dlg.getAddress());
            }

            if(strAddr == dlg.getAddress()) {
                m_mapAddrLabel[strAddr] = dlg.getlabel();
            } else {
                m_mapAddrLabel.remove(strAddr);
                m_mapAddrLabel.insert(dlg.getAddress(), dlg.getlabel());
            }
        }
            break;
        }
    }
}

void AddressListUI::onSetMiningAction()
{
    if(!ui->tableView->selectionModel())
        return;
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
    if(indexes.isEmpty())
        return;

    QModelIndex origIndex = proxyModel->mapToSource(indexes.at(0));
    if (origIndex.data(Item_Label).toString() == "airdropaddr"){
        QMessageBox::warning(this, tr("提示"), tr("airdropaddr 地址不能设置为挖矿地址!"));
        return;
    }
    QJsonObject jsonParms;
    jsonParms.insert("addr", origIndex.data(Item_Address).toString());
    jsonParms.insert("label", "mining");
    QJsonArray params;
    params.insert(0, jsonParms);
    PostJsonMessage(ID_SetLabl, params);
}

void AddressListUI::contextualMenu(const QPoint &point)
{
    QModelIndex index = ui->tableView->indexAt(point);
    if(index.isValid()) {
        contextMenu->exec(QCursor::pos());
    }
}

void AddressListUI::done(int retval)
{
    QTableView *table = ui->tableView;
    if(!table->selectionModel() || !table->model() || mode == ForEditing)
        return;

    // Figure out which address was selected, and return it
    QModelIndexList indexes = table->selectionModel()->selectedRows(0);
    foreach (QModelIndex index, indexes)
    {
        m_strReturnLabel = table->model()->data(index, Item_Label).toString();
        m_strReturnAddr = table->model()->data(index, Item_Address).toString();
        m_dBalance = table->model()->data(index, Item_Balance).toDouble();
    }

    if(m_strReturnAddr.isEmpty()) {
        // If no address entry selected, return rejected
        retval = Rejected;
    }

    QDialog::done(retval);
}

bool caseInsensitiveLessThan(const QVariant &s1, const QVariant &s2)
{
    return (s1.toMap()["acc"].toMap())["label"].toString().toLower() > (s2.toMap()["acc"].toMap())["label"].toString().toLower();
}

void AddressListUI::requestFinished(const QVariant &result, const QString &error)
{
    if(ID_GetAccounts == m_nID) {
        ChangeType status;

        QMap<QString, QVariant> resultMap = result.toMap();
        QList<QVariant> walletsList = (resultMap["wallets"]).toList();

        for (int i=0; i<walletsList.size(); ++i) {
            QMap<QString, QVariant> addrMap = walletsList[i].toMap();
            if(m_mapAddrLabel.find((addrMap["acc"].toMap())["addr"].toString()) == m_mapAddrLabel.end()) {
                status = CT_NEW;
            } else {
                status = CT_UPDATED;
            }

            model->updateEntry(AddressTableEntry(TabsReceiving, addrMap["label"].toString(), (addrMap["acc"].toMap())["addr"].toString(), Getbalance(addrMap["acc"].toMap()["balance"]), Getbalance(addrMap["acc"].toMap()["frozen"])), status);
            if (addrMap["label"].toString().indexOf("Deleted") == -1)
                m_mapAddrLabel.insert((addrMap["acc"].toMap())["addr"].toString(), addrMap["label"].toString());
        }

        emit updateWalletInfo(walletsList);
        if(NULL!= m_lpAddressListThread) {
            m_lpAddressListThread->Wakeup();
        }
    } else if (ID_NewAccount == m_nID) {
        if(!error.isEmpty())
            QMessageBox::critical(this, tr("新建地址失败"), error, tr("ok"));
    } else if (ID_SetLabl == m_nID) {
        if(!error.isEmpty()){
            QMessageBox::critical(this, tr("错误"), tr("设置挖矿地址失败,%1").arg(error));
        } else {
            QMessageBox::information(this, tr("提示"), tr("设置挖矿地址成功"));
        }
    }
}

void AddressListUI::StopAddressListThread()
{
    if(TabsReceiving == tab && NULL != m_lpAddressListThread)
    {
        m_lpAddressListThread->Stop();
    }
}

void AddressListUI::ResumeMyAddressListThread()
{
    if(TabsReceiving == tab && NULL != m_lpAddressListThread)
    {
        m_lpAddressListThread->Resume();
    }
}
