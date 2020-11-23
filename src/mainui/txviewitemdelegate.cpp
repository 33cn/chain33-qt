#include "txviewitemdelegate.h"
#include "basefuntion.h"
#include "platformstyle.h"
#include <QCheckBox>
#include <QApplication>
#include <QMouseEvent>
#ifndef _MSC_VER
#include "receiverequestdialog.h"
#endif

#define COPYBTN_W   66
#define COPYBTN_H   25

TxViewItemDelegate::TxViewItemDelegate(TypeList typeList, const PlatformStyle *platformStyle)
    : QAbstractItemDelegate()
    , m_nTypeList(typeList)
    , m_platformStyle (platformStyle)
{
    m_RedColor = 0xec5151;
    m_GreyColor = 0x999999;

    if(CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE){
        m_YellowColor = 0x2c77ef;
        m_BlackColor = 0x1a1a38;
        m_WhiteColor = 0x3D3D3D;
    } else {
        m_YellowColor = 0xffba26;
        m_BlackColor = 0x353535;
        m_WhiteColor = 0xffffff;
    }
}

void TxViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    QRect mainRect = option.rect;

    int halfheight = (mainRect.height() - 2*MARGIN_x_y)/2;
    QRect amountRect(mainRect.left() + MARGIN_x_y-2, mainRect.top()+MARGIN_x_y, mainRect.width() - MARGIN_x_y-10, halfheight);
    QRect addressRect(mainRect.left() + MARGIN_x_y-2, mainRect.top()+MARGIN_x_y+halfheight+SPACE_ITEM, mainRect.width() - MARGIN_x_y, halfheight);

    QString strDecoration;
    if (m_nTypeList == TransactionList)
    {
        strDecoration = index.data(TyItem_TimeData).toString();
    }
    else if (TYPERECEIVEING == index.data(TypeRole).toString())
    {
        double dBalance = index.data(Item_Balance).toDouble();
        QString strBalance = QString::number(dBalance, 'f', 4);
        if(!strBalance.isEmpty())
        {
            strBalance = tr("可用") + CStyleConfig::GetInstance().GetUnitName() + ": " +strBalance;
        }

        strDecoration = strBalance;
    }

    if(!strDecoration.isEmpty())
    {
        QFont font;
#ifndef WIN32
        font.setPointSize(10);
#endif
        painter->setFont(font);
        painter->setPen(m_GreyColor);
        painter->drawText(amountRect, Qt::AlignRight|Qt::AlignVCenter, strDecoration);

#ifndef WIN32
        font.setPointSize(12);
#endif

        painter->setFont(font);
    }

    if (m_nTypeList == TransactionList)
    {
        QString label = index.data(TyItem_ToLabel).toString();
        QString address = index.data(TyItem_ToAddress).toString();
        if(SendToAddress == index.data(TyItem_TypeTy).toInt() || SendToMining == index.data(TyItem_TypeTy).toInt())
        {
            label = index.data(TyItem_FromLabel).toString();
            address = index.data(TyItem_FromAddress).toString();
        }

        painter->setPen(m_GreyColor);
        if(label.isEmpty())
        {
            painter->drawText(addressRect, Qt::AlignLeft|Qt::AlignVCenter, address);
        }
        else
        {
            painter->drawText(addressRect, Qt::AlignLeft|Qt::AlignVCenter, label + "--" + address);
        }

        if(Generated == index.data(TyItem_TypeTy).toInt() || RecvWithAddress == index.data(TyItem_TypeTy).toInt() || RecvFromMining == index.data(TyItem_TypeTy).toInt())
        {
            painter->setPen(m_WhiteColor);
        }
        else
        {
            painter->setPen(m_YellowColor);
        }

        if(TyFailure == index.data(TyItem_TypeTy).toInt())
        {
            painter->setPen(m_RedColor);
        }

        QString strAmount = index.data(TyItem_Amount).toString();
        painter->drawText(amountRect, Qt::AlignLeft|Qt::AlignVCenter, strAmount);

        QRect typeRect(mainRect.left()+200 + MARGIN_x_y, mainRect.top()+MARGIN_x_y, mainRect.width() - MARGIN_x_y, halfheight);

        if(TyFailure == index.data(TyItem_TypeTy).toInt())
        {
            painter->setPen(m_RedColor);
        }
        else
        {
            painter->setPen(m_YellowColor);
        }
        QString typeTy = index.data(TyItem_StrTypeTy).toString();
        painter->drawText(typeRect, Qt::AlignLeft|Qt::AlignVCenter, typeTy);
    }
    else
    {
        painter->setPen(m_GreyColor);
        QString address = index.data(Item_Address).toString();
        painter->drawText(addressRect, Qt::AlignLeft|Qt::AlignVCenter, address);

        painter->setPen(m_WhiteColor);
        QString labelText = index.data(Item_Label).toString();
        painter->drawText(amountRect, Qt::AlignLeft|Qt::AlignVCenter, labelText);

        QRect copyBtnRect(mainRect.right()-COPYBTN_W, mainRect.bottom()-COPYBTN_H, COPYBTN_W-5, COPYBTN_H);
        QStyleOptionButton copyBtn;
        copyBtn.rect = copyBtnRect;
        copyBtn.features = QStyleOptionButton::CommandLinkButton;
        copyBtn.state |= QStyle::State_MouseOver;
        QPalette palette;
        palette.setColor(QPalette::ButtonText, m_YellowColor);
        copyBtn.palette = palette;

#ifdef LINUX
        copyBtn.features = QStyleOptionButton::Flat;
#else
        if(m_platformStyle){
            copyBtn.icon = m_platformStyle->SingleColorIcon(":/icons/key");
        } else {
            copyBtn.icon = QIcon(":/icons/key");
        }
#endif
        copyBtn.iconSize = QSize(16, 16);

#ifndef _MSC_VER
        copyBtn.text = tr("详情");
#else
        copyBtn.text = tr("复制");
#endif
        //not show detail button for node award in my address
        if(!index.data(Item_Label).toString().startsWith("node award") && !index.data(Item_Label).toString().startsWith("airdropaddr"))
        {
            QApplication::style()->drawControl(QStyle::CE_PushButton, &copyBtn, painter);
        }
    }

    painter->setPen(m_BlackColor);
    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE)
        painter->setPen(0xF2F3F5);
    QLine l(mainRect.left() + MARGIN_x_y-2, mainRect.bottom()-1, mainRect.width() - MARGIN_x_y, mainRect.bottom()-1);
    painter->drawLine(l);

    // 增加选中框
    QStyleOptionViewItem itemOption(option);
    if (itemOption.state & QStyle::State_HasFocus)
    {
        painter->setPen(m_GreyColor);
        QRect focusRect(mainRect.left(), mainRect.top(), mainRect.width()-1, mainRect.height()-2);
        painter->drawRect(focusRect);
    }

    painter->restore();
}

bool TxViewItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        QRect mainRect = option.rect;
        QRect copyBtnRect(mainRect.right()-COPYBTN_W, mainRect.bottom()-COPYBTN_H, COPYBTN_W-5, COPYBTN_H);
        QMouseEvent *mouse_event = static_cast<QMouseEvent*>(event);
        //not deal with detail button for node award in my address
        if(mouse_event->button() == Qt::LeftButton && copyBtnRect.contains(mouse_event->pos()) && m_nTypeList == AddressList
                && !index.data(Item_Label).toString().startsWith("node award") && !index.data(Item_Label).toString().startsWith("airdropaddr"))
        {
            QString address = index.data(Item_Address).toString();
#ifndef _MSC_VER
            ReceiveRequestDialog dlg(NULL, address);
            dlg.exec();
#else
            GUIUtil::setClipboard(address);
#endif
        }
    }

    else if(event->type() == QEvent::KeyPress)
    {
        if(static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
           static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return QAbstractItemDelegate::editorEvent(event, model, option, index);
}

QSize TxViewItemDelegate::sizeHint(const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    return QSize(300, 51);
}

