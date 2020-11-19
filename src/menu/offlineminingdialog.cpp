#include "offlineminingdialog.h"
#include "ui_offlineminingdialog.h"
#include <QPushButton>
#include <QFileDialog>
#include "mainui.h"
#include "base58.h"
#include "addresslistui.h"
#include "basefuntion.h"

OfflineMiningDialog::OfflineMiningDialog(QWidget *parent, const PlatformStyle *platformStyle) :
    JsonConnectorDialog(parent),
    ui(new Ui::OfflineMiningDialog),
    m_platformStyle (platformStyle)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("离线挖矿授权"));
    this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("生成"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("关闭"));
    ui->buttonBox->button(QDialogButtonBox::Save)->setText(tr("保存"));
    ui->AmountEdit->setValidator(new QDoubleValidator(0.0, 5*le8, 4, this));

    connect(ui->buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SLOT(SaveTxhex()));
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(CreateTxhex()));

    ui->buttonBox->button(QDialogButtonBox::Save)->setVisible(false);
    ui->ErrorVerticalWidget->setVisible(false);
    ui->ResultVerticalWidget->setVisible(false);
}

OfflineMiningDialog::~OfflineMiningDialog()
{
    delete ui;
}

void OfflineMiningDialog::accept()
{/*
#ifdef QT_DEBUG
    ui->buttonBox->button(QDialogButtonBox::Save)->setVisible(true);
    ui->ErrorVerticalWidget->setVisible(false);
    ui->ResultVerticalWidget->setVisible(true);
    ui->txhexBindEdit->setText(tr("0x7ee6331ed623de7f02721f84f898a502494f9a2f6524cce942fb077e0624896d"));
    ui->txhexTransEdit->setText(tr("0x652ee6331ed623de7f02721f84f4cce942fb077e0624896d7898a502494f9a2f"));
#endif*/
}

void OfflineMiningDialog::PostMsgCreateBindMiner()
{
    QString strOriginAddr = ui->originAddrEdit->text();
    QString strBindAddr = ui->bindAddrEdit->text();
    QString strAmount = ui->AmountEdit->text();
    qint64 nAmount = strAmount.toDouble()*le8;
    nAmount = 0; // 默认传入 0

    QJsonObject jsonParms;
    jsonParms.insert("bindAddr", strBindAddr);
    jsonParms.insert("originAddr", strOriginAddr);
    jsonParms.insert("amount", nAmount);
    jsonParms.insert("checkBalance", false);
    QJsonArray params;
    params.insert(0, jsonParms);
    PostJsonMessage(ID_CreateBindMiner, params);
}

void OfflineMiningDialog::PostMsgCreateRawTransaction()
{
    QString strAmount = ui->AmountEdit->text();
    qint64 nAmount = strAmount.toDouble()*le8;

    QJsonObject jsonParms;
    jsonParms.insert("to", "16htvcBNSEA7fZhAdLJphDwQRQJaHpyHTp");
    jsonParms.insert("amount", nAmount);
    jsonParms.insert("note", "coins->ticket");
    jsonParms.insert("execName", "ticket");
    QJsonArray params;
    params.insert(0, jsonParms);
    PostJsonMessage(ID_CreateRawTransaction, params);
}

void OfflineMiningDialog::PostMsgSignRawTx()
{
    if(!m_strtxHex_CreateBindMiner.isEmpty() && !m_strtxHex_CreateRawTransaction.isEmpty())
    {
        PostMsgSignRawTx(ID_SignRawTx_CreateBindMiner, m_strtxHex_CreateBindMiner);
        PostMsgSignRawTx(ID_SignRawTx_CreateRawTransaction, m_strtxHex_CreateRawTransaction);
    }
}

void OfflineMiningDialog::PostMsgSignRawTx(IdType nID, const QString &strtxHex)
{
    QJsonObject jsonParms;
    jsonParms.insert("addr", ui->originAddrEdit->text());
    jsonParms.insert("txhex", strtxHex);
    jsonParms.insert("expire", "1h");
    QJsonArray params;
    params.insert(0, jsonParms);
    PostJsonMessage(nID, params);
}

void OfflineMiningDialog::requestFinished(const QVariant &result, const QString &error)
{
    if(error.isEmpty() || error == "")
    {
        ui->ErrorVerticalWidget->setVisible(false);
    }
    else
    {
        ui->Errorlabel->setText(error);
        ui->ErrorVerticalWidget->setVisible(true);

        return;
    }

    QMap<QString, QVariant> resultMap = result.toMap();
    if(ID_CreateBindMiner == m_nID || ID_CreateRawTransaction == m_nID)
    {
        if(ID_CreateBindMiner == m_nID)
        {
            m_strtxHex_CreateBindMiner = resultMap["txHex"].toString();

            if(!m_strtxHex_CreateBindMiner.isEmpty())
                PostMsgCreateRawTransaction();
        }
        else
        {
            m_strtxHex_CreateRawTransaction = result.toString();
            if(!m_strtxHex_CreateRawTransaction.isEmpty())
                PostMsgSignRawTx();
        }

    }
    else if (ID_SignRawTx_CreateBindMiner == m_nID)
    {
        if(!result.toString().isEmpty())
        {
            ui->buttonBox->button(QDialogButtonBox::Save)->setVisible(true);
            ui->ResultVerticalWidget->setVisible(true);
        }
        ui->txhexBindEdit->setText(result.toString());
    }
    else if (ID_SignRawTx_CreateRawTransaction == m_nID)
    {
        if(!result.toString().isEmpty())
        {
            ui->buttonBox->button(QDialogButtonBox::Save)->setVisible(true);
            ui->ResultVerticalWidget->setVisible(true);
        }
        ui->txhexTransEdit->setText(result.toString());
    }
}

void OfflineMiningDialog::on_BindCopyButton_clicked()
{
    GUIUtil::setClipboard(ui->txhexBindEdit->text());
}

void OfflineMiningDialog::on_TransCopyButton_clicked()
{
    GUIUtil::setClipboard(ui->txhexTransEdit->text());
}

void OfflineMiningDialog::SaveTxhex()
{
    QString strtxhexBind = ui->txhexBindEdit->text();
    QString strtxhexTrans = ui->txhexTransEdit->text();

    QString filename = GUIUtil::getSaveFileName(this, tr("保存授权交易和转账交易信息"), QString(), tr("Config Files (*.txt)"), NULL);
    QFile file(filename);
    if(file.open(QIODevice::ReadWrite))
    {
        QString content = strtxhexBind + "\r\n" + strtxhexTrans;
        file.write(content.toStdString().c_str());
    }

    //关闭文件
    file.close();
}

void OfflineMiningDialog::CreateTxhex()
{
    QString strOriginAddr = ui->originAddrEdit->text();
    QString strBindAddr = ui->bindAddrEdit->text();
   QString strAmount = ui->AmountEdit->text();
//    qint64 nAmount = strAmount.toDouble()*le8;

    if(strBindAddr.isEmpty() || strOriginAddr.isEmpty() || strAmount.isEmpty())
    {
        ui->Errorlabel->setText(tr("输入项不能为空!"));
        ui->ErrorVerticalWidget->setVisible(true);
        return;
    }

    QString strError = tr("输入的地址\"%1\"不是有效的%2地址。");

    if(!IsAddrValid(strBindAddr.toStdString().c_str()))
    {
        ui->Errorlabel->setText(strError.arg(strBindAddr, CStyleConfig::GetInstance().GetAppName()));
        ui->ErrorVerticalWidget->setVisible(true);
        return;
    }

    if(!IsAddrValid(strOriginAddr.toStdString().c_str()))
    {
        ui->Errorlabel->setText(strError.arg(strOriginAddr, CStyleConfig::GetInstance().GetAppName()));
        ui->ErrorVerticalWidget->setVisible(true);
        return;
    }

    PostMsgCreateBindMiner();
}

void OfflineMiningDialog::on_toolButton_clicked()
{
    AddressListUI dlg(ForSending, TabsReceiving, this, m_platformStyle);
    if(dlg.exec())
        ui->originAddrEdit->setText(dlg.getReturnAddr());
}
