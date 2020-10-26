#ifndef TRANSACTIONSLISTMODEL_H
#define TRANSACTIONSLISTMODEL_H

/*
 * 交易列表 model
 */

#include <QAbstractTableModel>
#include <QStringList>
#include <QString>
#include <QDateTime>
#include "enumtype.h"

class TransactionsListEntry
{
public:
    TransactionsListEntry() {}
    TransactionsListEntry(const uint &nTimeData, const QString &strToAddress, const QString &strFromAddress, const QString &strHash, const double &dAmount, const double &nFee, const QString &strExecer, const QString &strActionname, int ReceiptTy, QString strNote, QString strError, int nVoteCount);
    TransactionsListEntry(const uint &nTimeData, const QString &strToAddress, const QString &strFromAddress, const QString &strHash, const double &dAmount, const double &nFee, const TransactionsType &typeTy, const QString &strTypeTy);

public:
    QString             m_strTimeData;
    QString             m_strToLabel;
    QString             m_strToAddress;
    QString             m_strFromLabel;
    QString             m_strFromAddress;
    QString             m_strHash;
    QString             m_strAmount;
    TransactionsType    m_typeTy;
    QString             m_strTypeTy;
    QString             m_strFee;
    uint                m_nTime;
    QString             m_strExecer;
    QString             m_strActionname;
    QString             m_strNote;
    QString             m_strError;

    // YCC 专用
    int                 m_nVoteCount; // 参与投票的次数
};

class TransactionsListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TransactionsListModel(QWidget *parent = 0);
    ~TransactionsListModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant data(const QModelIndex &index, int role) const;
    QString formatTxType(const TransactionsListEntry *wtx) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void reset();

    void AdddateEntry(const TransactionsListEntry &Entry);
    void RemoveALLEntry();

private:
    QList<TransactionsListEntry> cachedTransactionsList;

signals:
    void defaultAddressChanged(const QString &address);

public slots:
    friend class TransactionsListPriv;

};

#endif // TRANSACTIONSLISTMODEL_H
