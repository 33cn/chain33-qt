#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <QFile>
#include <QPushButton>
#include "mainui.h"
#include "basefuntion.h"

AboutDialog::AboutDialog(QWidget *parent) :
    JsonConnectorDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setWindowTitle(tr("关于%1").arg(CStyleConfig::GetInstance().GetAppName()));

    if(g_strVersion.isEmpty()){
        PostJsonMessage(ID_GetVersion);
    } else {
        ui->versionLabel->setText(g_strVersion);
    }

    this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());

    // ui->AboutWidget->setStyleSheet("QWidget { background-color:#2c2c2c; }");
    ui->copyrightLabel->setText(tr("版权所有 © 2018 %1开发组").arg(CStyleConfig::GetInstance().GetAppName()));
    QString strColor = "#ffba26";
    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE)
        strColor = "#2c77ef";
    ui->labelAbout->setStyleSheet("QLabel { color: " + strColor + "; font: " + QString::number(GetBaseFontSize() + 6) + "pt;}");
    ui->uccnLabel->setText(tr("Chain33 开发框架地址: %1").arg("<a style='color: " + strColor + ";' href=\"https://github.com/33cn/chain33\">https://github.com/33cn/chain33"));
    ui->uccnLabel->setOpenExternalLinks(true);
    ui->browserLabel->setText(tr("官方网站 和 文档地址: %1").arg("<a style='color: " + strColor + ";' href=\"https://chain.33.cn\">https://chain.33.cn"));
    ui->browserLabel->setOpenExternalLinks(true);

    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE)
    {
        ui->label_6->setPixmap(QPixmap(":/icons/blue_about"));
    } else {
        ui->label_6->setPixmap(QPixmap(":/icons/yellow_about"));
    }
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::requestFinished(const QVariant &result, const QString &/*error*/)
{
    QMap<QString, QVariant> resultMap = result.toMap();
    if(!resultMap["chain33"].toString().isEmpty()) {
        g_strVersion = "chain33:" + resultMap["chain33"].toString() + " app:" + resultMap["app"].toString() + " localDb:" + resultMap["localDb"].toString();
        ui->versionLabel->setText(g_strVersion);
    } else {
        ui->versionLabel->setText(tr("版本获取失败，请在控制台输入 version 获取版本"));
    }
}

void AboutDialog::on_buttonBox_accepted()
{
    close();
}
