#include "transactionslistui.h"
#include "ui_transactionslistui.h"
#include <QScrollBar>
#include "transactionfilterproxy.h"
#include "transactionrecord.h"
#include "menu/transactiondescdialog.h"
#include "editaddressdialog.h"
#include "csvmodelwriter.h"
#include "addresstablemodel.h"
#include "transactionslistmodel.h"
#include "addressui.h"
#include "mainui.h"
#include "addresslistui.h"
#include "cstyleconfig.h"

#define COUNT_NUM   15

void TransactionsThread::SetRunPostMsg(bool bRunPostMsg)
{
    m_mutex.lock();
    m_bRunPostMsg = bRunPostMsg;
    m_mutex.unlock();
}

void TransactionsThread::run()
{
    while (true)
    {
        m_mutex.lock();
        if(m_bRunPostMsg) {
            m_mutex.unlock();
            emit PostMsgWalletTxList();
        } else {
            m_mutex.unlock();
        }

        sleep(12);
    }
}

TransactionsListUI::TransactionsListUI(QWidget *parent)
    : JsonConnectorWidget(parent)
    , ui(new Ui::TransactionsListUI)
    , txdelegate(new TxViewItemDelegate())
    , transactionProxyModel(0)
    , m_lpTransactionsThread (NULL)
    , m_nPage (0)
    , m_ndirection (0)
    , m_nMaxHeight (0)
    , m_nMaxIndex (0)
    , m_bFirstPost (true)
    , m_bSyncFinish (true)
{
    ui->setupUi(this);

    this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());
    if(CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE) {
        this->setStyleSheet("QWidget {background-color:#FFFFFF;border:none;}" + CStyleConfig::GetInstance().GetStylesheet());
    }

    ui->horizontalWidget0->setStyleSheet("border-top-left-radius:4px;border-top-right-radius:4px;");
    ui->horizontalWidget3->setStyleSheet("border-bottom-left-radius:4px;border-bottom-right-radius:4px;");

    ui->labelTransactionsStatus->setText("(" + tr("out of sync") + ")");
    showOutOfSyncWarning(true);

    // 暂时都不显示
    ui->searchWidget->setVisible(false);
    ui->dataFilteringWidget->setVisible(false);
    ui->dateRangeWidget->setVisible(false);
 //   initUI();

    ui->listTransactions->setSelectionMode(QAbstractItemView::SingleSelection);
//    connect(ui->listTransactions->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged()));
    ui->listTransactions->setItemDelegate(txdelegate);
    ui->listTransactions->horizontalHeader()->setVisible(false);
    ui->listTransactions->verticalHeader()->setVisible(false);
    ui->listTransactions->setSelectionBehavior(QAbstractItemView::SelectRows);  // 选中整行
    ui->listTransactions->setShowGrid(false);
    ui->listTransactions->setFrameShape(QFrame::NoFrame);
    ui->listTransactions->horizontalHeader()->setStretchLastSection(true);
    ui->listTransactions->verticalHeader()->setDefaultSectionSize(51);
    ui->listTransactions->setEditTriggers(QAbstractItemView::SelectedClicked);

    model = new TransactionsListModel(this);
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->listTransactions->setModel(proxyModel);
    ui->listTransactions->sortByColumn(0, Qt::DescendingOrder);

    ui->prevPageBtn->setEnabled(false);

    m_lpTransactionsThread = new TransactionsThread();
    connect(m_lpTransactionsThread, SIGNAL(PostMsgWalletTxList()), this, SLOT(PostMsgWalletTxList()));
    PostMsgWalletTxList();
    m_lpTransactionsThread->start();
}

TransactionsListUI::~TransactionsListUI()
{
    if(m_lpTransactionsThread) {
        m_lpTransactionsThread->terminate();
        m_lpTransactionsThread->wait();
    }
    delete ui;
}

void TransactionsListUI::requestFinished(const QVariant &result, const QString &/*error*/)
{
    if(ID_WalletTxList == m_nID) {
        QMap<QString, QVariant> resultMap = result.toMap();
        QList<QVariant> txList = (resultMap["txDetails"]).toList();

        model->RemoveALLEntry();
        QString strFromFirst;
        QString strFromEnd;
        for (int i = 0; i<txList.size(); ++i) {
            QMap<QString, QVariant> txMap = txList[i].toMap();
            double dAmount = txMap["amount"].toDouble();
            QString strToAddr = txMap["tx"].toMap()["to"].toString();
            QString strFromAddr = txMap["fromAddr"].toString();
            QString strTxHash = txMap["txHash"].toString();
            uint nTime = txMap["blockTime"].toUInt();
            int nFee = txMap["tx"].toMap()["fee"].toUInt();
            QString strExecer = txMap["tx"].toMap()["execer"].toString();
            QString strActionname = txMap["actionName"].toString();
            int nTy = txMap["receipt"].toMap()["ty"].toInt();
            QString strNote = txMap["tx"].toMap()["payload"].toMap()["Value"].toMap()["Transfer"].toMap()["note"].toString();

            QString strError;
            if(nTy == 1) {
                QList<QVariant> MapError = txMap["receipt"].toMap()["logs"].toList();
                for(int j=0; j<MapError.size(); ++j) {
                    if(MapError[j].toMap()["ty"] == 1) {
                        strError = MapError[j].toMap()["log"].toString();
                        break;
                    }
                }
            }

            // YCC 专用 现在不用了 都写在 amount 数据中
            int nVoteCount = 0; // 参与投票的次数
           /* if (strActionname == "miner") {
                QList<QVariant> txLogsList = txMap["receipt"].toMap()["logs"].toList();
                for (int j = 0; j<txLogsList.size(); ++j) {
                    QMap<QString, QVariant> txLogMap = txLogsList[j].toMap();
                    if (txLogMap["ty"] == "335") {
                        QString strAddr = txLogMap["log"].toMap()["addr"].toString();
                        if(strAddr == strFromAddr){
                            ++nVoteCount;
                        }
                    }
                }
            }*/

            model->AdddateEntry(TransactionsListEntry(nTime, strToAddr, strFromAddr, strTxHash, dAmount, nFee, strExecer, strActionname, nTy, strNote, strError, nVoteCount));

            if(i == 0)
                strFromFirst = QString().sprintf("%013d", txMap["height"].toInt()) + QString().sprintf("%05d", txMap["index"].toInt());
            if(i == txList.size() -1)
                strFromEnd = QString().sprintf("%013d", txMap["height"].toInt()) + QString().sprintf("%05d", txMap["index"].toInt());       
        }

        if(m_ndirection == 0){
            m_strFromTxPrev = strFromFirst;
            m_strFromTxNext = strFromEnd;
        } else {
            m_strFromTxNext = strFromFirst;
            m_strFromTxPrev = strFromEnd;
        }

        if(txList.size() < COUNT_NUM && m_ndirection == 0) {
            ui->nextPageBtn->setEnabled(false);
        } else {
            ui->nextPageBtn->setEnabled(true);
        }
    }
}

void TransactionsListUI::PostMsgWalletTxList()
{
    QJsonObject jsonParams;
    jsonParams.insert("fromTx", m_strFromTx);
    jsonParams.insert("count", COUNT_NUM);
    jsonParams.insert("direction", m_ndirection);

    QJsonArray params;
    params.insert(0, jsonParams);
    PostJsonMessage(ID_WalletTxList, params);
}

void TransactionsListUI::on_firstPageBtn_clicked()
{
    m_nPage = 0;
    m_ndirection = 0;
    m_strFromTx = "";
    PostMsgWalletTxList();
    m_lpTransactionsThread->SetRunPostMsg(true);

    ui->prevPageBtn->setEnabled(false);
}

void TransactionsListUI::on_prevPageBtn_clicked()
{
    --m_nPage;
    if(m_nPage == 0) {
        on_firstPageBtn_clicked();
    } else {
        m_ndirection = 1;
        m_strFromTx = m_strFromTxPrev;
        PostMsgWalletTxList();
    }
}

void TransactionsListUI::on_nextPageBtn_clicked()
{
    ++m_nPage;
    m_lpTransactionsThread->SetRunPostMsg(false);

    m_ndirection = 0;
    m_strFromTx = m_strFromTxNext;
    PostMsgWalletTxList();

    ui->prevPageBtn->setEnabled(true);
}

void TransactionsListUI::SendAddrSucceedUpData()
{
    if(0 == m_nPage)
        on_firstPageBtn_clicked();
}

void TransactionsListUI::on_listTransactions_doubleClicked(const QModelIndex &index)
{
    TransactionDescDialog dlg(index, this);
    dlg.exec();
}

void TransactionsListUI::showOutOfSyncWarning(bool fShow)
{
    ui->labelTransactionsStatus->setVisible(fShow);
    if(!fShow && m_bSyncFinish) {
        m_bSyncFinish = false;
        on_firstPageBtn_clicked();
    }
}

void TransactionsListUI::initUI()
{
    ui->searchWidget->setStyleSheet("QWidget {background-color:#2c2c2c; border-radius: 9px;}");

    ui->dateWidget->addItem(tr("All"), All);
    ui->dateWidget->addItem(tr("Today"), Today);
    ui->dateWidget->addItem(tr("This week"), ThisWeek);
    ui->dateWidget->addItem(tr("This month"), ThisMonth);
    ui->dateWidget->addItem(tr("Last month"), LastMonth);
    ui->dateWidget->addItem(tr("This year"), ThisYear);
    ui->dateWidget->addItem(tr("Range..."), Range);

    ui->typeWidget->addItem(tr("All"), TransactionFilterProxy::ALL_TYPES);
    ui->typeWidget->addItem(tr("Received with"), TransactionFilterProxy::TYPE(TransactionRecord::RecvWithAddress) | TransactionFilterProxy::TYPE(TransactionRecord::RecvFromOther));
    ui->typeWidget->addItem(tr("Sent to"), TransactionFilterProxy::TYPE(TransactionRecord::SendToAddress) | TransactionFilterProxy::TYPE(TransactionRecord::SendToOther));
    ui->typeWidget->addItem(tr("To yourself"), TransactionFilterProxy::TYPE(TransactionRecord::SendToSelf));
    ui->typeWidget->addItem(tr("Mined"), TransactionFilterProxy::TYPE(TransactionRecord::Generated));
    ui->typeWidget->addItem(tr("Other"), TransactionFilterProxy::TYPE(TransactionRecord::Other));
    ui->typeWidget->addItem(tr("Back Notify"), TransactionFilterProxy::TYPE(TransactionRecord::BackDecl));

#if QT_VERSION >= 0x040700
    /* Do not move this to the XML file, Qt before 4.7 will choke on it */
    ui->searchEdit->setPlaceholderText(tr("Enter address or label to search"));
    ui->minAmountEdit->setPlaceholderText(tr("Min amount"));
#endif
    createDateRangeWidget();

    // Actions
    QAction *copyAddressAction = new QAction(tr("Copy address"), this);
    QAction *copyLabelAction = new QAction(tr("Copy label"), this);
    QAction *copyAmountAction = new QAction(tr("Copy amount"), this);
    QAction *copyTxIDAction = new QAction(tr("Copy transaction ID"), this);
    QAction *editLabelAction = new QAction(tr("Edit label"), this);

    contextMenu = new QMenu();
    contextMenu->addAction(copyAddressAction);
    contextMenu->addAction(copyLabelAction);
    contextMenu->addAction(copyAmountAction);
    contextMenu->addAction(copyTxIDAction);
    contextMenu->addAction(editLabelAction);

    connect(ui->dateWidget, SIGNAL(activated(int)), this, SLOT(chooseDate(int)));
    connect(ui->typeWidget, SIGNAL(activated(int)), this, SLOT(chooseType(int)));
    connect(ui->searchEdit, SIGNAL(textChanged(QString)), this, SLOT(changedPrefix(QString)));
    connect(ui->minAmountEdit, SIGNAL(textChanged(QString)), this, SLOT(changedAmount(QString)));

    connect(ui->listTransactions, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualMenu(QPoint)));

    connect(copyAddressAction, SIGNAL(triggered()), this, SLOT(copyAddress()));
    connect(copyLabelAction, SIGNAL(triggered()), this, SLOT(copyLabel()));
    connect(copyAmountAction, SIGNAL(triggered()), this, SLOT(copyAmount()));
    connect(copyTxIDAction, SIGNAL(triggered()), this, SLOT(copyTxID()));
}

void TransactionsListUI::createDateRangeWidget()
{
    ui->dateFrom->setDisplayFormat("dd/MM/yy");
    ui->dateFrom->setCalendarPopup(true);
    ui->dateFrom->setMinimumWidth(100);
    ui->dateFrom->setDate(QDate::currentDate().addDays(-7));

    ui->dateTo->setDisplayFormat("dd/MM/yy");
    ui->dateTo->setCalendarPopup(true);
    ui->dateTo->setMinimumWidth(100);
    ui->dateTo->setDate(QDate::currentDate());

    ui->dateRangeWidget->setVisible(false);

    connect(ui->dateFrom, SIGNAL(dateChanged(QDate)), this, SLOT(dateRangeChanged()));
    connect(ui->dateTo, SIGNAL(dateChanged(QDate)), this, SLOT(dateRangeChanged()));
}

void TransactionsListUI::chooseDate(int idx)
{
    if(!transactionProxyModel)
        return;
    QDate current = QDate::currentDate();
    ui->dateRangeWidget->setVisible(false);
    switch(ui->dateWidget->itemData(idx).toInt())
    {
    case All:
        transactionProxyModel->setDateRange( TransactionFilterProxy::MIN_DATE, TransactionFilterProxy::MAX_DATE);
        break;
    case Today:
        transactionProxyModel->setDateRange( QDateTime(current), TransactionFilterProxy::MAX_DATE);
        break;
    case ThisWeek: {
        // Find last Monday
        QDate startOfWeek = current.addDays(-(current.dayOfWeek()-1));
        transactionProxyModel->setDateRange( QDateTime(startOfWeek), TransactionFilterProxy::MAX_DATE);
        } break;
    case ThisMonth:
        transactionProxyModel->setDateRange( QDateTime(QDate(current.year(), current.month(), 1)), TransactionFilterProxy::MAX_DATE);
        break;
    case LastMonth:
        transactionProxyModel->setDateRange( QDateTime(QDate(current.year(), current.month()-1, 1)), QDateTime(QDate(current.year(), current.month(), 1)));
        break;
    case ThisYear:
        transactionProxyModel->setDateRange( QDateTime(QDate(current.year(), 1, 1)), TransactionFilterProxy::MAX_DATE);
        break;
    case Range:
        ui->dateRangeWidget->setVisible(true);
        dateRangeChanged();
        break;
    }
}

void TransactionsListUI::chooseType(int idx)
{
    if(!transactionProxyModel)
        return;
    transactionProxyModel->setTypeFilter(ui->typeWidget->itemData(idx).toInt());
}

void TransactionsListUI::changedPrefix(const QString &prefix)
{
    if(!transactionProxyModel)
        return;
    transactionProxyModel->setAddressPrefix(prefix);
}

void TransactionsListUI::changedAmount(const QString &/*amount*/)
{
    if(!transactionProxyModel)
        return;
}

void TransactionsListUI::focusTransaction(const QModelIndex &idx)
{
    if(!transactionProxyModel)
        return;
    QModelIndex targetIdx = transactionProxyModel->mapFromSource(idx);
    ui->listTransactions->scrollTo(targetIdx);
    ui->listTransactions->setCurrentIndex(targetIdx);
    ui->listTransactions->setFocus();
}

void TransactionsListUI::contextualMenu(const QPoint &point)
{
    QModelIndex index = ui->listTransactions->indexAt(point);
    if(index.isValid())
    {
        contextMenu->exec(QCursor::pos());
    }
}

void TransactionsListUI::dateRangeChanged()
{
    if(!transactionProxyModel)
        return;
    transactionProxyModel->setDateRange( QDateTime(ui->dateFrom->date()), QDateTime(ui->dateTo->date()).addDays(1));
}

void TransactionsListUI::copyAddress()
{
    GUIUtil::copyEntryData(ui->listTransactions, 0, TyItem_FromAddress);
}

void TransactionsListUI::copyLabel()
{
    GUIUtil::copyEntryData(ui->listTransactions, 0, TyItem_FromLabel);
}

void TransactionsListUI::copyAmount()
{
    GUIUtil::copyEntryData(ui->listTransactions, 0, TyItem_Amount);
}

void TransactionsListUI::copyTxID()
{
    GUIUtil::copyEntryData(ui->listTransactions, 0, TyItem_Hash);
}

void TransactionsListUI::on_pushButton_clicked()
{
    // CSV is currently the only supported format
    QString filename = GUIUtil::getSaveFileName( this, tr("Export Transaction Data"), QString(), tr("Comma separated file (*.csv)"));

    if (filename.isNull()) return;

    CSVModelWriter writer(filename);
    // name, column, role
    writer.setModel(model);
    writer.addColumn(tr("Date"), 1, TyItem_TimeData);
    writer.addColumn(tr("Type"), TyItem_StrTypeTy);
    writer.addColumn(tr("Label"), 1, TyItem_FromLabel);
    writer.addColumn(tr("Address"), 1, TyItem_FromAddress);
    writer.addColumn(tr("Label"), 1, TyItem_ToLabel);
    writer.addColumn(tr("Address"), 1, TyItem_ToAddress);
    writer.addColumn(tr("Amount"), 1, TyItem_Amount);
    writer.addColumn(tr("ID"), 1, TyItem_Hash);

    if(!writer.write()) {
        QMessageBox::critical(this, tr("Error exporting"), tr("Could not write to file %1.").arg(filename), QMessageBox::Abort, QMessageBox::Abort);
    }
}

