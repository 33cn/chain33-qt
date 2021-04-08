#include "introdialog.h"
#include "ui_introdialog.h"
#include "mainui.h"
#include "basefuntion.h"
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

#ifdef WIN32
#include <shlobj.h>
#endif

IntroDialog::IntroDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IntroDialog)
{
#ifndef Q_OS_MAC
    QString strMainIcon;
    if (CStyleConfig::GetInstance().GetStyleType() == QSS_BLUE){
        strMainIcon = ":/icons/yellow_icon";
    } else {
        strMainIcon = ":/icons/blue_icon";
    }
    qApp->setWindowIcon(QIcon(strMainIcon));
    setWindowIcon(QIcon(strMainIcon));
#endif

    ui->setupUi(this);
    this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定"));
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint); // 没有帮助按钮
    ui->storageLabel->setText(ui->storageLabel->text().arg(tr("%1 钱包").arg(CStyleConfig::GetInstance().GetAppName())));
    setWindowTitle(tr("欢迎"));

    QDir sourceDir(QCoreApplication::applicationDirPath() + "/datadir/wallet/wallet.db");
    if(!sourceDir.exists())
    {
        ui->checkBoxCopy->setVisible(false);
    }
    ui->checkBoxCopy->setChecked(true);
    ui->checkBoxCopy->setStyleSheet("QCheckBox{color: #ec5151;}");

    ui->dataDirectory->setEnabled(false);
    ui->errorMessage->setText(tr("即将在该目录下创建一个名为 %1Data 的新目录").arg(CStyleConfig::GetInstance().GetAppName_en()));
    on_dataDirDefault_clicked();
    CheckDir();
}

IntroDialog::~IntroDialog()
{
    delete ui;
}

void IntroDialog::accept()
{
    if(CheckDir() != ST_OK)
        return;

    QString dataDirStr = ui->dataDirectory->text() + "\\" + CStyleConfig::GetInstance().GetAppName_en() + "Data";
    SetRegDataDir(dataDirStr);

    if(ui->checkBoxCopy->isChecked())
    {
        copyDirectoryFiles(QCoreApplication::applicationDirPath() + "\\datadir\\wallet\\wallet.db", dataDirStr + "\\wallet\\wallet.db");
        QFile::copy(QCoreApplication::applicationDirPath() + "\\FriendsAddrList.xml", dataDirStr + "\\FriendsAddrList.xml");
    }

    QDialog::accept();
}

int IntroDialog::CheckDir()
{
    QString dataDirStr = ui->dataDirectory->text();
    uint64_t freeBytesAvailable = GetfreeBytesAvailable(dataDirStr);
    int replyStatus = ST_OK;

    QString freeString = tr("有 %n GB 空闲空间", "", freeBytesAvailable/GB_BYTES);
    if(freeBytesAvailable < BASE_REQUIRED_SPACE * GB_BYTES) {
        freeString += " " + tr("(需要 %n GB空间)", "", BASE_REQUIRED_SPACE);
        ui->freeSpace->setStyleSheet("QLabel { color: #EC5151 }");
        replyStatus = ST_ERROR;
    } else {
        ui->freeSpace->setStyleSheet("");
    }
    ui->freeSpace->setText(freeString + ".");

    return replyStatus;
}

void IntroDialog::on_ellipsisButton_clicked()
{
    // Choose data directory
    QString dir = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(0, tr("选择数据库目录"), ui->dataDirectory->text()));
    if(!dir.isEmpty())
    {
        ui->dataDirectory->setText(dir);
        CheckDir();
    }
}

void IntroDialog::on_dataDirDefault_clicked()
{
#ifdef WIN32
    ui->dataDirectory->setText(GetSpecialFolderPath(CSIDL_APPDATA));
#endif
    ui->dataDirDefault->setChecked(true);
    ui->ellipsisButton->setEnabled(false);
    CheckDir();
}

void IntroDialog::on_dataDirCustom_clicked()
{
    ui->ellipsisButton->setEnabled(true);
}
