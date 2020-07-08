#ifndef TXVIEWITEMDELEGATE_H
#define TXVIEWITEMDELEGATE_H

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QDateTime>
#include <qglobal.h>
#include <guiutil.h>
#include <QStyledItemDelegate>
//#include "bitcoinunits.h"
#include "guiconstants.h"
#include "enumtype.h"
#include "mainui.h"

/*
 * 交易列表 list 和地址list 自定义显示
 */

enum TypeList {
    TransactionList = 0,
    AddressList = 1
};

#define SPACE_ITEM 5
#define MARGIN_x_y 10

class PlatformStyle;

class TxViewItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    TxViewItemDelegate(TypeList typeList = TransactionList, const PlatformStyle *platformStyle = NULL);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

    // 响应鼠标事件，更新数据
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
  //  int     unit;
    int     m_nTypeList;
    QColor  m_WhiteColor;
    QColor  m_GreyColor;
    QColor  m_BlackColor;
    QColor  m_YellowColor;
    QColor  m_RedColor;
    const PlatformStyle *m_platformStyle;
};


#endif // TXVIEWITEMDELEGATE_H
