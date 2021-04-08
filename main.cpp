#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QTextStream>
#include <QMessageBox>
#include <QTextCodec>
#include <QDebug>
#include <QSettings>
#if QT_VERSION >= 0x050600
#include <QGuiApplication>
#include <QScreen>
#include <QDesktopWidget>
#endif
#include "mainui.h"
#include "seedui.h"
#include "manageui.h"
#include "singleapplication.h"
#include "enumtype.h"
#include "basefuntion.h"
#include "introdialog.h"
#include "cstyleconfig.h"



ManageUI* lpManageUI = NULL;

#if QT_VERSION > 0x050100
#ifdef WIN32
#include <Windows.h>
#include <DbgHelp.h>

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException){//程式异常捕获
    qCritical() << ("产生崩溃，退出chain33。");
    if(lpManageUI)
    {
        lpManageUI->CloseChain33();
    }

    EXCEPTION_RECORD* record = pException->ExceptionRecord;
    QString errCode(QString::number(record->ExceptionCode,16));
    QString errAdr(QString::number((uint)record->ExceptionAddress,16));
    qCritical() << ("错误代码：") << errCode << (" 错误地址： ") << errAdr;
    //创建 Dump 文件

    QString dmpName = CStyleConfig::GetInstance().GetAppName_en() + "_MMdd_hhmmss.dmp";
    HANDLE hDumpFile = CreateFile(QDateTime::currentDateTime().toString(dmpName).toStdWString().c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if( hDumpFile != INVALID_HANDLE_VALUE){
        //Dump信息
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;
        //写入Dump文件内容
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    }
    //这里弹出一个错误对话框并退出程序
    QMessageBox::critical(NULL, "程式崩溃", QString("%1").arg(errAdr)+QString("<div>错误代码：%1</div><div>错误地址：%2</div></FONT>").arg(errCode).arg(errAdr), QMessageBox::Ok);
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif
#endif

void SetEnvironmentDPI(){
#if QT_VERSION > 0x050100
    // Generate high-dpi pixmaps
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

#if QT_VERSION >= 0x050600
    bool bEnableHighDpiScaling = true;

#ifdef WIN32
    DEVMODE DevMode;
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DevMode);
    DWORD Dw = DevMode.dmPelsWidth;
    DWORD Dh = DevMode.dmPelsHeight;

    if(Dw * Dh <= 1920 * 1080)
        bEnableHighDpiScaling = false;
#endif

    if(bEnableHighDpiScaling)
        QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

#ifdef Q_OS_MAC
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif
}

int main(int argc, char *argv[])
{
    SetEnvironmentDPI();

    QString appName = "chain33-qt";
    QFileInfo fileInfo("./StyleConfig.ini");
    if(fileInfo.exists())
    {
        QSettings *lpconfigIni = new QSettings("./StyleConfig.ini", QSettings::IniFormat);
        lpconfigIni->setIniCodec(QTextCodec::codecForName("UTF-8"));

        if (lpconfigIni) {
            QString strConfig = lpconfigIni->value("Config/AppName").toString();
            if(!strConfig.isEmpty()) {
                appName = strConfig + "-qt";
            }
        }
        delete lpconfigIni;
    }

    SingleApplication app(argc, argv, appName);
#ifndef QT_DEBUG
    if (app.IsRunning()) {
        QMessageBox::information(NULL, "提示", "钱包已经在运行");
        return 0;
    }
#endif

#if QT_VERSION > 0x050100
#ifdef WIN32
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);//注冊异常捕获函数
#endif
#endif

    qDebug("\r\nIn Main\r\n");
    // 设置默认字体及编码
    InitMainUI(app);

    // 翻译为当前语言 目前只支持中文 locale/bitcoin_zh_CN.ts 和英文 locale/bitcoin_en.ts
    QString lang_territory = QString::fromStdString(QLocale::system().name().toStdString());
    if(lang_territory != "en")
        lang_territory = "zh_CN";

    QString lang = lang_territory;
    lang.truncate(lang_territory.lastIndexOf('_'));

    QTranslator qtTranslatorBase, qtTranslator, translatorBase, translator;
    if (qtTranslatorBase.load("qt_" + lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(&qtTranslatorBase);
    if (qtTranslator.load("qt_" + lang_territory, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(&qtTranslator);
    if (translatorBase.load(lang, ":/translations/"))
        app.installTranslator(&translatorBase);
    if (translator.load(lang_territory, ":/translations/"))
        app.installTranslator(&translator);

#ifdef WIN32
    QString strDir = GetRegDataDir();
    if(strDir.isEmpty())
    {
        IntroDialog dlg;
        if(!dlg.exec())
        {
            return 0;
        }
    }
#endif

    QString stylesheet = CStyleConfig::GetInstance().GetStylesheet();
    lpManageUI = new ManageUI(NULL, stylesheet.toStdString().c_str());
    lpManageUI->show();

    MainUI* lpmainUI = new MainUI(stylesheet);
    lpmainUI->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_main());
    lpmainUI->hide();

    app.connect( &app, SIGNAL( lastWindowClosed() ), lpManageUI, SLOT( CloseChain33() ) );
    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );

    int result = app.exec();
    delete lpmainUI;
    delete lpManageUI;
    return result;
}

