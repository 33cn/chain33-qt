#include "manageui.h"
#include "ui_manageui.h"
#include "mainui.h"
#include "seedui.h"
#include <QPainter>
#include <QString>
#include <string.h>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>
#include "basefuntion.h"
#include <cstdlib>
#include <string>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <qiodevice.h>
#include <QFileInfo>
#ifdef WIN32
#include <wincon.h>
#endif
#include <closeingdialog.h>

ManageUI*   g_lpManageUI = NULL;

RuningThread::RuningThread()
    : m_bFinish (false)
    , m_bQuit (false)
{

}

void RuningThread::SetFinish()
{
    m_mutexFinish.lock();
    m_bFinish = true;
    m_mutexFinish.unlock();
}

void RuningThread::SetQuit(bool bQuit)
{
    m_mutexQuit.lock();
    m_bQuit = bQuit;
    m_mutexQuit.unlock();
}

void RuningThread::run()
{
    while (true)
    {
        if(!GetProcessidFromName())
        {
            emit startChain33();
        }
        else
        {
            m_mutexFinish.lock();
            if(!m_bFinish) {
                m_mutexFinish.unlock();
                emit PostMsgGetWalletstatus();
                sleep(5);
            } else {
                m_mutexFinish.unlock();
            }
        }

        sleep(5);

        m_mutexQuit.lock();
        if(m_bQuit) {
            m_mutexQuit.unlock();
            break;
        } else {
            m_mutexQuit.unlock();
        }
    }
}

ManageUI::ManageUI(QWidget *parent, const char* lpstylesheet)
    : JsonConnectorMainWindow(parent)
    , ui(new Ui::ManageUI)
    , m_bFirst(true)
    , m_nCloseChain33 (0)
    , m_dwChain33ProcessId (-1)
#ifdef WIN32
    , m_clearThread(NULL)
#endif
{
    g_lpManageUI = this;
    m_lpQProcess = new QProcess(this);
    startChain33();

    m_lpThread = new RuningThread();
    connect(m_lpThread, SIGNAL(PostMsgGetWalletstatus()), this, SLOT(PostMsgGetWalletstatus()));
    connect(m_lpThread, SIGNAL(startChain33()), this, SLOT(startChain33()));
#ifdef WIN32
    m_clearThread = new ClearThread();
#endif
    StartThread();

    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background:transparent;");
    this->setStyleSheet(lpstylesheet);

    m_stylesheet = CStyleConfig::GetInstance().GetStylesheet_main();

    PostMsgGetWalletstatus();
}

ManageUI::~ManageUI()
{
    delete m_lpQProcess;
#ifdef WIN32
    delete m_clearThread;
#endif
    delete ui;
}

void ManageUI::requestFinished(const QVariant &result, const QString &/*error*/)
{
    this->ShowHide();
    if(ID_GetWalletStatus == m_nID)
    {
        if(m_lpThread)
            m_lpThread->SetFinish();

        if(m_bFirst)
        {
            m_bFirst = false;
        }
        else
        {
            return;
        }

        QMap<QString, QVariant> resultMap = result.toMap();
        bool bhasSeed = resultMap["isHasSeed"].toBool();
        if(bhasSeed)
        {
            if(g_lpMainUI)
                g_lpMainUI->show();

            PostJsonMessage(ID_GetAccounts);
        }
        else
        {
            SeedUi* lpSeedUI = new SeedUi();
            lpSeedUI->setStyleSheet(m_stylesheet);
            lpSeedUI->show();
        }
    }
    else if (ID_CloseQueue == m_nID)
    {

    }
    else if (ID_UnLock == m_nID)
    {
        QMap<QString, QVariant> resultMap = result.toMap();
        bool isOK = resultMap["isOK"].toBool();
        if(!isOK)
        {
            qDebug() << ("设置过开启或关闭自动修复时间解锁，解锁失败 ") << g_mapErrorCode[resultMap["msg"].toString()];

            this->setStyleSheet(m_stylesheet);
            QMessageBox::critical(this, tr("提示"), tr("请重新解锁钱包！"), tr("确定"));
        }
    }
    else if(ID_GetAccounts == m_nID)
    {
        QMap<QString, QVariant> resultMap = result.toMap();
        QList<QVariant> walletsList = (resultMap["wallets"]).toList();

        for (int i=0; i<walletsList.size(); ++i)
        {
            QMap<QString, QVariant> addrMap = walletsList[i].toMap();
            // add prompt for node award or airdropaddr> 1000.0
            if((addrMap["label"].toString().startsWith("node award") || addrMap["label"].toString().startsWith("airdropaddr"))
                    && Getbalance(addrMap["acc"].toMap()["balance"]) >= 1000.0)
            {
                this->setStyleSheet(m_stylesheet);
                QMessageBox::warning(this, tr("提示"), tr("%1 地址金额大于1000，请及时转出!").arg(addrMap["label"].toString()));
            }
        }
    }
    this->ShowHide();
}

void ManageUI::StartThread()
{
    m_nCloseChain33 = 0;
#ifdef WIN32
    m_lpThread->SetQuit(false);
#endif
    m_lpThread->start();
#ifdef WIN32
    m_clearThread->Resume();
#endif
}

void ManageUI::QuitThread()
{
    m_lpThread->SetQuit(true);
    m_lpThread->quit();
    m_lpThread->wait();
#ifdef WIN32
    m_clearThread->Stop();
#endif
}

void ManageUI::CloseQueueChain33()
{
    QString strCli = CStyleConfig::GetInstance().GetChain33cliPath();
    QStringList strList;
    strList << "close";

    qDebug() << ("发送 close 命令");

    QProcess process(this);
    process.start(strCli, strList);
    process.waitForStarted();
    process.waitForFinished();
    QString strResult = QString::fromUtf8(process.readAllStandardOutput());
    if(strResult.isEmpty())
    {
        strResult = QString::fromUtf8(process.readAllStandardError());
        // 去掉换行
        if(strResult.length() > 1)
        {
            strResult = strResult.left(strResult.length() - 1);
        }
        qDebug() << ("close error 结果 ") <<  strResult;
    }
    else
    {
        qDebug()<< ("close output 结果 ") <<  strResult;
    }
    process.close();
}

void ManageUI::ShowHide()
{
    this->hide();
    ui->manage_label->setText("");
}

void ManageUI::UnlockWallet(bool isWalletLock, bool isTicketLock)
{
    if(!isWalletLock || !isTicketLock)
    {
        std::stringstream ostr;
        std::string strbool;
        if(isWalletLock) {
            strbool = "true";
        } else {
            strbool = "false";
        }

        ostr << "{\"passwd\":\"" << g_strPsd.toStdString().c_str() << "\",\"walletorticket\":" << strbool << ",\"timeout\":" << 0 << "}";
        PostJsonMessage(ID_UnLock, ostr.str().c_str());
    }
}

void ManageUI::PostMsgGetWalletstatus()
{
    PostJsonMessage(ID_GetWalletStatus);
}

void ManageUI::startChain33()
{
    QString strPath = CStyleConfig::GetInstance().GetChain33Path();
    QString strPathError = GetDefaultDataDir() + "/logs/chain33_error.log";

    QString strWalletDatadir;
#ifdef WIN32
    strWalletDatadir = GetDefaultDataDir() + "\\wallet\\wallet.db";
#else
    strWalletDatadir = GetDefaultDataDir() + "/wallet/wallet.db";
#endif
    QFileInfo fileInfo(strWalletDatadir);

#if QT_VERSION >= 0x050000
    // 第一次运行该程序 如果没有logs文件夹 会失败
    QDir dir;
    QString dir_str = GetDefaultDataDir() + "/logs";
    if (!dir.exists(dir_str))
    {
        bool res = dir.mkpath(dir_str);
        qWarning() << "新建目录是否成功" << dir_str << res;
    }

    m_lpQProcess->setStandardErrorFile(strPathError, QAbstractSocket::Append);
#endif

#ifdef WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    HANDLE hReadFile, hWriteFile,hReadPipe,hWritePipe;
    SECURITY_ATTRIBUTES pipeattr;

    pipeattr.nLength = sizeof(SECURITY_ATTRIBUTES);
    pipeattr.lpSecurityDescriptor = NULL;
    pipeattr.bInheritHandle = true;

    CreatePipe(&hReadFile, &hWritePipe, &pipeattr, 0);
    CreatePipe(&hReadPipe, &hWriteFile, &pipeattr, 0);

    memset(&si, 0, sizeof(si));
    memset(&pi,0,sizeof(pi));

    SECURITY_ATTRIBUTES psa = {sizeof(psa),NULL,TRUE};;
    psa.bInheritHandle = TRUE;

    HANDLE herrorfile = CreateFile(strPathError.toStdWString().c_str(),GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE, &psa,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    SetFilePointer (herrorfile, 0, NULL, FILE_END);     // 追加错误信息
    si.hStdError = herrorfile;

    if(GetRepairTimeConfig())
    {
        strPath = strPath + " -fixtime";
    }
    strPath = strPath + " -datadir " + "\"" + GetDefaultDataDir() + "\"";

    if(!fileInfo.exists())
    {
        strPath = strPath + " -waitpid=true";
    }

    qDebug() << ("创建 chain33： ") << strPath.toStdString().c_str();
    if(CreateProcess(NULL, (LPWSTR)strPath.toStdWString().c_str(),  NULL, NULL, true, 0, NULL, NULL, &si, &pi))
    {
        WaitForSingleObject(pi.hProcess, 1);

        m_dwChain33ProcessId = pi.dwProcessId;
        //下面两行关闭句柄，解除本进程和新进程的关系，不然有可能不小心调用TerminateProcess函数关掉子进程
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        qDebug() << ("创建 chain33 进程成功， PID=") << (QString::number((int)m_dwChain33ProcessId, 10).toStdString().c_str());
    }
    else
    {
        qCritical() << ("创建 chain33 进程失败") << strPathError.toStdWString().c_str();
    }

    CloseHandle(hReadFile);
    CloseHandle(hWriteFile);
    CloseHandle(hReadPipe);
    CloseHandle(hWritePipe);
    CloseHandle(herrorfile);
#else
    QStringList strList;
    if(GetRepairTimeConfig())
    {
        strList << "-fixtime";
    }
    strList << "-datadir" << GetDefaultDataDir();
    if(!fileInfo.exists())
    {
         strList << " -waitpid=true";
    }
    m_lpQProcess->start(strPath, strList);
    m_dwChain33ProcessId = m_lpQProcess->pid();
#endif
}

void ManageUI::CloseChain33Temp()
{
    qDebug() << ("退出 Chain33");

    QuitThread();

 //   PostJsonMessage(ID_CloseQueue);
    CloseQueueChain33();
    g_lpMainUI->StopCommunicateChain33Thread();
}
void ManageUI::CloseChain33()
{
#ifndef WIN32
    m_dwChain33ProcessId = m_lpQProcess->pid();
#endif

#ifdef QT_DEBUG
    return;
#endif

    CloseChain33Temp();

    if(GetProcessidFromName())
    {
        CloseingDialog dlg;
        dlg.exec();
    }

    return;

#ifdef WIN32
    if(m_dwChain33ProcessId == DWORD(-1))
    {
        HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnap == INVALID_HANDLE_VALUE)
            return;

        PROCESSENTRY32 pe32;
        memset(&pe32, 0, sizeof(PROCESSENTRY32));
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hProcessSnap, &pe32)) {
            do{
                int nWcsicmp = wcsicmp(pe32.szExeFile, CStyleConfig::GetInstance().GetChain33Path().toStdWString().c_str());
                if(nWcsicmp == 0) {
                    m_dwChain33ProcessId = pe32.th32ProcessID;
                    break;
                }
            }
            while (Process32Next(hProcessSnap, &pe32));
        } else {
            qCritical() << ("chain33 不在运行 获取 PID 失败");
            CloseHandle(hProcessSnap);
            return;
        }
        CloseHandle(hProcessSnap);
    }

    qDebug() << ("退出 chain33 PID = ") << (QString::number((int)m_dwChain33ProcessId, 10).toStdString().c_str());
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_dwChain33ProcessId);
    if(hProcess != NULL)
    {
        DWORD dwExitCode = 0;
        GetExitCodeProcess(hProcess, &dwExitCode);
        qDebug() << ("退出 chain33 dwExitCode = ") << dwExitCode;
        TerminateProcess(hProcess, dwExitCode);
     //   ExitProcess(dwExitCode);
        CloseHandle(hProcess);
    }
    else
    {
        qCritical() << ("退出 chain33 失败，调用 OpenProcess 返回为句柄为空。");

        ++m_nCloseChain33;
        if(m_nCloseChain33 <= 10)
            CloseChain33();
    }
#endif
}

