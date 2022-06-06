#ifndef BASEFUNTION_H
#define BASEFUNTION_H

#include <QMap>
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QFile>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTextCodec>
#include <QNetworkAccessManager>
#include "singleapplication.h"
#include "cstyleconfig.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#define MAX_TIMEOUT_WAIT_RESPONSE_RESULT (60000) //60 second

const quint64 BASE_REQUIRED_SPACE = 30; //GB

const quint64 GB_BYTES = 1024*1024*1024;
extern QMap<QString, QString> g_mapErrorCode;
extern QNetworkAccessManager*  g_lpaccessManager;
extern SingleApplication* g_lpapp;

#ifdef WIN32
#include <windows.h>
QString GetSpecialFolderPath(int nFolder, bool fCreate = true);
#endif

quint64 GetfreeBytesAvailable(const QString& dataDirStr);

QString GetRegDataDir();

void SetRegDataDir(QString strDataDir);

QString GetDefaultDataDir();

int GetBaseFontSize();

void InitMainUI(const SingleApplication& app);

bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist = false);

bool DelDir(const QString &path);

bool GetProcessidFromName();

#if QT_VERSION >= 0x050000
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
#endif

// 初始化错误码
void InitErrorCode();

bool GetRepairTimeConfig();

void SetRepairTimeConfig(bool bConfig);

// 不进行四舍五入
double Getbalance(QVariant& value);
double GetbalanceD(double dvalue);

//calculate old dir size
quint64 GetOldDirFileSize(const QString &dirPath);

#endif // BASEFUNTION_H
