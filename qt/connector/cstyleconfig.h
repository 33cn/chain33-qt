#ifndef CSTYLECONFIG_H
#define CSTYLECONFIG_H

/*
 * 样式
 */

#include <QString>
#include <QSettings>

#ifdef WIN32
#include <windows.h>
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
#endif

enum SymbolTokenName
{
    SYMBOL_BTY,
    SYMBOL_YCC
};

class CStyleConfig
{
public:
    static const CStyleConfig &GetInstance();

    SymbolTokenName GetSymbol() const;
    const QString &GetAppName() const { return m_strAppName; }
    const QString &GetAppName_en() const { return m_strAppName_en; }
    const QString &GetUnitName() const { return m_strUnitName; }
    const QString &GetMinFee() const { return m_strMinFee; }
    const QString &GetStylesheet() const { return m_stylesheet; }
    const QString &GetStylesheet_main() const { return m_stylesheet_main; }
    const QString &GetStylesheet_child() const { return m_stylesheet_child; }
    const QString &GetChain33Path() const { return m_strChain33Path; }
    const QString &GetChain33cliPath() const { return m_strChain33cliPath; }
    const QString &GetChain33Name() const { return m_strChain33Name; }
private:
    CStyleConfig();
    void readConfigFile();
    void readValue(QSettings *lpconfig, const QString &key, QString &ret);
    void setChian33NamePath();
#ifdef WIN32
    bool isWow64();
#endif

private:
    static CStyleConfig* s_lpStyleConfig;

    QString m_strSymbol;            // 代币符号 目前只支持 BTY 和 YCC

    QString m_strAppName;           // APP名称 默认 bityuan
    QString m_strAppName_en;        // APP中文名称 默认 比特元
    QString m_strUnitName;          // 代币单位
    QString m_strMinFee;            // 最小手续费 BTY 0.001   YCC 20

    // QString m_stylesheet_file;      // qss 文件名称
    QString m_stylesheet;
    QString m_stylesheet_main;      // 颜色主样式
    QString m_stylesheet_child;

    QString m_strChain33Path;
    QString m_strChain33cliPath;

    QString m_strChain33Name;
    QString m_strChain33cliName;
};

#endif // CSTYLECONFIG_H
