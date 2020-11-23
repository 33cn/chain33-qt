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
#include <QGuiApplication>
#include <QScreen>
#include <QDesktopWidget>
#include "mainui.h"
#include "manageui.h"
#include "singleapplication.h"
#include "enumtype.h"
#include "basefuntion.h"
#include "introdialog.h"
#include "cstyleconfig.h"

ManageUI* lpManageUI = NULL;

#ifdef WIN32
#include <windows.h>
#include <dbghelp.h>

long ApplicationCrashHandler(EXCEPTION_POINTERS *pException) {
	qCritical() << ("产生崩溃，退出chain33。");
	if (lpManageUI) {
		lpManageUI->CloseChain33();
	}

    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("_MMdd_hhmmss");
    QString dmpName = CStyleConfig::GetInstance().GetAppName_en() + current_date + ".dmp";

    EXCEPTION_RECORD *record = pException->ExceptionRecord;
    QString errCode(QString::number(record->ExceptionCode, 16));
    QString errAddr(QString::number((uint)record->ExceptionAddress, 16));
    QString errFlag(QString::number(record->ExceptionFlags, 16));
    QString errPara(QString::number(record->NumberParameters, 16));
    qCritical() << "errCode: " << errCode << "errAddr: " << errAddr << "errFlag: " << errFlag << "errPara: " << errPara;
    HANDLE hDumpFile = CreateFile((LPCWSTR)QString("./" + dmpName).utf16(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hDumpFile != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
        CloseHandle(hDumpFile);
    } else{
        qCritical() << "hDumpFile == null";
    }
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void SetEnvironmentDPI(){
    // Generate high-dpi pixmaps
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

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

#ifdef WIN32
    //注冊异常捕获函数
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
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

