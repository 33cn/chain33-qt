#include "closeingdialog.h"
#include "ui_closeingdialog.h"
#include "mainui.h"
#include "basefuntion.h"
#include "manageui.h"

extern ManageUI*   g_lpManageUI;

CloseingThread::CloseingThread()
{

}

void CloseingThread::run()
{
    while (true)
    {
        if(!GetProcessidFromName()) {
            emit Chain33Closed();
            break;
        } else {
            if(g_lpManageUI)
                g_lpManageUI->CloseChain33Temp();
        }
        sleep(10);
    }
}

CloseingDialog::CloseingDialog(QWidget *parent, CloseType type)
    : JsonConnectorDialog(parent)
    , ui(new Ui::CloseingDialog)
    , m_Type (type)
{
    ui->setupUi(this);
    this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());
    setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint); // 没有帮助按钮

    m_lpCloseingThread = new CloseingThread();
    connect(m_lpCloseingThread, SIGNAL(Chain33Closed()), this, SLOT(Chain33Closed()));

    switch (m_Type) {
    case CloseUI:
        this->setWindowTitle(tr("关闭"));
        m_lpCloseingThread->start();
        break;
    case RestartNewChain33:
        this->setWindowTitle(tr("操作"));
        ui->labelText->setText(tr("正在处理，请稍等..."));
        PostJsonMessage(ID_GetWalletStatus);
    default:
        break;
    }
}

CloseingDialog::~CloseingDialog()
{
    delete ui;
}

void CloseingDialog::requestFinished(const QVariant &result, const QString &/*error*/)
{
    QMap<QString, QVariant> resultMap = result.toMap();
    if (m_nID == ID_GetWalletStatus)
    {
        m_isWalletLock = resultMap["isWalletLock"].toBool();
        m_isTicketLock = resultMap["isTicketLock"].toBool();
        m_lpCloseingThread->start();
    }
}

void CloseingDialog::Chain33Closed()
{
    qDebug() << ("关闭 退出 Chain33Closed 界面");
    if(m_Type == RestartNewChain33)
    {
        if(g_lpManageUI)
        {
            g_lpManageUI->StartThread();
            g_lpManageUI->startChain33();
            g_lpManageUI->UnlockWallet(m_isWalletLock, m_isTicketLock);
        }
        if(NULL != g_lpMainUI)
        {
            g_lpMainUI->ResumeCommunicateChain33Thread();
        }
    }

    QDialog::accept();
}
