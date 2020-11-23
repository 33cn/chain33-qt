#include "addresstablemodel.h"
#include "guiutil.h"
#include "base58.h"

#include <QFont>
#include <QColor>


struct AddressTableEntryLessThan
{
    bool operator()(const AddressTableEntry &a, const AddressTableEntry &b) const
    {
        return a.strAddress < b.strAddress;
    }
    bool operator()(const AddressTableEntry &a, const QString &b) const
    {
        return a.strAddress < b;
    }
    bool operator()(const QString &a, const AddressTableEntry &b) const
    {
        return a < b.strAddress;
    }
};

// Private implementation
class AddressTablePriv
{
public:
    QList<AddressTableEntry> cachedAddressTable;
    AddressTableModel *parent;

    AddressTablePriv(AddressTableModel *parent): parent(parent) {}

    void refreshAddressTable()
    {
        cachedAddressTable.clear();
        // qLowerBound() and qUpperBound() require our cachedAddressTable list to be sorted in asc order
        qSort(cachedAddressTable.begin(), cachedAddressTable.end(), AddressTableEntryLessThan());
    }

    void updateEntry(const AddressTableEntry &addressTableEntry, ChangeType status)
    {
        // Find address / label in model
        QList<AddressTableEntry>::iterator lower = qLowerBound( cachedAddressTable.begin(), cachedAddressTable.end(), addressTableEntry.strAddress, AddressTableEntryLessThan());
        QList<AddressTableEntry>::iterator upper = qUpperBound( cachedAddressTable.begin(), cachedAddressTable.end(), addressTableEntry.strAddress, AddressTableEntryLessThan());
        int lowerIndex = (lower - cachedAddressTable.begin());
        int upperIndex = (upper - cachedAddressTable.begin());
        bool inModel = (lower != upper);

        if(addressTableEntry.strLabel.indexOf("Deleted") >= 0)
        {
            if(inModel)
            {
                parent->beginRemoveRows(QModelIndex(), lowerIndex, upperIndex-1);
                cachedAddressTable.erase(lower, upper);
                parent->endRemoveRows();
            }
            return;
        }

        switch(status)
        {
        case CT_NEW:
            if(inModel)
            {
                //qInfo("Warning: AddressTablePriv::updateEntry: Got CT_NEW, but entry is already in model");
                break;
            }
            parent->beginInsertRows(QModelIndex(), lowerIndex, lowerIndex);
            cachedAddressTable.insert(lowerIndex, addressTableEntry);
            parent->endInsertRows();
            break;
        case CT_UPDATED:
            if(!inModel)
            {
                //qInfo("AddressTablePriv::updateEntry: Got CT_UPDATED, but entry is not in model");
                break;
            }
        //    lower->strAddress = addressTableEntry.strAddress;
            lower->typeTabs = addressTableEntry.typeTabs;
            lower->strLabel = addressTableEntry.strLabel;
            if(addressTableEntry.typeTabs == TabsReceiving)
            {
                lower->nCurrency = addressTableEntry.nCurrency;
                if(addressTableEntry.nBalance >= 0.0)
                {
                    lower->nBalance = addressTableEntry.nBalance;
                }

                if(addressTableEntry.nFrozen >= 0.0)
                {
                    lower->nFrozen = addressTableEntry.nFrozen;
                }
            }
            parent->emitDataChanged(lowerIndex);
            break;
        case CT_DELETED:
            if(!inModel)
            {
                //qInfo("AddressTablePriv::updateEntry: Got CT_DELETED, but entry is not in model\n");
                break;
            }
            parent->beginRemoveRows(QModelIndex(), lowerIndex, upperIndex-1);
            cachedAddressTable.erase(lower, upper);
            parent->endRemoveRows();
            break;
        }
    }

    void updateEntry(const AddressTableEntry &addressTableEntry, const QString &strChangeType)
    {
        // Find address / label in model
        QList<AddressTableEntry>::iterator lower = qLowerBound( cachedAddressTable.begin(), cachedAddressTable.end(), addressTableEntry.strAddress, AddressTableEntryLessThan());
        QList<AddressTableEntry>::iterator upper = qUpperBound( cachedAddressTable.begin(), cachedAddressTable.end(), addressTableEntry.strAddress, AddressTableEntryLessThan());
        int lowerIndex = (lower - cachedAddressTable.begin());
     //   int upperIndex = (upper - cachedAddressTable.begin());
        bool inModel = (lower != upper);

        if(!inModel)
        {
            //qInfo("AddressTablePriv::updateEntry: Got CT_UPDATED, but entry is not in model");
            return;
        }
        lower->strAddress = strChangeType;
        lower->typeTabs = addressTableEntry.typeTabs;
        lower->strLabel = addressTableEntry.strLabel;

        parent->emitDataChanged(lowerIndex);
    }

    int size()
    {
        return cachedAddressTable.size();
    }

    AddressTableEntry *index(int idx)
    {
        if(idx >= 0 && idx < cachedAddressTable.size())
        {
            return &cachedAddressTable[idx];
        }
        else
        {
            return 0;
        }
    }

    void RemoveALLEntry()
    {
        for(int i=cachedAddressTable.size()-1; i>=0; --i)
        {
            QList<AddressTableEntry>::iterator lower = qLowerBound( cachedAddressTable.begin(), cachedAddressTable.end(), cachedAddressTable[i].strAddress, AddressTableEntryLessThan());
            QList<AddressTableEntry>::iterator upper = qUpperBound( cachedAddressTable.begin(), cachedAddressTable.end(), cachedAddressTable[i].strAddress, AddressTableEntryLessThan());
            int lowerIndex = (lower - cachedAddressTable.begin());
            int upperIndex = (upper - cachedAddressTable.begin());
            bool inModel = (lower != upper);

            if(!inModel)
            {
                //qInfo("AddressTablePriv::updateEntry: Got CT_DELETED, but entry is not in model\n");
                continue;
            }

            parent->beginRemoveRows(QModelIndex(), lowerIndex, upperIndex-1);
            cachedAddressTable.erase(lower, upper);
            parent->endRemoveRows();
        }
    }
};

AddressTableModel::AddressTableModel(QWidget *parent) :
    QAbstractTableModel(parent), priv(NULL)
{
    columns << tr("Label") /*<< tr("Address")*/;
    priv = new AddressTablePriv(this);
    priv->refreshAddressTable();
    editStatus = EditStatus_OK;
}

AddressTableModel::~AddressTableModel()
{
    delete priv;
}

int AddressTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int AddressTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant AddressTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    AddressTableEntry *rec = static_cast<AddressTableEntry*>(index.internalPointer());

    if(role == Item_Label)
    {
        return rec->strLabel;
    }
    else if (role == Item_Address)
    {
        return rec->strAddress;
    }
    else if (role == Item_Balance)
    {
        return rec->nBalance;
    }
    else if (role == Item_Frozen)
    {
        return rec->nFrozen;
    }
    else if (role == Item_Currency)
    {
        return rec->nCurrency;
    }
    else if (role == TypeRole)
    {
        switch(rec->typeTabs)
        {
        case TabsSending:
            return TYPESENDING;
        case TabsReceiving:
            return TYPERECEIVEING;
        default: break;
        }
    }

    return QVariant();
}

bool AddressTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;

    AddressTableEntry *rec = static_cast<AddressTableEntry*>(index.internalPointer());

    editStatus = EditStatus_OK;
    if(role == Qt::EditRole && rec)
    {
        switch(m_nRole)
        {
        case 0:
            ++m_nRole;
            if(rec->strLabel == value.toString())
            {
                editStatus = EditStatus_NO_CHANGES;
                return false;
            }
            else if(rec->typeTabs == TabsReceiving)
            {
                // Post Message
            }
            else if(rec->typeTabs == TabsSending)
            {
                updateEntry(AddressTableEntry(rec->typeTabs, value.toString(), rec->strAddress, rec->nBalance, rec->nFrozen), CT_UPDATED);
            }
            break;
        case 1:
            if(rec->strAddress == value.toString())
            {
                editStatus = EditStatus_NO_CHANGES;
                return false;
            }
            else if(!IsAddrValid(value.toString().toStdString().c_str()))
            {
                editStatus = EditStatus_INVALID_ADDRESS;
                return false;
            }
            // Check for duplicate addresses to prevent accidental deletion of addresses, if you try
            // to paste an existing address over another address (with a different label) 检查重复
        //    else if(wallet->mapAddressBook.count(CBitcoinAddress(value.toString().toStdString()).Get()))
         //   {
          //      editStatus = EditStatus_DUPLICATE_ADDRESS;
          //      return false;
          //  }
            // Double-check that we're not overwriting a receiving address
            else if(rec->typeTabs == TabsReceiving)
            {
            }
            else if(rec->typeTabs == TabsSending)
            {
                priv->updateEntry(AddressTableEntry(rec->typeTabs, rec->strLabel, rec->strAddress), value.toString());
            //   updateEntry(AddressTableEntry(rec->typeTabs, rec->strLabel, value.toString()), CT_UPDATED);
            }
            break;
        }
        return true;
    }

    return false;
}

QVariant AddressTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
        {
            return columns[section];
        }
    }
    return QVariant();
}

Qt::ItemFlags AddressTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;
    AddressTableEntry *rec = static_cast<AddressTableEntry*>(index.internalPointer());

    Qt::ItemFlags retval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    // Can edit address and label for sending addresses,
    // and only label for receiving addresses.
    if(rec->typeTabs == TabsSending ||
      (rec->typeTabs == TabsReceiving && index.column()==0))
   //   (rec->typeTabs == TabsReceiving && index.column()==ColumnIndexLabel))
    {
        retval |= Qt::ItemIsEditable;
    }
    return retval;
}

QModelIndex AddressTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    AddressTableEntry *data = priv->index(row);
    if(data)
    {
        return createIndex(row, column, priv->index(row));
    }
    else
    {
        return QModelIndex();
    }
}

bool AddressTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    AddressTableEntry *rec = priv->index(row);
    if(count != 1 || !rec || rec->typeTabs == TabsReceiving)
    {
        return false;
    }

    updateEntry(AddressTableEntry(TabsSending, "", rec->strAddress), CT_DELETED);
    return true;
}

void AddressTableModel::updateEntry(const AddressTableEntry &addressTableEntry, ChangeType status)
{
    priv->updateEntry(addressTableEntry, status);
}

void AddressTableModel::RemoveALLEntry()
{
    priv->RemoveALLEntry();
}

void AddressTableModel::emitDataChanged(int idx)
{
    emit dataChanged(index(idx, 0, QModelIndex()), index(idx, columns.length()-1, QModelIndex()));
}
