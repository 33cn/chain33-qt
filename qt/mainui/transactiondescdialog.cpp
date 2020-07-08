#include "transactiondescdialog.h"
#include "ui_transactiondescdialog.h"
#include "enumtype.h"
#include <QModelIndex>
#include "addressui.h"
#include "mainui.h"
#include "addresslistui.h"
#include "basefuntion.h"

TransactionDescDialog::TransactionDescDialog(const QModelIndex &idx, QWidget *parent)
    : JsonConnectorDialog(parent)
    , ui(new Ui::TransactionDescDialog)
{
    ui->setupUi(this);
    QString desc = getHtmlText(idx);
    ui->detailText->setHtml(desc);
    ui->detailText->setContextMenuPolicy(Qt::NoContextMenu);
}

TransactionDescDialog::~TransactionDescDialog()
{
    delete ui;
}

void TransactionDescDialog::requestFinished(const QVariant &result, const QString &error)
{

}

QString TransactionDescDialog::getHtmlText(const QModelIndex &idx)
{
 //   QString strFromLabel = idx.data(TyItem_FromLabel).toString();
    QString strFromAddr = idx.data(TyItem_FromAddress).toString();
    contractAddr(strFromAddr);
 //   QString strToLabel = idx.data(TyItem_ToLabel).toString();
    QString strToAddr = idx.data(TyItem_ToAddress).toString();
    contractAddr(strToAddr);
    QString strTxHash = idx.data(TyItem_Hash).toString();
    QString strTime = idx.data(TyItem_TimeData).toString();
    QString dAmount = idx.data(TyItem_Amount).toString();
    TransactionsType typeTy = (TransactionsType)idx.data(TyItem_TypeTy).toInt();
    QString strFee = idx.data(TyItem_Fee).toString();
    QString strExecer = idx.data(TyItem_Execer).toString();
    QString strActionname = idx.data(TyItem_Actionname).toString();
    QString strNote = idx.data(TyItem_Note).toString();
    QString strError = idx.data(TyItem_Error).toString();

    QString strHTML;

    strHTML.reserve(4000);
    strHTML += "<html><font face='verdana, arial, helvetica, sans-serif'>";

    strHTML += "<b>" + tr("日期") + ":</b> " + strTime + "<br>";

    // From
    if (Generated == typeTy) {
        strHTML += "<b>" + tr("来源") + ":</b> " + tr("挖矿生成") + "<br>";
    } else {
        strHTML += "<b>" + tr("来自") + ":</b> " + strFromAddr + getLabelName(strFromAddr) + "<br>";
    }

    // To
    strHTML += "<b>" + tr("发送") + ":</b> " + strToAddr + getLabelName(strToAddr) + "<br>";

    // Amount
    if(Generated == typeTy || RecvWithAddress == typeTy || RecvFromMining == typeTy) {
        strHTML += "<b>" + tr("收入") + ":</b> " + dAmount + "<br>";

        if(RecvWithAddress == typeTy && "withdraw" == strActionname)
            strHTML += "<b>" + tr("交易费") + ":</b> " + strFee + "<br>";

        strHTML += "<b>" + tr("净额") + ":</b> " + dAmount + "<br>";
    } else if (TyFailure == typeTy) {
        strHTML += "<b>" + tr("交易费") + ":</b> " + strFee + "<br>";
        strHTML += "<b>" + tr("净额") + ":</b> " + strFee + "<br>";
    } else {
      /*  if (SendToSelf == typeTy) {
            strHTML += "<b>" + tr("收入") + ":</b> " + dAmount + "<br>";
        }*/
        if (SendToSelf != typeTy) {
            strHTML += "<b>" + tr("支出") + ":</b> " + dAmount + "<br>";
        }
        strHTML += "<b>" + tr("交易费") + ":</b> " + strFee + "<br>";

        if (SendToSelf == typeTy) {
            strHTML += "<b>" + tr("净额") + ":</b> " + strFee + "<br>";
        } else {
            QString NetAmount = dAmount;
            QStringList Amount = dAmount.split(" ");
            QStringList Fee = strFee.split(" ");
            if(Amount.size() == 3 && Fee.size() == 3) {
                double dNetAmount = Amount[1].toDouble() + Fee[1].toDouble();
                NetAmount = "- " + QString::number(dNetAmount, 'f', 4) + " " + g_UnitName;
            }

            strHTML += "<b>" + tr("净额") + ":</b> " + NetAmount + "<br>";
        }
    }

    if(!strNote.isEmpty())
    {
        strHTML += "<b>" + tr("备注") + ":</b> " + strNote + "<br>";
    }
    strHTML += "<b>" + tr("执行器") + ":</b> " + strExecer + "<br>";
    strHTML += "<b>" + tr("调用函数") + ":</b> " + strActionname + "<br>";
    // Message

    if(!strError.isEmpty() && typeTy == TyFailure)
    {
        if(!g_mapErrorCode[strError].isEmpty())
            strError = g_mapErrorCode[strError];
        strHTML += "<b>" + tr("错误信息") + ":</b> " + strError + "<br>";
    }

    strHTML += "<b>" + tr("交易哈希") + ":</b> " + strTxHash + "<br>";

    strHTML += "</font></html>";
    return strHTML;
}

QString TransactionDescDialog::getLabelName(const QString &strAddr)
{
    QString strHTML;
    QMap<QString, QString> mapMyAddr = g_lpMainUI->m_lpAddressUI->m_lpMyAddressList->m_mapAddrLabel;
    QMap<QString, QString> mapFriAddr = g_lpMainUI->m_lpAddressUI->m_lpFriendsAddressList->m_mapAddrLabel;
    QString strLabel = mapMyAddr[strAddr];
    if(!strLabel.isEmpty()) {
        strHTML = " (" + tr("自己的地址") + ", " + tr("标签") + ": " + strLabel + ")";
    } else {
        strLabel = mapFriAddr[strAddr];
        if(!strLabel.isEmpty()) {
            strHTML = " (" + tr("好友的地址") + ", " + tr("标签") + ": " + strLabel + ")";
        }
    }

    return strHTML;
}

void TransactionDescDialog::contractAddr(QString &strAddr)
{
    // 这4个地址，前面加上 (合约) 两个字 特殊的地址
    if("16htvcBNSEA7fZhAdLJphDwQRQJaHpyHTp" == strAddr
            || "1GaHYpWmqAJsqRwrpoNcB8VvgKtSwjcHqt" == strAddr
            || "1LFqVvGaRpxbEWCEJVpUHAwXnh5Rt591m" == strAddr
            || "1DzTdTLa5JPpLdNNP2PrV1a6JCtULA7GsT" == strAddr
            || "12hpJBHybh1mSyCijQ2MQJPk7z7kZ7jnQa" == strAddr
            || "1BXvgjmBw1aBgmGn1hjfGyRkmN3krWpFP4" == strAddr
            ){
        strAddr = tr("(contract)") + strAddr;
    }
}

void TransactionDescDialog::on_CloseBtn_clicked()
{
    close();
}
