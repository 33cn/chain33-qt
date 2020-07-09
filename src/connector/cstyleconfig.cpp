#include "cstyleconfig.h"
#include <QFileInfo>
#include <QTextStream>
#include <QTextCodec>
#include <QCoreApplication>
#include <stdio.h>
#include <QDebug>

CStyleConfig* CStyleConfig::s_lpStyleConfig = nullptr;

CStyleConfig::CStyleConfig()
: m_strChain33Name ("chain33")
, m_stylesheet_type ("yellow")
, m_strAppName ("chain33-qt")
, m_strAppName_en ("chain33-qt")
, m_strAppName_zh ("chain33-qt")
, m_strUnitName ("coin")
, m_strMinFee ("0.001")
{
    readConfigFile();
    setChain33NamePath();
}

void CStyleConfig::readConfigFile()
{
    QString strPath = QCoreApplication::applicationDirPath() + "/StyleConfig.ini";
    QFileInfo fileInfo(strPath);

    qDebug(strPath.toStdString().c_str());

    if(fileInfo.exists())
    {
        QSettings *lpconfigIni = new QSettings(strPath, QSettings::IniFormat);
        lpconfigIni->setIniCodec(QTextCodec::codecForName("UTF-8"));

        readValue(lpconfigIni, "Config/Chain33Name", m_strChain33Name);
        readValue(lpconfigIni, "Config/AppName", m_strAppName);
        readValue(lpconfigIni, "Config/AppName_zh", m_strAppName_zh);
        readValue(lpconfigIni, "Config/AppName_en", m_strAppName_en);
        readValue(lpconfigIni, "Config/MinFee", m_strMinFee);
        readValue(lpconfigIni, "Config/StyleType", m_stylesheet_type);
        delete lpconfigIni;
    }

    QString strQssName = ":/qss_yellow";
    if (m_stylesheet_type == "blue")
        strQssName = ":/qss_blue";

    QFile file(strQssName);
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    m_stylesheet = filetext.readAll();
    file.close();

    m_stylesheet_main = "QWidget {background-color:#2c2c2c;border:none;}" + m_stylesheet;
    m_stylesheet_child = "QWidget {background-color:#3d3d3d;border:none;}" + m_stylesheet;

    if (m_stylesheet_type == "blue") {
        m_stylesheet_main = "QWidget {background-color:#EDEEF2;border:none;}" + m_stylesheet;
        m_stylesheet_child = "QWidget {background-color:#EDEEF2;border:none;}" + m_stylesheet;
    }

    QString lang_territory = QString::fromStdString(QLocale::system().name().toStdString());
    if(lang_territory == "zh_CN") {
        m_strAppName = m_strAppName_zh;
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

void CStyleConfig::setChain33NamePath()
{
    m_strChain33Exe = m_strChain33Name;
    m_strChain33cliExe = m_strChain33Name + "-cli";

#ifdef WIN32
    if(!isWow64()) {
        m_strChain33Exe += "-x86";
        m_strChain33cliExe += "-x86";
    }
    m_strChain33Exe += ".exe";
    m_strChain33cliExe += ".exe";
#endif

    m_strChain33Path = QCoreApplication::applicationDirPath() + "/" + m_strChain33Exe;
    m_strChain33cliPath = QCoreApplication::applicationDirPath() + "/" + m_strChain33cliExe;

    qDebug() << "m_strChain33Name:" << m_strChain33Exe << "m_strChain33cliName:" << m_strChain33cliExe;
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

STYLE_QSS CStyleConfig::GetStyleType() const
{
    if (m_stylesheet_type == "blue")
        return QSS_BLUE;

    return QSS_YELLOW;
}
