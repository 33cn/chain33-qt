#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QFile>
#include <QPushButton>
#include <QFileInfo>
#include "mainui.h"
#include "basefuntion.h"

extern MainUI*  g_lpMainUI;

AboutDialog::AboutDialog(QWidget *parent) :
    JsonConnectorDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setWindowTitle(tr("关于%1").arg(CStyleConfig::GetInstance().GetAppName()));

    initAboutUi();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::requestFinished(const QVariant &result, const QString &/*error*/)
{
    QMap<QString, QVariant> resultMap = result.toMap();
    if(!resultMap["chain33"].toString().isEmpty()) {
        QString strVersion = "chain33:" + resultMap["chain33"].toString() + " app:" + resultMap["app"].toString() + " localDb:" + resultMap["localDb"].toString();
        ui->versionLabel->setText(strVersion);
        if (g_lpMainUI) {
            g_lpMainUI->m_strVersion = strVersion;
        }
    } else {
        ui->versionLabel->setText(tr("版本获取失败，请在控制台输入 version 获取版本"));
    }
}

void AboutDialog::initAboutUi()
{
    readAboutConfigFile();
    this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());
    QString strColor = "#ffba26";
    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE)
    {
        ui->label_6->setPixmap(QPixmap(":/icons/blue_about"));
        strColor = "#2c77ef";
    } else {
        ui->label_6->setPixmap(QPixmap(":/icons/yellow_about"));
    }

    ui->labelAbout->setStyleSheet("QLabel { color: " + strColor + "; font: " + QString::number(GetBaseFontSize() + 6) + "pt;}");
    if (!m_strIntro.isEmpty()) {
        ui->labelAbout->setText(m_strIntro);
    }

    QString strVersion;
    if(g_lpMainUI) {
        strVersion = g_lpMainUI->m_strVersion;
    }
    if(strVersion.isEmpty()){
        PostJsonMessage(ID_GetVersion);
    } else {
        ui->versionLabel->setText(strVersion);
    }

    if (m_strFrameUrl.isEmpty()) {
        ui->frameLabel->setVisible(false);
    } else {
        QString strFrameName = "Chain33";
        if (!m_strFrameName.isEmpty()) {
            strFrameName = m_strFrameName;
        }

        ui->frameLabel->setText(tr("%1 开发框架地址: %2").arg(strFrameName, "<a style='color: " + strColor + ";' href=\"" + m_strFrameUrl + "\">" + m_strFrameUrl));
        ui->frameLabel->setOpenExternalLinks(true);
    }

    if (m_strWebsite.isEmpty()) {
        ui->uccnLabel->setVisible(false);
    } else {
        ui->uccnLabel->setText(tr("官方网站: %1").arg("<a style='color: " + strColor + ";' href=\"" + m_strWebsite + "\">" + m_strWebsite));
        ui->uccnLabel->setOpenExternalLinks(true);
    }

    if (m_strDocumentUrl.isEmpty()) {
        ui->documentLabel->setVisible(false);
    } else {
        ui->documentLabel->setText(tr("文档地址: %1").arg("<a style='color: " + strColor + ";' href=\"" + m_strDocumentUrl + "\">" + m_strDocumentUrl));
        ui->documentLabel->setOpenExternalLinks(true);
    }

    if (m_strBrowserUrl.isEmpty()) {
        ui->browserLabel->setVisible(false);
    } else {
        ui->browserLabel->setText(tr("区块链浏览器地址: %1").arg("<a style='color: " + strColor + ";' href=\"" + m_strBrowserUrl + "\">" + m_strBrowserUrl));
        ui->browserLabel->setOpenExternalLinks(true);
    }

    ui->copyrightLabel->setText(tr("版权所有 © 2020 %1开发组").arg(CStyleConfig::GetInstance().GetAppName()));
}

void AboutDialog::readAboutConfigFile()
{
    QString strPath = QCoreApplication::applicationDirPath() + "/StyleConfig.ini";
    QFileInfo fileInfo(strPath);

    if(fileInfo.exists()) {
        QSettings *lpconfigIni = new QSettings(strPath, QSettings::IniFormat);
        lpconfigIni->setIniCodec(QTextCodec::codecForName("UTF-8"));

        readValue(lpconfigIni, "About/Intro", m_strIntro);
        readValue(lpconfigIni, "About/FrameName", m_strFrameName);
        readValue(lpconfigIni, "About/FrameUrl", m_strFrameUrl);
        readValue(lpconfigIni, "About/Website", m_strWebsite);
        readValue(lpconfigIni, "About/DocumentUrl", m_strDocumentUrl);
        readValue(lpconfigIni, "About/BrowserUrl", m_strBrowserUrl);
        delete lpconfigIni;
    }
}

void AboutDialog::readValue(QSettings *lpconfig, const QString &key, QString &ret)
{
    if (lpconfig) {
        QString strConfig = lpconfig->value(key).toString();
        if(!strConfig.isEmpty())
            ret = strConfig;
    }
}

void AboutDialog::on_buttonBox_accepted()
{
    close();
}
