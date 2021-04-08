#include "basefuntion.h"
#include <QLocale>
#include <QDir>
#include "mainui.h"
#if QT_VERSION >= 0x050000
#include <QGuiApplication>
#endif
#include <QScreen>
#include <QSettings>
#ifdef WIN32
#include <wincon.h>
#include <windows.h>
#include <tlhelp32.h>
#endif

QMap<QString, QString> g_mapErrorCode;

int g_nBaseFontSize = 9;

//space a little bigger than all file size in windows
static const quint64 SPACE_MINUS_SIZE = 100*1024*1024;

QNetworkAccessManager*  g_lpaccessManager;
SingleApplication* g_lpapp;

#ifdef WIN32
#include <stdio.h>
#include <shlobj.h>
#include <windows.h>

#pragma comment(lib, "shell32.lib")

QString GetSpecialFolderPath(int nFolder, bool fCreate)
{
    char pszPath[MAX_PATH] = "";
#if QT_VERSION >= 0x050000
    if(SHGetSpecialFolderPathA(NULL, pszPath, nFolder, fCreate))
    {
        return QString(pszPath);
    }
#else
    if(!GetSystemDirectoryA(pszPath, MAX_PATH))
    {
        QString strPath = QString(pszPath[0]) + ":C:\\Documents and Settings\\All Users\\Application Data";
        return strPath;
    }
    else
    {
        return "C:\\Documents and Settings\\All Users\\Application Data";
    }
#endif
    return "";
}
#endif

QString GetRegDataDir()
{
    QString fileName = "HKEY_CURRENT_USER\\Software\\" + CStyleConfig::GetInstance().GetChain33Name() + "\\" + CStyleConfig::GetInstance().GetAppName();
    QSettings *pReg = new QSettings(fileName, QSettings::NativeFormat);
    QString strDir = pReg->value("strDataDir").toString(); //读取注册表值
    strDir.replace("/", "\\");
    strDir.replace("\\\\", "\\");
    delete pReg;
    return strDir;
}

void SetRegDataDir(QString strDataDir)
{
    strDataDir.replace("/", "\\");
    strDataDir.replace("\\\\", "\\");
    QString fileName = "HKEY_CURRENT_USER\\Software\\" + CStyleConfig::GetInstance().GetChain33Name() + "\\" + CStyleConfig::GetInstance().GetAppName();
    QSettings *pReg = new QSettings(fileName, QSettings::NativeFormat);
    pReg->setValue("strDataDir", strDataDir);
    delete pReg;
}

QString GetDefaultDataDir()
{
    //! Windows < Vista: C:\Documents and Settings\Username\Application Data\chain33-qt
    //! Windows >= Vista: C:\Users\Username\AppData\Roaming\chain33-qt
    //! Mac: ~/Library/Application Support/chain33-qt
    //! Unix: ~/.chain33-qt
#ifdef WIN32
    QString strDir = GetRegDataDir();
    if(strDir.isEmpty())
    {
        return GetSpecialFolderPath(CSIDL_APPDATA) + "\\" + CStyleConfig::GetInstance().GetAppName_en();
    }
    else
    {
        return strDir;
    }
#else
    QString pathRet;
    char* pszHome = getenv("HOME");
    if (pszHome == nullptr || strlen(pszHome) == 0)
        pathRet = "/";
    else
        pathRet = pszHome;
#ifdef MAC_OSX
    // Mac
    return pathRet + "/Library/Application\ Support/" + CStyleConfig::GetInstance().GetAppName_en();
#else
    // Unix
    return pathRet + "/." + CStyleConfig::GetInstance().GetAppName_en();
#endif
#endif
}

int GetBaseFontSize()
{
#ifdef WIN32
    return g_nBaseFontSize;
#elif LINUX
    return 10;
#else
    return 12;
#endif
}

void InitMainUI(const SingleApplication& app)
{
    g_lpapp = (SingleApplication*)&app;

#if QT_VERSION >= 0x050000
#ifdef WIN32
    qInstallMessageHandler(outputMessage);
#endif
#endif

#if QT_VERSION >= 0x050000
    QScreen *srn = QApplication::screens().at(0);
    if(srn)
    {
        double dotsPerInch = (qreal)srn->logicalDotsPerInch();
        double dPerCent = (dotsPerInch/96.0)*100;
        if((int)dPerCent == 125)
        {
            g_nBaseFontSize = 7;
        }
        else if ((int)dPerCent == 150)
        {
            g_nBaseFontSize = 6;
        }
        else if ((int)dPerCent == 175)
        {
            g_nBaseFontSize = 5;
        }
        else if ((int)dPerCent == 200)
        {
            g_nBaseFontSize = 4;
        }
        else if ((int)dPerCent > 200)
        {
            g_nBaseFontSize = 3;
        }
    }
#endif

    QFont font;
#ifdef WIN32
    font.setFamily(("msyh"));
#endif
    font.setPointSize(GetBaseFontSize());
    app.setFont(font);

    QPalette pa;
    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE){
        pa.setColor(QPalette::WindowText, 0x37383C);
        pa.setColor(QPalette::ButtonText,Qt::white);
        pa.setColor(QPalette::Text, 0x37383C);
    } else {
        pa.setColor(QPalette::WindowText,Qt::white);
        pa.setColor(QPalette::ButtonText,Qt::white);
        pa.setColor(QPalette::Text,Qt::white);
    }
    app.setPalette(pa);

    g_lpaccessManager = new QNetworkAccessManager();

    InitErrorCode();
}

#if QT_VERSION >= 0x050000
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtInfoMsg:
        text = QString("Info:");
        break;

    case QtWarningMsg:
        mutex.unlock();
        return;
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date_hour = QString("_%1_%2").arg(current_date_time.mid(0, 10)).arg(current_date_time.mid(11,2));
    QString message = QString("%1 %2 %3 (%4)").arg(text).arg(context_info).arg(msg).arg(current_date_time);

    QString strUILog = GetDefaultDataDir() + "/logs/chain33-qt" + current_date_hour + ".txt";
    QFile file(strUILog);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}
#endif

void InitErrorCode()
{
    QString lang_territory = QString::fromStdString(QLocale::system().name().toStdString());
    if(lang_territory == "zh_CN" /*|| lang_territory == "zh_TW"*/)
    {
        g_mapErrorCode.insert("ErrNotFound", "未找到");
        g_mapErrorCode.insert("ErrNoBalance", "余额不足");
        g_mapErrorCode.insert("ErrBalanceLessThanTenTimesFee", "余额小于十倍交易费");
        g_mapErrorCode.insert("ErrBlockExec", "区块执行错误");
        g_mapErrorCode.insert("ErrCheckStateHash", "状态哈希错误");
        g_mapErrorCode.insert("ErrCheckTxHash", "交易哈希错误");
        g_mapErrorCode.insert("ErrReRunGenesis", "重复运行创世区块");
        g_mapErrorCode.insert("ErrActionNotSupport", "操作不支持");
        g_mapErrorCode.insert("ErrChannelFull", "信道已满");
        g_mapErrorCode.insert("ErrAmount", "总量错误");
        g_mapErrorCode.insert("ErrTxExpire", "交易过期");
        g_mapErrorCode.insert("ErrNoTicket", "票不足");
        g_mapErrorCode.insert("ErrMinerIsStared", "矿工已启动");
        g_mapErrorCode.insert("ErrMinerNotStared", "矿工未启动");
        g_mapErrorCode.insert("ErrTicketCount", "票数量错误");
        g_mapErrorCode.insert("ErrHashlockAmount", "哈希锁数量错误");
        g_mapErrorCode.insert("ErrHashlockHash", "哈希锁哈希错误");
        g_mapErrorCode.insert("ErrHashlockStatus", "哈希锁状态错误");
        g_mapErrorCode.insert("ErrFeeTooLow", "交易费过低");
        g_mapErrorCode.insert("ErrNoPeer", "无远程节点");
        g_mapErrorCode.insert("ErrSign", "签名错误");
        g_mapErrorCode.insert("ErrExecNameNotMath", "执行名称不匹配");
        g_mapErrorCode.insert("ErrChannelClosed", "信道关闭");
        g_mapErrorCode.insert("ErrNotMinered", "非矿工");
        g_mapErrorCode.insert("ErrTime", "时间错误");
        g_mapErrorCode.insert("ErrFromAddr", "发送方地址错误");
        g_mapErrorCode.insert("ErrBlockHeight", "区块高度错误");
        g_mapErrorCode.insert("ErrEmptyTx", "空交易");
        g_mapErrorCode.insert("ErrCoinBaseExecer", "Coinbase执行器错误");
        g_mapErrorCode.insert("ErrCoinBaseTxType", "Coinbase交易类型错误");
        g_mapErrorCode.insert("ErrCoinBaseExecErr", "Coinbase执行错误");
        g_mapErrorCode.insert("ErrCoinBaseTarget", "Coinbase目标错误");
        g_mapErrorCode.insert("ErrCoinbaseReward", "Coinbase奖励错误");
        g_mapErrorCode.insert("ErrNotAllowDeposit", "不允许存款");
        g_mapErrorCode.insert("ErrCoinBaseIndex", "Coinbase索引错误");
        g_mapErrorCode.insert("ErrCoinBaseTicketStatus", "Coinbase票状态错误");
        g_mapErrorCode.insert("ErrBlockNotFound", "未发现区块");
        g_mapErrorCode.insert("ErrHashlockReturnAddrss", "哈希锁返回地址错误");
        g_mapErrorCode.insert("ErrHashlockTime", "哈希锁时间错误");
        g_mapErrorCode.insert("ErrHashlockReapeathash", "哈希锁重复哈希错误");
        g_mapErrorCode.insert("ErrStartBigThanEnd", "开始高度大于结束高度");
        g_mapErrorCode.insert("ErrToAddrNotSameToExecAddr", "接收地址与执行地址不同");
        g_mapErrorCode.insert("ErrTypeAsset", "类型资产错误");
        g_mapErrorCode.insert("ErrEmpty", "空");
        g_mapErrorCode.insert("ErrSendSameToRecv", "发送地址与接收地址相同");
        g_mapErrorCode.insert("ErrTxMsgSizeTooBig", "交易消息过大");
        g_mapErrorCode.insert("ErrTxFeeTooLow", "交易费过低");
        g_mapErrorCode.insert("ErrExecNameNotAllow", "执行名称不允许");
        g_mapErrorCode.insert("ErrLocalDBPerfix", "本地数据库前缀错误");
        g_mapErrorCode.insert("ErrTimeout", "超时");
        g_mapErrorCode.insert("ErrTxExist", "交易已存在");
        g_mapErrorCode.insert("ErrManyTx", "同账户拥有过多交易");
        g_mapErrorCode.insert("ErrDupTx", "重复交易");
        g_mapErrorCode.insert("ErrMemFull", "Mempool已满");
        g_mapErrorCode.insert("ErrHashNotExist", "hash不存在");
        g_mapErrorCode.insert("ErrHeightNotExist", "对应高度的区块不存在");
        g_mapErrorCode.insert("ErrTxNotExist", "交易不存在");
        g_mapErrorCode.insert("ErrAddrNotExist", "地址不存在");
        g_mapErrorCode.insert("ErrStartHeight", "起始高度错误");
        g_mapErrorCode.insert("ErrEndLessThanStartHeight", "起始高度大于结尾高度");
        g_mapErrorCode.insert("ErrClientNotBindQueue", "客户端没有绑定到队列上");
        g_mapErrorCode.insert("ErrContinueBack", "继续向前回退");
        g_mapErrorCode.insert("ErrUnmarshal", "解压错误");
        g_mapErrorCode.insert("ErrMarshal", "压缩错误");
        g_mapErrorCode.insert("ErrBlockExist", "区块已存在");
        g_mapErrorCode.insert("ErrParentBlockNoExist", "父区块不存在");
        g_mapErrorCode.insert("ErrBlockHeightNoMatch", "区块高度不匹配");
        g_mapErrorCode.insert("ErrParentTdNoExist", "父区块的总难度值不存在");
        g_mapErrorCode.insert("ErrBlockHashNoMatch", "区块hash不匹配");
        g_mapErrorCode.insert("ErrIsClosed", "模块实例已关闭");
        g_mapErrorCode.insert("ErrDecode", "解码错误");
        g_mapErrorCode.insert("ErrInputPara", "入参错误");
        g_mapErrorCode.insert("ErrWalletIsLocked", "钱包已锁定");
        g_mapErrorCode.insert("ErrSaveSeedFirst", "请保存种子");
        g_mapErrorCode.insert("ErrUnLockFirst", "请解锁钱包");
        g_mapErrorCode.insert("ErrLabelHasUsed", "标签已使用");
        g_mapErrorCode.insert("ErrPrivkeyExist", "私钥已存在");
        g_mapErrorCode.insert("ErrPrivkey", "私钥错误");
        g_mapErrorCode.insert("ErrInsufficientBalance", "余额不足");
        g_mapErrorCode.insert("ErrVerifyOldpasswdFail", "密码检测不过");
        g_mapErrorCode.insert("ErrInputPassword", "输入密码错误");
        g_mapErrorCode.insert("ErrSeedlang", "种子语言格式错误");
        g_mapErrorCode.insert("ErrSeedNotExist", "种子不存在");
        g_mapErrorCode.insert("ErrSubPubKeyVerifyFail", "子公钥校验错误");
        g_mapErrorCode.insert("ErrLabelNotExist", "标签不存在");
        g_mapErrorCode.insert("ErrAccountNotExist", "账户不存在");
        g_mapErrorCode.insert("ErrSeedExist", "种子已存在");
        g_mapErrorCode.insert("ErrNotSupport", "类型不支持");
        g_mapErrorCode.insert("ErrSeedWordNum", "种子个数错误，必须是15个单词或者汉字");
        g_mapErrorCode.insert("ErrOnlyTicketUnLocked", "只解锁买票挖矿功能");
        g_mapErrorCode.insert("ErrNewCrypto", "新建加密算法错误");
        g_mapErrorCode.insert("ErrFromHex", "解析十六进制私钥字符串错误");
        g_mapErrorCode.insert("ErrPrivKeyFromBytes", "创建私钥错误");
        g_mapErrorCode.insert("ErrSeedWord", "种子错误");        
        g_mapErrorCode.insert("ErrNewWalletFromSeed", "通过种子创建钱包错误");
        g_mapErrorCode.insert("ErrNewKeyPair", "创建私钥公钥对错误");
        g_mapErrorCode.insert("ErrPrivkeyToPub", "私钥生成公钥错误");
        g_mapErrorCode.insert("ErrNotSync", "同步未完成");
        g_mapErrorCode.insert("ErrInvalidPassWord", "密码格式错误");
    }
    else
    {
        g_mapErrorCode.insert("ErrNotFound", "Not Found");
        g_mapErrorCode.insert("ErrNoBalance", "Low Balance");
        g_mapErrorCode.insert("ErrBalanceLessThanTenTimesFee", "Balance Less Than Ten Times TxFee");
        g_mapErrorCode.insert("ErrBlockExec", "Block Execution Error");
        g_mapErrorCode.insert("ErrCheckStateHash", "State Hash Error");
        g_mapErrorCode.insert("ErrCheckTxHash", "Tx Hash Error");
        g_mapErrorCode.insert("ErrReRunGenesis", "Rerun Genesis");
        g_mapErrorCode.insert("ErrActionNotSupport", "Action Not Support");
        g_mapErrorCode.insert("ErrChannelFull", "Channel Is Full");
        g_mapErrorCode.insert("ErrAmount", "Amount Error");
        g_mapErrorCode.insert("ErrTxExpire", "Tx Expire");
        g_mapErrorCode.insert("ErrNoTicket", "No Ticket");
        g_mapErrorCode.insert("ErrMinerIsStared", "Miner Is Stared");
        g_mapErrorCode.insert("ErrMinerNotStared", "Miner Not Stared");
        g_mapErrorCode.insert("ErrTicketCount", "Ticket Count Error");
        g_mapErrorCode.insert("ErrHashlockAmount", "Hashlock Amount Error");
        g_mapErrorCode.insert("ErrHashlockHash", "Hashlock Hash Error");
        g_mapErrorCode.insert("ErrHashlockStatus", "Hashlock Status Error");
        g_mapErrorCode.insert("ErrFeeTooLow", "Fee Too Low");
        g_mapErrorCode.insert("ErrNoPeer", "No Peer");
        g_mapErrorCode.insert("ErrSign", "Sign Error");
        g_mapErrorCode.insert("ErrExecNameNotMath", "Exec Name Not Math");
        g_mapErrorCode.insert("ErrChannelClosed", "Channel Closed");
        g_mapErrorCode.insert("ErrNotMinered", "Not Minered");
        g_mapErrorCode.insert("ErrTime", "Time Error");
        g_mapErrorCode.insert("ErrFromAddr", "From Addr Error");
        g_mapErrorCode.insert("ErrBlockHeight", "Block Height Error");
        g_mapErrorCode.insert("ErrEmptyTx", "Empty Tx");
        g_mapErrorCode.insert("ErrCoinBaseExecer", "Coinbase Execer Error");
        g_mapErrorCode.insert("ErrCoinBaseTxType", "Coinbase Tx Type Error");
        g_mapErrorCode.insert("ErrCoinBaseExecErr", "Coinbase Exec Error");
        g_mapErrorCode.insert("ErrCoinBaseTarget", "Coinbase Target Error");
        g_mapErrorCode.insert("ErrCoinbaseReward", "Coinbase Reward Error");
        g_mapErrorCode.insert("ErrNotAllowDeposit", "Not Allow Deposit");
        g_mapErrorCode.insert("ErrCoinBaseIndex", "Coinbase Index Error	");
        g_mapErrorCode.insert("ErrCoinBaseTicketStatus", "Coinbase Ticket Status Error");
        g_mapErrorCode.insert("ErrBlockNotFound", "Block Not Found");
        g_mapErrorCode.insert("ErrHashlockReturnAddrss", "Hashlock Return Addrss Error");
        g_mapErrorCode.insert("ErrHashlockTime", "Hashlock Time Error");
        g_mapErrorCode.insert("ErrHashlockReapeathash", "Hashlock Reapeat Hash Error");
        g_mapErrorCode.insert("ErrStartBigThanEnd", "Start Bigger Than End");
        g_mapErrorCode.insert("ErrToAddrNotSameToExecAddr", "To Addr Not Same To Exec Addr");
        g_mapErrorCode.insert("ErrTypeAsset", "Type Asset Error");
        g_mapErrorCode.insert("ErrEmpty", "Empty");
        g_mapErrorCode.insert("ErrSendSameToRecv", "Send Same To Recv");
        g_mapErrorCode.insert("ErrTxMsgSizeTooBig", "Tx Msg Size Too Big");
        g_mapErrorCode.insert("ErrTxFeeTooLow", "TxFee Too Low");
        g_mapErrorCode.insert("ErrExecNameNotAllow", "Exec Name Not Allow");
        g_mapErrorCode.insert("ErrLocalDBPerfix", "Local DB Perfix Error");
        g_mapErrorCode.insert("ErrTimeout", "Timeout");
        g_mapErrorCode.insert("ErrTxExist", "Tx Exists");
        g_mapErrorCode.insert("ErrManyTx", "Acount Has Too Many Txs");
        g_mapErrorCode.insert("ErrDupTx", "Duplicated Tx");
        g_mapErrorCode.insert("ErrMemFull", "Mempool Is Full");
        g_mapErrorCode.insert("ErrHashNotExist", "	Hash Not Exist");
        g_mapErrorCode.insert("ErrHeightNotExist", "Height Not Exist");
        g_mapErrorCode.insert("ErrTxNotExist", "Tx Not Exist");
        g_mapErrorCode.insert("ErrAddrNotExist", "Addr No tExist");
        g_mapErrorCode.insert("ErrStartHeight", "Start Height error");
        g_mapErrorCode.insert("ErrEndLessThanStartHeight", "End Height Less Than Start Height");
        g_mapErrorCode.insert("ErrClientNotBindQueue", "Client Not Bind Queue");
        g_mapErrorCode.insert("ErrContinueBack", "Continue Back");
        g_mapErrorCode.insert("ErrUnmarshal", "Unmarshal error");
        g_mapErrorCode.insert("ErrMarshal", "Marshal error");
        g_mapErrorCode.insert("ErrBlockExist", "Block Exist");
        g_mapErrorCode.insert("ErrParentBlockNoExist", "Parent Block Not Exist");
        g_mapErrorCode.insert("ErrBlockHeightNoMatch", "Block Height No Match");
        g_mapErrorCode.insert("ErrParentTdNoExist", "Parent Td Not Exist");
        g_mapErrorCode.insert("ErrBlockHashNoMatch", "Block Hash No Match");
        g_mapErrorCode.insert("ErrIsClosed", "Is Closed");
        g_mapErrorCode.insert("ErrDecode", "Decode error");
        g_mapErrorCode.insert("ErrInputPara", "Input Para error");
        g_mapErrorCode.insert("ErrWalletIsLocked", "Wallet Is Locked");
        g_mapErrorCode.insert("ErrSaveSeedFirst", "Save Seed First");
        g_mapErrorCode.insert("ErrUnLockFirst", "UnLock First");
        g_mapErrorCode.insert("ErrLabelHasUsed", "Label Has Used");
        g_mapErrorCode.insert("ErrPrivkeyExist", "Privkey Exist");
        g_mapErrorCode.insert("ErrPrivkey", "ErrPrivkey error");
        g_mapErrorCode.insert("ErrInsufficientBalance", "Insufficient Balance");
        g_mapErrorCode.insert("ErrVerifyOldpasswdFail", "Verify Old passwd Fail");
        g_mapErrorCode.insert("ErrInputPassword", "Input Password");
        g_mapErrorCode.insert("ErrSeedlang", "Seed lang error");
        g_mapErrorCode.insert("ErrSeedNotExist", "Seed Not Exist");
        g_mapErrorCode.insert("ErrSubPubKeyVerifyFail", "Sub PubKey Verify Fail");
        g_mapErrorCode.insert("ErrLabelNotExist", "Label Not Exist");
        g_mapErrorCode.insert("ErrAccountNotExist", "Account Not Exist");
        g_mapErrorCode.insert("ErrSeedExist", "Seed Exist");
        g_mapErrorCode.insert("ErrNotSupport", "Not Support");
        g_mapErrorCode.insert("ErrSeedWordNum", "Seed Words Num error");
        g_mapErrorCode.insert("ErrOnlyTicketUnLocked", "Only Ticket UnLocked");
        g_mapErrorCode.insert("ErrNewCrypto", "New Crypto Error");
        g_mapErrorCode.insert("ErrFromHex", "From Hex Error");
        g_mapErrorCode.insert("ErrPrivKeyFromBytes", "PrivKey From Bytes");
        g_mapErrorCode.insert("ErrSeedWord", "Seed Error");
        g_mapErrorCode.insert("ErrNewWalletFromSeed", "New Wallet From Seed");
        g_mapErrorCode.insert("ErrNewKeyPair", "New Key Pair");
        g_mapErrorCode.insert("ErrPrivkeyToPub", "Privkey To Pub");
        g_mapErrorCode.insert("ErrNotSync", "Not Sync");
        g_mapErrorCode.insert("ErrInvalidPassWord", "Invalid Password");
    }
}

bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist)
{
    QDir sourceDir(fromDir);
    QDir targetDir(toDir);
    if(!targetDir.exists())
    {    /**< 如果目标目录不存在，则进行创建 */
        if(!targetDir.mkpath(targetDir.absolutePath()))
            return false;
    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir())
        {    /**< 当为目录时，递归的进行copy */
            copyDirectoryFiles(fileInfo.filePath(), targetDir.filePath(fileInfo.fileName()), coverFileIfExist);
        }
        else
        {
            /**< 当允许覆盖操作时，将旧文件进行删除操作 */
            if(coverFileIfExist && targetDir.exists(fileInfo.fileName()))
                targetDir.remove(fileInfo.fileName());

            /// 进行文件copy
            if(!QFile::copy(fileInfo.filePath(), targetDir.filePath(fileInfo.fileName())))
                qCritical() << "拷贝文件失败 " << fileInfo.filePath() << " " << targetDir.filePath(fileInfo.fileName());
             //   return false;
        }
    }
    return true;
}

bool DelDir(const QString &path)
{
    if (path.isEmpty())
    {
        return false;
    }

    QDir dir(path);
    if(!dir.exists())
    {
        return true;
    }

    if(dir.dirName() == "wallet")
    {
        // 过滤这个文件夹
        return true;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList)
    { //遍历文件信息
        if (file.isFile())
        { // 是文件，删除
            file.dir().remove(file.fileName());
        }
        else
        { // 递归删除
            DelDir(file.absoluteFilePath());
        }
    }

    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}

quint64 GetfreeBytesAvailable(const QString &dataDirStr)
{
    uint64_t freeBytesAvailable = 0;

#ifdef WIN32
    QString strRoot = dataDirStr.left(2);
    DWORD64 qwFreeBytes, qwFreeBytesToCaller, qwTotalBytes;
    BOOL bResult = GetDiskFreeSpaceEx(strRoot.toStdWString().c_str(),
    (PULARGE_INTEGER)&qwFreeBytesToCaller,
    (PULARGE_INTEGER)&qwTotalBytes,
    (PULARGE_INTEGER)&qwFreeBytes);
    if(bResult)
    {
        freeBytesAvailable = qwFreeBytes;
    }
#endif

    return freeBytesAvailable;
}

bool GetProcessidFromName()
{
    bool bRet = false;
#ifdef WIN32
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
        return false;

    PROCESSENTRY32 pe32;
    memset(&pe32, 0, sizeof(PROCESSENTRY32));
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hProcessSnap, &pe32)) {
        do{
            int nWcsicmp = wcsicmp(pe32.szExeFile, CStyleConfig::GetInstance().GetChain33Exe().toStdWString().c_str());
            if(nWcsicmp == 0) {
                bRet = true;
                break;
            }
        }
        while (Process32Next(hProcessSnap, &pe32));
    } else {
        bRet = false;
    }
    CloseHandle(hProcessSnap);
#else
    char line[200];
    FILE *fp;
    std::string cmd = "ps -fe | grep '" + CStyleConfig::GetInstance().GetChain33Exe().toStdString() + " ' | grep -v grep";
    const char *sysCommand = cmd.data();
    if ((fp = popen(sysCommand, "r")) != NULL) {
        fgets(line, sizeof(line)-1, fp);
        std::string strResult = line;

        if(strResult.find(CStyleConfig::GetInstance().GetChain33Exe().toStdString().c_str()) != std::string::npos)
            bRet = true;
    }
    pclose(fp);

#endif
    return bRet;
}

bool GetRepairTimeConfig()
{
    QString strPath = GetDefaultDataDir();
#ifdef WIN32
    strPath += "\\wallet\\QtConfig.ini";
#else
    strPath += "/wallet/QtConfig.ini";
#endif

    QFileInfo fileInfo(strPath);
    if(fileInfo.exists())
    {
        QSettings *lpconfigIni = new QSettings(strPath, QSettings::IniFormat);
        bool bConfig = lpconfigIni->value("Config/RepairTime").toBool();
        delete lpconfigIni;
        return bConfig;
    }
    else
    {
        return false;
    }
}

void SetRepairTimeConfig(bool bConfig)
{
    QString strPath = GetDefaultDataDir();
#ifdef WIN32
    strPath += "\\wallet\\QtConfig.ini";
#else
    strPath += "/wallet/QtConfig.ini";
#endif

    QSettings *lpconfigIni = new QSettings(strPath, QSettings::IniFormat);
    lpconfigIni->setValue("Config/RepairTime", bConfig);
    delete lpconfigIni;
}

double Getbalance(QVariant &value)
{
    return (value.toLongLong()/10000)/10000.0;
}

double GetbalanceD(double dvalue)
{
    return (qint64(dvalue)/10000)/10000.0;
}

quint64 GetOldDirFileSize(const QString &dirPath)
{
    QDir dir(dirPath);
    quint64 size = 0;
    foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::Files))
    {
        //计算文件大小
        size += fileInfo.size();
    }

    foreach(QString subDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        //若存在子目录，则递归调用dirFileSize()函数
        size += GetOldDirFileSize(dirPath + QDir::separator() + subDir);
    }

    return size + SPACE_MINUS_SIZE;
}
