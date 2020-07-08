#include "askpassphrasedialog.h"
#include "ui_askpassphrasedialog.h"
#include "guiconstants.h"
#include "basefuntion.h"
#include "mainui.h"

#include <QMessageBox>
#include <QPushButton>
#include <QKeyEvent>


AskPassphraseDialog::AskPassphraseDialog(Mode mode, QWidget *parent)
    : JsonConnectorDialog(parent)
    , ui(new Ui::AskPassphraseDialog)
    , mode(mode)
 //   model(0),
    , fCapsLock(false)
{
    ui->setupUi(this);
    this->setStyleSheet(CStyleConfig::GetInstance().GetStylesheet_child());

    ui->passEdit1->setMaxLength(MAX_PASSPHRASE_SIZE);
    ui->passEdit2->setMaxLength(MAX_PASSPHRASE_SIZE);
    ui->passEdit3->setMaxLength(MAX_PASSPHRASE_SIZE);
    
    // Setup Caps Lock detection.
    ui->passEdit1->installEventFilter(this);
    ui->passEdit2->installEventFilter(this);
    ui->passEdit3->installEventFilter(this);

//    ui->stakingCheckBox->setChecked(fWalletUnlockStakingOnly);

    switch(mode)
    {
   //     case UnlockStaking:
        case Unlock: // Ask passphrase
            ui->stakingCheckBox->setChecked(true);
            ui->stakingCheckBox->show();
            ui->warningLabel->setText(tr("This operation needs your wallet passphrase to unlock the wallet."));
            ui->passLabel2->hide();
            ui->passEdit2->hide();
            ui->passLabel3->hide();
            ui->passEdit3->hide();
            setWindowTitle(tr("Unlock wallet"));
        //    m_nStatus = Wallet_Locked;
            break;
        case Decrypt:   // Ask passphrase
            ui->warningLabel->setText(tr("This operation needs your wallet passphrase to decrypt the wallet."));
            ui->passLabel2->hide();
            ui->passEdit2->hide();
            ui->passLabel3->hide();
            ui->passEdit3->hide();
            setWindowTitle(tr("Decrypt wallet"));
            break;
        case ChangePass: // Ask old passphrase + new passphrase x2
            setWindowTitle(tr("Change passphrase"));
            ui->warningLabel->setText(tr("Enter the old and new passphrase to the wallet."));
            break;
    }

    textChanged();
    connect(ui->passEdit1, SIGNAL(textChanged(QString)), this, SLOT(textChanged()));
    connect(ui->passEdit2, SIGNAL(textChanged(QString)), this, SLOT(textChanged()));
    connect(ui->passEdit3, SIGNAL(textChanged(QString)), this, SLOT(textChanged()));

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定")); //将buttonbox中的ok 变成汉化
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));

//    ui->buttonBox->button(QDialogButtonBox::Ok)->setStyleSheet("QPushButton { background-color: #575757; border-radius: 4px;border: none; height: 20px;} QPushButton:pressed { color: #333333; background: #ffba26; }");
//    ui->buttonBox->button(QDialogButtonBox::Cancel)->setStyleSheet("QPushButton { background-color: #575757; border-radius: 4px;border: none; height: 20px;} QPushButton:pressed { color: #333333; background: #ffba26; }");
}

AskPassphraseDialog::~AskPassphraseDialog()
{
    secureClearPassFields();
    delete ui;
}
/*
void AskPassphraseDialog::setModel(WalletModel *model)
{
    this->model = model;
}*/

void AskPassphraseDialog::accept()
{
    QString oldpass, newpass1, newpass2;
    oldpass = ui->passEdit1->text();
    newpass1 = ui->passEdit2->text();
    newpass2 = ui->passEdit3->text();
    m_strOldPsd = oldpass;
    m_strNewPsd = newpass1;
    bool fWalletUnlockStakingOnly = ui->stakingCheckBox->isChecked();

    secureClearPassFields();

    switch(mode)
    {
    //case UnlockStaking:
    case Unlock:
     {
/*#if QT_VERSION >= 0x050000
        QJsonObject jsonParms;
        jsonParms.insert("passwd", oldpass);
        jsonParms.insert("walletorticket", fWalletUnlockStakingOnly);
        jsonParms.insert("timeout", 0);
        QJsonArray params;
        params.insert(0, jsonParms);
        PostJsonMessage(ID_UnLock, params);
#endif*/

        std::stringstream ostr;
        std::string strbool;
        if(fWalletUnlockStakingOnly) {
            strbool = "true";
        } else {
            strbool = "false";
        }

        ostr << "{\"passwd\":\"" << oldpass.toStdString().c_str() << "\",\"walletorticket\":" << strbool << ",\"timeout\":" << 0 << "}";
        PostJsonMessage(ID_UnLock, ostr.str().c_str());
    }
        break;
    case Decrypt:
        QDialog::accept(); // Success
        break;
    case ChangePass:
        if(newpass1.size() >= 8)
        {
            if(newpass1 == newpass2)
            {
             //   PostJsonMessage(ID_SetPasswd, "oldPass=" + oldpass + "<>newPass=" + newpass1);
                std::stringstream ostr;
                ostr << "{\"oldPass\":\"" << oldpass.toStdString().c_str() << "\",\"newPass\":\"" << newpass1.toStdString().c_str() << "\"}";
                PostJsonMessage(ID_SetPasswd, ostr.str().c_str());
            }
            else
            {
                QMessageBox::critical(this, tr("Wallet encryption failed"), tr("The supplied passphrases do not match."), tr("ok"));
            }
        }
        else
        {
            QMessageBox::critical(this, tr("Wallet encryption failed"), tr("密码不能少于8位!"), tr("ok"));
        }
        break;
    }
}

void AskPassphraseDialog::requestFinished(const QVariant &result, const QString &/*error*/)
{
    QMap<QString, QVariant> resultMap = result.toMap();
    bool isOK = resultMap["isOK"].toBool();

    if(ID_SetPasswd == m_nID)
    {        
        if (ChangePass == mode)
        {
            if(isOK)
            {
                QMessageBox::information(this, tr("Wallet encrypted"), tr("Wallet passphrase was successfully changed."), tr("ok"));
                g_strPsd = m_strNewPsd;
                QDialog::accept(); // Success
            }
            else
            {
                QMessageBox::critical(this, tr("Wallet encryption failed"), g_mapErrorCode[resultMap["msg"].toString()], tr("ok"));
            }
        }
    }
    else if (ID_UnLock == m_nID)
    {
        if(!isOK)
        {
            QMessageBox::critical(this, tr("Wallet unlock failed"), g_mapErrorCode[resultMap["msg"].toString()], tr("ok"));
        }
        else
        {
            if(ui->stakingCheckBox->isChecked()){
                m_nStatus = Wallet_Unlocked_MinerOnly;
            } else {
                m_nStatus = Wallet_Unlocked;
            }
            g_strPsd = m_strOldPsd;
            QDialog::accept(); // Success
        }
    }
}

void AskPassphraseDialog::textChanged()
{
    // Validate input, set Ok button to enabled when acceptable
    bool acceptable = false;
    switch(mode)
    {
    //case UnlockStaking:
    case Unlock: // Old passphrase x1
    case Decrypt:
        acceptable = !ui->passEdit1->text().isEmpty();
        break;
    case ChangePass: // Old passphrase x1, new passphrase x2
        acceptable = !ui->passEdit1->text().isEmpty() && !ui->passEdit2->text().isEmpty() && !ui->passEdit3->text().isEmpty();
        break;
    }
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(acceptable);
}

bool AskPassphraseDialog::event(QEvent *event)
{
    // Detect Caps Lock key press.
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_CapsLock) {
            fCapsLock = !fCapsLock;
        }
        if (fCapsLock) {
            ui->capsLabel->setText(tr("Warning: The Caps Lock key is on!"));
        } else {
            ui->capsLabel->clear();
        }
    }
    return QWidget::event(event);
}

bool AskPassphraseDialog::eventFilter(QObject *object, QEvent *event)
{
    /* Detect Caps Lock.
     * There is no good OS-independent way to check a key state in Qt, but we
     * can detect Caps Lock by checking for the following condition:
     * Shift key is down and the result is a lower case character, or
     * Shift key is not down and the result is an upper case character.
     */
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        QString str = ke->text();
        if (str.length() != 0) {
            const QChar *psz = str.unicode();
            bool fShift = (ke->modifiers() & Qt::ShiftModifier) != 0;
            if ((fShift && psz->isLower()) || (!fShift && psz->isUpper())) {
                fCapsLock = true;
                ui->capsLabel->setText(tr("Warning: The Caps Lock key is on!"));
            } else if (psz->isLetter()) {
                fCapsLock = false;
                ui->capsLabel->clear();
            }
        }
    }
    return QDialog::eventFilter(object, event);
}

void AskPassphraseDialog::secureClearPassFields()
{
    // Attempt to overwrite text so that they do not linger around in memory
    ui->passEdit1->setText(QString(" ").repeated(ui->passEdit1->text().size()));
    ui->passEdit2->setText(QString(" ").repeated(ui->passEdit2->text().size()));
    ui->passEdit3->setText(QString(" ").repeated(ui->passEdit3->text().size()));

    ui->passEdit1->clear();
    ui->passEdit2->clear();
    ui->passEdit3->clear();
}
