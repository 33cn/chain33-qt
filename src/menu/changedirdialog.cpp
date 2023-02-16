#include "changedirdialog.h"
#include "ui_changedirdialog.h"
#include "mainui.h"
#include <QDir>
#include <QFileDialog>
#include "basefuntion.h"
#include "manageui.h"

extern ManageUI*   g_lpManageUI;

CopyThread::CopyThread()
{

}

void CopyThread::SetDataDir(const QString &strDir)
{
    m_mutex.lock();
    m_dataDirStr = strDir;
    m_mutex.unlock();
}

void CopyThread::run()
{
    while (true)
    {
        if(!GetProcessidFromName()) {
            break;
        } else {
            if(g_lpManageUI)
                g_lpManageUI->CloseChain33Temp();
        }
        sleep(1);
    }

    bool bCopy0 = copyDirectoryFiles(GetDefaultDataDir() + "\\wallet", m_dataDirStr + "\\wallet", true);
    bool bCopy1 = QFile::copy(GetDefaultDataDir() + "\\FriendsAddrList.xml", m_dataDirStr + "\\FriendsAddrList.xml");
    bool bCopy2 = copyDirectoryFiles(GetDefaultDataDir() + "\\datadir", m_dataDirStr + "\\datadir", true);
    if(bCopy2) {
        DelDir(GetDefaultDataDir() + "\\datadir");
        DelDir(GetDefaultDataDir() + "\\logs");
    } else {
        qCritical() << "拷贝文件失败 " << bCopy0 << bCopy1 << bCopy2;
    }
    m_mutex.lock();
    SetRegDataDir(m_dataDirStr);
    m_mutex.unlock();

    emit FinishCopy();
}

ChangeDirDialog::ChangeDirDialog(QWidget *parent) :
    JsonConnectorDialog(parent),
    ui(new Ui::ChangeDirDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("更改数据目录"));
    this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());
    setWindowFlags(Qt::Dialog | Qt::WindowMinimizeButtonHint); // 没有帮助按钮

    m_lpCopyThread = new CopyThread();
    connect(m_lpCopyThread, SIGNAL(FinishCopy()), this, SLOT(FinishCopy()));

    ui->labelDatadir->setText(GetDefaultDataDir());
    ui->errorMessage->setText(tr("即将在该目录下创建一个名为 %1Data 的新目录").arg(CStyleConfig::GetInstance().GetAppName_en()));
    ui->labelAttention->setText(tr("提示: 需要较长的时候处理文件，操作过程中请不要关闭界面。"));    
    ui->progressBar->setMaximum(0);
    ui->progressBar->setTextVisible(false);
    ui->progressBar->setVisible(false);
    ui->EditNewDataDir->setEnabled(false);
    ui->pushButton_Finish->setVisible(false);
    ui->labelFreeSpace->setText(tr("(需要 %n GB空间.)", "", BASE_REQUIRED_SPACE));
}

ChangeDirDialog::~ChangeDirDialog()
{
    delete ui;
}

void ChangeDirDialog::accept()
{
    if (ui->EditNewDataDir->text().isEmpty())
    {
        QMessageBox::warning(this, tr("提示"), tr("新数据目录不能为空!"));
        return;
    }

    PostJsonMessage(ID_GetPeerInfo);
    ui->progressBar->setVisible(true);
    ui->labelAttention->setText(tr("正在处理，请稍等..."));

    ui->pushButton_Change->setEnabled(false);
    ui->pushButton_Cancel->setEnabled(false);
    ui->ellipsisButton->setEnabled(false);

    QString dataDirStr = ui->EditNewDataDir->text() + "\\" + CStyleConfig::GetInstance().GetAppName_en() + "Data";
    m_lpCopyThread->SetDataDir(dataDirStr);
    m_lpCopyThread->start();
}

void ChangeDirDialog::requestFinished(const QVariant &result, const QString &/*error*/)
{
    QMap<QString, QVariant> resultMap = result.toMap();
    if (m_nID == ID_GetPeerInfo)
    {
        QList<QVariant> peersList = (resultMap["peers"]).toList();

        for (int i=peersList.size()-1; i>=0; --i) {
            QMap<QString, QVariant> headerMap = peersList[i].toMap();
            int nPeerHeight = (headerMap["header"].toMap())["height"].toInt();

            if(headerMap["self"].toBool())
            {
                ui->labelAttention->setText(tr("正在处理，大约需要 %1 分钟...").arg(QString::number((int)(nPeerHeight/100000) + 2)));
                break;
            }
        }
    }
}

void ChangeDirDialog::on_ellipsisButton_clicked()
{
    QString dataDirStr = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(0, tr("选择数据库目录"), ui->EditNewDataDir->text()));
    if(!dataDirStr.isEmpty())
    {
        ui->EditNewDataDir->setText(dataDirStr);
        QString dataDirStr = ui->EditNewDataDir->text() + "\\" + CStyleConfig::GetInstance().GetAppName_en();

        quint64 freeBytesAvailable = GetfreeBytesAvailable(dataDirStr);

        QString freeString = tr("有 %n GB 空闲空间", "", freeBytesAvailable/GB_BYTES);
        quint64 requiredSpace = GetOldDirFileSize(GetDefaultDataDir());
        if(freeBytesAvailable < requiredSpace)
        {
            freeString += " " + tr("(需要 %n GB空间)", "", requiredSpace/GB_BYTES);
            ui->labelFreeSpace->setStyleSheet("QLabel { color: #EC5151 }");
            ui->pushButton_Change->setEnabled(false);
        }
        else if(dataDirStr == GetDefaultDataDir())
        {
            ui->labelAttention->setText(tr("新目录与原来目录相同，请选择其他目录。"));
            ui->pushButton_Change->setEnabled(false);
        }
        else
        {
            ui->labelAttention->setText(tr("提示: 需要较长的时候处理文件，操作过程中请不要关闭界面。"));
            ui->labelFreeSpace->setStyleSheet("");
            ui->pushButton_Change->setEnabled(true);
        }        
        ui->labelFreeSpace->setText(freeString + ".");
    }
}

void ChangeDirDialog::FinishCopy()
{
    if(g_lpManageUI)
        g_lpManageUI->StartThread();

    ui->progressBar->setVisible(false);
    ui->labelAttention->setText(tr("数据迁移成功。"));

    ui->ellipsisButton->setEnabled(true);
    ui->pushButton_Change->setVisible(false);
    ui->pushButton_Cancel->setVisible(false);
    ui->pushButton_Finish->setVisible(true);
}

void ChangeDirDialog::on_pushButton_Finish_clicked()
{
    ui->pushButton_Finish->setVisible(false);
    ui->pushButton_Change->setEnabled(true);
    ui->pushButton_Cancel->setEnabled(true);
    done(2);
}
