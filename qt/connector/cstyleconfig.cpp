#include "cstyleconfig.h"
#include <QFileInfo>
#include <QTextStream>
#include <QTextCodec>
#include <QCoreApplication>
#include <stdio.h>
#include <QDebug>

CStyleConfig* CStyleConfig::s_lpStyleConfig = nullptr;

CStyleConfig::CStyleConfig()
: m_strSymbol ("BTY")
, m_strAppName ("bityuan")
, m_strAppName_en ("bityuan")
, m_strUnitName ("BTY")
, m_strMinFee ("0.001")
{
    readConfigFile();
    setChian33NamePath();
}

void CStyleConfig::readConfigFile()
{
    QString strPath = QCoreApplication::applicationDirPath() + "/StyleConfig.ini";
    QString strAppName_zh = "比特元";
    QFileInfo fileInfo(strPath);

    qDebug(strPath.toStdString().c_str());

    if(fileInfo.exists())
    {
        QSettings *lpconfigIni = new QSettings(strPath, QSettings::IniFormat);
        lpconfigIni->setIniCodec(QTextCodec::codecForName("UTF-8"));

        readValue(lpconfigIni, "Config/Symbol", m_strSymbol);
        readValue(lpconfigIni, "Config/AppName_zh", strAppName_zh);
        readValue(lpconfigIni, "Config/AppName_en", m_strAppName_en);
        readValue(lpconfigIni, "Config/UnitName", m_strUnitName);
        readValue(lpconfigIni, "Config/MinFee", m_strMinFee);

        delete lpconfigIni;
    }

    QString strQssName = ":/qss_bty";
    if (m_strSymbol == "YCC")
        strQssName = ":/qss_ycc";

    QFile file(strQssName);
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    m_stylesheet = filetext.readAll();
    file.close();

    m_stylesheet_main = "QWidget {background-color:#2c2c2c;border:none;}" + m_stylesheet;
    m_stylesheet_child = "QWidget {background-color:#3d3d3d;border:none;}" + m_stylesheet;

    if (m_strSymbol == "YCC") {
        m_stylesheet_main = "QWidget {background-color:#EDEEF2;border:none;}" + m_stylesheet;
        m_stylesheet_child = "QWidget {background-color:#EDEEF2;border:none;}" + m_stylesheet;
    }

    QString lang_territory = QString::fromStdString(QLocale::system().name().toStdString());
    if(lang_territory == "zh_CN") {
        m_strAppName = strAppName_zh;
    } else {
        m_strAppName = m_strAppName_en;
    }
}

void CStyleConfig::readValue(QSettings *lpconfig, const QString &key, QString &ret)
{
    if (lpconfig) {
        QString strConfig = lpconfig->value(key).toString();
        if(!strConfig.isEmpty())
            ret = strConfig;
    }
}

void CStyleConfig::setChian33NamePath()
{
    m_strChain33Name = "chain33";
    m_strChain33cliName = "chain33-cli";

    if (m_strSymbol == "YCC") {
        m_strChain33Name += "-YCC";
        m_strChain33cliName += "-YCC";
    }

#ifdef WIN32
    if(!isWow64()) {
        m_strChain33Name += "-x86";
        m_strChain33cliName += "-x86";
    }
    m_strChain33Name += ".exe";
    m_strChain33cliName += ".exe";
#endif

    m_strChain33Path = QCoreApplication::applicationDirPath() + "/" + m_strChain33Name;
    m_strChain33cliPath = QCoreApplication::applicationDirPath() + "/" + m_strChain33cliName;

    qDebug() << "m_strChain33Name:" << m_strChain33Name << "m_strChain33cliName:" << m_strChain33cliName;
    qDebug() << "m_strChain33Path:" << m_strChain33Path << "m_strChain33cliPath:" << m_strChain33cliPath;
}

#ifdef WIN32
bool CStyleConfig::isWow64()
{
    LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(L"kernel32"), "IsWow64Process");
    BOOL bIsWow64 = FALSE;

    if (NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            return false;
        }
    }
    return bIsWow64 == TRUE;
}
#endif

const CStyleConfig &CStyleConfig::GetInstance()
{
    if(s_lpStyleConfig == nullptr) {
          s_lpStyleConfig = new CStyleConfig;
    }
    return *s_lpStyleConfig;
}

SymbolTokenName CStyleConfig::GetSymbol() const
{
    if (m_strSymbol == "YCC")
        return SYMBOL_YCC;

    return SYMBOL_BTY;
}
