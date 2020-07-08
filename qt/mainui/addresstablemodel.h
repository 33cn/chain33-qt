#ifndef ADDRESSTABLEMODEL_H
#define ADDRESSTABLEMODEL_H

/*
 * 添加地址 model
 */

#include <QAbstractTableModel>
#include <QStringList>
#include "enumtype.h"

struct AddressTableEntry
{
    AddrTypeTabs    typeTabs;
    QString         strLabel;
    QString         strAddress;
    uint32_t        nCurrency;
    double        nBalance;
    double        nFrozen;

    AddressTableEntry() {}
    AddressTableEntry(AddrTypeTabs type, const QString &label, const QString &address):
        typeTabs(type), strLabel(label), strAddress(address), nCurrency(0), nBalance(0.0), nFrozen(0.0) {}
    AddressTableEntry(AddrTypeTabs type, const QString &label, const QString &address, const double &balance, const double &frozen):
        typeTabs(type), strLabel(label), strAddress(address), nCurrency(0), nBalance(balance), nFrozen(frozen) {}
    AddressTableEntry(AddrTypeTabs type, const QString &label, const QString &address, const double &balance, const double &frozen, const uint32_t &currency):
        typeTabs(type), strLabel(label), strAddress(address), nCurrency(currency), nBalance(balance), nFrozen(frozen) {}
};

class AddressTablePriv;
class AddressTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AddressTableModel(QWidget *parent = 0);
    ~AddressTableModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void updateEntry(const AddressTableEntry &addressTableEntry, ChangeType status);

    void SetRole(int nRole){m_nRole = nRole;}
    EditStatus getEditStatus(){return editStatus;}

    void RemoveALLEntry();
private:
    AddressTablePriv    *priv;
    QStringList         columns;

signals:
    void defaultAddressChanged(const QString &address);

public slots:
    friend class AddressTablePriv;

private:
    void emitDataChanged(int index);

private:
    int m_nRole; // 0 is label    1 is addr
    EditStatus editStatus;
};

#endif // ADDRESSTABLEMODEL_H
