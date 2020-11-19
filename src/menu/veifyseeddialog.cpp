#include "veifyseeddialog.h"
#include "ui_veifyseeddialog.h"
#include "mainui.h"
#include "basefuntion.h"

extern MainUI* g_lpMainUI;

veifySeedDialog::veifySeedDialog(QWidget *parent)
: JsonConnectorDialog(parent)
, ui(new Ui::veifySeedDialog)
, m_bveify (true)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setWindowTitle(tr("验证助记词"));

    this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());
    ui->seedTextEdit->setFocus();
    ui->seedTextEdit->installEventFilter(this);
    ui->seedTextEdit->setContextMenuPolicy(Qt::NoContextMenu);
    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE) {
        ui->seedTextEdit->setStyleSheet("QTextEdit{ font: " + QString::number(GetBaseFontSize() + 4) + "pt; background-color: #ffffff; border-radius: 4px; border: none; padding: 10px; color: #5282DB;}");
    } else {
        ui->seedTextEdit->setStyleSheet("QTextEdit{ font: " + QString::number(GetBaseFontSize() + 4) + "pt; background-color: #202020; border-radius: 4px; border: none; padding: 10px; color: #ffba26;}");
    }
}

veifySeedDialog::~veifySeedDialog()
{
    delete ui;
}

void veifySeedDialog::requestFinished(const QVariant &result, const QString &error)
{
    QMap<QString, QVariant> resultMap = result.toMap();
    bool isOK = resultMap["isOK"].toBool();

    if (ID_UnLock == m_nID) {
        if(!isOK) {
            ui->errorLabel->setText(g_mapErrorCode[resultMap["msg"].toString()]);
            ui->veifypushButton->setEnabled(true);
            return;
        } else {
            if(m_bveify) {
                QString strPsd = ui->psdEdit->text();
                QJsonObject jsonParms;
                jsonParms.insert("passwd", strPsd);
                QJsonArray params;
                params.insert(0, jsonParms);
                PostJsonMessage(ID_GetSeed, params);
                m_bveify = false;
            }
        }
    } else if(ID_GetSeed == m_nID) {
        if(!error.isEmpty()) {
            ui->errorLabel->setText(error);
            ui->veifypushButton->setEnabled(true);
            return;
        } else {
            QString strGetSeed = resultMap["seed"].toString();
            QString strSeedCode = ui->seedTextEdit->toPlainText();
            if(strGetSeed == strSeedCode) {
                ui->errorLabel->setText(tr("验证通过"));
            } else {
                ui->errorLabel->setText(tr("验证失败"));
            }

            if(Wallet_Unlocked == m_nLockStatus) {
                ui->veifypushButton->setEnabled(true);
            } else {
                PostJsonMessage(ID_Lock);
            }
        }
    } else if (m_nID == ID_Lock) {
        if(!isOK) {
            // error
            qCritical() << ("ID_Lock error ") << g_mapErrorCode[resultMap["msg"].toString()];
        } else {
            if(Wallet_Unlocked_MinerOnly == m_nLockStatus) {
                QString strPsd = ui->psdEdit->text();
                QJsonObject jsonParms;
                jsonParms.insert("passwd", strPsd);
                jsonParms.insert("walletorticket", true);
                jsonParms.insert("timeout", 0);
                QJsonArray params;
                params.insert(0, jsonParms);
                PostJsonMessage(ID_UnLock, params);
            }
            ui->veifypushButton->setEnabled(true);
        }
    }

}

void veifySeedDialog::on_veifypushButton_clicked()
{
    QString strSeedCode = ui->seedTextEdit->toPlainText();
    if(strSeedCode.isEmpty()) {
        ui->errorLabel->setText(tr("助记词不能为空!"));
        return;
    }

    QString strPsd = ui->psdEdit->text();
    if(strPsd.isEmpty()) {
        ui->errorLabel->setText(tr("密码不能为空!"));
        return;
    }

    ui->errorLabel->setText(tr("正在验证中..."));
    ui->veifypushButton->setEnabled(false);
    m_bveify = true;

    if (g_lpMainUI) {
        m_nLockStatus = g_lpMainUI->m_nStatus;
    }
    if(Wallet_Unlocked == m_nLockStatus) {
        QString strPsd = ui->psdEdit->text();
        QJsonObject jsonParms;
        jsonParms.insert("passwd", strPsd);
        QJsonArray params;
        params.insert(0, jsonParms);
        PostJsonMessage(ID_GetSeed, params);
    } else {
        QJsonObject jsonParms;
        jsonParms.insert("passwd", strPsd);
        jsonParms.insert("walletorticket", false);
        jsonParms.insert("timeout", 0);
        QJsonArray params;
        params.insert(0, jsonParms);
        PostJsonMessage(ID_UnLock, params);
    }
}
