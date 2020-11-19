#include "basejsonconnector.h"
#include "manageui.h"
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include "mainui.h"
#include "statusbarui.h"
#include "basefuntion.h"
#include "cstyleconfig.h"

QMap<IdType, QString>   g_methodMap;
extern MainUI*              g_lpMainUI;

void InitMethodMap()
{
    g_methodMap.insert(ID_Lock, "Chain33.Lock");
    g_methodMap.insert(ID_UnLock, "Chain33.UnLock");
    g_methodMap.insert(ID_SetPasswd, "Chain33.SetPasswd");
    g_methodMap.insert(ID_SetLabl, "Chain33.SetLabl");
    g_methodMap.insert(ID_NewAccount, "Chain33.NewAccount");
    g_methodMap.insert(ID_GetAccounts, "Chain33.GetAccounts");
    g_methodMap.insert(ID_GetAccounts_GetBalance, "Chain33.GetAccounts");
    g_methodMap.insert(ID_MergeBalance, "Chain33.MergeBalance");
    g_methodMap.insert(ID_SetTxFee, "Chain33.SetTxFee");
    g_methodMap.insert(ID_SendToAddress, "Chain33.SendToAddress");
    g_methodMap.insert(ID_ImportPrivKey, "Chain33.ImportPrivkey");
    g_methodMap.insert(ID_WalletTxList, "Chain33.WalletTxList");
    g_methodMap.insert(ID_GetMemPool, "Chain33.GetMempool");
    g_methodMap.insert(ID_SendTransaction, "Chain33.SendTransaction");
    g_methodMap.insert(ID_QueryTransaction, "Chain33.QueryTransaction");
    g_methodMap.insert(ID_GetTxByAddr, "Chain33.GetTxByAddr");
    g_methodMap.insert(ID_GetTxByHashes, "Chain33.GetTxByHashes");
    g_methodMap.insert(ID_GetBlocks, "Chain33.GetBlocks");
    g_methodMap.insert(ID_GetLastHeader, "Chain33.GetLastHeader");
    g_methodMap.insert(ID_GetPeerInfo, "Chain33.GetPeerInfo");
    g_methodMap.insert(ID_GetNetInfo, "Chain33.GetNetInfo");
    g_methodMap.insert(ID_GenSeed, "Chain33.GenSeed");
    g_methodMap.insert(ID_SaveSeed, "Chain33.SaveSeed");
    g_methodMap.insert(ID_GetSeed, "Chain33.GetSeed");
    g_methodMap.insert(ID_GetWalletStatus, "Chain33.GetWalletStatus");
    g_methodMap.insert(ID_GetBalance_coins, "Chain33.GetBalance");
    g_methodMap.insert(ID_GetBalance_ticket, "Chain33.GetBalance");
    g_methodMap.insert(ID_GetBalance_trade, "Chain33.GetBalance");
    if (CStyleConfig::GetInstance().GetCoinsType() == TOKEN_YCC){
        g_methodMap.insert(ID_SetAutoMining, "pos33.SetAutoMining");
        g_methodMap.insert(ID_GetTicketCount, "pos33.GetPos33TicketCount");
    } else {
        g_methodMap.insert(ID_SetAutoMining, "ticket.SetAutoMining");
        g_methodMap.insert(ID_GetTicketCount, "ticket.GetTicketCount");
    }
    g_methodMap.insert(ID_GetVersion, "Chain33.Version");
    g_methodMap.insert(ID_IsSync, "Chain33.IsSync");
    g_methodMap.insert(ID_IsNtpClockSync, "Chain33.IsNtpClockSync");
    g_methodMap.insert(ID_Query, "Chain33.Query");
    g_methodMap.insert(ID_Query_GetTokens, "Chain33.Query");
    g_methodMap.insert(ID_Query_GetTokenSellOrderByStatus, "Chain33.Query");
    g_methodMap.insert(ID_CreateRawTradeSellTx, "trade.CreateRawTradeSellTx");
    g_methodMap.insert(ID_CreateRawTradeBuyTx, "trade.CreateRawTradeBuyTx");
    g_methodMap.insert(ID_CreateRawTradeRevokeTx, "trade.CreateRawTradeRevokeTx");
    g_methodMap.insert(ID_SignRawTx, "Chain33.SignRawTx");
    g_methodMap.insert(ID_SignRawTx_CreateBindMiner, "Chain33.SignRawTx");
    g_methodMap.insert(ID_SignRawTx_CreateRawTransaction, "Chain33.SignRawTx");
    g_methodMap.insert(ID_GetAccountTokenAssets_token, "Chain33.Query");
    g_methodMap.insert(ID_GetAccountTokenAssets_trade, "Chain33.Query");
    g_methodMap.insert(ID_GetFatalFailure, "Chain33.GetFatalFailure");
    g_methodMap.insert(ID_CreateBindMiner, "ticket.CreateBindMiner");
    g_methodMap.insert(ID_CreateRawTransaction, "Chain33.CreateRawTransaction");
    g_methodMap.insert(ID_GetTimeStatus, "Chain33.GetTimeStatus");
    g_methodMap.insert(ID_CloseQueue, "Chain33.CloseQueue");
    g_methodMap.insert(ID_SendToAddress_Token, "Chain33.SendToAddress");
    g_methodMap.insert(ID_CreateRawTxGroup, "Chain33.CreateRawTxGroup");
    g_methodMap.insert(ID_GetCoinSymbol, "Chain33.GetCoinSymbol");
    g_methodMap.insert(ID_GetProperFee, "Chain33.GetProperFee");
}

BaseJsonConnector::BaseJsonConnector()
    : m_nID (-1)
{
    if(g_methodMap.empty())
    {
        InitMethodMap();
    }
}

BaseJsonConnector::~BaseJsonConnector()
{
}

void BaseJsonConnector::PostJsonMessage(IdType id, const QJsonArray &params)
{
    m_nID = id;
    QJsonObject json;
    json.insert("jsonrpc", "2.0");
    json.insert("id", m_nID);
    json.insert("method", g_methodMap[id]);
    json.insert("params", params);

    PostJson(json);
}

void BaseJsonConnector::PostJson(const QJsonObject &PostJson)
{
    QJsonDocument document;
    document.setObject(PostJson);
    QByteArray postData = document.toJson(QJsonDocument::Compact);
    PostJsonData(postData);
}

void BaseJsonConnector::PostJsonMessage(IdType id)
{
    m_nID = id;
    QJsonObject json;
    json.insert("jsonrpc", "2.0");
    json.insert("id", m_nID);
    json.insert("method", g_methodMap[id]);
    QJsonArray params;
    json.insert("params", params);
    PostJson(json);
}

void BaseJsonConnector::PostJsonMessage(IdType id, const char* strParams)
{
    m_nID = id;
    QByteArray dataArray;
    std::stringstream ostr;
    ostr << "{\"id\":" << id << ",\"jsonrpc\":\"2.0\",\"method\":\"" << g_methodMap[id].toStdString().c_str() << "\",\"params\":[" << strParams << "]}";
    dataArray.append(ostr.str().c_str());
    PostJsonData(dataArray);
}

bool BaseJsonConnector::bPrintf()
{
    return true;
#ifdef QT_DEBUG
        return true;
#endif

    if(m_nID == ID_SendToAddress
            || m_nID == ID_CreateRawTradeSellTx
            || m_nID == ID_CreateRawTradeBuyTx
            || m_nID == ID_CreateRawTradeRevokeTx
            || m_nID == ID_SignRawTx
            || m_nID == ID_SignRawTx_CreateBindMiner
            || m_nID == ID_SignRawTx_CreateRawTransaction
            || m_nID == ID_CreateRawTxGroup
            || m_nID == ID_SendTransaction
            || m_nID == ID_CreateBindMiner
            || m_nID == ID_CreateRawTransaction
            || m_nID == ID_SetAutoMining
            || m_nID == ID_CloseQueue)
    {
        return true;
    }

    return false;
}

QNetworkReply *BaseJsonConnector::GetNetworkReplyPost(const QByteArray &postData)
{
    m_postData = postData;

    if(bPrintf())
        qDebug() << ("Post 内容: ") << postData;

    QNetworkRequest request;
    request.setUrl(QUrl(CStyleConfig::GetInstance().GetNetworkUrl()));

    return g_lpaccessManager->post(request, postData);
}

void BaseJsonConnector::finishedNetwork(QNetworkReply *reply)
{
    if(reply ==  NULL)
        return;

    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();  // bytes

        if(bPrintf())
            qDebug() << ("Post 结果: ") << bytes;

        int id = -1;
        QVariant result;
        QString error;
        #if QT_VERSION >= 0x050000
        QJsonDocument parse_doucment = QJsonDocument::fromJson(bytes);
        if(parse_doucment.isObject())
        {
            QJsonObject obj = parse_doucment.object();
            if(obj.contains("id"))
            {
                id = obj.take("id").toVariant().toInt();                
            }

            if(obj.contains("result"))
            {
                result = obj.take("result").toVariant();                
            }

            if(obj.contains("error"))
            {
                error = obj.take("error").toVariant().toString();
            }
        }
#else
        QScriptEngine engine;

        QScriptValue sc = engine.evaluate("value=" + QString(bytes));
        id = sc.property("id").toInt32();
        result = sc.property("result").toVariant();
        error = sc.property("error").toString();
#endif

        m_nID = id;

        if(!error.isEmpty() && !g_mapErrorCode[error].isEmpty())
        {
            error = g_mapErrorCode[error];
        }
        requestFinished(result, error);
    }
    else
    {
        if (g_lpMainUI && g_lpMainUI->m_lpStatusBarUI)
            g_lpMainUI->m_lpStatusBarUI->setNumConnections(0);
    }

    reply->abort();
    reply->close();
    reply->deleteLater();
    g_lpapp->processEvents();
}

JsonConnectorDialog::JsonConnectorDialog(QWidget *parent) : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint); // 没有帮助按钮
}

void JsonConnectorDialog::PostJsonData(const QByteArray &postData)
{
   QObject::connect(GetNetworkReplyPost(postData), SIGNAL(finished()), this, SLOT(finishedSlot()));
}

void JsonConnectorDialog::finishedSlot()
{
    QNetworkReply* reply = (QNetworkReply*)sender();
    finishedNetwork(reply);
}

JsonConnectorMainWindow::JsonConnectorMainWindow(QWidget *parent) : QMainWindow(parent)
{
}

void JsonConnectorMainWindow::PostJsonData(const QByteArray &postData)
{
    QObject::connect(GetNetworkReplyPost(postData), SIGNAL(finished()), this, SLOT(finishedSlot()));
}

void JsonConnectorMainWindow::finishedSlot()
{
    QNetworkReply* reply = (QNetworkReply*)sender();
    finishedNetwork(reply);
}

JsonConnectorWidget::JsonConnectorWidget(QWidget *parent) : QWidget(parent)
{
}

void JsonConnectorWidget::PostJsonData(const QByteArray &postData)
{
    QObject::connect(GetNetworkReplyPost(postData), SIGNAL(finished()), this, SLOT(finishedSlot()));
}

void JsonConnectorWidget::finishedSlot()
{
    QNetworkReply* reply = (QNetworkReply*)sender();
    finishedNetwork(reply);
}
