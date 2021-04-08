#include "inputseedui.h"
#include "ui_inputseedui.h"
#include "enumtype.h"
#include "mainui.h"
#include "seedui.h"
#include <QElapsedTimer>
#include <QSignalMapper>
#include <QMessageBox>
#include <cstdlib>
#include <QKeyEvent>
#include <QFontMetrics>
#include "basefuntion.h"
#include <QDebug>
#include <QObjectUserData>

extern MainUI* g_lpMainUI;

class CDataBtn : public QObjectUserData
{
public:
    CDataBtn(bool bCheck){m_bCheck = bCheck;}

    bool m_bCheck;
};

InputSeedUi::InputSeedUi(QWidget *parent, int Tab, QWidget *seedUi)
    : JsonConnectorWidget (parent)
    , ui (new Ui::InputSeedUi)
    , m_nTab (Tab)
    , m_seedUi (seedUi)
{
    ui->setupUi(this);
    Init();
}

InputSeedUi::~InputSeedUi()
{
    delete ui;
}

void InputSeedUi::Init()
{
    switch (m_nTab)
    {
    case CreateSeed_Tab:
        ui->SeedTextEdit->setReadOnly(true);
        break;
    case InputSeed_Tab:
        //ui->SeedCodeWidget->setVisible(false);
        ui->SeedPrevBtn->setVisible(false);
        ui->SeedCreateWalletBtn->setText(tr("导入钱包"));
        ui->SeedTextLabel->setText(tr("请输入钱包助记词, 用空格分隔。"));
        ui->SeedClearBtn->setVisible(false);
        break;
    default:
        break;
    }

    ui->SeedErrorLabel->setVisible(false);
    ui->PsdErrorLabel->setVisible(false);
    ui->PsdSureErrorLabel->setVisible(false);

#if QT_VERSION >= 0x040700
    ui->SeedPsdEdit->setPlaceholderText(tr("请输入密码"));
    ui->SeedSurePsdEdit->setPlaceholderText(tr("请重复密码"));
#endif

    if(m_nTab == CreateSeed_Tab) {
        for(int j=0; j<2; ++j) {
            QHBoxLayout* lpHBoxLayout = new QHBoxLayout(this);
            lpHBoxLayout->setSpacing(10);
            for(int i=0; i<8-j; ++i) {
                QPushButton* lpPushBtn = new QPushButton(this);
                lpPushBtn->setUserData(0, new CDataBtn(false));
                lpPushBtn->setObjectName("inputseedBtn");
                m_listBtn.push_back(lpPushBtn);
                lpPushBtn->setText(" ");
                lpHBoxLayout->addWidget(lpPushBtn);
                connect(lpPushBtn, SIGNAL(clicked(bool)), this, SLOT(on_SeedItemBtn_clicked()));
            }
            QSpacerItem* lpSpacerItem = new QSpacerItem(20,20,QSizePolicy::Expanding);
            lpHBoxLayout->addItem(lpSpacerItem);
            ui->SeedCodeLayout->addItem(lpHBoxLayout);
        }
    }

    ui->SeedTextEdit->setFocus();
    ui->SeedTextEdit->installEventFilter(this);
    ui->SeedTextEdit->setContextMenuPolicy(Qt::NoContextMenu);

    ui->SeedTextEdit->setStyleSheet("QTextEdit{ font: " + QString::number(GetBaseFontSize() + 4) + "pt; background-color: #202020; border-radius: 4px; border: none; padding: 10px; color: #ffba26;}");
    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE)
        ui->SeedTextEdit->setStyleSheet("QTextEdit{ font: " + QString::number(GetBaseFontSize() + 4) + "pt; background-color: #ffffff; border-radius: 4px; border: none; padding: 10px; color: #4574CC;}");
}

void InputSeedUi::SetSeedCodeShow(const char *lpcSeedCode)
{
    m_strSeedCode = QString(lpcSeedCode);
    QStringList strListSeedCode = m_strSeedCode.split(" ");

    int n = 15;
    int a[15];
    int t;
    for(int i=0;i<n;i++) {
        t=rand()%n;
        a[i]=t;   //赋值
        for(int j=0; j<i;j++) {
            if(a[j] == t) {
                i--;    //如果相等 把i减1，跳出循环
                break;
            }
        }
    }

    if(m_listBtn.size() >= n && strListSeedCode.size() >= n) {
        for(int i=0; i<n; ++i) {
            int j = a[i];
            m_listBtn[i]->setText(strListSeedCode[j]);
            m_listBtn[i]->setObjectName(strListSeedCode[j]);
            m_listBtn[i]->setEnabled(true);
        }
    }

    on_SeedClearBtn_clicked();
}

void InputSeedUi::requestFinished(const QVariant &result, const QString &error)
{
    QMap<QString, QVariant> resultMap = result.toMap();
    bool isOK = resultMap["isOK"].toBool();
    if(ID_SaveSeed == m_nID) {
        if(isOK) {
            QJsonObject jsonParms;
            jsonParms.insert("passwd", ui->SeedPsdEdit->text());
            jsonParms.insert("timeout", 0);
            jsonParms.insert("ismineronly", false);
            QJsonArray params;
            params.insert(0, jsonParms);
            PostJsonMessage(ID_UnLock, params);

            if(m_seedUi)
                m_seedUi->hide();
        } else {
            // error
            ui->SeedErrorLabel->setVisible(true);
            ui->SeedErrorLabel->setText(g_mapErrorCode[resultMap["msg"].toString()]);
        }
    } else if(ID_UnLock == m_nID) {
        if(!isOK) {
            qCritical() << ("ID_UnLock error ") << g_mapErrorCode[resultMap["msg"].toString()];
            if(g_lpMainUI)
                g_lpMainUI->show();
        } else {
            if(InputSeed_Tab == m_nTab) {
                m_nCount = 0;
                for(int i=0; i<5; ++i)
                {
              //      PostJsonMessage(ID_NewAccount, "label=" + tr("标签") + QString::number(i+1, 10));
                    std::stringstream ostr;
                    ostr << "{\"label\":\"" << tr("标签").toStdString().c_str() << i+1 << "\"}";
                    PostJsonMessage(ID_NewAccount, ostr.str().c_str());
                }
            } else {
             //   PostJsonMessage(ID_NewAccount, "label=" + tr("标签") + QString::number(1, 10));
                std::stringstream ostr;
                ostr << "{\"label\":\"" << tr("标签").toStdString().c_str() << 1 << "\"}";
                PostJsonMessage(ID_NewAccount, ostr.str().c_str());
            }
        }
    } else if(ID_NewAccount == m_nID) {
        if(!error.isEmpty())
            qCritical() << ("ID_NewAccount error ") << error;

        if(g_lpMainUI) {
            g_lpMainUI->show();
        }

        QElapsedTimer t;
        t.start();
        while(t.elapsed()<30000) {
            QCoreApplication::processEvents();
        }

        if(InputSeed_Tab == m_nTab) {
            ++ m_nCount;
            if( m_nCount >= 5)
                PostJsonMessage(ID_Lock);
        } else {
            PostJsonMessage(ID_Lock);
        }
    } else if (m_nID == ID_Lock) {
        if(isOK) {
            if(g_lpMainUI) {
                // 锁定
                g_lpMainUI->setEncryptionStatus(Wallet_Locked);
            }
        } else {
            // error
            qCritical() << ("ID_Lock error ") << g_mapErrorCode[resultMap["msg"].toString()];
        }
    }
}

bool InputSeedUi::eventFilter(QObject *target, QEvent *event)
{
    // 放开粘贴
// #ifdef QT_DEBUG
    return QWidget::eventFilter(target, event);
// #endif

    if (target == ui->SeedTextEdit)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->matches(QKeySequence::Paste))
            {
             //   qDebug() << "Ctrl + V";
                return true;
            }
        }
        if (event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::MidButton)
            {
            //   qDebug() << "Mouse MidButton Release";
                return true;
            }
        }
    }

    return QWidget::eventFilter(target, event);
}

void InputSeedUi::on_SeedPrevBtn_clicked()
{
    emit on_PrevBtn();
}

void InputSeedUi::on_SeedCreateWalletBtn_clicked()
{    
    QString strSeedCode = ui->SeedTextEdit->toPlainText();    
    QStringList strlist = strSeedCode.split(" ");
    strSeedCode = "";
    for(int i=0; i<strlist.size(); ++i)
    {
        if(strlist[i] != "")
        {
            strSeedCode += strlist[i];
            if(i != strlist.size() -1)
                strSeedCode += " ";
        }
    }
    if(strSeedCode.right(1) == " ")
        strSeedCode.chop(1);

//#ifndef QT_DEBUG
    if(strSeedCode.isEmpty() || (m_nTab == CreateSeed_Tab && m_strSeedCode != strSeedCode) )
    {
        ui->SeedErrorLabel->setVisible(true);
        return;
    }
    else
    {
        ui->SeedErrorLabel->setVisible(false);
    }
//#endif

    QString strPsd = ui->SeedPsdEdit->text();
    QString strSurePsd = ui->SeedSurePsdEdit->text();
#ifndef QT_DEBUG
    if(strPsd.size() < 8)
    {
        ui->PsdErrorLabel->setVisible(true);
        return;
    }
    else
    {
        ui->PsdErrorLabel->setVisible(false);
    }

    if(strPsd != strSurePsd)
    {
        ui->PsdSureErrorLabel->setVisible(true);
        return;
    }
    else
    {
        ui->PsdSureErrorLabel->setVisible(false);
    }
#endif

 //   QString strParams = "seed=" + strSeedCode +"<>passwd=" + strPsd;
 //   PostJsonMessage(ID_SaveSeed, strParams);
    std::stringstream ostr;
    ostr << "{\"seed\":\"" << strSeedCode.toStdString().c_str() << "\",\"passwd\":\"" << strPsd.toStdString().c_str() << "\"}";
    PostJsonMessage(ID_SaveSeed, ostr.str().c_str());
}

void InputSeedUi::on_SeedItemBtn_clicked()
{
    QString buttonName = sender()->objectName();

    QPushButton* lpPushBtn = (QPushButton*)sender();
    CDataBtn* lpDataBth = (CDataBtn*)lpPushBtn->userData(0);
    lpDataBth->m_bCheck = !lpDataBth->m_bCheck;
    lpPushBtn->setUserData(0, lpDataBth);

    QString strSeed = ui->SeedTextEdit->toPlainText();

    if(lpDataBth->m_bCheck) {
        strSeed += buttonName;
        strSeed += " ";
        if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE) {
            lpPushBtn->setStyleSheet("QPushButton{ font: " + QString::number(GetBaseFontSize() + 2) + "pt; color: #ffffff; border-radius: 4px; padding-left: 13px; padding-right: 13px;background: #5282DB; height: 30px; } QPushButton:hover{ background-color: #5574E9 }");
        } else {
            lpPushBtn->setStyleSheet("QPushButton{ font: " + QString::number(GetBaseFontSize() + 2) + "pt; color: #333333; border-radius: 4px; padding-left: 13px; padding-right: 13px;background: #ffba26; height: 30px; } QPushButton:hover{ background-color: #ffdc26 }");
        }
     } else {
        strSeed.replace(buttonName + " ", "");
        if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE) {
            lpPushBtn->setStyleSheet("QPushButton{ font: " + QString::number(GetBaseFontSize() + 2) + "pt; color: #1B2740; border-radius: 4px; padding-left: 13px; padding-right: 13px;background: #FFFFFF; height: 30px; } QPushButton:hover{ background-color: #6F9EF7 }");
        } else {
            lpPushBtn->setStyleSheet("QPushButton{ font: " + QString::number(GetBaseFontSize() + 2) + "pt; color: #ffffff; border-radius: 4px; padding-left: 13px; padding-right: 13px;background: #3d3d3d; height: 30px; } QPushButton:hover{ background-color: #4d4d4d }");
        }
    }

    ui->SeedTextEdit->setText(strSeed);
    ui->SeedTextEdit->moveCursor(QTextCursor::End);
}

void InputSeedUi::on_SeedClearBtn_clicked()
{
    ui->SeedTextEdit->setText("");
    ui->SeedErrorLabel->setVisible(false);

    for(int i=0; i<15; ++i)
    {
        CDataBtn* lpDataBth = (CDataBtn*)m_listBtn[i]->userData(0);
        lpDataBth->m_bCheck = false;
        m_listBtn[i]->setUserData(0, lpDataBth);
        if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE) {
            m_listBtn[i]->setStyleSheet("QPushButton{ font: " + QString::number(GetBaseFontSize() + 2) + "pt; color: #1B2740; border-radius: 4px; padding-left: 13px; padding-right: 13px;background: #FFFFFF; height: 30px; } QPushButton:hover{ background-color: #6F9EF7 }");
        } else {
            m_listBtn[i]->setStyleSheet("QPushButton{ font: " + QString::number(GetBaseFontSize() + 2) + "pt; color: #ffffff; border-radius: 4px; padding-left: 13px; padding-right: 13px;background: #3d3d3d; height: 30px; } QPushButton:hover{ background-color: #4d4d4d }");
        }
    }
}

