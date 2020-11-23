#include <QFont>
#include <QColor>
#include "transactionslistmodel.h"
#include "guiutil.h"
#include "addressui.h"
#include "mainui.h"
#include "addresslistui.h"
#include "basefuntion.h"

extern MainUI*      g_lpMainUI;

TransactionsListEntry::TransactionsListEntry(const uint &nTimeData, const QString &strToAddress, const QString &strFromAddress, const QString &strHash, const double &dAmount, const double &nFee, const QString &strExecer, const QString &strActionname, int ReceiptTy, QString strNote, QString strError, int nVoteCount)
    : m_strToAddress(strToAddress)
    , m_strFromAddress(strFromAddress)
    , m_strHash(strHash)
    , m_strExecer(strExecer)
    , m_strActionname(strActionname)
    , m_strNote(strNote)
    , m_strError(strError)
    , m_nVoteCount(nVoteCount)
{
    m_nTime = nTimeData;
    QDateTime dateTime = QDateTime::fromTime_t(nTimeData);
    m_strTimeData = dateTime.toString("yyyy-MM-dd hh:mm:ss");

    QMap<QString, QString> mapMyAddr;
    if (g_lpMainUI && g_lpMainUI->m_lpAddressUI && g_lpMainUI->m_lpAddressUI->m_lpMyAddressList)
        mapMyAddr = g_lpMainUI->m_lpAddressUI->m_lpMyAddressList->m_mapAddrLabel;

    m_strFromLabel = mapMyAddr[strFromAddress];
    m_strToLabel = mapMyAddr[strToAddress];

    int npos = strExecer.toStdString().find("user.");
    if(ReceiptTy != 2 && strExecer != "none" && npos != 0) {
        m_typeTy = TyFailure;
    } else if ("ticket" == strExecer && "withdraw" == strActionname) {
        m_typeTy = RecvFromMining;
    } else if ("ticket" == strExecer && "miner" == strActionname) {
        m_typeTy = Generated;
    } else if ("ticket" == strExecer && "open" == strActionname) {
        m_typeTy = OpenTicket;
    } else if ("ticket" == strExecer && "close" == strActionname) {
        m_typeTy = CloseTicket;
    } else if ("coins" == strExecer && "transfer" == strActionname && "16htvcBNSEA7fZhAdLJphDwQRQJaHpyHTp" == strToAddress) {
        m_typeTy = SendToMining;
    } else if ("pos33" == strExecer && "withdraw" == strActionname && CStyleConfig::GetInstance().GetCoinsType() == TOKEN_YCC) {
        m_typeTy = RecvFromMining;
    } else if ("pos33" == strExecer && "miner" == strActionname && CStyleConfig::GetInstance().GetCoinsType() == TOKEN_YCC) {
        m_typeTy = Generated;
    } else if ("pos33" == strExecer && "topen" == strActionname && CStyleConfig::GetInstance().GetCoinsType() == TOKEN_YCC) {
        m_typeTy = OpenTicket;
    } else if ("pos33" == strExecer && "tclose" == strActionname && CStyleConfig::GetInstance().GetCoinsType() == TOKEN_YCC) {
        m_typeTy = CloseTicket;
    } else if ("coins" == strExecer && "transfer" == strActionname && "1Wj2mPoBwJMVwAQLKPNDseGpDNibDt9Vq" == strToAddress && CStyleConfig::GetInstance().GetCoinsType() == TOKEN_YCC) {
        m_typeTy = SendToMining;
    } else if ("coins" == strExecer && "withdraw" == strActionname && "1Wj2mPoBwJMVwAQLKPNDseGpDNibDt9Vq" == strFromAddress && CStyleConfig::GetInstance().GetCoinsType() == TOKEN_YCC) {
        m_typeTy = RecvFromMining;
    } else if(!m_strFromLabel.isEmpty() && !m_strToLabel.isEmpty()) {
        m_typeTy = SendToSelf;
    } else if (!m_strFromLabel.isEmpty()) {
        m_typeTy = SendToAddress;
    } else if (!m_strToLabel.isEmpty()) {
        m_typeTy = RecvWithAddress;
    } else {
        m_typeTy = Other;
    }

    double amount = GetbalanceD(dAmount);
    m_strAmount = QString::number(amount, 'f', 4);
    m_strAmount = m_strAmount + " " + CStyleConfig::GetInstance().GetUnitName();

/*    // YCC 专用 有投票奖励 去掉不用了
    if(CStyleConfig::GetInstance().GetCoinsType() == TOKEN_YCC && nVoteCount > 0){
        double amountVote = nVoteCount*0.35;
        QString strAmountVote = QString::number(amountVote, 'f', 2);
        QString strAmount = QString::number(amount, 'f', 2);
        double amountAll = amountVote + amount;
        m_strAmount = QString::number(amountAll, 'f', 4);
        m_strAmount = m_strAmount + " " + CStyleConfig::GetInstance().GetUnitName();
        m_strAmount = m_strAmount + " ( " + strAmount + " + " + strAmountVote + " )";
    }*/

    if(Generated == m_typeTy || RecvWithAddress == m_typeTy || RecvFromMining == m_typeTy) {
        m_strAmount = "+ " + m_strAmount;
    } else {
        m_strAmount = "- " + m_strAmount;
    }

    double Fee = GetbalanceD(nFee);
    m_strFee = QString::number(Fee, 'f', 4);
    m_strFee = "- " + m_strFee + " " + CStyleConfig::GetInstance().GetUnitName();
}

TransactionsListEntry::TransactionsListEntry(const uint &nTimeData, const QString &strToAddress, const QString &strFromAddress, const QString &strHash, const double &dAmount, const double &nFee, const TransactionsType &typeTy, const QString &strTypeTy)
    : m_strToAddress(strToAddress), m_strFromAddress(strFromAddress), m_strHash(strHash), m_typeTy(typeTy), m_strTypeTy(strTypeTy)
{
    m_nTime = nTimeData;
    QDateTime dateTime = QDateTime::fromTime_t(nTimeData);
    m_strTimeData = dateTime.toString("yyyy-MM-dd hh:mm:ss");

    QMap<QString, QString> mapMyAddr = g_lpMainUI->m_lpAddressUI->m_lpMyAddressList->m_mapAddrLabel;
    m_strFromLabel = mapMyAddr[strFromAddress];
    m_strToLabel = mapMyAddr[strToAddress];

    double amount = GetbalanceD(dAmount);
    m_strAmount = QString::number(amount, 'f', 4);
    m_strAmount = m_strAmount + " " + CStyleConfig::GetInstance().GetUnitName();
    if(SendToAddress == m_typeTy || SendToMining == m_typeTy || TyFailure == m_typeTy) {
        m_strAmount = "- " + m_strAmount;
    } else {
        m_strAmount = "+ " + m_strAmount;
    }

    double Fee = GetbalanceD(nFee);
    m_strFee = QString::number(Fee, 'f', 4);
    m_strFee = "- " + m_strFee + " " + CStyleConfig::GetInstance().GetUnitName();
}

struct TransactionsListEntryLessThan
{
    bool operator()(const TransactionsListEntry &a, const TransactionsListEntry &b) const {
        return a.m_strHash < b.m_strHash;
    }
    bool operator()(const TransactionsListEntry &a, const QString &b) const {
        return a.m_strHash < b;
    }
    bool operator()(const QString &a, const TransactionsListEntry &b) const {
        return a < b.m_strHash;
    }
};

TransactionsListModel::TransactionsListModel(QWidget *parent)
    : QAbstractTableModel(parent)
{
    cachedTransactionsList.clear();
    qSort(cachedTransactionsList.begin(), cachedTransactionsList.end(), TransactionsListEntryLessThan());
}

TransactionsListModel::~TransactionsListModel()
{
}

int TransactionsListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return cachedTransactionsList.size();
}

int TransactionsListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

bool TransactionsListModel::setData(const QModelIndex &/*index*/, const QVariant &/*value*/, int /*role*/)
{
    beginResetModel();//在开始添加此函数
    endResetModel();  //在结束前添加此函数
    return true;
}

QVariant TransactionsListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    TransactionsListEntry *rec = static_cast<TransactionsListEntry*>(index.internalPointer());

    if(role == TyItem_FromLabel)
    {
        return rec->m_strFromLabel;
    }
    else if (role == TyItem_FromAddress)
    {
        return rec->m_strFromAddress;
    }
    else if(role == TyItem_ToLabel)
    {
        return rec->m_strToLabel;
    }
    else if (role == TyItem_ToAddress)
    {
        return rec->m_strToAddress;
    }
    else if (role == TyItem_Amount)
    {
        return rec->m_strAmount;
    }
    else if (role == TyItem_TypeTy)
    {
        return rec->m_typeTy;
    }
    else if (role == TyItem_StrTypeTy)
    {
        return formatTxType(rec);
        //return rec->strTypeTy;
    }
    else if (role == TyItem_TimeData)
    {
        return rec->m_strTimeData;
    }
    else if (role == TyItem_Hash)
    {
        return rec->m_strHash;
    }
    else if (role == TyItem_Fee)
    {
        return rec->m_strFee;
    }
    else if (role == Qt::DisplayRole)
    {
        return rec->m_nTime;
    }
    else if (role == TyItem_Execer)
    {
        return rec->m_strExecer;
    }
    else if (role == TyItem_Actionname)
    {
        return rec->m_strActionname;
    }
    else if (role == TyItem_Note)
    {
        return rec->m_strNote;
    }
    else if (role == TyItem_Error)
    {
        return rec->m_strError;
    }

    return QVariant();
}

QString TransactionsListModel::formatTxType(const TransactionsListEntry *wtx) const
{
    switch(wtx->m_typeTy){
    case TyFailure:
        return tr("失败");
        break;
    case Other:
        return tr("其他");
        break;
    case Generated:
        return tr("挖矿所得");
        break;
    case OpenTicket:
        return tr("购票成功");
        break;
    case CloseTicket:
        return tr("关闭选票");
        break;
    case SendToAddress:
        return tr("发送");
        break;
    case SendToMining:
        return tr("冻结挖矿");
        break;
    case RecvWithAddress:
        return tr("接收");
        break;
    case RecvFromMining:
        return tr("挖矿取回");
        break;
    case SendToSelf:
        return tr("到自己");
        break;
    case BackDecl:
        return tr("钱包找回通知");
        break;
    default:
        return tr("其他");
        break;
    }
}

Qt::ItemFlags TransactionsListModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return QAbstractItemModel::flags(index);

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void TransactionsListModel::reset()
{
    beginResetModel();
    endResetModel();
}

void TransactionsListModel::AdddateEntry(const TransactionsListEntry &Entry)
{
    QList<TransactionsListEntry>::iterator lower = qLowerBound( cachedTransactionsList.begin(), cachedTransactionsList.end(), Entry.m_strHash, TransactionsListEntryLessThan());
    QList<TransactionsListEntry>::iterator upper = qUpperBound( cachedTransactionsList.begin(), cachedTransactionsList.end(), Entry.m_strHash, TransactionsListEntryLessThan());
    int lowerIndex = (lower - cachedTransactionsList.begin());
//    int upperIndex = (upper - cachedTransactionsList.begin());
    bool inModel = (lower != upper);

    if(inModel)
    {
        return;
    }

    beginInsertRows(QModelIndex(), lowerIndex, lowerIndex);
    cachedTransactionsList.insert(lowerIndex, Entry);
    endInsertRows();
}

void TransactionsListModel::RemoveALLEntry()
{
    for(int i=cachedTransactionsList.size()-1; i>=0; --i)
    {
        QList<TransactionsListEntry>::iterator lower = qLowerBound( cachedTransactionsList.begin(), cachedTransactionsList.end(), cachedTransactionsList[i].m_strHash, TransactionsListEntryLessThan());
        QList<TransactionsListEntry>::iterator upper = qUpperBound( cachedTransactionsList.begin(), cachedTransactionsList.end(), cachedTransactionsList[i].m_strHash, TransactionsListEntryLessThan());
        int lowerIndex = (lower - cachedTransactionsList.begin());
        int upperIndex = (upper - cachedTransactionsList.begin());

        beginRemoveRows(QModelIndex(), lowerIndex, upperIndex-1);
        cachedTransactionsList.erase(lower, upper);
        endRemoveRows();
    }
}

QModelIndex TransactionsListModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    TransactionsListEntry *data = NULL;
    if(row >= 0 && row < cachedTransactionsList.size())
    {
        data = (TransactionsListEntry *)&(cachedTransactionsList[row]);
    }

    if(data)
    {
        return createIndex(row, column, data);
    }
    else
    {
        return QModelIndex();
    }
}
